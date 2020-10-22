/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 */

#include <board.h>

/*  When you USING SERIAL, you must define a serial port*/
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1) || defined(BSP_USING_UART2) || \
    defined(BSP_USING_UART3) || defined(BSP_USING_UART4) || defined(BSP_USING_UART5) || \
    defined(BSP_USING_UART6) || defined(BSP_USING_UART7) || defined(BSP_USING_LPUART1)
#include <drv_uart.h>

#if defined(FM33LC0XX) /*The FM33LC0XX supported serial port is Uart0 Uart1 Uart4 Uart5 LUart1*/
    #if defined(BSP_USING_UART2) || defined(BSP_USING_UART3) || defined(BSP_USING_LPUART0)
        #error "FM33LC0XX nonsupport UART2 UART3 LUART0 !"
    #endif
#endif

#ifdef BSP_USING_UART0
    char uart0_rxbuf[BS_UART0_RX_BUFSZ];
    char uart0_txbuf[BS_UART0_TX_BUFSZ];
#endif
#ifdef BSP_USING_UART1
    char uart1_rxbuf[BS_UART1_RX_BUFSZ];
    char uart1_txbuf[BS_UART1_TX_BUFSZ];
#endif
#ifdef BSP_USING_UART2
    char uart2_rxbuf[BS_UART2_RX_BUFSZ];
    char uart2_txbuf[BS_UART2_TX_BUFSZ];
#endif
#ifdef BSP_USING_UART3
    char uart3_rxbuf[BS_UART3_RX_BUFSZ];
    char uart3_txbuf[BS_UART3_TX_BUFSZ];
#endif
#ifdef BSP_USING_UART4
    char uart4_rxbuf[BS_UART4_RX_BUFSZ];
    char uart4_txbuf[BS_UART4_TX_BUFSZ];
#endif
#ifdef BSP_USING_UART5
    char uart5_rxbuf[BS_UART5_RX_BUFSZ];
    char uart5_txbuf[BS_UART5_TX_BUFSZ];
#endif
#ifdef BSP_USING_LPUART1
    char lpuart1_rxbuf[BS_LPUART1_RX_BUFSZ];
    char lpuart1_txbuf[BS_LPUART1_TX_BUFSZ];
#endif

enum {
#ifdef BSP_USING_UART0
    UART0_INDEX,
#endif
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif
#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif
#ifdef BSP_USING_UART5
    UART5_INDEX,
#endif
#ifdef BSP_USING_LPUART1
    LPUART1_INDEX,
#endif
};

static struct serial_configure serial_default_config[] = {
#ifdef BSP_USING_UART0
    BS_UART0_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_UART1
    BS_UART1_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_UART2
    BS_UART2_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_UART3
    BS_UART3_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_UART4
    BS_UART4_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_UART5
    BS_UART5_CONFIG_DEFAULT,
#endif
#ifdef BSP_USING_LPUART1
    BS_LPUART1_CONFIG_DEFAULT,
#endif
};


static struct fm_uart_config uart_config[] = {
#ifdef BSP_USING_UART0
    UART0_CONFIG,
#endif
#ifdef BSP_USING_UART1
    UART1_CONFIG,
#endif
#ifdef BSP_USING_UART2
    UART2_CONFIG,
#endif
#ifdef BSP_USING_UART3
    UART3_CONFIG,
#endif
#ifdef BSP_USING_UART4
    UART4_CONFIG,
#endif
#ifdef BSP_USING_UART5
    UART5_CONFIG,
#endif
#ifdef BSP_USING_LPUART1
    LPUART1_CONFIG,
#endif
};

struct fm_uart_class uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static bs_err_t fm_configure(struct bs_serial_device *serial, struct serial_configure *cfg)
{
    LL_UART_InitTypeDef UART_InitStruct;
    struct fm_uart_config *uartx = BS_NULL;
    BS_ASSERT(serial != BS_NULL);
    BS_ASSERT(cfg != BS_NULL);

    uartx = (struct fm_uart_config *)serial->parent.user_data;
    UART_InitStruct.BaudRate = serial->config.baud_rate;

    switch (cfg->data_bits) {
    case DATA_BITS_7:
        UART_InitStruct.DataWidth = LL_UART_DATAWIDTH_7B;
        break;
    case DATA_BITS_8:
        UART_InitStruct.DataWidth = LL_UART_DATAWIDTH_8B;
        break;
    case DATA_BITS_9:
        UART_InitStruct.DataWidth = LL_UART_DATAWIDTH_9B;
        break;
    default:
        return BS_EINVAL;
    }

    switch (cfg->parity) {
    case PARITY_NONE:
        UART_InitStruct.Parity = LL_UART_PARITY_NONE;
        break;
    case PARITY_ODD:
        UART_InitStruct.Parity = LL_UART_PARITY_ODD;
        break;
    case PARITY_EVEN:
        UART_InitStruct.Parity = LL_UART_PARITY_EVEN;
        break;
    default:
        return BS_EINVAL;
    }

    switch (cfg->stop_bits) {
    case STOP_BITS_1:
        UART_InitStruct.StopBits = LL_UART_STOPBITS_1;
        break;
    case STOP_BITS_2:
        UART_InitStruct.StopBits = LL_UART_STOPBITS_2;
        break;
    default:
        return BS_EINVAL;
    }

    UART_InitStruct.ClockSrc = LL_RCC_UART_OPERATION_CLOCK_SOURCE_APBCLK1;
    UART_InitStruct.TransferDirection = LL_UART_DIRECTION_TX_RX;
    UART_InitStruct.InfraredModulation = DISABLE;
    LL_UART_Init(uartx->Instance, &UART_InitStruct);

    return BS_EOK;
}

static bs_err_t fm_control(struct bs_serial_device *serial, int cmd, void *arg)
{
    struct fm_uart_config *uart;
    unsigned long irq_type = (unsigned long)arg;

    BS_ASSERT(serial != BS_NULL);
    uart = (struct fm_uart_config *)serial->parent.user_data;
    BS_ASSERT(uart != BS_NULL);

    NVIC_DisableIRQ(uart->irq_type);
    NVIC_SetPriority(uart->irq_type, 2);
    NVIC_EnableIRQ(uart->irq_type);

    switch (cmd) {
    /* disable interrupt */
    case BS_DEVICE_CTRL_CLR_INT:
        if (irq_type == BS_DEVICE_FLAG_INT_RX) {
            /* disable rx irq */
            LL_UART_DisableIT_ReceiveBuffFull(uart->Instance);
        } else if (irq_type == BS_DEVICE_FLAG_INT_TX) {
            /* disable tx irq */
            LL_UART_DisableIT_TxBuffEmpty(uart->Instance);
        } else {
            //LOG_EXT_D("invalide irq type %d\r\n", (int)irq_type);
            return BS_ERROR;
        }
        break;
    /* enable interrupt */
    case BS_DEVICE_CTRL_SET_INT:
        /* Enable the uart interrupt in the NVIC */
        if (irq_type == BS_DEVICE_FLAG_INT_RX) {
            /* enable rx irq */
            LL_UART_EnableIT_ReceiveBuffFull(uart->Instance);
        } else if (irq_type == BS_DEVICE_FLAG_INT_TX) {
            /* enable tx irq */
            LL_UART_EnableIT_TxBuffEmpty(uart->Instance);
        } else {
            //LOG_EXT_D("invalide irq type %d\r\n", (int)irq_type);
            return BS_ERROR;
        }
        break;
    /* UART config */
    case BS_DEVICE_CTRL_CONFIG:
        break;
    }

    return BS_EOK;
}

static int fm_putc(struct bs_serial_device *serial, char c)
{
    struct fm_uart_config *uartx = BS_NULL;
    BS_ASSERT(serial != BS_NULL);

    uartx = (struct fm_uart_config *)serial->parent.user_data;

    bs_hw_interrupt_disable();
    LL_UART_TransmitData(uartx->Instance, c);
    bs_hw_interrupt_enable();

    while (SET != LL_UART_IsActiveFlag_TXSE(uartx->Instance));

    return BS_EOK;
}

static int fm_getc(struct bs_serial_device *serial)
{
    int ch = -1;
    struct fm_uart_config *uart;

    BS_ASSERT(serial != BS_NULL);
    uart = (struct fm_uart_config *)serial->parent.user_data;
    BS_ASSERT(uart != BS_NULL);

    if (SET == LL_UART_IsActiveFlag_RXBF(uart->Instance)) {
        ch = LL_UART_ReceiveData(uart->Instance);
    }

    return ch;
}

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct bs_serial_device *serial)
{
    struct fm_uart_config *uart;

    BS_ASSERT(serial != BS_NULL);
    uart = (struct fm_uart_config *)serial->parent.user_data;
    BS_ASSERT(uart != BS_NULL);

    if ((ENABLE == LL_UART_IsEnbaledIT_ReceiveBuffFull(uart->Instance))
        && (SET == LL_UART_IsActiveFlag_RXBF(uart->Instance))) {
        bs_hw_serial_isr(serial, BS_SERIAL_EVENT_RX_IND);
    }

    if ((ENABLE == LL_UART_IsEnbaledIT_ShiftBuffEmpty(uart->Instance))
        && (SET == LL_UART_IsActiveFlag_TXSE(uart->Instance))) {
        bs_hw_serial_isr(serial, BS_SERIAL_EVENT_TX_DONE);
    }
}


#if defined(BSP_USING_UART0)
void UART0_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART0_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_UART1)
void UART1_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART1_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_UART2)
void UART2_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART2_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_UART3)
void UART3_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART3_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_UART4)
void UART4_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART4_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_UART5)
void UART5_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[UART5_INDEX].serial));
    /* leave interrupt */
}
#endif

#if defined(BSP_USING_LPUART1)
void LPUART1_IRQHandler(void)
{
    /* enter interrupt */
    uart_isr(&(uart_obj[LPUART1_INDEX].serial));
    /* leave interrupt */
}
#endif

static void GPIO_Configuration(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = DISABLE;
    GPIO_InitStruct.RemapPin = DISABLE;

#if defined(BSP_USING_UART0)
    /* Make sure the UART RX and TX pins are enabled */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif /* BSP_USING_UART0 */

#if defined(BSP_USING_UART1)
    /* Make sure the UART RX and TX pins are enabled */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
    /* Make sure the UART RX and TX pins are enabled */
#endif /* BSP_USING_UART2 */

#if defined(BSP_USING_UART3)
    /* Make sure the UART RX and TX pins are enabled */
#endif /* BSP_USING_UART3 */

#if defined(BSP_USING_UART4)
    /* Make sure the UART RX and TX pins are enabled */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* BSP_USING_UART4 */

#if defined(BSP_USING_UART5)
    /* Make sure the UART RX and TX pins are enabled */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif /* BSP_USING_UART5 */

#if defined(BSP_USING_LPUART1)
    /* Make sure the UART RX and TX pins are enabled */
#endif /* BSP_USING_UART5 */
}


static const struct bs_uart_ops fm_uart_ops = {
    .configure = fm_configure,
    .control = fm_control,
    .putc = fm_putc,
    .getc = fm_getc,
    .dma_transmit = BS_NULL
};


int bs_hw_usart_init(void)
{
    bs_err_t result = 0;

    GPIO_Configuration();

#ifdef BSP_USING_UART0
    /* init UART object */
    uart_obj[UART0_INDEX].config = &uart_config[UART0_INDEX];
    uart_obj[UART0_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART0_INDEX].serial.config.baud_rate = serial_default_config[UART0_INDEX].baud_rate;
    uart_obj[UART0_INDEX].serial.config.data_bits = serial_default_config[UART0_INDEX].data_bits;
    uart_obj[UART0_INDEX].serial.config.stop_bits = serial_default_config[UART0_INDEX].stop_bits;
    uart_obj[UART0_INDEX].serial.config.parity = serial_default_config[UART0_INDEX].parity;
    uart_obj[UART0_INDEX].serial.config.bit_order = serial_default_config[UART0_INDEX].bit_order;
    uart_obj[UART0_INDEX].serial.config.invert = serial_default_config[UART0_INDEX].invert;
    uart_obj[UART0_INDEX].serial.config.bufsz = serial_default_config[UART0_INDEX].bufsz;
    uart_obj[UART0_INDEX].serial.config.reserved = serial_default_config[UART0_INDEX].reserved;
    uart_obj[UART0_INDEX].serial.serial_rx = uart0_rxbuf;
    uart_obj[UART0_INDEX].serial.serial_tx = uart0_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART0_INDEX].serial, uart_obj[UART0_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART0_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART1
    /* init UART object */
    uart_obj[UART1_INDEX].config = &uart_config[UART1_INDEX];
    uart_obj[UART1_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART1_INDEX].serial.config.baud_rate = serial_default_config[UART1_INDEX].baud_rate;
    uart_obj[UART1_INDEX].serial.config.data_bits = serial_default_config[UART1_INDEX].data_bits;
    uart_obj[UART1_INDEX].serial.config.stop_bits = serial_default_config[UART1_INDEX].stop_bits;
    uart_obj[UART1_INDEX].serial.config.parity = serial_default_config[UART1_INDEX].parity;
    uart_obj[UART1_INDEX].serial.config.bit_order = serial_default_config[UART1_INDEX].bit_order;
    uart_obj[UART1_INDEX].serial.config.invert = serial_default_config[UART1_INDEX].invert;
    uart_obj[UART1_INDEX].serial.config.bufsz = serial_default_config[UART1_INDEX].bufsz;
    uart_obj[UART1_INDEX].serial.config.reserved = serial_default_config[UART1_INDEX].reserved;
    uart_obj[UART1_INDEX].serial.serial_rx = uart1_rxbuf;
    uart_obj[UART1_INDEX].serial.serial_tx = uart1_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART1_INDEX].serial, uart_obj[UART1_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART1_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART2
    /* init UART object */
    uart_obj[UART2_INDEX].config = &uart_config[UART2_INDEX];
    uart_obj[UART2_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART2_INDEX].serial.config.baud_rate = serial_default_config[UART2_INDEX].baud_rate;
    uart_obj[UART2_INDEX].serial.config.data_bits = serial_default_config[UART2_INDEX].data_bits;
    uart_obj[UART2_INDEX].serial.config.stop_bits = serial_default_config[UART2_INDEX].stop_bits;
    uart_obj[UART2_INDEX].serial.config.parity = serial_default_config[UART2_INDEX].parity;
    uart_obj[UART2_INDEX].serial.config.bit_order = serial_default_config[UART2_INDEX].bit_order;
    uart_obj[UART2_INDEX].serial.config.invert = serial_default_config[UART2_INDEX].invert;
    uart_obj[UART2_INDEX].serial.config.bufsz = serial_default_config[UART2_INDEX].bufsz;
    uart_obj[UART2_INDEX].serial.config.reserved = serial_default_config[UART2_INDEX].reserved;
    uart_obj[UART2_INDEX].serial.serial_rx = uart2_rxbuf;
    uart_obj[UART2_INDEX].serial.serial_tx = uart2_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART2_INDEX].serial, uart_obj[UART2_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART2_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART3
    /* init UART object */
    uart_obj[UART3_INDEX].config = &uart_config[UART3_INDEX];
    uart_obj[UART3_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART3_INDEX].serial.config.baud_rate = serial_default_config[UART3_INDEX].baud_rate;
    uart_obj[UART3_INDEX].serial.config.data_bits = serial_default_config[UART3_INDEX].data_bits;
    uart_obj[UART3_INDEX].serial.config.stop_bits = serial_default_config[UART3_INDEX].stop_bits;
    uart_obj[UART3_INDEX].serial.config.parity = serial_default_config[UART3_INDEX].parity;
    uart_obj[UART3_INDEX].serial.config.bit_order = serial_default_config[UART3_INDEX].bit_order;
    uart_obj[UART3_INDEX].serial.config.invert = serial_default_config[UART3_INDEX].invert;
    uart_obj[UART3_INDEX].serial.config.bufsz = serial_default_config[UART3_INDEX].bufsz;
    uart_obj[UART3_INDEX].serial.config.reserved = serial_default_config[UART3_INDEX].reserved;
    uart_obj[UART3_INDEX].serial.serial_rx = uart3_rxbuf;
    uart_obj[UART3_INDEX].serial.serial_tx = uart3_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART3_INDEX].serial, uart_obj[UART3_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART3_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART4
    /* init UART object */
    uart_obj[UART4_INDEX].config = &uart_config[UART4_INDEX];
    uart_obj[UART4_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART4_INDEX].serial.config.baud_rate = serial_default_config[UART4_INDEX].baud_rate;
    uart_obj[UART4_INDEX].serial.config.data_bits = serial_default_config[UART4_INDEX].data_bits;
    uart_obj[UART4_INDEX].serial.config.stop_bits = serial_default_config[UART4_INDEX].stop_bits;
    uart_obj[UART4_INDEX].serial.config.parity = serial_default_config[UART4_INDEX].parity;
    uart_obj[UART4_INDEX].serial.config.bit_order = serial_default_config[UART4_INDEX].bit_order;
    uart_obj[UART4_INDEX].serial.config.invert = serial_default_config[UART4_INDEX].invert;
    uart_obj[UART4_INDEX].serial.config.bufsz = serial_default_config[UART4_INDEX].bufsz;
    uart_obj[UART4_INDEX].serial.config.reserved = serial_default_config[UART4_INDEX].reserved;
    uart_obj[UART4_INDEX].serial.serial_rx = uart4_rxbuf;
    uart_obj[UART4_INDEX].serial.serial_tx = uart4_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART4_INDEX].serial, uart_obj[UART4_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART4_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART5
    /* init UART object */
    uart_obj[UART5_INDEX].config = &uart_config[UART5_INDEX];
    uart_obj[UART5_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[UART5_INDEX].serial.config.baud_rate = serial_default_config[UART5_INDEX].baud_rate;
    uart_obj[UART5_INDEX].serial.config.data_bits = serial_default_config[UART5_INDEX].data_bits;
    uart_obj[UART5_INDEX].serial.config.stop_bits = serial_default_config[UART5_INDEX].stop_bits;
    uart_obj[UART5_INDEX].serial.config.parity = serial_default_config[UART5_INDEX].parity;
    uart_obj[UART5_INDEX].serial.config.bit_order = serial_default_config[UART5_INDEX].bit_order;
    uart_obj[UART5_INDEX].serial.config.invert = serial_default_config[UART5_INDEX].invert;
    uart_obj[UART5_INDEX].serial.config.bufsz = serial_default_config[UART5_INDEX].bufsz;
    uart_obj[UART5_INDEX].serial.config.reserved = serial_default_config[UART5_INDEX].reserved;
    uart_obj[UART5_INDEX].serial.serial_rx = uart5_rxbuf;
    uart_obj[UART5_INDEX].serial.serial_tx = uart5_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[UART5_INDEX].serial, uart_obj[UART5_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[UART5_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_LPUART1
    /* init UART object */
    uart_obj[LPUART1_INDEX].config = &uart_config[LPUART1_INDEX];
    uart_obj[LPUART1_INDEX].serial.ops = &fm_uart_ops;
    uart_obj[LPUART1_INDEX].serial.config.baud_rate = serial_default_config[LPUART1_INDEX].baud_rate;
    uart_obj[LPUART1_INDEX].serial.config.data_bits = serial_default_config[LPUART1_INDEX].data_bits;
    uart_obj[LPUART1_INDEX].serial.config.stop_bits = serial_default_config[LPUART1_INDEX].stop_bits;
    uart_obj[LPUART1_INDEX].serial.config.parity = serial_default_config[LPUART1_INDEX].parity;
    uart_obj[LPUART1_INDEX].serial.config.bit_order = serial_default_config[LPUART1_INDEX].bit_order;
    uart_obj[LPUART1_INDEX].serial.config.invert = serial_default_config[LPUART1_INDEX].invert;
    uart_obj[LPUART1_INDEX].serial.config.bufsz = serial_default_config[LPUART1_INDEX].bufsz;
    uart_obj[LPUART1_INDEX].serial.config.reserved = serial_default_config[LPUART1_INDEX].reserved;
    uart_obj[LPUART1_INDEX].serial.serial_rx = lpuart1_rxbuf;
    uart_obj[LPUART1_INDEX].serial.serial_tx = lpuart1_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[LPUART1_INDEX].serial, uart_obj[LPUART1_INDEX].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   | BS_DEVICE_FLAG_STANDALONE
                                   , &uart_config[LPUART1_INDEX]);
    BS_ASSERT(result == BS_EOK);
#endif
    return result;
}

INIT_DEVICE_EXPORT(bs_hw_usart_init);

#endif /* RT_USING_SERIAL */
