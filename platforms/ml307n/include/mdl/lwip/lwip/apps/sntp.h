/**
 * @file
 * SNTP client API
 */

/*
 * Copyright (c) 2007-2009 Frédéric Bernon, Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Frédéric Bernon, Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_APPS_SNTP_H
#define LWIP_HDR_APPS_SNTP_H

#include "lwip/apps/sntp_opts.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SNTP operating modes: default is to poll using unicast.
   The mode has to be set before calling sntp_init(). */
#define SNTP_OPMODE_POLL            0
#define SNTP_OPMODE_LISTENONLY      1
void sntp_setoperatingmode(u8_t operating_mode);
u8_t sntp_getoperatingmode(void);

void sntp_init(void);
void sntp_stop(void);
u8_t sntp_enabled(void);

void sntp_setserver(u8_t idx, const ip_addr_t *addr);
const ip_addr_t* sntp_getserver(u8_t idx);

#if SNTP_MONITOR_SERVER_REACHABILITY
u8_t sntp_getreachability(u8_t idx);
#endif /* SNTP_MONITOR_SERVER_REACHABILITY */

#if SNTP_SERVER_DNS
void sntp_setservername(u8_t idx, const char *server);
const char *sntp_getservername(u8_t idx);
#endif /* SNTP_SERVER_DNS */

#if SNTP_GET_SERVERS_FROM_DHCP
void sntp_servermode_dhcp(int set_servers_from_dhcp);
#else /* SNTP_GET_SERVERS_FROM_DHCP */
#define sntp_servermode_dhcp(x)
#endif /* SNTP_GET_SERVERS_FROM_DHCP */

#define SNTP_API_SINGLE_CONTROL     1  /* AT 命令时需要传入 port 、timeout、回调等参数, 为了不改动 sntp.c 中原有的函数, 重新封装函数给 AT 应用调用 */
#if SNTP_API_SINGLE_CONTROL
typedef void (*sntp_reply_cb)(u8_t user_id, s32_t reply_time);
void sntp_regist_callback(sntp_reply_cb callback);
void sntp_unregist_callback(void);
void sntp_stop_single(void);
s8_t sntp_init_single(u8_t user_id, u16_t port, u8_t timeout, sntp_reply_cb reply_cb, const struct netif *netif);
#endif
unsigned int sntp_get_state(void);
typedef int (*sntp_res_cb_t)(int sec, char *p_time);
int tms_sntp_async(char *p_sntp_server, int port, int is_update_sys_time,
                                 int timeout, sntp_res_cb_t result_cb);
#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_SNTP_H */
