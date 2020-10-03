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

    switch (cmd)
    {
    /* disable interrupt */
    case BS_DEVICE_CTRL_CLR_INT:
        if (irq_type == BS_DEVICE_FLAG_INT_RX)
        {
            /* disable rx irq */
            LL_UART_DisableIT_ReceiveBuffFull(uart->Instance);
        }
        else if (irq_type == BS_DEVICE_FLAG_INT_TX)
        {
            /* disable tx irq */
            LL_UART_DisableIT_TxBuffEmpty(uart->Instance);
        }
        else
        {
            //LOG_EXT_D("invalide irq type %d\r\n", (int)irq_type);
            return BS_ERROR;
        }
        break;
    /* enable interrupt */
    case BS_DEVICE_CTRL_SET_INT:
        /* Enable the uart interrupt in the NVIC */
        if (irq_type == BS_DEVICE_FLAG_INT_RX)
        {
            /* enable rx irq */
            LL_UART_EnableIT_ReceiveBuffFull(uart->Instance);
        }
        else if (irq_type == BS_DEVICE_FLAG_INT_TX)
        {
            /* enable tx irq */
            LL_UART_EnableIT_TxBuffEmpty(uart->Instance);
        }
        else
        {
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
    bs_base_t level;
    BS_ASSERT(serial != BS_NULL);

    uartx = (struct fm_uart_config *)serial->parent.user_data;

    level = bs_hw_interrupt_disable();
    LL_UART_TransmitData(uartx->Instance, c);
    bs_hw_interrupt_enable(level);

    while (SET != LL_UART_IsActiveFlag_TXSE(uartx->Instance));

    return 1;
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
    bs_size_t obj_num = sizeof(uart_obj) / sizeof(struct fm_uart_class);
    bs_err_t result = 0;

    GPIO_Configuration();

    for (int i = 0; i < obj_num; i++) {
        /* init UART object */
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops = &fm_uart_ops;
        uart_obj[i].serial.config.baud_rate = serial_default_config[i].baud_rate;
        uart_obj[i].serial.config.data_bits = serial_default_config[i].data_bits;
        uart_obj[i].serial.config.stop_bits = serial_default_config[i].stop_bits;
        uart_obj[i].serial.config.parity = serial_default_config[i].parity;
        uart_obj[i].serial.config.bit_order = serial_default_config[i].bit_order;
        uart_obj[i].serial.config.invert = serial_default_config[i].invert;
        uart_obj[i].serial.config.bufsz = serial_default_config[i].bufsz;
        uart_obj[i].serial.config.reserved = serial_default_config[i].reserved;

        /* register UART device */
        result = bs_hw_serial_register(&uart_obj[i].serial, uart_obj[i].config->name,
                                       BS_DEVICE_FLAG_RDWR
                                       | BS_DEVICE_FLAG_INT_RX
                                       | BS_DEVICE_FLAG_INT_TX
                                       , &uart_config[i]);
        BS_ASSERT(result == BS_EOK);
    }

    return result;
}

INIT_DEVICE_EXPORT(bs_hw_usart_init);
