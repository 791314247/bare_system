/*
 * Copyright (c) 2006-2018, bare-system Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 *
 */

#include "timer.h"


/**
 * 软件定时器内部变量
 */
static bs_timer_t head_point = BS_NULL;

static bs_bool_t is_node_already_creat(bs_timer_t node)
{
    bs_timer_t list;   //list保存当前需要检查的节点的地址
    if (node == BS_NULL)
        return BS_FALSE;

    list = head_point;
    while (list != BS_NULL) {
        if (list == node)
            return BS_TRUE;
        list = list->next;       //后移一个节点
    }
    return BS_FALSE;
}

static bs_timer_t creat_node(bs_timer_t node)
{
    bs_timer_t list;   //list保存当前需要检查的节点的地址
    if (node == BS_NULL)
        return head_point;

    if (is_node_already_creat(node) != BS_FALSE) {
        return head_point;  /* 当节点已经存在的时候退出 */
    }
    //当头节点为空时，将传入的节点作为头节点，返回头节点
    if (head_point == BS_NULL)         {
        head_point = node;
        node->next = BS_NULL;
        return head_point;
    }
    list = head_point;
    while (list->next != BS_NULL) {
        list = list->next;       //后移一个节点
    }
    list->next = node;
    node->next = BS_NULL;
    return head_point;
}

static bs_err_t delete_node(bs_timer_t node)
{
    bs_timer_t list;   //list保存当前需要检查的节点的地址
    bs_timer_t temp;
    if (node == BS_NULL)
        return BS_EOK;

    list = head_point;
    if (node == head_point) {
        head_point = head_point->next;    /* 如果要删除头指针，就将头指针后移  */
    } else {
        while (list != BS_NULL) { /*头节点如果不为空 */
            temp = list;     /* 记录当前节点 */
            list = list->next; /* 检索的是下一个节点  */
            if (list == BS_NULL) {
                return BS_EOK;
            }
            if (list == node) {
                temp->next = list->next; /* 删除此节点 */
                return BS_EOK;
            }
        }
    }
    return (-BS_ERROR);
}



/*
 * 创建一个软件定时器
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
bs_err_t bs_timer_init(bs_timer_t timer,
                       bs_uint32_t time,
                       void (*timeout)(void *parameter),
                       void       *parameter,
                       bs_uint8_t  flags)
{
    BS_ASSERT(timer != BS_NULL);
    BS_ASSERT(timeout != BS_NULL);
    BS_ASSERT(time > 0);

    bs_hw_interrupt_disable();
    timer->counter = 0;
    timer->duration = time;
    timer->timeout = timeout;
    timer->parameter = parameter;
    timer->flags = flags;
    head_point = creat_node(timer);
    bs_hw_interrupt_enable();
    return BS_EOK;
}


bs_err_t bs_timer_control(bs_timer_t timer, bs_uint8_t cmd, void *arg)
{
    BS_ASSERT(timer != BS_NULL);

    bs_hw_interrupt_disable();
    switch (cmd) {
    case BS_TIMER_CTRL_START:
        timer->counter = 0;
        timer->flags |= BS_TIMER_FLAG_START;
        break;

    case BS_TIMER_CTRL_STOP:
        timer->flags &= ~BS_TIMER_FLAG_START;
        timer->flags &= ~BS_TIMER_FLAG_LOOP_START;
        timer->counter = 0;
        break;

    case BS_TIMER_CTRL_SET_TIME:
        if (arg != BS_NULL)
            timer->duration = *(bs_uint32_t *)arg;
        timer->counter = 0;
        timer->flags |= BS_TIMER_FLAG_START;
        break;

    case BS_TIMER_CTRL_GET_TIME:
        if (arg != BS_NULL)
            *(bs_uint32_t *)arg = timer->duration;
        break;

    case BS_TIMER_CTRL_RESTART:
        if (arg != BS_NULL)
            timer->duration = *(bs_uint32_t *)arg;
        timer->flags |= BS_TIMER_FLAG_START;
        timer->counter = 0;
        creat_node(timer);
        break;

    case BS_TIMER_CTRL_DELETE:
        timer->flags &= ~BS_TIMER_FLAG_START;
        timer->counter = 0;
        delete_node(timer);
        break;
    }
    bs_hw_interrupt_enable();
    return BS_EOK;
}

/**
 * 系统main循环进程，用于执行轮询模式的回调函数
 */
void soft_timer_main_loop(void)
{
    bs_timer_t list = head_point;

    while (list != BS_NULL) { //下一个节点如果不为空
        if (list->flags & BS_TIMER_FLAG_LOOP_START) {
            list->flags &= ~BS_TIMER_FLAG_LOOP_START;
            if (list->timeout)
                list->timeout(list->parameter);
            if (!(list->flags & BS_TIMER_FLAG_START))
                delete_node(list);   /* 如果定时器被删除就删除节点 */
        }
        /*  寻找下一个有意义的节点  */
        list = list->next;
    }
}

/**
 * 此函数为tick中断服务函数，需要挂载在外部硬件定时器上
 * 因此软件定时器的定时精度由此函数挂载的硬件定时时间决定，
 * 比如此函数挂载在定时50ms的外部定时器上，那么定时dutation
 * 为20时定时时间就是20*50ms=1S
 */
void soft_timer_isr(void)
{
    bs_timer_t list = head_point;

    while (list != BS_NULL) { //下一个节点如果不为空
        if (list->flags & BS_TIMER_FLAG_START) { /* 判断当前定时器是否被开启  */
            if (++list->counter >= list->duration) { /* 判断当前计时有没有到达 */
                if (list->flags & BS_TIMER_FLAG_ONE_SHOT) {
                    list->flags &= ~BS_TIMER_FLAG_START;
                    list->flags |= BS_TIMER_FLAG_LOOP_START;
                }
                if (list->flags & BS_TIMER_FLAG_PERIODIC) {
                    list->flags |= BS_TIMER_FLAG_LOOP_START;
                }
                if (list->flags & BS_TIMER_FLAG_INTERRUPT_MODE) {
                    if (list->timeout)
                        list->timeout(list->parameter);   /* 中断内直接运行回调函数，用于实时性比较高的程序 */
                    if (!(list->flags & BS_TIMER_FLAG_START))
                        delete_node(list);
                }
                list->counter = 0;
            }
        }
        /*  寻找下一个有意义的节点  */
        list = list->next;
    }
}
