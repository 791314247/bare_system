/*
 * COPYRIGHT (C) 2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "rtc.h"

/* BS-Thread Device Interface */

/*
 * This function initializes serial
 */
static bs_err_t bs_rtc_init(struct bs_device *dev)
{
    bs_rtc_t *rtc;

    BS_ASSERT(dev != BS_NULL);
    rtc = (bs_rtc_t *)dev;
    if (rtc->ops->init) {
        return (rtc->ops->init(rtc));
    }

    return (BS_EOK);
}


static bs_err_t bs_rtc_control(struct bs_device *dev,
                               int              cmd,
                               void             *args)
{
    bs_rtc_t *rtc;

    BS_ASSERT(dev != BS_NULL);
    rtc = (bs_rtc_t *)dev;

    return (rtc->ops->control(rtc, cmd, args));
}


const static struct bs_device_ops rtc_ops = {
    bs_rtc_init,
    BS_NULL,
    BS_NULL,
    BS_NULL,
    BS_NULL,
    bs_rtc_control,
};


/**
 * This function register a rtc device
 */
bs_err_t bs_hw_rtc_register(struct bs_rtc_device *rtc,
                            const char                *name,
                            bs_uint32_t                flag,
                            void                      *data)
{
    struct bs_device *device;
    BS_ASSERT(rtc != BS_NULL);

    device = &(rtc->parent);

    device->type        = BS_Device_Class_Miscellaneous;
    device->rx_indicate = BS_NULL;
    device->tx_complete = BS_NULL;
    device->ops         = &rtc_ops;
    device->user_data   = data;

    /* register a character device */
    return bs_device_register(device, name, flag);
}
