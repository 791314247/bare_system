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
    rt_interrupt_enter();

    if ((LL_LPTIM_IsEnabled_CounterOver(LPTIM) == ENABLE) &&
        (LL_LPTIM_IsActiveFlag_CounterOver(LPTIM) == SET))
    {
        LL_LPTIM_ClearFlag_CounterOver(LPTIM);       //????     
    }	
    
    /* leave interrupt */
    rt_interrupt_leave();
}


/**
 * This function get current count value of LPTIM
 *
 * @return the count vlaue
 */
rt_uint32_t stm32l4_lptim_get_current_tick(void)
{
    return LL_LPTIM_GetAutoReload(LPTIM);
}

/**
 * This function get the max value that LPTIM can count
 *
 * @return the max count
 */
rt_uint32_t stm32l4_lptim_get_tick_max(void)
{
    return (0xFFFF);
}

/**
 * This function start LPTIM with reload value
 *
 * @param reload The value that LPTIM count down from
 *
 * @return RT_EOK
 */
rt_err_t stm32l4_lptim_start(rt_uint32_t reload)
{
    LL_LPTIM_SetAutoReload(LPTIM,32767);
    LL_LPTIM_Enable(LPTIM);

    return (RT_EOK);
}

/**
 * This function stop LPTIM
 */
void stm32l4_lptim_stop(void)
{
    LL_LPTIM_Disable(LPTIM);
}

/**
 * This function get the count clock of LPTIM
 *
 * @return the count clock frequency in Hz
 */
rt_uint32_t stm32l4_lptim_get_countfreq(void)
{
    return 32768;
}

/**
 * This function initialize the lptim
 */
int stm32l4_hw_lptim_init(void)
{
    LL_LPTIM_TimeInitTypeDef  LPTIM_InitStruct={0};
    
    LPTIM_InitStruct.PrescalerClockSource = LL_LPTIM_PRESCALER_INPUT_LPTIM_CLK;   
    
    LPTIM_InitStruct.ClockSource = LL_RCC_LPTIM_OPERATION_CLOCK_SOURCE_LSCLK; 

	LPTIM_InitStruct.Prescaler = LL_LPTIM_CLOCKDIVISION_DIV1;         

	LPTIM_InitStruct.Mode = LL_LPTIM_OPERATION_MODE_NORMAL;           

	LPTIM_InitStruct.OneState = LL_LPTIM_ONE_STATE_TIMER_CONTINUE;     

    // LPTIM_InitStruct.Edge = LL_LPTIM_EXTERNAL_INPUT_TRIGGER_RINSING;          
    // LPTIM_InitStruct.Polarity = LL_LPTIM_EXTERNAL_INPUT_POlARITY_RISING;   
   
    LL_LPTIM_TimeModeInit(LPTIM,&LPTIM_InitStruct);
    
    LL_LPTIM_SetAutoReload(LPTIM,32767);  
    
    LL_LPTIM_ClearFlag_CounterOver(LPTIM);  
    
    LL_LPTIM_EnableIT_CounterOver(LPTIM);  
    NVIC_DisableIRQ(LPTIM_IRQn);
    NVIC_SetPriority(LPTIM_IRQn,2);
    NVIC_EnableIRQ(LPTIM_IRQn); 
    
    LL_LPTIM_Disable(LPTIM); 

    return 0;
}

INIT_DEVICE_EXPORT(stm32l4_hw_lptim_init);
