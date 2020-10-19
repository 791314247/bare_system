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

static struct bs_serial_rx_fifo rx_fifo;

/*
 * Serial interrupt routines
 */
bs_inline int _serial_int_rx(struct bs_serial_device *serial, bs_uint8_t *data, int length)
{
    int size;

    BS_ASSERT(serial != BS_NULL);
    size = length;

    /* read from software FIFO */
    while (length) {
        int ch;

        /* disable interrupt */
        bs_hw_interrupt_disable();

        /* there's no data: */
        if ((rx_fifo.get_index == rx_fifo.put_index) && (rx_fifo.is_full == BS_FALSE)) {
            /* no data, enable interrupt and break out */
            bs_hw_interrupt_enable();
            break;
        }

        /* otherwise there's the data: */
        ch = rx_fifo.buffer[rx_fifo.get_index];
        rx_fifo.get_index += 1;
        if (rx_fifo.get_index >= serial->config.bufsz) rx_fifo.get_index = 0;

        if (rx_fifo.is_full == BS_TRUE) {
            rx_fifo.is_full = BS_FALSE;
        }

        /* enable interrupt */
        bs_hw_interrupt_enable();

        *data = ch & 0xff;
        data ++;
        length --;
    }

    return size - length;
}

/*
 * Serial poll routines
 */
bs_inline int _serial_poll_rx(struct bs_serial_device *serial, bs_uint8_t *data, int length)
{
    int ch;
    int size;

    BS_ASSERT(serial != BS_NULL);
    size = length;

    while (length) {
        ch = serial->ops->getc(serial);
        if (ch == -1) break;

        *data = ch;
        data ++;
        length --;

        if (ch == '\n') break;
    }

    return size - length;
}

bs_inline int _serial_int_tx(struct bs_serial_device *serial, const bs_uint8_t *data, int length)
{
    return 0;
}

bs_inline int _serial_poll_tx(struct bs_serial_device *serial, const bs_uint8_t *data, int length)
{
    int size;
    BS_ASSERT(serial != BS_NULL);

    size = length;
    while (length) {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
        if (*data == '\n' && (serial->parent.open_flag & BS_DEVICE_FLAG_STREAM)) {
            serial->ops->putc(serial, '\r');
        }
        serial->ops->putc(serial, *data);
        ++ data;
        -- length;
    }
    return size - length;
}

/*
 * This function initializes serial device.
 */
static bs_err_t bs_serial_init(struct bs_device *dev)
{
    bs_err_t result = BS_EOK;
    struct bs_serial_device *serial;

    BS_ASSERT(dev != BS_NULL);
    serial = (struct bs_serial_device *)dev;

    /* check initialize rx/tx */
    BS_ASSERT(serial->serial_rx != BS_NULL);
    BS_ASSERT(serial->serial_tx != BS_NULL);

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

    // bs_kprintf("open serial device: 0x%08x with open flag: 0x%04x",
    //         dev, oflag);
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
    if (rx_fifo.buffer != (bs_uint8_t *)serial->serial_rx) {
        if (oflag & BS_DEVICE_FLAG_INT_RX) {
            BS_ASSERT(serial->serial_rx != BS_NULL);
            rx_fifo.buffer = (bs_uint8_t *)(serial->serial_rx);
            bs_memset(rx_fifo.buffer, 0, serial->config.bufsz);
            rx_fifo.put_index = 0;
            rx_fifo.get_index = 0;
            rx_fifo.is_full = BS_FALSE;
            dev->open_flag |= BS_DEVICE_FLAG_INT_RX;
            /* configure low level device */
            serial->ops->control(serial, BS_DEVICE_CTRL_SET_INT, (void *)BS_DEVICE_FLAG_INT_RX);
        }
    }

    if (oflag & BS_DEVICE_FLAG_INT_TX) {
        dev->open_flag |= BS_DEVICE_FLAG_INT_TX;
        /* configure low level device */
        serial->ops->control(serial, BS_DEVICE_CTRL_SET_INT, (void *)BS_DEVICE_FLAG_INT_TX);
    }


    /* set stream flag */
    dev->open_flag |= stream_flag;
    return BS_EOK;
}

static bs_err_t bs_serial_close(struct bs_device *dev)
{
    // struct bs_serial_device *serial;
    // BS_ASSERT(dev != BS_NULL);
    // serial = (struct bs_serial_device *)dev;

    /* this device has more reference count */
    if (dev->ref_count > 1) return BS_EOK;

    if (dev->open_flag & BS_DEVICE_FLAG_INT_RX) {
    }
    if (dev->open_flag & BS_DEVICE_FLAG_INT_TX) {
    }
    return BS_EOK;
}

static bs_size_t bs_serial_read(struct bs_device *dev,
                                bs_off_t          pos,
                                void             *buffer,
                                bs_size_t         size)
{
    struct bs_serial_device *serial;

    BS_ASSERT(dev != BS_NULL);
    if (size == 0) return 0;

    serial = (struct bs_serial_device *)dev;

    if (dev->open_flag & BS_DEVICE_FLAG_INT_RX) {
        return _serial_int_rx(serial, (bs_uint8_t *)buffer, size);
    }
    return _serial_poll_rx(serial, (bs_uint8_t *)buffer, size);
}

static bs_size_t bs_serial_write(struct bs_device *dev,
                                 bs_off_t          pos,
                                 const void       *buffer,
                                 bs_size_t         size)
{
    struct bs_serial_device *serial;

    BS_ASSERT(dev != BS_NULL);
    if (size == 0) return 0;

    serial = (struct bs_serial_device *)dev;

    if (dev->open_flag & BS_DEVICE_FLAG_INT_TX) {
        return _serial_int_tx(serial, (const bs_uint8_t *)buffer, size);
    } else {
        return _serial_poll_tx(serial, (const bs_uint8_t *)buffer, size);
    }
}

static bs_err_t bs_serial_control(struct bs_device *dev,
                                  int              cmd,
                                  void             *args)
{
    return BS_EOK;
}


/* ISR for serial interrupt */
void bs_hw_serial_isr(struct bs_serial_device *serial, int event)
{
    BS_ASSERT(serial != BS_NULL);
    switch (event & 0xff) {
    case BS_SERIAL_EVENT_RX_IND: {
        int ch = -1;
        /* interrupt mode receive */
        while (1) {
            ch = serial->ops->getc(serial);
            if (ch == -1) break;

            rx_fifo.buffer[rx_fifo.put_index] = ch;
            rx_fifo.put_index += 1;
            if (rx_fifo.put_index >= serial->config.bufsz) rx_fifo.put_index = 0;

            /* if the next position is read index, discard this 'read char' */
            if (rx_fifo.put_index == rx_fifo.get_index) {
                rx_fifo.get_index += 1;
                rx_fifo.is_full = BS_TRUE;
                if (rx_fifo.get_index >= serial->config.bufsz) rx_fifo.get_index = 0;
                bs_kprintf("Warning: There is no enough buffer (%ld) for saving data,"
                        " please increase the BS_UARTx_RX_BUFSZ option.", serial->config.bufsz);
            }

            /* invoke callback */
            if (serial->parent.rx_indicate != BS_NULL) {
                bs_size_t rx_length;

                /* get rx length */
                rx_length = (rx_fifo.put_index >= rx_fifo.get_index) ? (rx_fifo.put_index - rx_fifo.get_index) :
                            (serial->config.bufsz - (rx_fifo.get_index - rx_fifo.put_index));

                if (rx_length) {
                    serial->parent.rx_indicate(&serial->parent, rx_length);
                }
            }
        }
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

