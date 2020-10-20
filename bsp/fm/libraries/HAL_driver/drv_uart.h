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
#if defined(BSP_USING_UART0)
#define BS_UART0_RX_BUFSZ         128
#define BS_UART0_TX_BUFSZ         1
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
    BS_UART0_RX_BUFSZ,  /* buf size */     \
    0                                      \
}
#endif /* BSP_USING_UART0 */

/************************** uart1 config **************************************/
#if defined(BSP_USING_UART1)
#define BS_UART1_RX_BUFSZ         128
#define BS_UART1_TX_BUFSZ         1
#ifndef UART1_CONFIG
#define UART1_CONFIG                                                \
    {                                                               \
        .name = "uart1",                                            \
        .Instance = UART1,                                         \
        .irq_type = UART1_IRQn,                                    \
    }
#endif /* UART1_CONFIG */

/* Default config for serial_configure structure */
#define BS_UART1_CONFIG_DEFAULT            \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    BS_UART1_RX_BUFSZ,  /* buf size */     \
    0                                      \
}
#endif /* BSP_USING_UART1 */

/************************** uart4 config **************************************/
#if defined(BSP_USING_UART4)
#define BS_UART4_RX_BUFSZ         128
#define BS_UART4_TX_BUFSZ         1
#ifndef UART4_CONFIG
#define UART4_CONFIG                                                \
    {                                                               \
        .name = "uart4",                                            \
        .Instance = UART4,                                         \
        .irq_type = UART4_IRQn,                                    \
    }
#endif /* UART4_CONFIG */

/* Default config for serial_configure structure */
#define BS_UART4_CONFIG_DEFAULT            \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    BS_UART1_RX_BUFSZ,  /* buf size */     \
    0                                      \
}
#endif /* BSP_USING_UART4 */



/************************** end **************************************/


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
