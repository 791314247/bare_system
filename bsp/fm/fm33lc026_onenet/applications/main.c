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
#include <bsdevice.h>
#include <board.h>




int main(void)
{
    bs_device_t wdt_dev;

    wdt_dev = bs_device_find("wdt");
    if (wdt_dev == BS_NULL) {
        Error_Handler();
    }
    bs_device_open(wdt_dev, BS_DEVICE_FLAG_DEACTIVATE);
    bs_device_control(wdt_dev, BS_DEVICE_CTRL_WDT_START, BS_NULL);

    while (1) {
        soft_timer_main_loop();
        //DeepSleep();
        bs_device_control(wdt_dev, BS_DEVICE_CTRL_WDT_KEEPALIVE, BS_NULL);
    }
}


