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
    serial->rx_index = 0;
    serial->tx_index = 0;

    /* apply configuration */
    if (serial->ops->configure)
        result = serial->ops->configure(serial, &serial->config);

    return result;
}

static bs_err_t bs_serial_open(struct bs_device *dev, bs_uint16_t oflag)
{
    bs_uint16_t stream_flag = 0;
    struct bs_serial_device *serial;

    BS_ASSERT(dev != BS_NULL);
    serial = (struct bs_serial_device *)dev;

    kprintf("open serial device: 0x%08x with open flag: 0x%04x",
            dev, oflag);
    /* check device flag with the open flag */
    if ((oflag & BS_DEVICE_FLAG_DMA_RX) && !(dev->flag & BS_DEVICE_FLAG_DMA_RX))
        return -BS_EIO;
    if ((oflag & BS_DEVICE_FLAG_DMA_TX) && !(dev->flag & BS_DEVICE_FLAG_DMA_TX))
        return -BS_EIO;
    if ((oflag & BS_DEVICE_FLAG_INT_RX) && !(dev->flag & BS_DEVICE_FLAG_INT_RX))
        return -BS_EIO;
    if ((oflag & BS_DEVICE_FLAG_INT_TX) && !(dev->flag & BS_DEVICE_FLAG_INT_TX))
        return -BS_EIO;

    /* keep steam flag */
    if ((oflag & BS_DEVICE_FLAG_STREAM) || (dev->open_flag & BS_DEVICE_FLAG_STREAM))
        stream_flag = BS_DEVICE_FLAG_STREAM;

    /* get open flags */
    dev->open_flag = oflag & 0xff;

    /* initialize the Rx/Tx structure according to open flag */
    if (oflag & BS_DEVICE_FLAG_INT_RX) {
        bs_memset(serial->serial_rx, 0, serial->config.bufsz);
        bs_memset(serial->serial_tx, 0, serial->config.bufsz);
        serial->rx_index = 0;
        serial->tx_index = 0;
        dev->open_flag |= BS_DEVICE_FLAG_INT_RX;
        /* configure low level device */
        serial->ops->control(serial, BS_DEVICE_CTRL_SET_INT, (void *)BS_DEVICE_FLAG_INT_RX);
    }

    /* set stream flag */
    dev->open_flag |= stream_flag;
    return BS_EOK;
}

static bs_err_t bs_serial_close(struct bs_device *dev)
{
    struct rt_serial_device *serial;
    RT_ASSERT(dev != RT_NULL);
    serial = (struct rt_serial_device *)dev;

    /* this device has more reference count */
    if (dev->ref_count > 1) return RT_EOK;

    if (dev->open_flag & RT_DEVICE_FLAG_INT_RX)
    {
        struct rt_serial_rx_fifo* rx_fifo;

        rx_fifo = (struct rt_serial_rx_fifo*)serial->serial_rx;
        RT_ASSERT(rx_fifo != RT_NULL);

        rt_free(rx_fifo);
        serial->serial_rx = RT_NULL;
        dev->open_flag &= ~RT_DEVICE_FLAG_INT_RX;
        /* configure low level device */
        serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void*)RT_DEVICE_FLAG_INT_RX);
    }
#ifdef RT_SERIAL_USING_DMA
    else if (dev->open_flag & RT_DEVICE_FLAG_DMA_RX)
    {
        if (serial->config.bufsz == 0) {
            struct rt_serial_rx_dma* rx_dma;

            rx_dma = (struct rt_serial_rx_dma*)serial->serial_rx;
            RT_ASSERT(rx_dma != RT_NULL);

            rt_free(rx_dma);
        } else {
            struct rt_serial_rx_fifo* rx_fifo;

            rx_fifo = (struct rt_serial_rx_fifo*)serial->serial_rx;
            RT_ASSERT(rx_fifo != RT_NULL);

            rt_free(rx_fifo);
        }
        /* configure low level device */
        serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *) RT_DEVICE_FLAG_DMA_RX);
        serial->serial_rx = RT_NULL;
        dev->open_flag &= ~RT_DEVICE_FLAG_DMA_RX;
    }
#endif /* RT_SERIAL_USING_DMA */
    
    if (dev->open_flag & RT_DEVICE_FLAG_INT_TX)
    {
        struct rt_serial_tx_fifo* tx_fifo;

        tx_fifo = (struct rt_serial_tx_fifo*)serial->serial_tx;
        RT_ASSERT(tx_fifo != RT_NULL);

        rt_free(tx_fifo);
        serial->serial_tx = RT_NULL;
        dev->open_flag &= ~RT_DEVICE_FLAG_INT_TX;
        /* configure low level device */
        serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void*)RT_DEVICE_FLAG_INT_TX);
    }
#ifdef RT_SERIAL_USING_DMA
    else if (dev->open_flag & RT_DEVICE_FLAG_DMA_TX)
    {
        struct rt_serial_tx_dma* tx_dma;

        tx_dma = (struct rt_serial_tx_dma*)serial->serial_tx;
        RT_ASSERT(tx_dma != RT_NULL);

        rt_free(tx_dma);
        serial->serial_tx = RT_NULL;
        dev->open_flag &= ~RT_DEVICE_FLAG_DMA_TX;
    }
#endif /* RT_SERIAL_USING_DMA */
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

static void serial_rx_timeout(void *args)
{
    struct bs_serial_device *serial;
    serial = (struct bs_serial_device *)args;
    BS_ASSERT(serial != BS_NULL);

    if (serial->parent.rx_indicate)
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
                                TIMER_100MS_DELAY, serial_rx_timeout, (void *)serial);
    }
    case BS_SERIAL_EVENT_TX_DONE: {

    }

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

