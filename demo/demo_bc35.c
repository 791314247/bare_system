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

#ifdef BS_USING_DEMO_BC35

static bs_device_t bc35_dev = BS_NULL;

/* 接入bc35后可进行网络初始化流程，已测试ok */
void bc35_demo_process(void)
{
     bs_device_control(bc35_dev, BS_DEVICE_CTRL_BC35_NET_INIT, BS_NULL);
}


int bc35_demo_init(void)
{
    bc35_dev = bs_device_find("bc35");
    if (bc35_dev == BS_NULL) {
        bs_kprintf("Bc35 demo find %s failed!\n", "bc35");
        return (-BS_ERROR);
    }
    if (bs_device_open(bc35_dev, BS_DEVICE_FLAG_RDONLY) != BS_EOK) {
        bs_kprintf("Bc35 demo open fail!");
        return (-BS_ERROR);
    }
    bs_kprintf("Bc35 demo opened!");
    return 0;
}

INIT_APP_EXPORT(bc35_demo_init);

#endif
