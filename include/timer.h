/*
 * Copyright (c) 2006-2018, bare-system Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 *
 */

#ifndef __SOFT_TIMER_LIST_H
#define __SOFT_TIMER_LIST_H

#include <bare_sys.h>


/**
 * timer config
 * MCU下长睡眠参考功耗：1.1uA
 * 仅MCU下的系统功耗：
 * 100 tick/s   :    7uA
 * 50 tick/s   :     4.8uA
 * 20 tick/s   :     2.5uA
 * 10 tick/s   :     2.0uA
 * 5 tick/s   :      1.6uA
 * 1 tick/s   :      1.3uA
 */
#define TIMER_TICK_PER_SECOND                10U    //每秒tick数(t=us)
#if (TIMER_TICK_PER_SECOND >= 10)
    #define TIMER_100MS_DELAY                    (TIMER_TICK_PER_SECOND/10)
#endif
#define TIMER_SEC_DELAY                      TIMER_TICK_PER_SECOND



/**
 * timer init macros
 */
#define BS_TIMER_FLAG_ONE_SHOT          0x1    /**< 单次计数，计数完毕后自动删除定时器 */
#define BS_TIMER_FLAG_PERIODIC          0x2    /**< 持续计数 */
/**
 * 定时超时后运行的回调函数可以选择在中断直接运行或者挂起任务轮询执行
 * 只要在定时需求准确的时候才建议选择中断模式执行，类似无磁传感器脉冲测量
 * 像一些超时判断类的应用以轮询的方式进行执行
 * 中断执行模式越多，其他定时器越不准,毕竟中断执行占时间，查询其他定时器时
 * 会有延时
 */
#define BS_TIMER_FLAG_INTERRUPT_MODE    0x4
/**
 * 添加此flag后可在init的同时启动定时器
 */
#define BS_TIMER_FLAG_START             0x8
/**
 * 此flag用户不必使用
 */
#define BS_TIMER_FLAG_LOOP_START        0x10


/**
 * timer ctrl macros
 */
#define BS_TIMER_CTRL_START             0x0       /**< start timer */
#define BS_TIMER_CTRL_STOP              0x1       /**< stop timer */
#define BS_TIMER_CTRL_SET_TIME          0x2       /**< 设置新的定时时长并重新开始计数 */
#define BS_TIMER_CTRL_GET_TIME          0x3       /**< 获取当前定时时长 */
#define BS_TIMER_CTRL_RESTART           0x4       /**< 重新启动定时器，如果定时器已从链表删除，则重新添加 */
#define BS_TIMER_CTRL_DELETE            0x5       /**< 停止定时器并从链表删除 */



/**
 * 软件定时器基本类型
 */
struct bs_timer {
    bs_uint32_t counter;                     /* 计数              */
    bs_uint32_t duration;                    /* 定时时长          */
    void (*timeout)(void *parameter);        /* 回调函数          */
    void *parameter;
    bs_uint8_t flags;
    struct bs_timer *next;
};

typedef struct bs_timer *bs_timer_t;



bs_err_t bs_timer_init(bs_timer_t timer,
                       bs_uint32_t time,
                       void (*timeout)(void *parameter),
                       void       *parameter,
                       bs_uint8_t  flags);
bs_err_t bs_timer_control(bs_timer_t timer, bs_uint8_t cmd, void *arg);
/**
 * 系统main循环进程，用于执行轮询模式的回调函数
 */
void soft_timer_main_loop(void);
/**
 * 此函数为tick中断服务函数，需要挂载在外部硬件定时器上
 * 因此软件定时器的定时精度由此函数挂载的硬件定时时间决定，
 * 比如此函数挂载在定时50ms的外部定时器上，那么定时dutation
 * 为20时定时时间就是20*50ms=1S
 */
void soft_timer_isr(void);



#endif /* !1__SOFT_TIMER_LIST_H */
