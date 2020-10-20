/*
 * COPYRIGHT (C) 2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __AT_BASE_H
#define __AT_BASE_H

#include <bare_sys.h>
#include <bsdevice.h>

#ifndef BS_USING_AT_SERIAL
#define BS_USING_AT_SERIAL          "uart1"
#endif

bs_err_t at_base_send(const char *command,
                         const char *exec,
                         bs_uint8_t wait_time,
                         bs_uint8_t resend_num);
char *at_base_read_recv(const char *exec);
void at_base_clean_recv(void);


#endif /*__AT_BASE_H*/
