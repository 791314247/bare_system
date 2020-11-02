/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "adc.h"

/*
 * This function initializes serial
 */
static bs_err_t adc_init(struct bs_device *dev)
{
    struct bs_adc_device *adc;

    BS_ASSERT(dev != BS_NULL);
    adc = (struct bs_adc_device *)dev;
    if (adc->ops->init) {
        return (adc->ops->init(adc));
    }
    return (BS_EOK);
}

static bs_size_t adc_read(bs_device_t dev, bs_off_t pos, void *buffer, bs_size_t size)
{
    return 0;
}


static bs_err_t adc_control(bs_device_t dev, int cmd, void *args)
{
    bs_adc_device_t adc = (struct bs_adc_device *)dev;

    if (adc->ops->control == BS_NULL) {
        return -BS_ENOSYS;
    }
    return adc->ops->control(adc, cmd, args);;
}

const static struct bs_device_ops adc_ops = {
    adc_init,
    BS_NULL,
    BS_NULL,
    adc_read,
    BS_NULL,
    adc_control,
};

bs_err_t bs_hw_adc_register(bs_adc_device_t device, const char *name, const struct bs_adc_ops *ops, const void *user_data)
{
    bs_err_t result = BS_EOK;
    BS_ASSERT(ops != BS_NULL);

    device->parent.type = BS_Device_Class_Miscellaneous;
    device->parent.rx_indicate = BS_NULL;
    device->parent.tx_complete = BS_NULL;
    device->parent.ops         = &adc_ops;
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    result = bs_device_register(&device->parent, name, BS_DEVICE_FLAG_RDWR);

    return result;
}
