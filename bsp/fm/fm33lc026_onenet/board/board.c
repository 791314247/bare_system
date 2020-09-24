/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-21     zylx         first version
 */

#include "board.h"

#define NVR_CHECK(_N_VALUE_, _T_VALUE_)                         \
                            ((((_N_VALUE_ >> 16) & 0xffff) ==   \
                            ((~_N_VALUE_) & 0xffff)) ? _N_VALUE_ : _T_VALUE_)

#define RCHF8M_DEF_TRIM     (0x30)      // RC8M 经验校准值
#define RCHF16M_DEF_TRIM    (0x2A)      // RC16M 经验校准值
#define RCHF24M_DEF_TRIM    (0x27)      // RC24M 经验校准值

#define RCHF8M_NVR_TRIM     (*(uint32_t *)0x1FFFFB40)   // RC8M 常温校准值
#define RCHF16M_NVR_TRIM    (*(uint32_t *)0x1FFFFB3C)   // RC16M 常温校准值
#define RCHF24M_NVR_TRIM    (*(uint32_t *)0x1FFFFB38)   // RC24M 常温校准值

uint32_t systemClock;

void FoutInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = DISABLE;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    LL_GPIO_SetFout0(GPIO_COMMON, LL_GPIO_FOUT0_SELECT_AHBCLK_DIV64);
}

void SystemClock_Config(void)
{
    uint32_t trim;
    switch (RCHF_CLOCK) {
    case LL_RCC_RCHF_FREQUENCY_8MHZ:
        systemClock = 8000000;
        LL_RCC_SetRCHFFrequency(RCHF_CLOCK);
        trim = NVR_CHECK(RCHF8M_NVR_TRIM, RCHF8M_DEF_TRIM) & 0x7f;
        LL_RCC_SetRCHFTrimValue(trim);
        break;

    case LL_RCC_RCHF_FREQUENCY_16MHZ:
        systemClock = 16000000;
        LL_RCC_SetRCHFFrequency(RCHF_CLOCK);
        trim = NVR_CHECK(RCHF16M_NVR_TRIM, RCHF16M_DEF_TRIM) & 0x7f;
        LL_RCC_SetRCHFTrimValue(trim);
        break;

    case LL_RCC_RCHF_FREQUENCY_24MHZ:
        systemClock = 24000000;
        LL_RCC_SetRCHFFrequency(RCHF_CLOCK);
        trim = NVR_CHECK(RCHF24M_NVR_TRIM, RCHF24M_DEF_TRIM) & 0x7f;
        LL_RCC_SetRCHFTrimValue(trim);
        break;

    default:
        systemClock = 8000000;
        LL_RCC_SetRCHFFrequency(RCHF_CLOCK);
        trim = NVR_CHECK(RCHF8M_NVR_TRIM, RCHF8M_DEF_TRIM) & 0x7f;
        LL_RCC_SetRCHFTrimValue(trim);
        break;
    }

    LL_RCC_SetSystemClockSource(LL_RCC_SYSTEM_CLKSOURCE_RCHF);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
}
