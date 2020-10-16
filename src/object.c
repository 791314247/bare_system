/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <bare_sys.h>

static struct bs_object *head_point = BS_NULL;


/**
 * This function will return the specified type of object information.
 * @return the object type information or BS_NULL
 */
static struct bs_object *bs_object_find_node(struct bs_object *node)
{
    struct bs_object *list;
    BS_ASSERT(node != BS_NULL)

    list = head_point;
    while (list != BS_NULL) {
        if (list == node)
            return list;
        list = list->next;
    }
    return BS_NULL;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
static void bs_list_insert_after(struct bs_object *node)
{
    struct bs_object *list;   //list保存当前需要检查的节点的地址
    BS_ASSERT(node != BS_NULL)

    if (bs_object_find_node(node) != BS_NULL) {
        return;
    }
    //当头节点为空时，将传入的节点作为头节点，返回头节点
    if (head_point == BS_NULL)         {
        head_point = node;
        node->next = BS_NULL;
        return;
    }
    list = head_point;
    while (list->next != BS_NULL) {
        list = list->next;       //后移一个节点
    }
    list->next = node; //将该节点插入链表的末尾
    node->next = BS_NULL;
}


/**
 * This function will return the specified type of object information.
 * @return the object type information or BS_NULL
 */
struct bs_object *bs_object_find_node_from_name(const char *name)
{
    struct bs_object *list;
    BS_ASSERT(name != BS_NULL)

    list = head_point;
    while (list != BS_NULL) {
        if (bs_strncmp(list->name, name, BS_NAME_MAX) == 0)
            return list;
        list = list->next;
    }
    return BS_NULL;
}


/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param object the specified object to be initialized.
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
void bs_object_init(struct bs_object *object, const char *name)
{
    BS_ASSERT(object != BS_NULL);

    /* try to find object */
    if (BS_NULL != bs_object_find_node(object)) {
        bs_kprintf("This node:%s already exists!\r\n", object->name);
        BS_ASSERT(0);
        return;
    }

    /* initialize object's parameters */
    bs_strncpy(object->name, name, BS_NAME_MAX);

    /* insert object into information object list */
    bs_list_insert_after(object);
}




/**@}*/
