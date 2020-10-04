/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-05-06     Zero-Free       first version
 */

#include <board.h>
#include <drv_lptim.h>


void LPTIM_IRQHandler(void)
{
    /* enter interrupt */

    if ((LL_LPTIM_IsEnabled_CounterOver(LPTIM) == ENABLE) &&
        (LL_LPTIM_IsActiveFlag_CounterOver(LPTIM) == SET)) {
        LL_LPTIM_ClearFlag_CounterOver(LPTIM);

        soft_timer_isr();
    }

    /* leave interrupt */
}


/**
 * This function initialize the lptim
 */
int hw_lptim_init()
{
    LL_LPTIM_TimeInitTypeDef  LPTIM_InitStruct = {0};

    LPTIM_InitStruct.PrescalerClockSource = LL_LPTIM_PRESCALER_INPUT_LPTIM_CLK;

    LPTIM_InitStruct.ClockSource = LL_RCC_LPTIM_OPERATION_CLOCK_SOURCE_LSCLK;

    LPTIM_InitStruct.Prescaler = LL_LPTIM_CLOCKDIVISION_DIV1;

    LPTIM_InitStruct.Mode = LL_LPTIM_OPERATION_MODE_NORMAL;

    LPTIM_InitStruct.OneState = LL_LPTIM_ONE_STATE_TIMER_CONTINUE;

    // LPTIM_InitStruct.Edge = LL_LPTIM_EXTERNAL_INPUT_TRIGGER_RINSING;
    // LPTIM_InitStruct.Polarity = LL_LPTIM_EXTERNAL_INPUT_POlARITY_RISING;

    LL_LPTIM_TimeModeInit(LPTIM, &LPTIM_InitStruct);

    LL_LPTIM_SetAutoReload(LPTIM,(uint32_t)(32768U/TIMER_TICK_PER_SECOND));

    LL_LPTIM_ClearFlag_CounterOver(LPTIM);

    LL_LPTIM_EnableIT_CounterOver(LPTIM);
    NVIC_DisableIRQ(LPTIM_IRQn);
    NVIC_SetPriority(LPTIM_IRQn, 2);
    NVIC_EnableIRQ(LPTIM_IRQn);

    LL_LPTIM_Enable(LPTIM);

    return 0;
}

INIT_DEVICE_EXPORT(hw_lptim_init);
