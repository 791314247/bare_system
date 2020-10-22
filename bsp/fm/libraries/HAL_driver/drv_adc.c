/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <board.h>
#include <bsdevice.h>

#if defined(BSP_USING_ADC)

struct bs_adc_device adc_obj;

static void fm_adc_gpio_init(void)
{
    LL_GPIO_InitTypeDef         GPIO_InitStruct;

    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = DISABLE;
    GPIO_InitStruct.RemapPin   = DISABLE;
#ifdef BSP_USING_ADC0
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_9;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC1
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_10;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC2
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_11;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC3
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_0;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC4
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_1;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC5
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_2;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC6
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_13;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC7
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_14;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC8
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC9
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_8;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC10
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_ADC11
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
}

static bs_err_t fm_adc_init(bs_adc_device_t device)
{
    LL_ADC_CommonInitTypeDef    ADC_CommonInitStruct;
    LL_ADC_InitTypeDef          ADC_InitStruct ;

    ADC_CommonInitStruct.AdcClockSource    = LL_RCC_ADC_OPERATION_CLOCK_PRESCALLER_RCHF; //RCHF
    ADC_CommonInitStruct.AdcClockPrescaler = LL_RCC_ADC_OPERATION_CLOCK_PRESCALER_DIV8; //16·ÖÆµ
    LL_ADC_CommonInit(&ADC_CommonInitStruct);

    ADC_InitStruct.ADC_ContinuousConvMode  = LL_ADC_CONV_SINGLE;//µ¥´ÎÄ£Ê½
    ADC_InitStruct.ADC_AutoMode            = LL_ADC_SINGLE_CONV_MODE_AUTO;//×Ô¶¯
    ADC_InitStruct.ADC_ScanDirection       = LL_ADC_SEQ_SCAN_DIR_FORWARD;//Í¨µÀÕýÐòÉ¨Ãè
    ADC_InitStruct.ADC_ExternalTrigConv    = LL_ADC_EXT_TRIGGER_NONE;//½ûÖ¹´¥·¢ÐÅºÅ
    ADC_InitStruct.ADC_SamplingTrigSource  = LL_ADC_TRIG_EXT_PA8;//Ó²¼þ´¥·¢Ô´Ñ¡Ôñ
    ADC_InitStruct.ADC_OverrunMode         = LL_ADC_OVR_DATA_OVERWRITTEN;//¸²¸ÇÉÏ´ÎÊý¾Ý
    ADC_InitStruct.ADC_WaitMode            = LL_ADC_WAIT_MODE_WAIT;//µÈ´ý
    ADC_InitStruct.ADC_Channel_Swap_Wait   = LL_ADC_SAMPLEING_INTERVAL_11_CYCLES;//Í¨µÀÇÐ»»µÈ´ýÊ±¼ä
    ADC_InitStruct.ADC_Channel_Fast_Time   = LL_ADC_FAST_CH_SAMPLING_TIME_4_ADCCLK;//¿ìËÙÍ¨µÀ²ÉÑùÊ±¼ä
    ADC_InitStruct.ADC_Channel_Slow_Time   = LL_ADC_SLOW_CH_SAMPLING_TIME_192_ADCCLK;//ÂýËÙÍ¨µÀ²ÉÑùÊ±¼ä
    ADC_InitStruct.ADC_Oversampling        = DISABLE;//¹ý²ÉÑù¹Ø±Õ
    ADC_InitStruct.ADC_OverSampingRatio    = LL_ADC_OVERSAMPLING_8X;//8±¶¹ý²ÉÑù
    ADC_InitStruct.ADC_OversamplingShift   = LL_ADC_OVERSAMPLING_RESULT_DIV8;//Êý¾ÝÓÒÒÆ, /8
    if (LL_ADC_Init(ADC, &ADC_InitStruct))
        return BS_EOK;
    else
        return BS_ERROR;
}


static bs_uint32_t fm_adc_get_channel(bs_uint32_t channel)
{
    bs_uint32_t fm_channel = 0;

    switch (channel) {
    case  0:
        fm_channel = LL_ADC_EXTERNAL_CH_0;
        break;
    case  1:
        fm_channel = LL_ADC_EXTERNAL_CH_1;
        break;
    case  2:
        fm_channel = LL_ADC_EXTERNAL_CH_2;
        break;
    case  3:
        fm_channel = LL_ADC_EXTERNAL_CH_3;
        break;
    case  4:
        fm_channel = LL_ADC_EXTERNAL_CH_4;
        break;
    case  5:
        fm_channel = LL_ADC_EXTERNAL_CH_5;
        break;
    case  6:
        fm_channel = LL_ADC_EXTERNAL_CH_6;
        break;
    case  7:
        fm_channel = LL_ADC_EXTERNAL_CH_7;
        break;
    case  8:
        fm_channel = LL_ADC_EXTERNAL_CH_8;
        break;
    case  9:
        fm_channel = LL_ADC_EXTERNAL_CH_9;
        break;
    case 10:
        fm_channel = LL_ADC_EXTERNAL_CH_10;
        break;
    case 11:
        fm_channel = LL_ADC_EXTERNAL_CH_11;
        break;
    }

    return fm_channel;
}

static bs_uint32_t fm_get_adc_value(bs_uint32_t channel)
{
    uint16_t adc_value = 0;
    bs_uint32_t fm_channel = 0;

    fm_channel = fm_adc_get_channel(channel);
    LL_RCC_SetADCPrescaler(LL_RCC_ADC_OPERATION_CLOCK_PRESCALER_DIV1);
    LL_ADC_EnalbleSequencerChannel(ADC, fm_channel);
    LL_ADC_ClearFlag_EOC(ADC);
    LL_ADC_Enable(ADC);
    LL_ADC_StartConversion(ADC);

    while (LL_ADC_IsActiveFlag_EOC(ADC) == RESET);
    LL_ADC_ClearFlag_EOC(ADC);
    adc_value = LL_ADC_ReadConversionData12(ADC);

    LL_ADC_Disable(ADC);
    LL_ADC_DisableSequencerChannel(ADC, fm_channel);

    return (bs_uint32_t)adc_value;
}


static bs_err_t fm_adc_control(bs_watchdog_t *wdt, int cmd, void *arg)
{
    uint32_t *adc_value = (uint32_t *)arg;

    switch (cmd) {
    /* feed the watchdog */
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL0_VALUE:
        *adc_value = fm_get_adc_value(0);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL1_VALUE:
        *adc_value = fm_get_adc_value(1);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL2_VALUE:
        *adc_value = fm_get_adc_value(2);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL3_VALUE:
        *adc_value = fm_get_adc_value(3);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL4_VALUE:
        *adc_value = fm_get_adc_value(4);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL5_VALUE:
        *adc_value = fm_get_adc_value(5);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL6_VALUE:
        *adc_value = fm_get_adc_value(6);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL7_VALUE:
        *adc_value = fm_get_adc_value(7);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL8_VALUE:
        *adc_value = fm_get_adc_value(8);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL9_VALUE:
        *adc_value = fm_get_adc_value(9);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL10_VALUE:
        *adc_value = fm_get_adc_value(10);
        break;
    case BS_DEVICE_CTRL_ADC_GET_CHANNEL11_VALUE:
        *adc_value = fm_get_adc_value(11);
        break;
    default:
        bs_kprintf("This command is not supported.");
        return -BS_ERROR;
    }
    return BS_EOK;
}


static const struct bs_adc_ops fm_adc_ops = {
    .init = fm_adc_init,
    .control = fm_adc_control,
};

static int fm_adc_init(void)
{
    fm_adc_gpio_init();
    bs_hw_adc_register(&adc_obj, "ADC", &fm_adc_ops, BS_NULL);
    return BS_EOK;
}
INIT_BOARD_EXPORT(stm32_adc_init);

#endif /* BSP_USING_ADC */
