/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "adc.h"



static bs_size_t _adc_read(bs_device_t dev, bs_off_t pos, void *buffer, bs_size_t size)
{
    bs_err_t result = BS_EOK;
    bs_size_t i;
    struct bs_adc_device *adc = (struct bs_adc_device *)dev;
    bs_uint32_t *value = (bs_uint32_t *)buffer;

    for (i = 0; i < size; i += sizeof(int)) {
        result = adc->ops->convert(adc, pos + i, value);
        if (result != BS_EOK) {
            return 0;
        }
        value++;
    }

    return i;
}

static bs_err_t _adc_control(bs_device_t dev, int cmd, void *args)
{
    bs_err_t result = BS_EOK;
    bs_adc_device_t adc = (struct bs_adc_device *)dev;

    if (adc->ops->enabled == BS_NULL) {
        return -BS_ENOSYS;
    }
    if (cmd == BS_ADC_CMD_ENABLE) {
        result = adc->ops->enabled(adc, (bs_uint32_t)args, BS_TRUE);
    } else if (cmd == BS_ADC_CMD_DISABLE) {
        result = adc->ops->enabled(adc, (bs_uint32_t)args, BS_FALSE);
    }

    return result;
}

const static struct bs_device_ops adc_ops = {
    BS_NULL,
    BS_NULL,
    BS_NULL,
    _adc_read,
    BS_NULL,
    _adc_control,
};




bs_uint32_t bs_adc_read(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_uint32_t value;

    BS_ASSERT(dev);

    dev->ops->convert(dev, channel, &value);

    return value;
}

bs_err_t bs_adc_enable(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_err_t result = BS_EOK;

    BS_ASSERT(dev);
    if (dev->ops->enabled != BS_NULL) {
        result = dev->ops->enabled(dev, channel, BS_TRUE);
    } else {
        result = -BS_ENOSYS;
    }

    return result;
}

bs_err_t bs_adc_disable(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_err_t result = BS_EOK;

    BS_ASSERT(dev);
    if (dev->ops->enabled != BS_NULL) {
        result = dev->ops->enabled(dev, channel, BS_FALSE);
    } else {
        result = -BS_ENOSYS;
    }

    return result;
}

bs_err_t bs_hw_adc_register(bs_adc_device_t device, const char *name, const struct bs_adc_ops *ops, const void *user_data)
{
    bs_err_t result = BS_EOK;
    BS_ASSERT(ops != BS_NULL && ops->convert != BS_NULL);

    device->parent.type = BS_Device_Class_Miscellaneous;
    device->parent.rx_indicate = BS_NULL;
    device->parent.tx_complete = BS_NULL;
    device->parent.ops         = &adc_ops;
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    result = bs_device_register(&device->parent, name, BS_DEVICE_FLAG_RDWR);

    return result;
}
