/**
 * @file mqtt_client.h
 * @brief MQTT 客户端定义
 *
 * 基于 net_socket 实现的 MQTT 3.1.1 客户端，支持完整的 QoS 0/1/2、自动重连等
 */
#ifndef IOT_MQTT_CLIENT_H
#define IOT_MQTT_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "net.h"
#include "mqtt_packets.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * MQTT 客户端错误码
 *===========================================================*/

typedef enum {
    MQTT_ERR_SUCCESS         = 0,    /* 成功 */
    MQTT_ERR_CONNECT         = -1,   /* 连接失败 */
    MQTT_ERR_SEND           = -2,   /* 发送失败 */
    MQTT_ERR_RECV           = -3,   /* 接收失败 */
    MQTT_ERR_TIMEOUT        = -4,   /* 超时 */
    MQTT_ERR_PROTOCOL       = -5,   /* 协议错误 */
    MQTT_ERR_QOS            = -6,   /* QoS 错误 */
    MQTT_ERR_KEEPALIVE      = -7,   /* 心跳超时 */
    MQTT_ERR_MEMORY         = -8,   /* 内存不足 */
    MQTT_ERR_PARAM          = -9,   /* 参数错误 */
} mqtt_error_t;

/*===========================================================
 * MQTT 客户端状态
 *===========================================================*/

typedef enum {
    MQTT_STATE_DISCONNECTED  = 0,    /* 未连接 */
    MQTT_STATE_CONNECTING    = 1,    /* 连接中 */
    MQTT_STATE_CONNECTED     = 2,    /* 已连接 */
    MQTT_STATE_ERROR         = 3,    /* 错误状态 */
} mqtt_state_t;

/*===========================================================
 * 连接选项
 *===========================================================*/

typedef struct {
    const char* host;                /* 服务器地址 */
    uint16_t port;                   /* 服务器端口（默认 1883） */
    const char* client_id;           /* 客户端标识符 */
    const char* username;            /* 用户名（可选） */
    const char* password;            /* 密码（可选） */
    int keepalive;                  /* 心跳间隔（秒，默认 60） */
    bool clean_session;             /* 清理会话（默认 true） */
    
    /* 遗嘱消息 */
    const char* will_topic;          /* 遗嘱主题（可选） */
    const char* will_message;        /* 遗嘱消息（可选） */
    mqtt_qos_t will_qos;           /* 遗嘱 QoS（默认 0） */
    bool will_retain;               /* 遗嘱保留（默认 false） */
    
    int timeout_ms;                 /* 连接超时（毫秒，默认 30000） */
} mqtt_connect_options_t;

/*===========================================================
 * 消息回调
 *===========================================================*/

typedef void (*mqtt_message_callback_t)(void* user_data, const char* topic, 
                                       const uint8_t* payload, size_t payload_len,
                                       mqtt_qos_t qos, bool retain);

/*===========================================================
 * 事件回调
 *===========================================================*/

typedef enum {
    MQTT_EVENT_CONNECTED     = 0,    /* 连接成功 */
    MQTT_EVENT_DISCONNECTED  = 1,    /* 连接断开 */
    MQTT_EVENT_ERROR         = 2,    /* 错误发生 */
} mqtt_event_type_t;

typedef void (*mqtt_event_callback_t)(void* user_data, mqtt_event_type_t event);

/*===========================================================
 * 消息记录（用于 QoS 1/2）
 *===========================================================*/

typedef struct mqtt_outgoing_msg mqtt_outgoing_msg_t;

struct mqtt_outgoing_msg {
    uint16_t packet_id;              /* 数据包标识符 */
    mqtt_qos_t qos;                 /* QoS 级别 */
    char* topic;                     /* 主题 */
    uint8_t* payload;                /* 负载 */
    size_t payload_len;              /* 负载长度 */
    bool retain;                    /* 保留标志 */
    int retry_count;                /* 重试次数 */
    uint32_t send_time;              /* 发送时间 */
    mqtt_outgoing_msg_t* next;      /* 下一条消息 */
};

/*===========================================================
 * MQTT 客户端
 *===========================================================*/

typedef struct mqtt_client mqtt_client_t;

/**
 * @brief 创建 MQTT 客户端
 * @return 客户端句柄，失败返回 NULL
 */
mqtt_client_t* mqtt_client_create(void);

/**
 * @brief 销毁 MQTT 客户端
 * @param client 客户端句柄
 */
void mqtt_client_destroy(mqtt_client_t* client);

/**
 * @brief 连接到 MQTT 服务器
 * @param client 客户端句柄
 * @param options 连接选项
 * @return 0 成功，< 0 失败
 */
int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options);

/**
 * @brief 断开 MQTT 连接
 * @param client 客户端句柄
 * @return 0 成功，< 0 失败
 */
int mqtt_client_disconnect(mqtt_client_t* client);

/**
 * @brief 发布消息
 * @param client 客户端句柄
 * @param topic 主题名
 * @param payload 消息负载
 * @param payload_len 负载长度
 * @param qos QoS 级别
 * @param retain 保留标志
 * @return 0 成功，< 0 失败
 */
int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain);

/**
 * @brief 订阅主题
 * @param client 客户端句柄
 * @param topic_filter 主题过滤器
 * @param qos QoS 级别
 * @param callback 消息回调
 * @param user_data 用户数据
 * @return 0 成功，< 0 失败
 */
int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);

/**
 * @brief 取消订阅
 * @param client 客户端句柄
 * @param topic_filter 主题过滤器
 * @return 0 成功，< 0 失败
 */
int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter);

/**
 * @brief 设置事件回调
 * @param client 客户端句柄
 * @param callback 事件回调
 * @param user_data 用户数据
 */
void mqtt_client_set_event_callback(mqtt_client_t* client, 
                                    mqtt_event_callback_t callback, void* user_data);

/**
 * @brief 获取客户端状态
 * @param client 客户端句柄
 * @return 客户端状态
 */
mqtt_state_t mqtt_client_get_state(mqtt_client_t* client);

/**
 * @brief 获取最后错误码
 * @param client 客户端句柄
 * @return 错误码
 */
mqtt_error_t mqtt_client_get_error(mqtt_client_t* client);

/**
 * @brief 启用自动重连
 * @param client 客户端句柄
 * @param interval_ms 重连间隔（毫秒）
 */
void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms);

/**
 * @brief 禁用自动重连
 * @param client 客户端句柄
 */
void mqtt_client_disable_auto_reconnect(mqtt_client_t* client);

/**
 * @brief 轮询处理（需要在主循环中调用）
 * @param client 客户端句柄
 * @param timeout_ms 超时时间（毫秒）
 */
void mqtt_client_poll(mqtt_client_t* client, int timeout_ms);

/**
 * @brief 检查是否已连接
 * @param client 客户端句柄
 * @return true 已连接
 */
bool mqtt_client_is_connected(mqtt_client_t* client);

/**
 * @brief 设置 Socket 句柄（内部使用）
 * @param client 客户端句柄
 * @param sock Socket 句柄
 */
void mqtt_client_set_socket(mqtt_client_t* client, sock_t sock);

/**
 * @brief 获取下一个数据包 ID
 * @param client 客户端句柄
 * @return 数据包 ID
 */
uint16_t mqtt_client_next_packet_id(mqtt_client_t* client);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_CLIENT_H */
