/*
 * COPYRIGHT (C) 2018
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef  __WATCHDOG_H__
#define  __WATCHDOG_H__

#include <bare_sys.h>

#define BS_DEVICE_CTRL_WDT_GET_TIMEOUT    (1) /* get timeout(in seconds) */
#define BS_DEVICE_CTRL_WDT_SET_TIMEOUT    (2) /* set timeout(in seconds) */
#define BS_DEVICE_CTRL_WDT_GET_TIMELEFT   (3) /* get the left time before reboot(in seconds) */
#define BS_DEVICE_CTRL_WDT_KEEPALIVE      (4) /* refresh watchdog */
#define BS_DEVICE_CTRL_WDT_START          (5) /* start watchdog */
#define BS_DEVICE_CTRL_WDT_STOP           (6) /* stop watchdog */

struct bs_watchdog_ops;
struct bs_watchdog_device
{
    struct bs_device parent;
    const struct bs_watchdog_ops *ops;
};
typedef struct bs_watchdog_device bs_watchdog_t;

struct bs_watchdog_ops
{
    bs_err_t (*init)(bs_watchdog_t *wdt);
    bs_err_t (*control)(bs_watchdog_t *wdt, int cmd, void *arg);
};

bs_err_t bs_hw_watchdog_register(bs_watchdog_t *wdt,
                                 const char    *name,
                                 bs_uint32_t    flag,
                                 void          *data);

#endif /* __WATCHDOG_H__ */
