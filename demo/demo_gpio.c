/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     balanceTWK   first version
 */

#include <bare_sys.h>
#include "bsdevice.h"
#include "board.h"

#ifdef BS_USING_DEMO_GPIO

static struct bs_timer led1;
static struct bs_timer led2;
static struct bs_timer led3;
static struct bs_timer led4;


#define LED1_PIN    GET_PIN(C, 0)
#define LED2_PIN    GET_PIN(C, 1)
#define LED3_PIN    GET_PIN(B, 10)
#define LED4_PIN    GET_PIN(B, 9)


static void led1_timeout(void *args)
{
    if (bs_pin_read(LED1_PIN) == PIN_HIGH)
        bs_pin_write(LED1_PIN, PIN_LOW);
    else
        bs_pin_write(LED1_PIN, PIN_HIGH);
}

static void led2_timeout(void *args)
{
    if (bs_pin_read(LED2_PIN) == PIN_HIGH)
        bs_pin_write(LED2_PIN, PIN_LOW);
    else
        bs_pin_write(LED2_PIN, PIN_HIGH);
}

static void led3_timeout(void *args)
{
    if (bs_pin_read(LED3_PIN) == PIN_HIGH)
        bs_pin_write(LED3_PIN, PIN_LOW);
    else
        bs_pin_write(LED3_PIN, PIN_HIGH);
}

static void led4_timeout(void *args)
{
    if (bs_pin_read(LED4_PIN) == PIN_HIGH)
        bs_pin_write(LED4_PIN, PIN_LOW);
    else
        bs_pin_write(LED4_PIN, PIN_HIGH);
}


int gpio_demo(void)
{
    /* set LED0 pin mode to output */
    bs_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    bs_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    bs_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    bs_pin_mode(LED4_PIN, PIN_MODE_OUTPUT);

    bs_timer_init(&led1, TIMER_100MS_DELAY * 2, led1_timeout, BS_NULL,
                  BS_TIMER_FLAG_PERIODIC | BS_TIMER_FLAG_START);
    bs_timer_init(&led2, TIMER_100MS_DELAY * 3, led2_timeout, BS_NULL,
                  BS_TIMER_FLAG_PERIODIC | BS_TIMER_FLAG_START);
    bs_timer_init(&led3, TIMER_100MS_DELAY * 4, led3_timeout, BS_NULL,
                  BS_TIMER_FLAG_PERIODIC | BS_TIMER_FLAG_START);
    bs_timer_init(&led4, TIMER_100MS_DELAY * 5, led4_timeout, BS_NULL,
                  BS_TIMER_FLAG_PERIODIC | BS_TIMER_FLAG_START);

    bs_kprintf("Gpio demo opened !");
    return 0;
}

INIT_APP_EXPORT(gpio_demo);
#endif
