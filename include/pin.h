/*
 * Copyright (c) 2006-2018, bs-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2017-10-20      ZYH          add mode open drain and input pull down
 */

#ifndef PIN_H__
#define PIN_H__

#include <bare_sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* pin device and operations for bs */
struct bs_pin_ops
{
    void (*pin_mode)(bs_base_t pin, bs_base_t mode);
    void (*pin_write)(bs_base_t pin, bs_base_t value);
    int (*pin_read)(bs_base_t pin);

    /* TODO: add GPIO interrupt */
    bs_err_t (*pin_attach_irq)( bs_int32_t pin,
                      bs_uint32_t mode, void (*hdr)(void *args), void *args);
    bs_err_t (*pin_detach_irq)( bs_int32_t pin);
    bs_err_t (*pin_irq_enable)( bs_base_t pin, bs_uint32_t enabled);
};

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1


void bs_device_pin_register(const struct bs_pin_ops *ops);

void bs_pin_mode(bs_base_t pin, bs_base_t mode);
void bs_pin_write(bs_base_t pin, bs_base_t value);
int  bs_pin_read(bs_base_t pin);
bs_err_t bs_pin_attach_irq(bs_int32_t pin, bs_uint32_t mode,
                             void (*hdr)(void *args), void  *args);
bs_err_t bs_pin_detach_irq(bs_int32_t pin);
bs_err_t bs_pin_irq_enable(bs_base_t pin, bs_uint32_t enabled);

#ifdef __cplusplus
}
#endif

#endif
