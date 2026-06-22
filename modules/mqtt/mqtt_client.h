/**
 * @file mqtt_client.h
 * @brief MQTT 客户端接口定义
 *
 * 提供 MQTT 3.1.1 协议客户端功能，支持连接、发布、订阅、QoS 等特性
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

/**
 * @brief MQTT 错误码
 */
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

/**
 * @brief MQTT 客户端状态
 */
typedef enum {
    MQTT_STATE_DISCONNECTED  = 0,    /* 已断开连接 */
    MQTT_STATE_CONNECTING    = 1,    /* 连接中 */
    MQTT_STATE_CONNECTED     = 2,    /* 已连接 */
    MQTT_STATE_ERROR         = 3,    /* 错误状态 */
} mqtt_state_t;

/**
 * @brief MQTT 事件类型
 */
typedef enum {
    MQTT_EVENT_CONNECTED      = 0,   /* 连接成功 */
    MQTT_EVENT_DISCONNECTED   = 1,   /* 连接断开 */
    MQTT_EVENT_MESSAGE        = 2,   /* 收到消息 */
    MQTT_EVENT_ERROR          = 3,   /* 发生错误 */
    MQTT_EVENT_SUBSCRIBED     = 4,   /* 订阅成功 */
    MQTT_EVENT_UNSUBSCRIBED   = 5,   /* 取消订阅成功 */
} mqtt_event_type_t;

/**
 * @brief MQTT 消息回调函数
 * @param topic 主题名
 * @param payload 消息负载
 * @param payload_len 负载长度
 * @param qos QoS 级别
 * @param retain 保留标志
 * @param user_data 用户数据
 */
typedef void (*mqtt_message_callback_t)(const char* topic, const uint8_t* payload,
                                       size_t payload_len, mqtt_qos_t qos, bool retain, void* user_data);

/**
 * @brief MQTT 事件回调函数
 * @param client 客户端指针
 * @param event 事件类型
 * @param user_data 用户数据
 */
typedef void (*mqtt_event_callback_t)(mqtt_client_t* client, mqtt_event_type_t event, void* user_data);

/**
 * @brief MQTT 连接选项
 */
typedef struct {
    const char* host;                 /* 服务器地址 */
    uint16_t port;                    /* 服务器端口 */
    const char* client_id;            /* 客户端标识符 */
    const char* username;             /* 用户名 */
    const char* password;             /* 密码 */
    int keepalive;                    /* 心跳间隔（秒） */
    bool clean_session;               /* 清理会话标志 */
    
    const char* will_topic;           /* 遗嘱主题 */
    const char* will_message;         /* 遗嘱消息 */
    mqtt_qos_t will_qos;              /* 遗嘱 QoS */
    bool will_retain;                 /* 遗嘱保留标志 */
    
    int timeout_ms;                   /* 超时时间（毫秒） */
    
    bool use_ssl;                     /* 是否使用 SSL */
    net_ssl_config_t ssl_config;      /* SSL 配置 */
} mqtt_connect_options_t;

/**
 * @brief MQTT 订阅条目
 */
typedef struct mqtt_subscribe_entry mqtt_subscribe_entry_t;

struct mqtt_subscribe_entry {
    char* topic_filter;               /* 主题过滤器 */
    mqtt_qos_t qos;                   /* QoS 级别 */
    mqtt_message_callback_t callback; /* 消息回调函数 */
    void* user_data;                  /* 用户数据 */
    mqtt_subscribe_entry_t* next;     /* 下一个条目 */
};

/**
 * @brief MQTT 待发送消息（QoS > 0）
 */
typedef struct mqtt_outgoing_msg mqtt_outgoing_msg_t;

struct mqtt_outgoing_msg {
    uint16_t packet_id;               /* 数据包标识符 */
    mqtt_qos_t qos;                   /* QoS 级别 */
    char* topic;                      /* 主题名 */
    uint8_t* payload;                 /* 消息负载 */
    size_t payload_len;               /* 负载长度 */
    bool retain;                      /* 保留标志 */
    int retry_count;                  /* 重试次数 */
    uint32_t send_time;               /* 发送时间 */
    mqtt_outgoing_msg_t* next;        /* 下一条消息 */
};

/**
 * @brief MQTT 客户端结构
 */
typedef struct mqtt_client mqtt_client_t;

struct mqtt_client {
    sock_t sock;                      /* socket 句柄 */
    mqtt_state_t state;               /* 客户端状态 */
    mqtt_error_t last_error;          /* 最后一次错误 */
    
    mqtt_connect_options_t options;   /* 连接选项 */
    
    mqtt_event_callback_t event_callback;  /* 事件回调函数 */
    void* event_user_data;            /* 事件回调用户数据 */
    
    uint16_t next_packet_id;          /* 下一个数据包标识符 */
    
    uint8_t* recv_buf;                /* 接收缓冲区 */
    size_t recv_len;                  /* 接收数据长度 */
    size_t recv_capacity;             /* 接收缓冲区容量 */
    
    mqtt_outgoing_msg_t* outgoing_head;   /* 待发送消息链表头 */
    mqtt_outgoing_msg_t* outgoing_tail;   /* 待发送消息链表尾 */
    size_t outgoing_count;            /* 待发送消息数量 */
    
    mqtt_subscribe_entry_t* subscribe_head;  /* 订阅条目链表头 */
    mqtt_subscribe_entry_t* subscribe_tail;  /* 订阅条目链表尾 */
    size_t subscribe_count;           /* 订阅条目数量 */
    
    bool auto_reconnect;              /* 是否自动重连 */
    int reconnect_interval_ms;        /* 重连间隔（毫秒） */
    uint32_t last_connect_attempt;    /* 最后一次连接尝试时间 */
    
    int keepalive;                    /* 心跳间隔（秒） */
    uint32_t last_ping_time;          /* 最后一次心跳时间 */
    uint32_t ping_sent_time;          /* 最后一次心跳发送时间 */
    
    mqtt_client_t* next;              /* 链表下一个节点 */
};

/**
 * @brief 创建 MQTT 客户端
 * @return 客户端指针，失败返回 NULL
 */
mqtt_client_t* mqtt_client_create(void);

/**
 * @brief 销毁 MQTT 客户端
 * @param client 客户端指针
 */
void mqtt_client_destroy(mqtt_client_t* client);

/**
 * @brief 连接到 MQTT 服务器
 * @param client 客户端指针
 * @param options 连接选项
 * @return 0 成功，其他值表示错误
 */
int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options);

/**
 * @brief 断开与 MQTT 服务器的连接
 * @param client 客户端指针
 * @return 0 成功，其他值表示错误
 */
int mqtt_client_disconnect(mqtt_client_t* client);

/**
 * @brief 发布消息
 * @param client 客户端指针
 * @param topic 主题名
 * @param payload 消息负载
 * @param payload_len 负载长度
 * @param qos QoS 级别
 * @param retain 保留标志
 * @return 0 成功，其他值表示错误
 */
int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain);

/**
 * @brief 订阅主题
 * @param client 客户端指针
 * @param topic_filter 主题过滤器
 * @param qos QoS 级别
 * @param callback 消息回调函数
 * @param user_data 用户数据
 * @return 0 成功，其他值表示错误
 */
int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);

/**
 * @brief 取消订阅主题
 * @param client 客户端指针
 * @param topic_filter 主题过滤器
 * @return 0 成功，其他值表示错误
 */
int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter);

/**
 * @brief 设置事件回调函数
 * @param client 客户端指针
 * @param callback 回调函数
 * @param user_data 用户数据
 */
void mqtt_client_set_event_callback(mqtt_client_t* client,
                                    mqtt_event_callback_t callback, void* user_data);

/**
 * @brief 获取客户端状态
 * @param client 客户端指针
 * @return 客户端状态
 */
mqtt_state_t mqtt_client_get_state(mqtt_client_t* client);

/**
 * @brief 获取最后一次错误
 * @param client 客户端指针
 * @return 错误码
 */
mqtt_error_t mqtt_client_get_error(mqtt_client_t* client);

/**
 * @brief 启用自动重连
 * @param client 客户端指针
 * @param interval_ms 重连间隔（毫秒）
 */
void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms);

/**
 * @brief 禁用自动重连
 * @param client 客户端指针
 */
void mqtt_client_disable_auto_reconnect(mqtt_client_t* client);

/**
 * @brief 检查是否已连接
 * @param client 客户端指针
 * @return true 已连接，false 未连接
 */
bool mqtt_client_is_connected(mqtt_client_t* client);

/**
 * @brief 启动 MQTT 管理器
 * @return 0 成功，-1 失败
 */
int mqtt_manager_start(void);

/**
 * @brief 停止 MQTT 管理器
 */
void mqtt_manager_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_CLIENT_H */