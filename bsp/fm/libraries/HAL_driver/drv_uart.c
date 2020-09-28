/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 */

#include <board.h>
#include <drv_uart.h>

static struct bs_serial_device serial;

// static const struct bs_uart_ops fm_uart_ops = {
//     .configure = BS_NULL,
//     .control = BS_NULL,
//     .putc = BS_NULL,
//     .getc = BS_NULL,
//     .dma_transmit = BS_NULL
// };


int bs_hw_usart_init(void)
{
    bs_err_t result = 0;

    /* init UART object */

    /* register UART device */
    result = bs_hw_serial_register(&serial, "name",
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , BS_NULL);
    BS_ASSERT(result == BS_EOK);
    return result;
}

INIT_DEVICE_EXPORT(bs_hw_usart_init);
