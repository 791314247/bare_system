/*
 * Copyright (c) 2006-2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __BARE_SYS_H_
#define __BARE_SYS_H_

#define BS_ASSERT(EX)

#include <bsconfig.h>
#include <bsdef.h>

/*
 * device (I/O) system interface
 */
bs_device_t bs_device_find(const char *name);
bs_err_t bs_device_register(bs_device_t dev,
                            const char *name,
                            bs_uint16_t flags);

/*
 * kernel object interface
 */
struct bs_object *bs_object_find_node_from_name(const char *name);
void bs_object_init(struct bs_object *object, const char *name);


/*
 * standard function interface
 */
void *bs_memset(void *src, int c, bs_ubase_t n);
void *bs_memcpy(void *dest, const void *src, bs_ubase_t n);

bs_int32_t bs_strncmp(const char *cs, const char *ct, bs_ubase_t count);
bs_int32_t bs_strcmp(const char *cs, const char *ct);
bs_size_t bs_strlen(const char *src);
bs_size_t bs_strnlen(const char *s, bs_ubase_t maxlen);
char *bs_strdup(const char *s);
char *bs_strstr(const char *str1, const char *str2);
bs_int32_t bs_sscanf(const char *buf, const char *fmt, ...);
char *bs_strncpy(char *dest, const char *src, bs_ubase_t n);
void *bs_memmove(void *dest, const void *src, bs_ubase_t n);
bs_int32_t bs_memcmp(const void *cs, const void *ct, bs_ubase_t count);
bs_uint32_t bs_strcasecmp(const char *a, const char *b);





#endif /* !1__SOFT_TIMER_LIST_H */
