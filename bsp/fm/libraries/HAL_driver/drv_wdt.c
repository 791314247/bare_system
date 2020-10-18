/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 */

#include <board.h>
#include <bsdevice.h>

#ifdef BS_USING_WDT


static struct bs_watchdog_device wdt_obj;

static bs_err_t wdt_init(bs_watchdog_t *wdt)
{
    LL_IWDT_InitTypeDef IWDT_InitStruct;
    /* 默认不适用窗口 */
    IWDT_InitStruct.IwdtWindows    = 0;
    /*最长溢出时间*/
    IWDT_InitStruct.OverflowPeriod = LL_IWDT_IWDT_OVERFLOW_PERIOD_1000MS;
    LL_IWDT_Init(IWDT, &IWDT_InitStruct);
    LL_IWDG_DisableStopCounterUnderSleep(IWDT);  /* default closed */
    return BS_EOK;
}

static bs_err_t wdt_control(bs_watchdog_t *wdt, int cmd, void *arg)
{
    switch (cmd) {
    /* feed the watchdog */
    case BS_DEVICE_CTRL_WDT_KEEPALIVE:
        LL_IWDG_ReloadCounter(IWDT);
        break;
    /* set watchdog timeout */
    case BS_DEVICE_CTRL_WDT_SET_TIMEOUT:
        LL_IWDG_SetCounterPeriod(IWDT, (*((bs_uint32_t *)arg)));
        break;
    case BS_DEVICE_CTRL_WDT_GET_TIMEOUT:
        LL_IWDG_GetCounterPeriod(IWDT);
        break;
    case BS_DEVICE_CTRL_WDT_START:
        LL_IWDG_EnableStopCounterUnderSleep(IWDT);
        break;
    case BS_DEVICE_CTRL_WDT_STOP:
        LL_IWDG_DisableStopCounterUnderSleep(IWDT);
        break;
    default:
        bs_kprintf("This command is not supported.");
        return -BS_ERROR;
    }
    return BS_EOK;
}

static const struct bs_watchdog_ops fm_wdt_ops = {
    .init = wdt_init,
    .control = wdt_control
};


int bs_wdt_init(void)
{
    wdt_obj.ops = &fm_wdt_ops;
    /* register watchdog device */
    if (bs_hw_watchdog_register(&wdt_obj, "wdt", BS_DEVICE_FLAG_DEACTIVATE, BS_NULL) != BS_EOK) {
        bs_kprintf("wdt device register failed.");
        return -BS_ERROR;
    }
    bs_kprintf("wdt device register success.");
    return BS_EOK;
}
INIT_DEVICE_EXPORT(bs_wdt_init);

#endif /* RT_USING_WDT */
