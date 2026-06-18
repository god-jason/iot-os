/**
 * @file
 * DNS API
 */

/**
 * lwip DNS resolver header file.

 * Author: Jim Pettinato
 *   April 2007

 * ported from uIP resolv.c Copyright (c) 2002-2003, Adam Dunkels.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LWIP_HDR_DNS_H
#define LWIP_HDR_DNS_H

#include "lwip/opt.h"

#if LWIP_DNS

#include "lwip/ip_addr.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** DNS timer period */
#define DNS_TMR_INTERVAL          1000

/* DNS resolve types: */
#define LWIP_DNS_ADDRTYPE_IPV4      0
#define LWIP_DNS_ADDRTYPE_IPV6      1
#define LWIP_DNS_ADDRTYPE_IPV4_IPV6 2 /* try to resolve IPv4 first, try IPv6 if IPv4 fails only */
#define LWIP_DNS_ADDRTYPE_IPV6_IPV4 3 /* try to resolve IPv6 first, try IPv4 if IPv6 fails only */
#if LWIP_IPV4 && LWIP_IPV6
#ifndef LWIP_DNS_ADDRTYPE_DEFAULT
#define LWIP_DNS_ADDRTYPE_DEFAULT   LWIP_DNS_ADDRTYPE_IPV4_IPV6
#endif
#elif LWIP_IPV4
#define LWIP_DNS_ADDRTYPE_DEFAULT   LWIP_DNS_ADDRTYPE_IPV4
#else
#define LWIP_DNS_ADDRTYPE_DEFAULT   LWIP_DNS_ADDRTYPE_IPV6
#endif

#define DNS_MAX_ADDR_ANSWER         4  //mod by cmiot begin
#if (LWIP_DNS_ADDRTYPE_DEFAULT == LWIP_DNS_ADDRTYPE_IPV6_IPV4)
#define LWIP_DNS_SERVER_IPV6_PRI_IDX 0
#define LWIP_DNS_SERVER_IPV6_SEC_IDX 1
#define LWIP_DNS_SERVER_IPV4_PRI_IDX 2
#define LWIP_DNS_SERVER_IPV4_SEC_IDX 3
#elif (LWIP_DNS_ADDRTYPE_DEFAULT == LWIP_DNS_ADDRTYPE_IPV4_IPV6)
#define LWIP_DNS_SERVER_IPV4_PRI_IDX 0
#define LWIP_DNS_SERVER_IPV4_SEC_IDX 1
#define LWIP_DNS_SERVER_IPV6_PRI_IDX 2
#define LWIP_DNS_SERVER_IPV6_SEC_IDX 3
#endif
#define LWIP_DNS_SERVER_MAX_IDX      4


#if DNS_LOCAL_HOSTLIST
/** struct used for local host-list */
struct local_hostlist_entry {
  /** static hostname */
  const char *name;
  /** static host address in network byteorder */
  ip_addr_t addr;
  struct local_hostlist_entry *next;
};
#define DNS_LOCAL_HOSTLIST_ELEM(name, addr_init) {name, addr_init, NULL}
#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
#ifndef DNS_LOCAL_HOSTLIST_MAX_NAMELEN
#define DNS_LOCAL_HOSTLIST_MAX_NAMELEN  DNS_MAX_NAME_LENGTH
#endif
#define LOCALHOSTLIST_ELEM_SIZE ((sizeof(struct local_hostlist_entry) + DNS_LOCAL_HOSTLIST_MAX_NAMELEN + 1))
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
#endif /* DNS_LOCAL_HOSTLIST */

#if LWIP_IPV4
extern const ip_addr_t dns_mquery_v4group;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
extern const ip_addr_t dns_mquery_v6group;
#endif /* LWIP_IPV6 */

#define DNS_MAX_ADDR_ANSWER    4

/** Callback which is invoked when a hostname is found.
 * A function of this type must be implemented by the application using the DNS resolver.
 * @param name pointer to the name that was looked up.
 * @param ipaddr pointer to an ip_addr_t containing the IP address of the hostname,
 *        or NULL if the name could not be found (or on any other error).
 * @param callback_arg a user-specified callback argument passed to dns_gethostbyname
*/
typedef void (*dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

void             dns_init(void);
void             dns_tmr(void);
void             dns_setserver(u8_t numdns, const ip_addr_t *dnsserver);
const ip_addr_t* dns_getserver(u8_t numdns);

/**
  扩展dns server api
  支持根据PDP获取和配置dns server
*/
void             dns_setserver_pdp(u8_t pdp_id, u8_t numdns, const ip_addr_t *dnsserver);
const ip_addr_t* dns_getserver_pdp(u8_t pdp_id, u8_t numdns);
void             dns_setdefaultserver_pdp(u8_t pdp_id);

enum DNS_SERVER_PRO{
    DNS_SERVER_PRO_PDP_FIRST  = 0,    /*   PDP Server   优先(默认值) */
    DNS_SERVER_PRO_USER_FIRST,        /*   User Server   优先 */
    DNS_SERVER_PRO_PDP_ONLY,          /*   仅    PDP Server */
    DNS_SERVER_PRO_USER_ONLY,         /*   仅    User Server */
    DNS_SERVER_PRO_MAX
};
struct dns_configure {
  u8_t server_priority; //  define in enum DNS_SERVER_PRO
};
err_t dns_set_cfg(struct dns_configure cfg);
err_t dns_get_cfg(struct dns_configure *cfg);

err_t            dns_gethostbyname(const char *hostname, ip_addr_t *addr,
                                   dns_found_callback found, void *callback_arg);
err_t            dns_gethostbyname_addrtype(const char *hostname, ip_addr_t *addr,
                                   dns_found_callback found, void *callback_arg,
                                   u8_t dns_addrtype);


#if DNS_LOCAL_HOSTLIST
size_t         dns_local_iterate(dns_found_callback iterator_fn, void *iterator_arg);
err_t          dns_local_lookup(const char *hostname, ip_addr_t *addr, u8_t dns_addrtype);
#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
int            dns_local_removehost(const char *hostname, const ip_addr_t *addr);
err_t          dns_local_addhost(const char *hostname, const ip_addr_t *addr);
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
#endif /* DNS_LOCAL_HOSTLIST */

u8_t dns_addrtype_get_by_netif(const struct netif *netif);
err_t dns_gethostbyname_addrtype_extend(const char *hostname, ip_addr_t *addr, u8_t pdp_id,
                                                    dns_found_callback found, void *callback_arg,
                                                    u8_t dns_addrtype);
/**
 ************************************************************************************
 * @brief           获取域名IP的TTL值
 *
 * @param[in]       hostname         域名
 * @param[in/out]   addr             域名对应的IP地址
 *
 * @return  dns缓存中的ttl值
 ************************************************************************************
*/
u32_t dns_gethost_ttl(const char *hostname, const ip_addr_t *addr);

/**
 ************************************************************************************
 * @brief           获取域名的所有IP
 *
 * @param[in]       hostname         域名
 * @param[in/out]   addr             域名对应的IP地址
 * @param[in]       addr_count       addr对应的ip_addr_t个数
 * @param[in]       pdp_id           指定的PDP值,用于使用PDP的Server地址并从这个PDP收发DNS报文
 * @param[in]       found            回调函数
 * @param[in]       callback_arg     调用found回调函数时的用户参数
 * @param[in]       dns_addrtype     地址类型
 *
 * @return
 ************************************************************************************
*/
err_t
dns_gethostbyname_addrtype_all(const char *hostname, ip_addr_t *addr, u32_t addr_count, u8_t pdp_id,
                                            dns_found_callback found, void *callback_arg, u8_t dns_addrtype);

/* start of porting CMIOT code */
int dns_gettimeout(void);
void dns_settimeout(int t);
void dns_clean_entries(void);
void dns_setmaxretries(u8_t maxretries);
u8_t dns_getmaxretries(void);
/* end of porting CMIOT code */

/**
 ************************************************************************************
 * @brief           设置 DNS 解析地址优先级
 *
 * @param[in]       dns_priority      DNS 解析优先级
 *
 * @return  void
 ************************************************************************************
*/
void
dns_addr_priority_set(u8_t dns_priority);

/**
 ************************************************************************************
 * @brief           获取 DNS 解析地址优先级
 *
 * @param[in]       void
 *
 * @return  DNS 解析地址优先级
 ************************************************************************************
*/
u8_t
dns_addr_priority_get(void);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_DNS */

#endif /* LWIP_HDR_DNS_H */
