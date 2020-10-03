/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <bare_sys.h>

#define device_init     (dev->ops->init)
#define device_open     (dev->ops->open)
#define device_close    (dev->ops->close)
#define device_read     (dev->ops->read)
#define device_write    (dev->ops->write)
#define device_control  (dev->ops->control)

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

    if (bs_device_find(name) != BS_NULL) {
        kprintf("To register device:%s failed, it was already registered !",
                dev->name);
        return -BS_ERROR;
    }

    bs_object_init(&(dev->parent), name);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

    return BS_EOK;
}

/**
 * This function will open a device
 *
 * @param dev the pointer of device driver structure
 * @param oflag the flags for device open
 *
 * @return the result
 */
bs_err_t bs_device_open(bs_device_t dev, bs_uint16_t oflag)
{
    bs_err_t result = BS_EOK;
    BS_ASSERT(dev != BS_NULL);

    /* if device is not initialized, initialize it. */
    if (!(dev->flag & BS_DEVICE_FLAG_ACTIVATED)) {
        if (device_init != BS_NULL) {
            result = device_init(dev);
            if (result != BS_EOK) {
                kprintf("To initialize device:%s failed. The error code is %d\n",
                        dev->parent.name, result);
                return result;
            }
        }
        dev->flag |= BS_DEVICE_FLAG_ACTIVATED;
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & BS_DEVICE_FLAG_STANDALONE) &&
        (dev->open_flag & BS_DEVICE_OFLAG_OPEN)) {
        return -BS_EBUSY;
    }

    /* call device_open interface */
    if (device_open != BS_NULL) {
        result = device_open(dev, oflag);
    } else {
        /* set open flag */
        dev->open_flag = (oflag & BS_DEVICE_OFLAG_MASK);
    }

    /* set open flag */
    if (result == BS_EOK || result == -BS_ENOSYS) {
        dev->open_flag |= BS_DEVICE_OFLAG_OPEN;

        dev->ref_count++;
        /* don't let bad things happen silently. If you are bitten by this assert,
         * please set the ref_count to a bigger type. */
        BS_ASSERT(dev->ref_count != 0);
    }

    return result;
}

/**
 * This function will close a device
 *
 * @param dev the pointer of device driver structure
 *
 * @return the result
 */
bs_err_t bs_device_close(bs_device_t dev)
{
    bs_err_t result = BS_EOK;
    BS_ASSERT(dev != BS_NULL);

    if (dev->ref_count == 0)
        return BS_ERROR;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return BS_EOK;

    /* call device_close interface */
    if (device_close == BS_NULL) {
        kprintf("To close device:%s failed, Function is empty !", dev->name);
        return BS_EEMPTY;
    }

    result = device_close(dev);
    /* set open flag */
    if (result == BS_EOK || result == -BS_ENOSYS)
        dev->open_flag = BS_DEVICE_OFLAG_CLOSE;

    return result;
}

/**
 * This function will read some data from a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 * @return the actually read size on successful, otherwise negative returned.
 */
bs_size_t bs_device_read(bs_device_t dev,
                         bs_off_t    pos,
                         void       *buffer,
                         bs_size_t   size)
{
    BS_ASSERT(dev != BS_NULL);
    BS_ASSERT(buffer != BS_NULL);

    if (dev->ref_count == 0) {
        kprintf("To read device:%s failed, It's not open !", dev->name);
        return BS_ERROR;
    }

    /* call device_read interface */
    if (device_read == BS_NULL) {
        kprintf("To read device:%s failed, Function is empty !", dev->name);
        return BS_EEMPTY;
    }

    return device_read(dev, pos, buffer, size);
}

/**
 * This function will write some data to a device.
 *
 * @param dev the pointer of device driver structure
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 * @return the actually written size on successful, otherwise negative returned.
 */
bs_size_t bs_device_write(bs_device_t dev,
                          bs_off_t    pos,
                          const void *buffer,
                          bs_size_t   size)
{
    BS_ASSERT(dev != BS_NULL);
    BS_ASSERT(buffer != BS_NULL);

    if (dev->ref_count == 0) {
        kprintf("To write device:%s failed, It's not open !", dev->name);
        return 0;
    }

    /* call device_write interface */
    if (device_write == BS_NULL) {
        kprintf("To write device:%s failed, Function is empty !", dev->name);
        return BS_EEMPTY;
    }

    return device_write(dev, pos, buffer, size);
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param dev the pointer of device driver structure
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
bs_err_t bs_device_control(bs_device_t dev, int cmd, void *arg)
{
    BS_ASSERT(dev != BS_NULL);
    BS_ASSERT(arg != BS_NULL);

    /* call device_write interface */
    if (device_control != BS_NULL) {
        return device_control(dev, cmd, arg);
    }

    return 0;
}

/**
 * This function will set the reception indication callback function. This callback function
 * is invoked when this device receives data.
 *
 * @param dev the pointer of device driver structure
 * @param rx_ind the indication callback function
 *
 * @return RT_EOK
 */
bs_err_t
bs_device_set_rx_indicate(bs_device_t dev,
                          bs_err_t (*rx_ind)(bs_device_t dev, bs_size_t size))
{
    BS_ASSERT(dev != BS_NULL);
    dev->rx_indicate = rx_ind;

    return BS_EOK;
}








