/**
 * @file http_manager.h
 * @brief HTTP 管理器接口定义
 *
 * HTTP 管理器头文件，提供 HTTP 客户端的统一管理接口，包括管理器初始化/反初始化、
 * 客户端注册与移除、互斥锁控制、默认超时配置等功能。管理器在独立线程中运行，
 * 负责超时检查和重试逻辑处理。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_HTTP_MANAGER_H
#define IOT_HTTP_MANAGER_H

#include "http_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_MANAGER_POLL_INTERVAL  50
#define HTTP_MAX_RETRY              3
#define HTTP_RETRY_INTERVAL_MS      5000

/**
 * @brief 初始化 HTTP 管理器
 * @return 0 成功，-1 失败
 */
int http_manager_init(void);

/**
 * @brief 反初始化 HTTP 管理器
 */
void http_manager_deinit(void);

/**
 * @brief 添加客户端到管理器
 * @param client 客户端指针
 * @return 0 成功，-1 失败
 */
int http_manager_add_client(http_client_t* client);

/**
 * @brief 从管理器移除客户端
 * @param client 客户端指针
 * @return 0 成功，-1 失败
 */
int http_manager_remove_client(http_client_t* client);

/**
 * @brief 锁定管理器互斥锁
 */
void http_manager_lock(void);

/**
 * @brief 解锁管理器互斥锁
 */
void http_manager_unlock(void);

/**
 * @brief 设置默认超时时间
 * @param timeout_ms 超时时间（毫秒）
 */
void http_manager_set_default_timeout(int timeout_ms);

/**
 * @brief 获取默认超时时间
 * @return 超时时间（毫秒）
 */
int http_manager_get_default_timeout(void);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_MANAGER_H */