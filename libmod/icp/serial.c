/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 */

#include <serial.h>

static struct bs_serial_device _hw_serial;

/*
 * This function initializes serial device.
 */
static bs_err_t bs_serial_init(struct bs_device *dev)
{
    bs_err_t result = BS_EOK;
    struct bs_serial_device *serial;

    BS_ASSERT(dev != BS_NULL);
    serial = (struct bs_serial_device *)dev;

    /* initialize rx/tx */
    serial->serial_rx = BS_NULL;
    serial->serial_tx = BS_NULL;

    /* apply configuration */
    if (serial->ops->configure)
        result = serial->ops->configure(serial, &serial->config);

    return result;
}

static bs_err_t bs_serial_open(struct bs_device *dev, bs_uint16_t oflag)
{
    return BS_EOK;
}

static bs_err_t bs_serial_close(struct bs_device *dev)
{
    return BS_EOK;
}

static bs_size_t bs_serial_read(struct bs_device *dev,
                                bs_off_t          pos,
                                void             *buffer,
                                bs_size_t         size)
{
    return BS_EOK;
}

static bs_size_t bs_serial_write(struct bs_device *dev,
                                 bs_off_t          pos,
                                 const void       *buffer,
                                 bs_size_t         size)
{
    return BS_EOK;
}

static bs_err_t bs_serial_control(struct bs_device *dev,
                                  int              cmd,
                                  void             *args)
{
    return BS_EOK;
}


const static struct bs_device_ops serial_ops = 
{
    bs_serial_init,
    bs_serial_open,
    bs_serial_close,
    bs_serial_read,
    bs_serial_write,
    bs_serial_control
};

/*
 * serial register
 */
bs_err_t bs_hw_serial_register(struct bs_serial_device *serial,
                               const char              *name,
                               bs_uint32_t              flag,
                               void                    *data)
{
    bs_err_t ret;
    struct bs_device *device;
    BS_ASSERT(serial != BS_NULL);

    device = &(serial->parent);

    device->type        = BS_Device_Class_Char;
    device->rx_indicate = BS_NULL;
    device->tx_complete = BS_NULL;
    device->ops         = &serial_ops;
    device->user_data   = data;

    /* register a character device */
    ret = bs_device_register(device, name, flag);

#if defined(BS_USING_POSIX)
    /* set fops */
    device->fops        = &_serial_fops;
#endif

    return ret;
}

