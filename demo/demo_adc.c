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

#ifdef BS_USING_DEMO_ADC

#define DEMO_ADC_DEV_NAME            "adc0"

static bs_device_t adc_dev = BS_NULL;
static struct bs_timer st_adc;


static void st_adc_timout_cb(void *args)
{
    bs_uint32_t value = 0;
    bs_device_control(adc_dev, BS_DEVICE_CTRL_ADC_GET_CONVERT_VALUE, &value);
    bs_kprintf("Current adc conversion value is %d.", value);
}


int adc_demo(void)
{
    adc_dev = bs_device_find(DEMO_ADC_DEV_NAME);
    if (adc_dev == BS_NULL) {
        bs_kprintf("Adc demo find %s failed!\n", DEMO_ADC_DEV_NAME);
        return (-BS_ERROR);
    }
    if (bs_device_open(adc_dev, BS_DEVICE_FLAG_RDONLY) != BS_EOK) {
        bs_kprintf("Adc demo open fail!");
        return (-BS_ERROR);
    }
    bs_kprintf("Adc demo opened!");
    creat_continue_soft_timer(&st_adc, RUN_IN_LOOP_MODE,
                            TIMER_SEC_DELAY * 2, st_adc_timout_cb, BS_NULL);

    return 0;
}

INIT_APP_EXPORT(adc_demo);
#endif
