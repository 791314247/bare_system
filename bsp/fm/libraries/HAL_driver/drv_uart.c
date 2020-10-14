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
    BS_UART3_CONFIG_DEFAULTG,
#endif
#ifdef BSP_USING_UART4
    BS_UART4_CONFIG_DEFAULTG,
#endif
#ifdef BSP_USING_UART5
    BS_UART5_CONFIG_DEFAULTG,
#endif
#ifdef BSP_USING_LPUART1
    BS_LPUART1_CONFIG_DEFAULTG,
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

    ch = LL_UART_ReceiveData(uart->Instance);

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

    if((ENABLE == LL_UART_IsEnbaledIT_ReceiveBuffFull(uart->Instance))
		&&(SET == LL_UART_IsActiveFlag_RXBF(uart->Instance)))
	{
        bs_hw_serial_isr(serial, BS_SERIAL_EVENT_RX_IND);
    }

    if((ENABLE == LL_UART_IsEnbaledIT_ShiftBuffEmpty(uart->Instance))
		&&(SET == LL_UART_IsActiveFlag_TXSE(uart->Instance)))
	{
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
    uart_obj[0].config = &uart_config[0];
    uart_obj[0].serial.ops = &fm_uart_ops;
    uart_obj[0].serial.config.baud_rate = serial_default_config[0].baud_rate;
    uart_obj[0].serial.config.data_bits = serial_default_config[0].data_bits;
    uart_obj[0].serial.config.stop_bits = serial_default_config[0].stop_bits;
    uart_obj[0].serial.config.parity = serial_default_config[0].parity;
    uart_obj[0].serial.config.bit_order = serial_default_config[0].bit_order;
    uart_obj[0].serial.config.invert = serial_default_config[0].invert;
    uart_obj[0].serial.config.bufsz = serial_default_config[0].bufsz;
    uart_obj[0].serial.config.reserved = serial_default_config[0].reserved;
    uart_obj[0].serial.serial_rx = uart0_rxbuf;
    uart_obj[0].serial.serial_tx = uart0_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[0].serial, uart_obj[0].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[0]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART1
    /* init UART object */
    uart_obj[1].config = &uart_config[i];
    uart_obj[1].serial.ops = &fm_uart_ops;
    uart_obj[1].serial.config.baud_rate = serial_default_config[1].baud_rate;
    uart_obj[1].serial.config.data_bits = serial_default_config[1].data_bits;
    uart_obj[1].serial.config.stop_bits = serial_default_config[1].stop_bits;
    uart_obj[1].serial.config.parity = serial_default_config[1].parity;
    uart_obj[1].serial.config.bit_order = serial_default_config[1].bit_order;
    uart_obj[1].serial.config.invert = serial_default_config[1].invert;
    uart_obj[1].serial.config.bufsz = serial_default_config[1].bufsz;
    uart_obj[1].serial.config.reserved = serial_default_config[1].reserved;
    uart_obj[1].serial.serial_rx = uart1_rxbuf;
    uart_obj[1].serial.serial_tx = uart1_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[1].serial, uart_obj[1].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[1]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART2
    /* init UART object */
    uart_obj[2].config = &uart_config[i];
    uart_obj[2].serial.ops = &fm_uart_ops;
    uart_obj[2].serial.config.baud_rate = serial_default_config[2].baud_rate;
    uart_obj[2].serial.config.data_bits = serial_default_config[2].data_bits;
    uart_obj[2].serial.config.stop_bits = serial_default_config[2].stop_bits;
    uart_obj[2].serial.config.parity = serial_default_config[2].parity;
    uart_obj[2].serial.config.bit_order = serial_default_config[2].bit_order;
    uart_obj[2].serial.config.invert = serial_default_config[2].invert;
    uart_obj[2].serial.config.bufsz = serial_default_config[2].bufsz;
    uart_obj[2].serial.config.reserved = serial_default_config[2].reserved;
    uart_obj[2].serial.serial_rx = uart2_rxbuf;
    uart_obj[2].serial.serial_tx = uart2_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[2].serial, uart_obj[2].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[2]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART3
    /* init UART object */
    uart_obj[3].config = &uart_config[i];
    uart_obj[3].serial.ops = &fm_uart_ops;
    uart_obj[3].serial.config.baud_rate = serial_default_config[3].baud_rate;
    uart_obj[3].serial.config.data_bits = serial_default_config[3].data_bits;
    uart_obj[3].serial.config.stop_bits = serial_default_config[3].stop_bits;
    uart_obj[3].serial.config.parity = serial_default_config[3].parity;
    uart_obj[3].serial.config.bit_order = serial_default_config[3].bit_order;
    uart_obj[3].serial.config.invert = serial_default_config[3].invert;
    uart_obj[3].serial.config.bufsz = serial_default_config[3].bufsz;
    uart_obj[3].serial.config.reserved = serial_default_config[3].reserved;
    uart_obj[3].serial.serial_rx = uart3_rxbuf;
    uart_obj[3].serial.serial_tx = uart3_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[3].serial, uart_obj[3].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[3]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART4
    /* init UART object */
    uart_obj[4].config = &uart_config[i];
    uart_obj[4].serial.ops = &fm_uart_ops;
    uart_obj[4].serial.config.baud_rate = serial_default_config[4].baud_rate;
    uart_obj[4].serial.config.data_bits = serial_default_config[4].data_bits;
    uart_obj[4].serial.config.stop_bits = serial_default_config[4].stop_bits;
    uart_obj[4].serial.config.parity = serial_default_config[4].parity;
    uart_obj[4].serial.config.bit_order = serial_default_config[4].bit_order;
    uart_obj[4].serial.config.invert = serial_default_config[4].invert;
    uart_obj[4].serial.config.bufsz = serial_default_config[4].bufsz;
    uart_obj[4].serial.config.reserved = serial_default_config[4].reserved;
    uart_obj[4].serial.serial_rx = uart4_rxbuf;
    uart_obj[4].serial.serial_tx = uart4_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[4].serial, uart_obj[4].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[4]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_UART5
    /* init UART object */
    uart_obj[5].config = &uart_config[i];
    uart_obj[5].serial.ops = &fm_uart_ops;
    uart_obj[5].serial.config.baud_rate = serial_default_config[5].baud_rate;
    uart_obj[5].serial.config.data_bits = serial_default_config[5].data_bits;
    uart_obj[5].serial.config.stop_bits = serial_default_config[5].stop_bits;
    uart_obj[5].serial.config.parity = serial_default_config[5].parity;
    uart_obj[5].serial.config.bit_order = serial_default_config[5].bit_order;
    uart_obj[5].serial.config.invert = serial_default_config[5].invert;
    uart_obj[5].serial.config.bufsz = serial_default_config[5].bufsz;
    uart_obj[5].serial.config.reserved = serial_default_config[5].reserved;
    uart_obj[5].serial.serial_rx = uart5_rxbuf;
    uart_obj[5].serial.serial_tx = uart5_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[5].serial, uart_obj[5].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[5]);
    BS_ASSERT(result == BS_EOK);
#endif
#ifdef BSP_USING_LPUART1
    /* init UART object */
    uart_obj[6].config = &uart_config[i];
    uart_obj[6].serial.ops = &fm_uart_ops;
    uart_obj[6].serial.config.baud_rate = serial_default_config[6].baud_rate;
    uart_obj[6].serial.config.data_bits = serial_default_config[6].data_bits;
    uart_obj[6].serial.config.stop_bits = serial_default_config[6].stop_bits;
    uart_obj[6].serial.config.parity = serial_default_config[6].parity;
    uart_obj[6].serial.config.bit_order = serial_default_config[6].bit_order;
    uart_obj[6].serial.config.invert = serial_default_config[6].invert;
    uart_obj[6].serial.config.bufsz = serial_default_config[6].bufsz;
    uart_obj[6].serial.config.reserved = serial_default_config[6].reserved;
    uart_obj[6].serial.serial_rx = lpuart1_rxbuf;
    uart_obj[6].serial.serial_tx = lpuart1_txbuf;
    /* register UART device */
    result = bs_hw_serial_register(&uart_obj[6].serial, uart_obj[6].config->name,
                                   BS_DEVICE_FLAG_RDWR
                                   | BS_DEVICE_FLAG_INT_RX
                                   | BS_DEVICE_FLAG_INT_TX
                                   , &uart_config[6]);
    BS_ASSERT(result == BS_EOK);
#endif
    return result;
}

INIT_DEVICE_EXPORT(bs_hw_usart_init);
