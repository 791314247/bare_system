/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "adc.h"



static bs_size_t _adc_read(bs_device_t dev, bs_off_t pos, void *buffer, bs_size_t size)
{
    bs_err_t result = BS_EOK;
    bs_size_t i;
    struct bs_adc_device *adc = (struct bs_adc_device *)dev;
    bs_uint32_t *value = (bs_uint32_t *)buffer;

    for (i = 0; i < size; i += sizeof(int))
    {
        result = adc->ops->convert(adc, pos + i, value);
        if (result != BS_EOK)
        {
            return 0;
        }
        value++;
    }

    return i;
}

static bs_err_t _adc_control(bs_device_t dev, int cmd, void *args)
{
    bs_err_t result = BS_EOK;
    bs_adc_device_t adc = (struct bs_adc_device *)dev;

    if (adc->ops->enabled == BS_NULL)
    {
        return -BS_ENOSYS;
    }
    if (cmd == BS_ADC_CMD_ENABLE)
    {
        result = adc->ops->enabled(adc, (bs_uint32_t)args, BS_TRUE);
    }
    else if (cmd == BS_ADC_CMD_DISABLE)
    {
        result = adc->ops->enabled(adc, (bs_uint32_t)args, BS_FALSE);
    }

    return result;
}

const static struct bs_device_ops adc_ops =
{
    BS_NULL,
    BS_NULL,
    BS_NULL,
    _adc_read,
    BS_NULL,
    _adc_control,
};


bs_err_t bs_hw_adc_register(bs_adc_device_t device, const char *name, const struct bs_adc_ops *ops, const void *user_data)
{
    bs_err_t result = BS_EOK;
    BS_ASSERT(ops != BS_NULL && ops->convert != BS_NULL);

    device->parent.type = BS_Device_Class_Miscellaneous;
    device->parent.rx_indicate = BS_NULL;
    device->parent.tx_complete = BS_NULL;
    device->parent.ops         = &adc_ops;
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    result = bs_device_register(&device->parent, name, BS_DEVICE_FLAG_RDWR);

    return result;
}

bs_uint32_t bs_adc_read(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_uint32_t value;

    BS_ASSERT(dev);

    dev->ops->convert(dev, channel, &value);

    return value;
}

bs_err_t bs_adc_enable(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_err_t result = BS_EOK;

    BS_ASSERT(dev);
    if (dev->ops->enabled != BS_NULL)
    {
        result = dev->ops->enabled(dev, channel, BS_TRUE);
    }
    else
    {
        result = -BS_ENOSYS;
    }

    return result;
}

bs_err_t bs_adc_disable(bs_adc_device_t dev, bs_uint32_t channel)
{
    bs_err_t result = BS_EOK;

    BS_ASSERT(dev);
    if (dev->ops->enabled != BS_NULL)
    {
        result = dev->ops->enabled(dev, channel, BS_FALSE);
    }
    else
    {
        result = -BS_ENOSYS;
    }

    return result;
}

#ifdef FINSH_USING_MSH

static int adc(int argc, char **argv)
{
    int value = 0;
    int result = BS_EOK;
    static bs_adc_device_t adc_device = BS_NULL;
    char *result_str;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc == 3)
            {
                adc_device = (bs_adc_device_t)bs_device_find(argv[2]);
                result_str = (adc_device == BS_NULL) ? "failure" : "success";
                bs_kprintf("probe %s %s \n", argv[2], result_str);
            }
            else
            {
                bs_kprintf("adc probe <adc_name>   - probe adc by name\n");
            }
        }
        else
        {
            if (adc_device == BS_NULL)
            {
                bs_kprintf("Please using 'adc probe <adc_name>' first\n");
                return -BS_ERROR;
            }
            if (!strcmp(argv[1], "enable"))
            {
                if (argc == 3)
                {
                    result = bs_adc_enable(adc_device, atoi(argv[2]));
                    result_str = (result == BS_EOK) ? "success" : "failure";
                    bs_kprintf("%s channel %d enables %s \n", adc_device->parent.parent.name, atoi(argv[2]), result_str);
                }
                else
                {
                    bs_kprintf("adc enable <channel>   - enable adc channel\n");
                }
            }
            else if (!strcmp(argv[1], "read"))
            {
                if (argc == 3)
                {
                    value = bs_adc_read(adc_device, atoi(argv[2]));
                    bs_kprintf("%s channel %d  read value is 0x%08X \n", adc_device->parent.parent.name, atoi(argv[2]), value);
                }
                else
                {
                    bs_kprintf("adc read <channel>     - read adc value on the channel\n");
                }
            }
            else if (!strcmp(argv[1], "disable"))
            {
                if (argc == 3)
                {
                    result = bs_adc_disable(adc_device, atoi(argv[2]));
                    result_str = (result == BS_EOK) ? "success" : "failure";
                    bs_kprintf("%s channel %d disable %s \n", adc_device->parent.parent.name, atoi(argv[2]), result_str);
                }
                else
                {
                    bs_kprintf("adc disable <channel>  - disable adc channel\n");
                }
            }
            else
            {
                bs_kprintf("Unknown command. Please enter 'adc' for help\n");
            }
        }
    }
    else
    {
        bs_kprintf("Usage: \n");
        bs_kprintf("adc probe <adc_name>   - probe adc by name\n");
        bs_kprintf("adc read <channel>     - read adc value on the channel\n");
        bs_kprintf("adc disable <channel>  - disable adc channel\n");
        bs_kprintf("adc enable <channel>   - enable adc channel\n");
        result = -BS_ERROR;
    }
    return BS_EOK;
}
MSH_CMD_EXPORT(adc, adc function);

#endif /* FINSH_USING_MSH */
