/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __ADC_H__
#define __ADC_H__
#include <bare_sys.h>
#include <bsdevice.h>

/* Control command */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL0_VALUE  (1) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL1_VALUE  (2) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL2_VALUE  (3) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL3_VALUE  (4) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL4_VALUE  (5) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL5_VALUE  (6) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL6_VALUE  (7) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL7_VALUE  (8) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL8_VALUE  (9) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL9_VALUE  (10) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL10_VALUE  (11) /* Gets the ADC conversion value */
#define BS_DEVICE_CTRL_ADC_GET_CHANNEL11_VALUE  (12) /* Gets the ADC conversion value */



struct bs_adc_device;
struct bs_adc_ops {
    bs_err_t (*init)(struct bs_adc_device *device);
    bs_err_t (*control)(struct bs_adc_device *device, int cmd, void *args);
};

struct bs_adc_device {
    struct bs_device parent;
    const struct bs_adc_ops *ops;
};
typedef struct bs_adc_device *bs_adc_device_t;

typedef enum {
    BS_ADC_CMD_ENABLE,
    BS_ADC_CMD_DISABLE,
} bs_adc_cmd_t;

bs_err_t bs_hw_adc_register(bs_adc_device_t adc, const char *name, const struct bs_adc_ops *ops, const void *user_data);

bs_uint32_t bs_adc_read(bs_adc_device_t dev, bs_uint32_t channel);
bs_err_t bs_adc_enable(bs_adc_device_t dev, bs_uint32_t channel);
bs_err_t bs_adc_disable(bs_adc_device_t dev, bs_uint32_t channel);

#endif /* __ADC_H__ */
