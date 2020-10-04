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

static SoftTimer serial_rx_timer;

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

static void serial_rx_timeout(void * args)
{
    struct bs_serial_device *serial;
    serial = (struct bs_serial_device *)args;
    BS_ASSERT(serial != BS_NULL);

    if(serial->parent.rx_indicate)
        serial->parent.rx_indicate((bs_device_t)serial, serial->rx_index);
    serial->rx_index = 0;
}

/* ISR for serial interrupt */
void bs_hw_serial_isr(struct bs_serial_device *serial, int event)
{
    BS_ASSERT(serial != BS_NULL);
    switch (event & 0xff) {
    case BS_SERIAL_EVENT_RX_IND: {
        /* interrupt mode receive */
        if (serial->rx_index <= serial->config.bufsz)
            serial->serial_rx[serial->rx_index++] = serial->ops->getc(serial);
        creat_single_soft_timer(&serial_rx_timer, RUN_IN_LOOP_MODE,
                                TIMER_100MS_DELAY, serial_rx_timeout, (void*)serial);
    }
    //case BS_SERIAL_EVENT_TX_DONE:

    }

}


const static struct bs_device_ops serial_ops = {
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

