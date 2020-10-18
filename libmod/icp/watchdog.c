/*
 * COPYRIGHT (C) 2018
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "watchdog.h"

/* BS-Thread Device Interface */

/*
 * This function initializes serial
 */
static bs_err_t bs_watchdog_init(struct bs_device *dev)
{
    bs_watchdog_t *wtd;

    BS_ASSERT(dev != BS_NULL);
    wtd = (bs_watchdog_t *)dev;
    if (wtd->ops->init)
    {
        return (wtd->ops->init(wtd));
    }

    return (BS_EOK);
}

static bs_err_t bs_watchdog_open(struct bs_device *dev, bs_uint16_t oflag)
{
    return (BS_EOK);
}

static bs_err_t bs_watchdog_close(struct bs_device *dev)
{
    bs_watchdog_t *wtd;

    BS_ASSERT(dev != BS_NULL);
    wtd = (bs_watchdog_t *)dev;

    if (wtd->ops->control(wtd, BS_DEVICE_CTRL_WDT_STOP, BS_NULL) != BS_EOK)
    {
        bs_kprintf(" This watchdog can not be stoped\n");

        return (-BS_ERROR);
    }

    return (BS_EOK);
}

static bs_err_t bs_watchdog_control(struct bs_device *dev,
                                    int              cmd,
                                    void             *args)
{
    bs_watchdog_t *wtd;

    BS_ASSERT(dev != BS_NULL);
    wtd = (bs_watchdog_t *)dev;

    return (wtd->ops->control(wtd, cmd, args));
}


const static struct bs_device_ops wdt_ops = 
{
    bs_watchdog_init,
    bs_watchdog_open,
    bs_watchdog_close,
    BS_NULL,
    BS_NULL,
    bs_watchdog_control,
};


/**
 * This function register a watchdog device
 */
bs_err_t bs_hw_watchdog_register(struct bs_watchdog_device *wtd,
                                 const char                *name,
                                 bs_uint32_t                flag,
                                 void                      *data)
{
    struct bs_device *device;
    BS_ASSERT(wtd != BS_NULL);

    device = &(wtd->parent);

    device->type        = BS_Device_Class_Miscellaneous;
    device->rx_indicate = BS_NULL;
    device->tx_complete = BS_NULL;
    device->ops         = &wdt_ops;
    device->user_data   = data;

    /* register a character device */
    return bs_device_register(device, name, flag);
}
