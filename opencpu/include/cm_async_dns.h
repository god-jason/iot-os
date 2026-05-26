/**
 * @file        cm_async_dns.h
 * @brief       异步DNS解析接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By wangk
 * @date        2021/08/16
 *
 * @defgroup async_dns async_dns
 * @ingroup NET
 * @{
 */

#ifndef __CM_ASYNC_DNS_H__
#define __CM_ASYNC_DNS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
// 包含平台特定的socket头文件
#include "lwip/sockets.h" // lwip socket
#include "cm_eloop.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** DNS resolve types */
typedef enum
{
    CM_ASYNC_DNS_ADDRTYPE_IPV4 = 0,  /**< resolve IPv4 address */
    CM_ASYNC_DNS_ADDRTYPE_IPV6,      /**< resolve IPv6 address */
    CM_ASYNC_DNS_ADDRTYPE_IPV4_IPV6, /**< try to resolve IPv4 first, try IPv6 if IPv4 fails only */
    CM_ASYNC_DNS_ADDRTYPE_IPV6_IPV4, /**< try to resolve IPv6 first, try IPv4 if IPv6 fails only */
} cm_async_dns_addrtype_e;

/** 异步DNS解析事件类型 */
typedef enum
{
    CM_ASYNC_DNS_EV_NONE = 0,
    // responses
    CM_ASYNC_DNS_RESOLVE_OK,   /**< 域名解析成功 */
    CM_ASYNC_DNS_RESOLVE_FAIL, /**< 域名解析失败 */
} cm_async_dns_event_e;

/** IP地址(异步DNS解析结果)类型 */
typedef struct cm_async_dns_ip_addr
{
    cm_async_dns_addrtype_e type; /**< 地址类型(IPv4或IPv6) */
    union {
        struct in_addr sin_addr;   /**< IPv4 address */
        struct in6_addr sin6_addr; /**< IPv6 address */
    } u_addr;
} cm_async_dns_ip_addr_t;

/**
  异步DNS解析事件回调函数类型
  @param req_id     [in] 请求ID
  @param event      [in] 事件类型
  @param cb_param   [in] 用户参数(创建socket时指定)
  @param host_name  [in] 域名
  @param ip_addr    [in] IP地址(DNS解析结果)
*/
typedef void (*cm_async_dns_event_cb)(int req_id, cm_async_dns_event_e event, void *cb_param,
                                      const char *host_name, const cm_async_dns_ip_addr_t *ip_addr);

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/**
 * @brief 获取异步dns模块运行的Event Loop
 *
 * @return  
 *   本模块运行的Event Loop
 *
 * @details 获取本模块运行的Event Loop(初始化传入或者系统默认Event Loop)
 */
cm_eloop_handle_t cm_async_dns_eloop(void);

/**
 * @brief 设置DNS解析全局优先级
 *
 * @param [in]  priority DNS解析优先级，0：v4优先，1：v6优先，默认v6优先
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details 开机默认v6优先，若使用业务(MQTT/HTTP/NTP)时单独配置了通道优先级，则全局优先级对该业务通道将不生效
 */
int32_t cm_async_dns_set_priority(uint8_t priority);

/**
 * @brief 请求执行异步DNS解析
 *
 * @param [in]  host_name    域名
 * @param [in]  addr_type    地址类型(IPV4或IPV6)
 * @param [out] ip_addr      IP地址(DNS解析结果)
 * @param [in]  event_cb     异步事件回调函数
 * @param [in]  cb_param     用户参数(将作为参数传入event_cb)
 *
 * @return  
 *   > 0  - DNS请求成功,返回异步DNS请求ID(等待回调函数传回执行结果) \n
 *   = 0  - DNS请求已完成(result输出解析结果) \n
 *   = -1 - 请求失败
 *
 * @details 请求执行异步DNS解析
 */
int cm_async_dns_request(const char *host_name, cm_async_dns_addrtype_e addr_type,
                         cm_async_dns_ip_addr_t *ip_addr, cm_async_dns_event_cb event_cb, void *cb_param);
#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_ASYNC_DNS_H__ */

/** @}*/
