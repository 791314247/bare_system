/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 */

#include <pin.h>


static struct bs_pin_ops _hw_pin;


/* RT-Thread Hardware PIN APIs */
void bs_pin_mode(bs_base_t pin, bs_base_t mode)
{
    BS_ASSERT(_hw_pin.pin_mode != BS_NULL);
    if (_hw_pin.pin_mode) {
        _hw_pin.pin_mode(pin, mode);
    }
}

void bs_pin_write(bs_base_t pin, bs_base_t value)
{
    BS_ASSERT(_hw_pin.pin_write != BS_NULL);
    if (_hw_pin.pin_write) {
        _hw_pin.pin_write(pin, value);
    }
}

int  bs_pin_read(bs_base_t pin)
{
    BS_ASSERT(_hw_pin.pin_read != BS_NULL);
    if (_hw_pin.pin_read) {
        return _hw_pin.pin_read(pin);
    }
    return 0;
}

bs_err_t bs_pin_attach_irq(bs_int32_t pin, bs_uint32_t mode,
                           void (*hdr)(void *args), void *args)
{
    BS_ASSERT(_hw_pin.hdr != BS_NULL);
    BS_ASSERT(_hw_pin.pin_attach_irq != BS_NULL);
    if (_hw_pin.pin_attach_irq) {
        return _hw_pin.pin_attach_irq(pin, mode, hdr, args);
    }
    return BS_ENOSYS;
}

bs_err_t bs_pin_detach_irq(bs_int32_t pin)
{
    BS_ASSERT(_hw_pin.pin_detach_irq != BS_NULL);
    if (_hw_pin.pin_detach_irq) {
        return _hw_pin.pin_detach_irq(pin);
    }
    return BS_ENOSYS;
}

bs_err_t bs_pin_irq_enable(bs_base_t pin, bs_uint32_t enabled)
{
    BS_ASSERT(_hw_pin.pin_irq_enable != BS_NULL);
    if (_hw_pin.pin_irq_enable) {
        return _hw_pin.pin_irq_enable(pin, enabled);
    }
    return BS_ENOSYS;
}

void bs_device_pin_register(const struct bs_pin_ops *ops)
{
    /* register a character device */
    BS_ASSERT(ops != RT_NULL);
    _hw_pin.pin_mode = ops->pin_mode;
    _hw_pin.pin_read = ops->pin_read;
    _hw_pin.pin_write = ops->pin_write;
    _hw_pin.pin_attach_irq = ops->pin_attach_irq;
    _hw_pin.pin_detach_irq = ops->pin_detach_irq;
    _hw_pin.pin_irq_enable = ops->pin_irq_enable;
}

