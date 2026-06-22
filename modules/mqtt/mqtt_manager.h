/**
 * @file mqtt_manager.h
 * @brief MQTT 管理器接口定义
 *
 * 管理 MQTT 客户端的生命周期，处理 keepalive、消息重传、事件处理等
 */

#ifndef IOT_MQTT_MANAGER_H
#define IOT_MQTT_MANAGER_H

#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_MANAGER_POLL_INTERVAL  50
#define MQTT_MAX_RETRY              3
#define MQTT_RETRY_INTERVAL_MS      5000
#define MQTT_KEEPALIVE_CHECK_INTERVAL 30

/**
 * @brief 初始化 MQTT 管理器
 * @return 0 成功，-1 失败
 */
int mqtt_manager_init(void);

/**
 * @brief 反初始化 MQTT 管理器
 */
void mqtt_manager_deinit(void);

/**
 * @brief 添加客户端到管理器
 * @param client 客户端指针
 * @return 0 成功，-1 失败
 */
int mqtt_manager_add_client(mqtt_client_t* client);

/**
 * @brief 从管理器移除客户端
 * @param client 客户端指针
 * @return 0 成功，-1 失败
 */
int mqtt_manager_remove_client(mqtt_client_t* client);

/**
 * @brief 锁定管理器互斥锁
 */
void mqtt_manager_lock(void);

/**
 * @brief 解锁管理器互斥锁
 */
void mqtt_manager_unlock(void);

/**
 * @brief 处理 socket 事件（由 net 模块回调）
 * @param client 客户端指针
 * @param event 网络事件类型
 */
void mqtt_manager_on_socket_event(mqtt_client_t* client, net_event_type_t event);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_MANAGER_H */