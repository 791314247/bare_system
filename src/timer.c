/**
 * sfor_timer_list.c
 * 链表实现的软件定时器库，需要用户的低功耗定时器做支持
 */

#include "timer.h"

/**
 * 软件定时器内部变量
 */
static SoftTimer *head_point = BS_NULL;

static struct SoftTimer *creat_node(SoftTimer *node);
static char delete_node(SoftTimer *node);
static bs_bool_t is_node_already_creat(SoftTimer *node);


/**
 * 系统main循环进程，用于执行轮询模式的回调函数
 */
void soft_timer_main_loop(void)
{
    struct SoftTimer *list = head_point;

    while (list != BS_NULL) { //下一个节点如果不为空
        if (list->loop_flag == BS_TRUE) {
            list->loop_flag = BS_FALSE;
            if (list->callback_function)
                list->callback_function(list->args);
            if (list->start_flag != BS_TRUE)
                delete_node(list);   /* 如果定时器被删除就删除节点 */
        }
        /*  寻找下一个有意义的节点  */
        list = list->next;
    }
}


/*
 * 创建一个只运行一次的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
void creat_single_soft_timer(SoftTimer *p,
                             TimerRunModeType mode,
                             unsigned long duration,
                             void(*timeout_handler)(void *args),
                             void *args)
{
    BS_ASSERT(p != BS_NULL);
    BS_ASSERT(timeout_handler != BS_NULL);
    BS_ASSERT(duration > 0);

    p->start_flag = BS_TRUE;
    p->counter = 0;
    p->loop_flag = BS_FALSE;
    p->duration = duration;
    if (mode == RUN_IN_LOOP_MODE)
        p->run_mode = RUN_IN_LOOP_MODE;
    else
        p->run_mode = RUN_IN_INTERRUPT_MODE;
    p->callback_function = timeout_handler;
    p->args = args;
    p->timing_mode = ONCE_MODE;

#ifdef ENABLE_CUSTOM_RUN_NUM
    p->run_num = 0; /* 只有在自定义运行次数的情况下此值才有效 */
#endif
    head_point = creat_node(p);
}


/*
 * 创建永远运行的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行(除非实在必要)还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
void creat_continue_soft_timer(SoftTimer *p,
                               TimerRunModeType mode,
                               unsigned long duration,
                               void(*timeout_handler)(void *args),
                               void *args)
{
    BS_ASSERT(p != BS_NULL);
    BS_ASSERT(timeout_handler != BS_NULL);
    BS_ASSERT(duration > 0);

    p->start_flag = BS_TRUE;
    p->counter = 0;
    p->loop_flag = BS_FALSE;
    p->duration = duration;
    if (mode == RUN_IN_LOOP_MODE)
        p->run_mode = RUN_IN_LOOP_MODE;
    else
        p->run_mode = RUN_IN_INTERRUPT_MODE;
    p->callback_function = timeout_handler;
    p->args = args;
    p->timing_mode = CONTINUE_MODE;

#ifdef ENABLE_CUSTOM_RUN_NUM
    p->run_num = 0;       /* 只有在自定义运行次数的情况下此值才有效 */
#endif
    head_point = creat_node(p);
}



/*
 * 创建指定次数运行的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        run_num：要定时的次数，比如1就是定时1次，5就是定时5次以后自动关闭定时器
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */

#ifdef ENABLE_CUSTOM_RUN_NUM
void creat_limit_num_soft_timer(SoftTimer *p,
                                TimerRunModeType mode,
                                unsigned long run_num,
                                unsigned long duration,
                                void(*timeout_handler)(void *args),
                                void args)
{
    BS_ASSERT(p != BS_NULL);
    BS_ASSERT(timeout_handler != BS_NULL);
    BS_ASSERT(duration > 0);

    p->start_flag = BS_TRUE;
    p->counter = 0;
    p->loop_flag = BS_FALSE;
    p->duration = duration;
    if (mode == RUN_IN_LOOP_MODE)
        p->run_mode = RUN_IN_LOOP_MODE;
    else
        p->run_mode = RUN_IN_INTERRUPT_MODE;
    p->callback_function = timeout_handler;
    p->args = args;
    p->timing_mode = CUSTOM_NUM_MODE;
    p->run_num = run_num;       /* 只有在自定义运行次数的情况下此值才有效 */
    head_point = creat_node(p);
}
#endif


/*
 * 重启指定的单次软件定时器
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
void restart_single_soft_timer(SoftTimer *p,
                               TimerRunModeType mode,
                               unsigned long duration,
                               void(*timeout_handler)(void *args),
                               void *args)
{
    BS_ASSERT(p != BS_NULL);
    BS_ASSERT(timeout_handler != BS_NULL);
    BS_ASSERT(duration > 0);

    p->start_flag = BS_TRUE;
    p->counter = 0;
    p->loop_flag = BS_FALSE;
    p->duration = duration;
    if (mode == RUN_IN_LOOP_MODE)
        p->run_mode = RUN_IN_LOOP_MODE;
    else
        p->run_mode = RUN_IN_INTERRUPT_MODE;
    p->callback_function = timeout_handler;
    p->args = args;
    p->timing_mode = ONCE_MODE;

#ifdef ENABLE_CUSTOM_RUN_NUM
    p->run_num = 0; /* 只有在自定义运行次数的情况下此值才有效 */
#endif
    if (is_node_already_creat(p) != BS_TRUE) /* 若之前的节点已被删除就重新创建 */
        head_point = creat_node(p);
}



/*
 * 重设指定定时器的计数值
 * 参数表：p: 定时器结构体指针，由用户创建
 *        duration: 要计时的时长，单位为硬件中断的tick
 * return：无
 */
void soft_timer_reset_interval(SoftTimer *p, unsigned long duration)
{
    BS_ASSERT(p != BS_NULL);
    BS_ASSERT(duration > 0);

    p->counter = 0;
    p->duration = duration;
}


/**
 * 封装后给用户使用
 */
void stop_timer(SoftTimer *p)
{
    BS_ASSERT(p != BS_NULL);

    if (p != BS_NULL) {
        p->counter = 0;
        p->start_flag = BS_FALSE;
        delete_node(p);
    }
}


static struct SoftTimer *creat_node(SoftTimer *node)
{
    struct SoftTimer *list;   //list保存当前需要检查的节点的地址
    if (node == BS_NULL)
        return head_point;

    if (is_node_already_creat(node) != BS_FALSE) {
        delete_node(node);  /* 当节点已经存在的时候在这里选择退出还是删除后重新创建，目前重新创建 */
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


static char delete_node(SoftTimer *node)
{
    struct SoftTimer *list;   //list保存当前需要检查的节点的地址
    struct SoftTimer *temp;
    if (node == BS_NULL)
        return 1;

    list = head_point;
    if (node == head_point) {
        head_point = head_point->next;    /* 如果要删除头指针，就将头指针后移  */
    } else {
        while (list != BS_NULL) { /*头节点如果不为空 */
            temp = list;     /* 记录当前节点 */
            list = list->next; /* 检索的是下一个节点  */
            if (list == BS_NULL) {
                return 1;
            }
            if (list == node) {
                temp->next = list->next; /* 删除此节点 */
                return 0;
            }
        }
    }
    return 0;
}


static bs_bool_t is_node_already_creat(SoftTimer *node)
{
    struct SoftTimer *list;   //list保存当前需要检查的节点的地址
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


/**
 * 此函数为tick中断服务函数，需要挂载在外部硬件定时器上
 * 因此软件定时器的定时精度由此函数挂载的硬件定时时间决定，
 * 比如此函数挂载在定时50ms的外部定时器上，那么定时dutation
 * 为20时定时时间就是20*50ms=1S
 */
void soft_timer_isr(void)
{
    struct SoftTimer *list = head_point;

    while (list != BS_NULL) { //下一个节点如果不为空
        if (list->start_flag != BS_FALSE) { /* 判断当前定时器是否被开启  */
            if (++list->counter >= list->duration) { /* 判断当前计时有没有到达 */
                switch (list->timing_mode) {
                case ONCE_MODE:
                    list->start_flag = BS_FALSE;
                    break;
                case CONTINUE_MODE:
                    break;
#ifdef ENABLE_CUSTOM_RUN_NUM
                case CUSTOM_NUM_MODE:
                    if (list->run_num > 0) {
                        if (--list->run_num == 0) {
                            list->start_flag = BS_FALSE;
                        }
                    }
#endif
                default:
                    break;
                }
                if (list->run_mode == RUN_IN_INTERRUPT_MODE) {
                    if (list->callback_function)
                        list->callback_function(list->args);   /* 中断内直接运行回调函数，用于实时性比较高的程序 */
                    if (list->start_flag != BS_TRUE)
                        delete_node(list);
                } else
                    list->loop_flag = BS_TRUE;
                list->counter = 0;
            }
        }
        /*  寻找下一个有意义的节点  */
        list = list->next;
    }
}
