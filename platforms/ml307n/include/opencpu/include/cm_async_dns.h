/**
 * @file        cm_async_dns.h
 * @brief       异步DNS解析接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By cmiot1325
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

#define DNS_MAX_ADDR_ANSWER 4
#define DNS_MAX_SERVERS 4

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** DNS解析类型 */
typedef enum
{
    CM_ASYNC_DNS_ADDRTYPE_IPV4 = 0,  /**< 解析IPv4地址 */
    CM_ASYNC_DNS_ADDRTYPE_IPV6,      /**< 解析IPv6地址 */
    CM_ASYNC_DNS_ADDRTYPE_IPV4_IPV6, /**< 首先尝试解析IPv4，仅当IPv4解析失败时才尝试IPv6 */
    CM_ASYNC_DNS_ADDRTYPE_IPV6_IPV4, /**< 首先尝试解析IPv6，仅当IPv6解析失败时才尝试IPv4 */
    CM_ASYNC_DNS_ADDRTYPE_MAX,       /**< 错误的地址类型 */
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
        struct in_addr sin_addr;   /**< IPv4地址 */
        struct in6_addr sin6_addr; /**< IPv6地址 */
    } u_addr;
} cm_async_dns_ip_addr_t;

/**
  异步DNS解析事件回调函数类型
  @param req_id     [in] 请求ID
  @param event      [in] 事件类型
  @param cb_param   [in] 用户参数(创建socket时指定)
  @param host_name  [in] 域名
  @param ip_addr    [in] IP地址数组(DNS解析结果)

  @details ip_addr数组最大长度DNS_MAX_ADDR_ANSWER
           通过cm_async_dns_get_result_addr_num(ip_addr)获取实际结果个数
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
 * @details 系统默认Event Loop，底层已完成初始化
 */
cm_eloop_handle_t cm_async_dns_eloop(void);

/**
 * @brief 设置DNS服务器地址
 *
 * @param [in]  dns_index   DNS服务器下标(必须小于DNS_MAX_SERVERS)
 * @param [in]  dns_server  DNS服务器地址
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details
 */
int32_t cm_async_dns_set_server(uint8_t dns_index, const cm_async_dns_ip_addr_t *dns_server);

/**
 * @brief 获取DNS服务器地址
 *
 * @param [in]  dns_index   DNS服务器下标(必须小于DNS_MAX_SERVERS)
 * @param [out] dns_server   DNS服务器地址
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details
 */
int32_t cm_async_dns_get_server(uint8_t dns_index, cm_async_dns_ip_addr_t *dns_server);

/**
 * @brief 设置DNS请求最大重试次数
 *
 * @param [in]  max_retries DNS请求最大重试次数(1-9)
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details
 */
int32_t cm_async_dns_set_max_retries(uint8_t max_retries);

/**
 * @brief 获取DNS请求最大重试次数
 *
 * @param [out]  max_retries DNS请求最大重试次数
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details
 */
int32_t cm_async_dns_get_max_retries(uint8_t *max_retries);

/**
 * @brief 获取DNS解析超时时间
 *
 * @return 超时时间(s)
 *
 * @details
 */
int32_t cm_async_dns_get_timeout(void);

/**
 * @brief 设置DNS解析超时时间
 *
 * @param [in]  timeout DNS解析超时时间(1-60，单位：s)
 *
 * @return
 *
 * @details
 */
void cm_async_dns_set_timeout(int32_t timeout);

/**
 * @brief 通过解析优先级获取DNS解析类型
 *
 * @return 解析类型
 *
 * @details
 */
cm_async_dns_addrtype_e cm_async_dns_get_type_by_priority(void);

/**
 * @brief 通过解析优先级获取DNS解析类型(指定cid)
 *
 * @return 解析类型
 *
 * @details
 */
cm_async_dns_addrtype_e cm_async_dns_get_type_by_priority_ex(uint8_t cid);

/**
 * @brief 设置DNS解析优先级
 *
 * @param [in]  priority DNS解析优先级
 *
 * @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details 参数范围0-1， 0：v4优先，1：v6优先，默认v6优先
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
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_async_dns_requeset_v2接口。
 */
int cm_async_dns_request(const char *host_name, cm_async_dns_addrtype_e addr_type,
                         cm_async_dns_ip_addr_t *ip_addr, cm_async_dns_event_cb event_cb, void *cb_param);

/**
 * @brief 请求执行异步DNS解析
 *
 * @param [in]  host_name    域名
 * @param [in]  addr_type    地址类型(IPV4或IPV6)
 * @param [out] ip_addr      IP地址(DNS解析结果)
 * @param [in]  event_cb     异步事件回调函数
 * @param [in]  cb_param     用户参数(将作为参数传入event_cb)
 * @param [in]  cid          PDP 场景 ID
 *
 * @return
 *   > 0  - DNS请求成功,返回异步DNS请求ID(等待回调函数传回执行结果) \n
 *   = 0  - DNS请求已完成(result输出解析结果) \n
 *   = -1 - 请求失败
 *
 * @details 请求执行异步DNS解析
 */
int cm_async_dns_request_v2(const char *host_name, cm_async_dns_addrtype_e addr_type,
                         cm_async_dns_ip_addr_t *ip_addr, cm_async_dns_event_cb event_cb, void *cb_param, uint8_t cid);


/**
 * @brief 请求执行异步DNS解析(获取所有解析结果)
 *
 * @param [in]  host_name    域名
 * @param [in]  addr_type    地址类型(IPV4或IPV6)
 * @param [out] ip_addr      IP地址数组(DNS解析结果)
 * @param [in]  addr_num     IP地址数组长度
 * @param [in]  event_cb     异步事件回调函数
 * @param [in]  cb_param     用户参数(将作为参数传入event_cb)
 *
 * @return
 *   > 0  - DNS请求成功,返回异步DNS请求ID(等待回调函数传回执行结果) \n
 *   = 0  - DNS请求已完成(result输出解析结果) \n
 *   = -1 - 请求失败
 *
 * @details 请求执行异步DNS解析(获取所有解析结果)
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_async_dns_req_all_v2接口。
 */
int cm_async_dns_req_all(const char *host_name, cm_async_dns_addrtype_e addr_type,
                         cm_async_dns_ip_addr_t *ip_addr, uint8_t addr_num,
                         cm_async_dns_event_cb event_cb, void *cb_param);

/**
 * @brief 请求执行异步DNS解析(获取所有解析结果)
 *
 * @param [in]  host_name    域名
 * @param [in]  addr_type    地址类型(IPV4或IPV6)
 * @param [out] ip_addr      IP地址数组(DNS解析结果)
 * @param [in]  addr_num     IP地址数组长度
 * @param [in]  event_cb     异步事件回调函数
 * @param [in]  cb_param     用户参数(将作为参数传入event_cb)
 * @param [in]  cid          PDP 场景 ID
 *
 * @return
 *   > 0  - DNS请求成功,返回异步DNS请求ID(等待回调函数传回执行结果) \n
 *   = 0  - DNS请求已完成(result输出解析结果) \n
 *   = -1 - 请求失败
 *
 * @details 请求执行异步DNS解析(获取所有解析结果)
 */
int cm_async_dns_req_all_v2(const char *host_name, cm_async_dns_addrtype_e addr_type,
                         cm_async_dns_ip_addr_t *ip_addr, uint8_t addr_num,
                         cm_async_dns_event_cb event_cb, void *cb_param, uint8_t cid);


/**
 * @brief 获取DNS请求响应结果地址个数
 *
 * @param [in]  ip_addr DNS请求响应结果IP地址
 *
 * @return
 *   DNS请求响应结果地址个数
 *
 * @details
 */
uint8_t cm_async_dns_get_result_addr_num(const cm_async_dns_ip_addr_t *ip_addr);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_ASYNC_DNS_H__ */

