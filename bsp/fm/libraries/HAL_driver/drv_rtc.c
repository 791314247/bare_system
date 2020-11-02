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

#ifdef BS_USING_RTC


static bs_rtc_t rtc_obj;


bs_err_t fm_rtc_get_time(LL_RTC_InitTypeDef *InitStructer)
{
    bs_uint8_t n, i;
    bs_err_t result = -BS_ERROR;

    LL_RTC_InitTypeDef TempTime1, TempTime2;

    for (n = 0 ; n < 3; n++) {
        LL_RTC_GetTime(RTC, &TempTime1);
        LL_RTC_GetTime(RTC, &TempTime2);

        for (i = 0; i < 7; i++) {
            if (((bs_uint32_t *)(&TempTime1))[i] != ((bs_uint32_t *)(&TempTime2))[i]) break;
        }
        if (i == 7) {
            result = BS_EOK;
            memcpy((bs_uint32_t *)(InitStructer), (bs_uint32_t *)(&TempTime1), 7 * sizeof(bs_uint32_t));
            break;
        }
    }
    return result;
}

bs_err_t fm_rtc_set_time(LL_RTC_InitTypeDef *InitStructer)
{
    bs_uint8_t n, i;
    bs_err_t result;
    LL_RTC_InitTypeDef TempTime1;

    for (n = 0 ; n < 3; n++) {
        LL_RTC_ConfigTime(RTC, InitStructer);
        result = fm_rtc_get_time(&TempTime1);
        if (result == BS_EOK) {
            result = -BS_ERROR;
            for (i = 0; i < 7; i++) {
                if (((bs_uint32_t *)(&TempTime1))[i] != ((bs_uint32_t *)(InitStructer))[i])
                    break;
            }
            if (i == 7) {
                result = BS_EOK;
                break;
            }
        }
    }
    return result;
}


static bs_err_t rtc_init(bs_rtc_t *rtc)
{
    LL_RTC_InitTypeDef   TempTime;

    TempTime.Year   = 0x20;
    TempTime.Month  = 0x02;
    TempTime.Day    = 0x21;
    TempTime.Week   = 0x04;
    TempTime.Hour   = 0x09;
    TempTime.Minute = 0x00;
    TempTime.Second = 0x00;
    LL_RTC_Init(RTC, &TempTime);

    NVIC_DisableIRQ(RTC_IRQn);
    return BS_EOK;
}

static bs_err_t rtc_control(bs_rtc_t *rtc, int cmd, void *arg)
{
    bs_time_t *time;
    LL_RTC_InitTypeDef fm_time;
    switch (cmd) {
    /* feed the rtc */
    case BS_DEVICE_CTRL_RTC_SET_TIME:
        time = (bs_time_t *)arg;
        fm_time.Year = time->Year;
        fm_time.Month = time->Month;
        fm_time.Day = time->Day;
        fm_time.Week = time->Week;
        fm_time.Hour = time->Hour;
        fm_time.Minute = time->Minute;
        fm_time.Second = time->Second;
        fm_rtc_set_time(&fm_time);
        break;
    /* set rtc timeout */
    case BS_DEVICE_CTRL_RTC_GET_TIME:
        time = (bs_time_t *)arg;
        fm_rtc_get_time(&fm_time);
        time->Year = fm_time.Year;
        time->Month = fm_time.Month;
        time->Day = fm_time.Day;
        time->Week = fm_time.Week;
        time->Hour = fm_time.Hour ;
        time->Minute = fm_time.Minute;
        time->Second = fm_time.Second;
        break;
    case BS_DEVICE_CTRL_RTC_OPEN_SEC_INT:
        LL_RTC_ClearFlag_Second(RTC);
        LL_RTC_EnableIT_Second(RTC);
        NVIC_DisableIRQ(RTC_IRQn);
        NVIC_SetPriority(RTC_IRQn, 2);
        NVIC_EnableIRQ(RTC_IRQn);
        break;
    case BS_DEVICE_CTRL_RTC_CLOSE_SEC_INT:
        LL_RTC_ClearFlag_Second(RTC);
        LL_RTC_DisableIT_Second(RTC);
        NVIC_DisableIRQ(RTC_IRQn);
        break;
    default:
        BS_ASSERT(0);
        bs_kprintf("This command is not supported.");
        return -BS_ERROR;
    }
    return BS_EOK;
}

void RTC_IRQHandler(void)
{
    if (ENABLE == LL_RTC_IsEnabledIT_Second(RTC) &&
        SET == LL_RTC_IsActiveFlag_Second(RTC)) {
        LL_GPIO_ToggleOutputPin(GPIOC, LL_GPIO_PIN_0);
        LL_RTC_ClearFlag_Second(RTC);
    }
}

static const struct bs_rtc_ops fm_rtc_ops = {
    .init = rtc_init,
    .control = rtc_control
};


int bs_rtc_init(void)
{
    rtc_obj.ops = &fm_rtc_ops;
    /* register rtc device */
    if (bs_hw_rtc_register(&rtc_obj, "rtc", BS_DEVICE_FLAG_DEACTIVATE, BS_NULL) != BS_EOK) {
        bs_kprintf("rtc device register failed.");
        return -BS_ERROR;
    }
    return BS_EOK;
}
INIT_DEVICE_EXPORT(bs_rtc_init);

#endif /* RT_USING_RTC */
