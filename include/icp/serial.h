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

    void *serial_rx;
    void *serial_tx;
};
typedef struct bs_serial_device bs_serial_t;

bs_err_t bs_hw_serial_register(struct bs_serial_device *serial,
                               const char              *name,
                               bs_uint32_t              flag,
                               void                    *data);




#ifdef __cplusplus
}
#endif

#endif
