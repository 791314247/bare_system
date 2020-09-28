/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <bare_sys.h>

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or BS_NULL on failure.
 */
bs_device_t bs_device_find(const char *name)
{
    BS_ASSERT(name != BS_NULL);
    /* try to find device object */
    return (struct bs_device *)bs_object_find_node_from_name(name);
}

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the capabilities flag of device
 *
 * @return the error code, BS_EOK on initialization successfully.
 */
bs_err_t bs_device_register(bs_device_t dev,
                            const char *name,
                            bs_uint16_t flags)
{
    if (dev == BS_NULL)
        return -BS_ERROR;

    if (bs_device_find(name) != BS_NULL)
        return -BS_ERROR;

    bs_object_init(&(dev->parent), name);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

    return BS_EOK;
}





