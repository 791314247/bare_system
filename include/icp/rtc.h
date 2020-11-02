/*
 * COPYRIGHT (C) 2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef  __RTC_H__
#define  __RTC_H__

#include <bare_sys.h>

#define BS_DEVICE_CTRL_RTC_SET_TIME          (0) /* set rtc time */
#define BS_DEVICE_CTRL_RTC_GET_TIME          (1) /* get rtc time */
#define BS_DEVICE_CTRL_RTC_OPEN_SEC_INT      (2) /* open sencond interrupt */
#define BS_DEVICE_CTRL_RTC_CLOSE_SEC_INT     (3) /* close sencond interrupt */

/**
 * rtc time BCD format
 */
struct bs_rtc_time {
    bs_uint32_t Year;
    bs_uint32_t Month;
    bs_uint32_t Day;
    bs_uint32_t Week;
    bs_uint32_t Hour;
    bs_uint32_t Minute;
    bs_uint32_t Second;
};
typedef struct bs_rtc_time bs_time_t;

struct bs_rtc_ops;
struct bs_rtc_device {
    struct bs_device parent;
    const struct bs_rtc_ops *ops;
};
typedef struct bs_rtc_device bs_rtc_t;

struct bs_rtc_ops {
    bs_err_t (*init)(bs_rtc_t *rtc);
    bs_err_t (*control)(bs_rtc_t *rtc, int cmd, void *arg);
};

bs_err_t bs_hw_rtc_register(bs_rtc_t *rtc,
                            const char    *name,
                            bs_uint32_t    flag,
                            void          *data);

#endif /* __RTC_H__ */
