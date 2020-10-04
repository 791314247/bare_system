/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 */

#ifndef  __DRV_UART_H__
#define  __DRV_UART_H__

#include <bare_sys.h>
#include <bsdevice.h>


/************************** uart0 config **************************************/
#define BS_UART0_BUFSZ             128

#if defined(BSP_USING_UART0)
#ifndef UART0_CONFIG
#define UART0_CONFIG                                                \
    {                                                               \
        .name = "uart0",                                            \
        .Instance = UART0,                                         \
        .irq_type = UART0_IRQn,                                    \
    }
#endif /* UART0_CONFIG */

/* Default config for serial_configure structure */
#define BS_UART0_CONFIG_DEFAULT            \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    BS_UART0_BUFSZ,   /* Buffer size */    \
    0                                      \
}
#endif /* BSP_USING_UART0 */


/* fm config class */
struct fm_uart_config
{
    const char *name;
    UART_Type *Instance;
    IRQn_Type irq_type;
};

/* fm uart dirver class */
struct fm_uart_class
{
    struct fm_uart_config *config;
    struct bs_serial_device serial;
};


#endif /* __DRV_PMTIMER_H__ */
