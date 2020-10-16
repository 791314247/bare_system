/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BSDEBUG_H__
#define __BSDEBUG_H__

#include <bsconfig.h>


/* Using this macro to control all kernel debug features. */
#ifdef BS_DEBUG

/* Turn on some of these (set to non-zero) to debug kernel */
#ifndef BS_DEBUG_MEM
#define BS_DEBUG_MEM                   0
#endif

#ifndef BS_DEBUG_MEMHEAP
#define BS_DEBUG_MEMHEAP               0
#endif

#ifndef BS_DEBUG_MODULE
#define BS_DEBUG_MODULE                0
#endif

#ifndef BS_DEBUG_SCHEDULER
#define BS_DEBUG_SCHEDULER             0
#endif

#ifndef BS_DEBUG_SLAB
#define BS_DEBUG_SLAB                  0
#endif

#ifndef BS_DEBUG_THREAD
#define BS_DEBUG_THREAD                0
#endif

#ifndef BS_DEBUG_TIMER
#define BS_DEBUG_TIMER                 0
#endif

#ifndef BS_DEBUG_IRQ
#define BS_DEBUG_IRQ                   0
#endif

#ifndef BS_DEBUG_IPC
#define BS_DEBUG_IPC                   0
#endif

#ifndef BS_DEBUG_INIT
#define BS_DEBUG_INIT                  0
#endif

/* Turn on this to enable context check */
#ifndef BS_DEBUG_CONTEXT_CHECK
#define BS_DEBUG_CONTEXT_CHECK         1
#endif

#define BS_DEBUG_LOG(type, message)                                           \
do                                                                            \
{                                                                             \
    if (type)                                                                 \
        bs_kprintf message;                                                   \
}                                                                             \
while (0)

#define BS_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    bs_assert_handler(#EX, __FUNCTION__, __LINE__);                           \
}

/* Macro to check current context */
#if BS_DEBUG_CONTEXT_CHECK
#define BS_DEBUG_NOT_IN_INTERRUPT                                             \
do                                                                            \
{                                                                             \
    bs_base_t level;                                                          \
    level = bs_hw_interrupt_disable();                                        \
    if (bs_interrupt_get_nest() != 0)                                         \
    {                                                                         \
        bs_kprintf("Function[%s] shall not be used in ISR\n", __FUNCTION__);  \
        BS_ASSERT(0)                                                          \
    }                                                                         \
    bs_hw_interrupt_enable(level);                                            \
}                                                                             \
while (0)

/* "In thread context" means:
 *     1) the scheduler has been started
 *     2) not in interrupt context.
 */
#define BS_DEBUG_IN_THREAD_CONTEXT                                            \
do                                                                            \
{                                                                             \
    bs_base_t level;                                                          \
    level = bs_hw_interrupt_disable();                                        \
    if (bs_thread_self() == BS_NULL)                                          \
    {                                                                         \
        bs_kprintf("Function[%s] shall not be used before scheduler start\n", \
                   __FUNCTION__);                                             \
        BS_ASSERT(0)                                                          \
    }                                                                         \
    BS_DEBUG_NOT_IN_INTERRUPT;                                                \
    bs_hw_interrupt_enable(level);                                            \
}                                                                             \
while (0)
#else
#define BS_DEBUG_NOT_IN_INTERRUPT
#define BS_DEBUG_IN_THREAD_CONTEXT
#endif

#else /* BS_DEBUG */

#define bs_kprintf(...)
#define BS_ASSERT(EX)
#define BS_DEBUG_LOG(type, message)
#define BS_DEBUG_NOT_IN_INTERRUPT
#define BS_DEBUG_IN_THREAD_CONTEXT

#endif /* BS_DEBUG */

#endif /* __BSDEBUG_H__ */
