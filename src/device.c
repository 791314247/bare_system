/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <bsdevice.h>

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or BS_NULL on failure.
 */
bs_device_t bs_device_find(const char *name)
{
    struct bs_object *object;
    struct bs_list_node *node;
    struct bs_object_information *information;

    /* enter critical */
    if (bs_thread_self() != BS_NULL)
        bs_enter_critical();

    /* try to find device object */
    information = bs_object_get_information(BS_Object_Class_Device);
    BS_ASSERT(information != BS_NULL);
    for (node  = information->object_list.next;
         node != &(information->object_list);
         node  = node->next) {
        object = bs_list_entry(node, struct bs_object, list);
        if (bs_strncmp(object->name, name, BS_NAME_MAX) == 0) {
            /* leave critical */
            if (bs_thread_self() != BS_NULL)
                bs_exit_critical();

            return (bs_device_t)object;
        }
    }

    /* leave critical */
    if (bs_thread_self() != BS_NULL)
        bs_exit_critical();

    /* not found */
    return BS_NULL;
}

/**
 * This function registers a device driver with specified name.
 *
 * @param dev the pointer of device driver structure
 * @param name the device driver's name
 * @param flags the capabilities flag of device
 *
 * @return the error code, BS_EOK on initialization successfully.
 */
bs_err_t bs_device_register(bs_device_t dev,
                            const char *name,
                            bs_uint16_t flags)
{
    if (dev == BS_NULL)
        return -BS_ERROR;

    if (bs_device_find(name) != BS_NULL)
        return -BS_ERROR;

    bs_object_init(&(dev->parent), BS_Object_Class_Device, name);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

#if defined(BS_USING_POSIX)
    dev->fops = BS_NULL;
    bs_wqueue_init(&(dev->wait_queue));
#endif

    return BS_EOK;
}





