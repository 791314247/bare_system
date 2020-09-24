/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     balanceTWK        first version
 * 2019-04-23     WillianChan       Fix GPIO serial number disorder
 * 2020-06-16     thread-liu        add STM32MP1
 */

#include <board.h>
#include "drv_gpio.h"

#ifdef RT_USING_PIN

static const struct pin_index pins[] = 
{
#if defined(GPIOA)
    __FM_PIN(0 ,  A, 0 ),
    __FM_PIN(1 ,  A, 1 ),
    __FM_PIN(2 ,  A, 2 ),
    __FM_PIN(3 ,  A, 3 ),
    __FM_PIN(4 ,  A, 4 ),
    __FM_PIN(5 ,  A, 5 ),
    __FM_PIN(6 ,  A, 6 ),
    __FM_PIN(7 ,  A, 7 ),
    __FM_PIN(8 ,  A, 8 ),
    __FM_PIN(9 ,  A, 9 ),
    __FM_PIN(10,  A, 10),
    __FM_PIN(11,  A, 11),
    __FM_PIN(12,  A, 12),
    __FM_PIN(13,  A, 13),
    __FM_PIN(14,  A, 14),
    __FM_PIN(15,  A, 15),
#if defined(GPIOB)
    __FM_PIN(16,  B, 0),
    __FM_PIN(17,  B, 1),
    __FM_PIN(18,  B, 2),
    __FM_PIN(19,  B, 3),
    __FM_PIN(20,  B, 4),
    __FM_PIN(21,  B, 5),
    __FM_PIN(22,  B, 6),
    __FM_PIN(23,  B, 7),
    __FM_PIN(24,  B, 8),
    __FM_PIN(25,  B, 9),
    __FM_PIN(26,  B, 10),
    __FM_PIN(27,  B, 11),
    __FM_PIN(28,  B, 12),
    __FM_PIN(29,  B, 13),
    __FM_PIN(30,  B, 14),
    __FM_PIN(31,  B, 15),
#if defined(GPIOC)
    __FM_PIN(32,  C, 0),
    __FM_PIN(33,  C, 1),
    __FM_PIN(34,  C, 2),
    __FM_PIN(35,  C, 3),
    __FM_PIN(36,  C, 4),
    __FM_PIN(37,  C, 5),
    __FM_PIN(38,  C, 6),
    __FM_PIN(39,  C, 7),
    __FM_PIN(40,  C, 8),
    __FM_PIN(41,  C, 9),
    __FM_PIN(42,  C, 10),
    __FM_PIN(43,  C, 11),
    __FM_PIN(44,  C, 12),
    __FM_PIN(45,  C, 13),
    __FM_PIN(46,  C, 14),
    __FM_PIN(47,  C, 15),
#if defined(GPIOD)
    __FM_PIN(48,  D, 0),
    __FM_PIN(49,  D, 1),
    __FM_PIN(50,  D, 2),
    __FM_PIN(51,  D, 3),
    __FM_PIN(52,  D, 4),
    __FM_PIN(53,  D, 5),
    __FM_PIN(54,  D, 6),
    __FM_PIN(55,  D, 7),
    __FM_PIN(56,  D, 8),
    __FM_PIN(57,  D, 9),
    __FM_PIN(58,  D, 10),
    __FM_PIN(59,  D, 11),
    __FM_PIN(60,  D, 12),
    __FM_PIN(61,  D, 13),
    __FM_PIN(62,  D, 14),
    __FM_PIN(63,  D, 15),
#if defined(GPIOE)
    __FM_PIN(64,  E, 0),
    __FM_PIN(65,  E, 1),
    __FM_PIN(66,  E, 2),
    __FM_PIN(67,  E, 3),
    __FM_PIN(68,  E, 4),
    __FM_PIN(69,  E, 5),
    __FM_PIN(70,  E, 6),
    __FM_PIN(71,  E, 7),
    __FM_PIN(72,  E, 8),
    __FM_PIN(73,  E, 9),
    __FM_PIN(74,  E, 10),
    __FM_PIN(75,  E, 11),
    __FM_PIN(76,  E, 12),
    __FM_PIN(77,  E, 13),
    __FM_PIN(78,  E, 14),
    __FM_PIN(79,  E, 15),
#if defined(GPIOF)
    __FM_PIN(80,  F, 0),
    __FM_PIN(81,  F, 1),
    __FM_PIN(82,  F, 2),
    __FM_PIN(83,  F, 3),
    __FM_PIN(84,  F, 4),
    __FM_PIN(85,  F, 5),
    __FM_PIN(86,  F, 6),
    __FM_PIN(87,  F, 7),
    __FM_PIN(88,  F, 8),
    __FM_PIN(89,  F, 9),
    __FM_PIN(90,  F, 10),
    __FM_PIN(91,  F, 11),
    __FM_PIN(92,  F, 12),
    __FM_PIN(93,  F, 13),
    __FM_PIN(94,  F, 14),
    __FM_PIN(95,  F, 15),
#if defined(GPIOG)
    __FM_PIN(96,  G, 0),
    __FM_PIN(97,  G, 1),
    __FM_PIN(98,  G, 2),
    __FM_PIN(99,  G, 3),
    __FM_PIN(100, G, 4),
    __FM_PIN(101, G, 5),
    __FM_PIN(102, G, 6),
    __FM_PIN(103, G, 7),
    __FM_PIN(104, G, 8),
    __FM_PIN(105, G, 9),
    __FM_PIN(106, G, 10),
    __FM_PIN(107, G, 11),
    __FM_PIN(108, G, 12),
    __FM_PIN(109, G, 13),
    __FM_PIN(110, G, 14),
    __FM_PIN(111, G, 15),
#if defined(GPIOH)
    __FM_PIN(112, H, 0),
    __FM_PIN(113, H, 1),
    __FM_PIN(114, H, 2),
    __FM_PIN(115, H, 3),
    __FM_PIN(116, H, 4),
    __FM_PIN(117, H, 5),
    __FM_PIN(118, H, 6),
    __FM_PIN(119, H, 7),
    __FM_PIN(120, H, 8),
    __FM_PIN(121, H, 9),
    __FM_PIN(122, H, 10),
    __FM_PIN(123, H, 11),
    __FM_PIN(124, H, 12),
    __FM_PIN(125, H, 13),
    __FM_PIN(126, H, 14),
    __FM_PIN(127, H, 15),
#if defined(GPIOI)
    __FM_PIN(128, I, 0),
    __FM_PIN(129, I, 1),
    __FM_PIN(130, I, 2),
    __FM_PIN(131, I, 3),
    __FM_PIN(132, I, 4),
    __FM_PIN(133, I, 5),
    __FM_PIN(134, I, 6),
    __FM_PIN(135, I, 7),
    __FM_PIN(136, I, 8),
    __FM_PIN(137, I, 9),
    __FM_PIN(138, I, 10),
    __FM_PIN(139, I, 11),
    __FM_PIN(140, I, 12),
    __FM_PIN(141, I, 13),
    __FM_PIN(142, I, 14),
    __FM_PIN(143, I, 15),
#if defined(GPIOJ)
    __FM_PIN(144, J, 0),
    __FM_PIN(145, J, 1),
    __FM_PIN(146, J, 2),
    __FM_PIN(147, J, 3),
    __FM_PIN(148, J, 4),
    __FM_PIN(149, J, 5),
    __FM_PIN(150, J, 6),
    __FM_PIN(151, J, 7),
    __FM_PIN(152, J, 8),
    __FM_PIN(153, J, 9),
    __FM_PIN(154, J, 10),
    __FM_PIN(155, J, 11),
    __FM_PIN(156, J, 12),
    __FM_PIN(157, J, 13),
    __FM_PIN(158, J, 14),
    __FM_PIN(159, J, 15),
#if defined(GPIOK)
    __FM_PIN(160, K, 0),
    __FM_PIN(161, K, 1),
    __FM_PIN(162, K, 2),
    __FM_PIN(163, K, 3),
    __FM_PIN(164, K, 4),
    __FM_PIN(165, K, 5),
    __FM_PIN(166, K, 6),
    __FM_PIN(167, K, 7),
    __FM_PIN(168, K, 8),
    __FM_PIN(169, K, 9),
    __FM_PIN(170, K, 10),
    __FM_PIN(171, K, 11),
    __FM_PIN(172, K, 12),
    __FM_PIN(173, K, 13),
    __FM_PIN(174, K, 14),
    __FM_PIN(175, K, 15),
#endif /* defined(GPIOK) */
#endif /* defined(GPIOJ) */
#endif /* defined(GPIOI) */
#endif /* defined(GPIOH) */
#endif /* defined(GPIOG) */
#endif /* defined(GPIOF) */
#endif /* defined(GPIOE) */
#endif /* defined(GPIOD) */
#endif /* defined(GPIOC) */
#endif /* defined(GPIOB) */
#endif /* defined(GPIOA) */
};


static struct rt_pin_irq_hdr pin_irq_hdr_tab[] =
{
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
};
static uint32_t pin_irq_enable_mask=0;

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
static const struct pin_index *get_pin(uint8_t pin)
{
    const struct pin_index *index;

    if (pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
        if (index->index == -1)
            index = RT_NULL;
    }
    else
    {
        index = RT_NULL;
    }

    return index;
};





static void fm_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    LL_GPIO_InitTypeDef  GPIO_InitStructure;
    
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
		
    GPIO_InitStructure.Pin = index->pin;
    GPIO_InitStructure.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStructure.Pull = DISABLE;
    
    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStructure.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStructure.Pull = DISABLE;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* output setting */
        GPIO_InitStructure.Mode = LL_GPIO_MODE_INPUT;
        GPIO_InitStructure.Pull = DISABLE;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* input setting: not pull. */
        GPIO_InitStructure.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStructure.Pull = DISABLE;
        GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStructure.Mode = LL_GPIO_MODE_INPUT;
        GPIO_InitStructure.Pull = ENABLE;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting:default. */
    }
    
    LL_GPIO_Init(index->gpio, &GPIO_InitStructure);
}

static void fm_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;
    
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
    if (value == PIN_LOW)
    {
        LL_GPIO_SetOutputPin(index->gpio, index->pin);
    }
    else if (value == PIN_HIGH)
    {
        LL_GPIO_ResetOutputPin(index->gpio, index->pin);
    }
}

static int fm_pin_read(rt_device_t dev, rt_base_t pin)
{
    rt_int32_t value = PIN_LOW;
    const struct pin_index *index;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }

    value = LL_GPIO_IsOutputPinSet(index->gpio, index->pin);

    return value;
}

static rt_err_t fm_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                                     rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
  
    return RT_EOK;
}

static rt_err_t fm_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{

    return RT_EOK;
}

static rt_err_t fm_pin_irq_enable(struct rt_device *device, rt_base_t pin,
                                     rt_uint32_t enabled)
{
 
    return RT_EOK;
}



const static struct rt_pin_ops am_pin_ops =
{
    fm_pin_mode,
    fm_pin_write,
    fm_pin_read,
    fm_pin_attach_irq,
    fm_pin_dettach_irq,
    fm_pin_irq_enable,
};


int rt_hw_pin_init(void)
{
   // RCC_PERCLK_SetableEx(PDCCLK, ENABLE);

    return rt_device_pin_register("pin", &am_pin_ops, RT_NULL);
}

#endif /* RT_USING_PIN */
