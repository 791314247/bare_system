/*
 * COPYRIGHT (C) 2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "at_base.h"

/* 可单独选择开TX流程或者RX流程，仅在DEBUG版本设置有效，release版设置无效 */
#ifdef BS_DEBUG
    // #define DEBUG_TX(x)
    // #define DEBUG_RX(x)
    #define DEBUG_TX(x)         bs_kprintf(x)
    #define DEBUG_RX(x)         bs_kprintf(x)
#else
    #define DEBUG_TX(x)
    #define DEBUG_RX(x)
#endif

static char recv_buf[256];

static SoftTimer st_serial;
static SoftTimer st_comm;

static bs_size_t data_length;
static bs_device_t serial_dev = BS_NULL;
static bs_bool_t timeoutFlag = BS_FALSE;

/**
 * 软件定时器200ms的回调函数，用于在串口接收中断超过200ms为接到数据时将所有数据读出来
 */
static void st_serial_timout_cb(void *args)
{
    char *p = recv_buf;
    if (data_length >= sizeof(recv_buf))
        data_length = sizeof(recv_buf);
    bs_device_read(serial_dev, -1, p, data_length);
    bs_device_write(serial_dev, 0, recv_buf, data_length);
}

/**
 * 串口中断的回调函数，用于在串口产生接收中断时调用
 */
static bs_err_t uart_input(bs_device_t dev, bs_size_t size)
{
    data_length = size;
    creat_single_soft_timer(&st_serial, RUN_IN_LOOP_MODE,
                            TIMER_100MS_DELAY * 2, st_serial_timout_cb, BS_NULL);
    return 0;
}

static void st_comm_timeout_cb(void *args)
{
    timeoutFlag = BS_TRUE;
}

bs_err_t at_base_send(const char *command,
                      const char *exec,
                      bs_uint8_t wait_time,
                      bs_uint8_t resend_num)
{
    static bs_bool_t sendBusy = BS_FALSE;          /* 限定发送未完成时禁止发送 */
    static bs_uint8_t resendCount = 0;
    BS_ASSERT(exec != BS_NULL);

    if (!sendBusy) {
        sendBusy = BS_TRUE;
        bs_memset(recv_buf, 0, sizeof(recv_buf));
        if (command != BS_NULL)
            bs_device_write(serial_dev, 0, command, bs_strlen(command));
        timeoutFlag = BS_FALSE;
        creat_single_soft_timer(&st_comm,
                                RUN_IN_LOOP_MODE,
                                TIMER_SEC_DELAY * wait_time,
                                st_comm_timeout_cb, BS_NULL);
        DEBUG_TX("MCU------------------------->Tx:");
        DEBUG_TX(command);
    } else if (bs_strstr(recv_buf, "OK\r\n")) {
        if (bs_strstr(recv_buf, exec) != BS_NULL) {
            stop_timer(&st_comm);
            sendBusy = BS_FALSE;
            resendCount = 0;
            DEBUG_RX("MCU------------------------->Rx:");
            DEBUG_RX(recv_buf);
            return (BS_EOK);
        }
    }

    if (timeoutFlag) {
        DEBUG_RX("MCU------------------------->Rx:");
        DEBUG_RX(recv_buf);
        sendBusy = BS_FALSE;
        if (++resendCount >= resend_num) {
            bs_kprintf("AT command fail: %s", command);
            resendCount = 0;
            return (-BS_ERROR);
        }
    }
    return (-BS_EBUSY);
}

char *at_base_read_recv(const char *exec)
{
    return bs_strstr(recv_buf, exec);
}

void at_base_clean_recv(void)
{
    bs_memset(recv_buf, 0, sizeof(recv_buf));
}

int at_base_init(void)
{
    serial_dev = bs_device_find(AT_SERIAL);
    if (serial_dev == BS_NULL) {
        bs_kprintf("find %s failed!\n", AT_SERIAL);
        return (-BS_ERROR);
    }
    bs_device_open(serial_dev, BS_DEVICE_FLAG_RDWR | BS_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    bs_device_set_rx_indicate(serial_dev, uart_input);
    return 0;
}

INIT_APP_EXPORT(at_base_init);
