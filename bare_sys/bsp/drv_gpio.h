/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     balanceTWK        first version
 * 2020-06-16     thread-liu        add stm32mp1
 */

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <drv_common.h>
#include <board.h>

#define __FM_PORT(port)  GPIO##port##_BASE
#define GET_PIN(PORTx,PIN) (bs_base_t)((((bs_base_t)__FM_PORT(PORTx) - (bs_base_t)GPIOA_BASE)/(0x04UL) ) + PIN)


#define __FM_PIN(index, gpio, gpio_index)             \
    {                                                 \
        index, GPIO##gpio, LL_GPIO_PIN_##gpio_index   \
    }

#define __FM_PIN_RESERVE \
    {                    \
        -1, 0, 0         \
    }

/* STM32 GPIO driver */
struct pin_index {
    int index;
    GPIO_Type *gpio;
    uint32_t pin;
};


#endif /* __DRV_GPIO_H__ */

