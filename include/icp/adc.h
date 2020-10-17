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

struct bs_adc_device;
struct bs_adc_ops
{
    bs_err_t (*enabled)(struct bs_adc_device *device, bs_uint32_t channel, bs_bool_t enabled);
    bs_err_t (*convert)(struct bs_adc_device *device, bs_uint32_t channel, bs_uint32_t *value);
};

struct bs_adc_device
{
    struct bs_device parent;
    const struct bs_adc_ops *ops;
};
typedef struct bs_adc_device *bs_adc_device_t;

typedef enum
{
    BS_ADC_CMD_ENABLE,
    BS_ADC_CMD_DISABLE,
} bs_adc_cmd_t;

bs_err_t bs_hw_adc_register(bs_adc_device_t adc,const char *name, const struct bs_adc_ops *ops, const void *user_data);

bs_uint32_t bs_adc_read(bs_adc_device_t dev, bs_uint32_t channel);
bs_err_t bs_adc_enable(bs_adc_device_t dev, bs_uint32_t channel);
bs_err_t bs_adc_disable(bs_adc_device_t dev, bs_uint32_t channel);

#endif /* __ADC_H__ */
