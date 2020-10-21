/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     balanceTWK   first version
 */

#include <bare_sys.h>
#include "bsdevice.h"
#include "drv_common.h"

#ifdef BS_USING_DEMO_SERIAL

#define DEMO_SERIAL            "uart4"

static bs_device_t serial_dev = BS_NULL;
static bs_size_t data_length = 0;
static SoftTimer st_serial;
static char buf[500] = {0};

/**
 * 软件定时器200ms的回调函数，用于在串口接收中断超过200ms为接到数据时将所有数据读出来
 */
static void st_serial_timout_cb(void *args)
{
    char *p = buf;
    if (data_length >= 500)
        data_length = 500;
    bs_device_read(serial_dev, -1, p, data_length);
    bs_device_write(serial_dev, 0, buf, data_length);
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

int serial_demo(void)
{
    serial_dev = bs_device_find(DEMO_SERIAL);
    if (serial_dev == BS_NULL) {
        bs_kprintf("Serial demo find %s failed!\n", DEMO_SERIAL);
        return (-BS_ERROR);
    }
    if (bs_device_open(serial_dev, BS_DEVICE_FLAG_RDWR | BS_DEVICE_FLAG_INT_RX) != BS_EOK) {
        bs_kprintf("Serial demo open fail!");
        return (-BS_ERROR);
    }
    bs_kprintf("Serial demo opened!");
    /* 设置接收回调函数 */
    bs_device_set_rx_indicate(serial_dev, uart_input);

    bs_device_write(serial_dev, 0, "Demo serial tx test !\r\n", bs_strlen("Demo serial tx test !\r\n"));
    return 0;
}

INIT_APP_EXPORT(serial_demo);
#endif
