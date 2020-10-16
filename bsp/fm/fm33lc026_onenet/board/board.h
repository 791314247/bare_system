/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <bare_sys.h>
#include "main.h"
#include "sleep.h"
#include "drv_common.h"
#include "drv_lptim.h"
#include "drv_gpio.h"



extern uint32_t systemClock;

// 8M: LL_RCC_RCHF_FREQUENCY_8MHZ
// 16M: LL_RCC_RCHF_FREQUENCY_16MHZ
// 24M: LL_RCC_RCHF_FREQUENCY_24MHZ
#define RCHF_CLOCK  LL_RCC_RCHF_FREQUENCY_8MHZ

void SystemClock_Config(void);
void FoutInit(void);









#endif /* __BOARD_H__ */
