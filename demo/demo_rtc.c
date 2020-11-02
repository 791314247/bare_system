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

#ifdef BS_USING_DEMO_RTC


static bs_device_t rtc_dev = BS_NULL;
static struct bs_timer st_rtc;


static void st_rtc_timout_cb(void *args)
{
    bs_time_t time;
    bs_device_control(rtc_dev, BS_DEVICE_CTRL_RTC_GET_TIME, &time);
    bs_kprintf("\r\nrtc year:%02X month:%02X day:%02X", time.Year, time.Month, time.Day);
    bs_kprintf("rtc hour:%02X minute:%02X second:%02X", time.Hour, time.Minute, time.Second);
}


int rtc_demo(void)
{
    bs_time_t time;
    rtc_dev = bs_device_find("rtc");
    if (rtc_dev == BS_NULL) {
        bs_kprintf("Rtc demo find %s failed!\n", "rtc");
        return (-BS_ERROR);
    }
    if (bs_device_open(rtc_dev, 0) != BS_EOK) {
        bs_kprintf("Rtc demo open fail!");
        return (-BS_ERROR);
    }
    bs_kprintf("Rtc demo opened!");
    time.Year = 0x20;
    time.Month = 0x11;
    time.Day = 0x02;
    time.Week = 0x01;
    time.Hour = 0x10;
    time.Minute = 0x19;
    time.Second = 0x00;
    bs_device_control(rtc_dev, BS_DEVICE_CTRL_RTC_SET_TIME, &time);
    bs_timer_init(&st_rtc, TIMER_SEC_DELAY * 1, st_rtc_timout_cb, BS_NULL,
                  BS_TIMER_FLAG_PERIODIC | BS_TIMER_FLAG_START);
    return 0;
}

INIT_APP_EXPORT(rtc_demo);
#endif
