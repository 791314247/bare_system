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


#ifdef BS_DEBUG

void bs_hw_console_output(const char *str)
{
    bs_size_t i = 0, size = 0;

    size = bs_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
			LL_UART_TransmitData(BS_DEBUG_UART, 0X0D);
		    while(SET != LL_UART_IsActiveFlag_TXSE(BS_DEBUG_UART));
		}
		LL_UART_TransmitData(BS_DEBUG_UART, *(uint8_t *)(str + i));
	    while(SET != LL_UART_IsActiveFlag_TXSE(BS_DEBUG_UART));
    }
}


static void uartx_init_for_debug(UART_Type* UARTx)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_UART_InitTypeDef UART_InitStruct = {0};    
    
	switch((uint32_t)UARTx)
	{
		case UART0_BASE:
			GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14;
			GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
			GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			GPIO_InitStruct.Pull = DISABLE;
			GPIO_InitStruct.RemapPin = DISABLE;
			LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			UART_InitStruct.ClockSrc = LL_RCC_UART_OPERATION_CLOCK_SOURCE_APBCLK1;
			break;
		case UART1_BASE:
			GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14;
			GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
			GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			GPIO_InitStruct.Pull = DISABLE;
			GPIO_InitStruct.RemapPin = DISABLE;			
			LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			UART_InitStruct.ClockSrc = LL_RCC_UART_OPERATION_CLOCK_SOURCE_APBCLK1;
			break;
		case UART4_BASE:
			GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
			GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
			GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			GPIO_InitStruct.Pull = DISABLE;
			GPIO_InitStruct.RemapPin = DISABLE;			
			LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			break;
		case UART5_BASE:
			GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
			GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
			GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			GPIO_InitStruct.Pull = DISABLE;
			GPIO_InitStruct.RemapPin = DISABLE;			
			LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
			break;
				
		default:
			break;
    }
	UART_InitStruct.BaudRate = 115200;								//波特率
	UART_InitStruct.DataWidth = LL_UART_DATAWIDTH_8B;				//数据位数
	UART_InitStruct.StopBits = LL_UART_STOPBITS_1;					//停止位
	UART_InitStruct.Parity = LL_UART_PARITY_NONE;					//奇偶校验
	UART_InitStruct.TransferDirection = LL_UART_DIRECTION_TX;	//接收-发送使能
	UART_InitStruct.InfraredModulation = DISABLE;			        
	LL_UART_Init(UARTx, &UART_InitStruct);
}

int debug_init(void)
{
    uartx_init_for_debug(BS_DEBUG_UART);
    return 0;
}
INIT_BOARD_EXPORT(debug_init);


#endif
