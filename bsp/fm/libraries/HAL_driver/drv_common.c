/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-7      SummerGift   first version
 */

#include "drv_common.h"
#include "board.h"

/**
 * This function will initial board.
 */
int rt_hw_board_init(void)
{
    /* enable interrupt */
    /* System clock initialization */
    SystemClock_Config();
    FoutInit();
    /* disable interrupt */
    return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char *s, int num)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
    }
    /* USER CODE END Error_Handler */
}

static int rti_start(void)
{
    return 0;
}
INIT_EXPORT(rti_start, "0");

static int rti_board_start(void)
{
    return 0;
}
INIT_EXPORT(rti_board_start, "0.end");

static int rti_board_end(void)
{
    return 0;
}
INIT_EXPORT(rti_board_end, "1.end");

static int rti_end(void)
{
    return 0;
}
INIT_EXPORT(rti_end, "6.end");

#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int $Super$$main(void);
/* re-define main function */
int $Sub$$main(void)
{
    const init_fn_t *fn_ptr;
    rt_hw_board_init();
    for (fn_ptr = &__bs_init_rti_board_start; fn_ptr < &__bs_init_rti_board_end; fn_ptr++) {
        (*fn_ptr)();
    }
    for (fn_ptr = &__bs_init_rti_board_end; fn_ptr < &__bs_init_rti_end; fn_ptr ++) {
        (*fn_ptr)();
    }
    $Super$$main();
    return 0;
}
#elif defined(__ICCARM__)
extern int main(void);
/* __low_level_init will auto called by IAR cstartup */
extern void __iar_data_init3(void);
int __low_level_init(void)
{
    // call IAR table copy function.
    __iar_data_init3();
    rt_hw_board_init();
    for (fn_ptr = &__bs_init_rti_board_start; fn_ptr < &__bs_init_rti_board_end; fn_ptr++) {
        (*fn_ptr)();
    }
    for (fn_ptr = &__bs_init_rti_board_end; fn_ptr < &__bs_init_rti_end; fn_ptr ++) {
        (*fn_ptr)();
    }
    main();
    return 0;
}
#elif defined(__GNUC__)
extern int main(void);
/* Add -eentry to arm-none-eabi-gcc argument */
int entry(void)
{
    rt_hw_board_init();
    for (fn_ptr = &__bs_init_rti_board_start; fn_ptr < &__bs_init_rti_board_end; fn_ptr++) {
        (*fn_ptr)();
    }
    for (fn_ptr = &__bs_init_rti_board_end; fn_ptr < &__bs_init_rti_end; fn_ptr ++) {
        (*fn_ptr)();
    }
    main();
    return 0;
}
#endif
