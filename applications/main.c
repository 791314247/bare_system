/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     balanceTWK   first version
 */

#include <board.h>
#include "main.h"
#include "sleep.h"
#include "bsdevice.h"


static SoftTimer led0;
static SoftTimer led4;

#define LED1_PIN    GET_PIN(C, 0)
#define LED4_PIN    GET_PIN(B, 9)



void DelayMs(uint32_t count)
{
    volatile unsigned long int i;
    for (i = (970 * count); i > 0; i--) {
    }
}


static void led1_timeout(void)
{
    if (bs_pin_read(LED1_PIN) == PIN_HIGH)
        bs_pin_write(LED1_PIN, PIN_LOW);
    else
        bs_pin_write(LED1_PIN, PIN_HIGH);
}

static void led4_timeout(void)
{
    if (bs_pin_read(LED4_PIN) == PIN_HIGH)
        bs_pin_write(LED4_PIN, PIN_LOW);
    else
        bs_pin_write(LED4_PIN, PIN_HIGH);
}


int main(void)
{
    /* set LED0 pin mode to output */
    bs_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    bs_pin_mode(LED4_PIN, PIN_MODE_OUTPUT);

    creat_continue_soft_timer(&led0, RUN_IN_LOOP_MODE,
                              TIMER_100MS_DELAY * 2, led1_timeout);
    creat_continue_soft_timer(&led4, RUN_IN_LOOP_MODE,
                              TIMER_100MS_DELAY * 1, led4_timeout);

    while (1) {
        soft_timer_main_loop();
        DeepSleep();
    }
}


/**
 4   * @brief  Reports th error has occurred.
 6   * @param  file: pointer to the source file name
 7   * @parame name of the source file and the source line number
 5   *         where the assert_param  line: assert_param error line source number
 8   * @retval None
 9   */
//void assert_failed(uint8_t *file, uint32_t line)
//{
//}

