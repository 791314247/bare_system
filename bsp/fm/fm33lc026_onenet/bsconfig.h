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
#define BS_NAME_MAX 8
#define BS_DEBUG
#define BS_DEBUG_UART        UART5       /* using debug */



/* device selection */
#define BS_USING_SERIAL
#define BSP_USING_UART0               /* using debug */
#define BSP_USING_UART1               /* using at base */
#define BSP_USING_UART1               /* using serial demo code */
#define BS_USING_WDT


/* components selection */
//#define BS_USING_AT_BASE


/* demo selection */
//#define BS_USING_DEMO_GPIO
#define BS_USING_DEMO_SERIAL









#endif
