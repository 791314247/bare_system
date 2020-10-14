/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <bare_sys.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_19200                 19200
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_921600                921600
#define BAUD_RATE_2000000               2000000
#define BAUD_RATE_3000000               3000000

#define DATA_BITS_5                     5
#define DATA_BITS_6                     6
#define DATA_BITS_7                     7
#define DATA_BITS_8                     8
#define DATA_BITS_9                     9

#define STOP_BITS_1                     0
#define STOP_BITS_2                     1
#define STOP_BITS_3                     2
#define STOP_BITS_4                     3

#define PARITY_NONE                     0
#define PARITY_ODD                      1
#define PARITY_EVEN                     2

#define BIT_ORDER_LSB                   0
#define BIT_ORDER_MSB                   1

#define NRZ_NORMAL                      0       /* Non Return to Zero : normal mode */
#define NRZ_INVERTED                    1       /* Non Return to Zero : inverted mode */

#ifndef BS_SERIAL_RB_BUFSZ
#define BS_SERIAL_RB_BUFSZ              64
#endif

#define BS_SERIAL_EVENT_RX_IND          0x01    /* Rx indication */
#define BS_SERIAL_EVENT_TX_DONE         0x02    /* Tx complete   */
#define BS_SERIAL_EVENT_RX_DMADONE      0x03    /* Rx DMA transfer done */
#define BS_SERIAL_EVENT_TX_DMADONE      0x04    /* Tx DMA transfer done */
#define BS_SERIAL_EVENT_RX_TIMEOUT      0x05    /* Rx timeout    */

#define BS_SERIAL_DMA_RX                0x01
#define BS_SERIAL_DMA_TX                0x02

#define BS_SERIAL_RX_INT                0x01
#define BS_SERIAL_TX_INT                0x02

#define BS_SERIAL_ERR_OVERRUN           0x01
#define BS_SERIAL_ERR_FRAMING           0x02
#define BS_SERIAL_ERR_PARITY            0x03

#define BS_SERIAL_TX_DATAQUEUE_SIZE     2048
#define BS_SERIAL_TX_DATAQUEUE_LWM      30

/* Default config for serial_configure structure */
#define BS_SERIAL_CONFIG_DEFAULT           \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    BS_SERIAL_RB_BUFSZ, /* Buffer size */  \
    0                                      \
}

/*
 * Serial FIFO mode 
 */
struct bs_serial_rx_fifo
{
    /* software fifo */
    bs_uint8_t *buffer;

    bs_uint16_t put_index, get_index;

    bs_bool_t is_full;
};

struct serial_configure
{
    bs_uint32_t baud_rate;
    bs_uint32_t data_bits               :4;
    bs_uint32_t stop_bits               :2;
    bs_uint32_t parity                  :2;
    bs_uint32_t bit_order               :1;
    bs_uint32_t invert                  :1;
    bs_uint32_t bufsz                   :16;
    bs_uint32_t reserved                :6;
};

/* serial device and operations for bs */
struct bs_serial_device
{
    struct bs_device          parent;

    const struct bs_uart_ops *ops;
    struct serial_configure   config;

    char *serial_rx;
    char *serial_tx;
};
typedef struct bs_serial_device bs_serial_t;

/**
 * uart operators
 */
struct bs_uart_ops
{
    bs_err_t (*configure)(struct bs_serial_device *serial, struct serial_configure *cfg);
    bs_err_t (*control)(struct bs_serial_device *serial, int cmd, void *arg);

    int (*putc)(struct bs_serial_device *serial, char c);
    int (*getc)(struct bs_serial_device *serial);

    bs_size_t (*dma_transmit)(struct bs_serial_device *serial, bs_uint8_t *buf, bs_size_t size, int direction);
};

void bs_hw_serial_isr(struct bs_serial_device *serial, int event);

bs_err_t bs_hw_serial_register(struct bs_serial_device *serial,
                               const char              *name,
                               bs_uint32_t              flag,
                               void                    *data);


#ifdef __cplusplus
}
#endif

#endif
