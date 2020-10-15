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
#include <board.h>
#include "main.h"
#include "sleep.h"
#include "bsdevice.h"



int main(void)
{
    while (1) {
        soft_timer_main_loop();
        //DeepSleep();
    }
}


