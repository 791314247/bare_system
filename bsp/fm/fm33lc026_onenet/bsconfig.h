/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef _BS_CONFIG_H__
#define _BS_CONFIG_H__

/* BS-System Configuration */
#define BS_NAME_MAX                8
#define BS_DEBUG
#define BS_DEBUG_UART              UART0       /* using debug */



/* device selection */
// #define BS_USING_UART1                     /* using at base */
#define BS_USING_UART4                        /* using serial demo code */
#define BS_USING_WDT
#define BS_USING_RTC
#define BSP_USING_ADC0


/* components selection */
// #define BS_USING_AT_BASE
// #define BS_USING_AT_SERIAL         "uart1"
// #define BS_USING_BC35



/* demo selection */
#define BS_USING_DEMO_GPIO
#define BS_USING_DEMO_SERIAL
#define BS_USING_DEMO_RTC
//#define BS_USING_DEMO_ADC
//#define BS_USING_DEMO_BC35









#endif
