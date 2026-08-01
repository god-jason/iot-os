/**
 * @file mqtt_packets.h
 * @brief MQTT 3.1.1 协议包定义与编解码接口
 *
 * 定义 MQTT 协议的 14 种数据包类型，提供完整的协议包构建和解析功能
 * 包括 CONNECT、PUBLISH、SUBSCRIBE 等数据包的编解码及主题匹配工具函数
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */
#ifndef IOT_MQTT_PACKETS_H
#define IOT_MQTT_PACKETS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * MQTT 协议常量
 *===========================================================*/

#define IOT_MQTT_PROTOCOL_NAME     "MQTT"
#define IOT_MQTT_PROTOCOL_VERSION  4       /* MQTT 3.1.1 */

#define IOT_MQTT_MAX_PACKET_SIZE   1024 * 128
#define IOT_MQTT_MAX_TOPIC_LEN     65535
#define IOT_MQTT_MAX_PAYLOAD_LEN   1024 * 64

/*===========================================================
 * MQTT 数据包类型
 *===========================================================*/

typedef enum {
    IOT_MQTT_PACKET_CONNECT     = 0x10,  /* 连接请求 */
    IOT_MQTT_PACKET_CONNACK      = 0x20,  /* 连接确认 */
    IOT_MQTT_PACKET_PUBLISH      = 0x30,  /* 发布消息 */
    IOT_MQTT_PACKET_PUBACK       = 0x40,  /* 发布确认（QoS 1） */
    IOT_MQTT_PACKET_PUBREC       = 0x50,  /* 发布收到（QoS 2 流程1） */
    IOT_MQTT_PACKET_PUBREL       = 0x60,  /* 发布释放（QoS 2 流程2） */
    IOT_MQTT_PACKET_PUBCOMP      = 0x70,  /* 发布完成（QoS 2 流程3） */
    IOT_MQTT_PACKET_SUBSCRIBE    = 0x82,  /* 订阅请求 */
    IOT_MQTT_PACKET_SUBACK       = 0x90,  /* 订阅确认 */
    IOT_MQTT_PACKET_UNSUBSCRIBE  = 0xA2, /* 取消订阅请求 */
    IOT_MQTT_PACKET_UNSUBACK     = 0xB0,  /* 取消订阅确认 */
    IOT_MQTT_PACKET_PINGREQ      = 0xC0,  /* 心跳请求 */
    IOT_MQTT_PACKET_PINGRESP     = 0xD0,  /* 心跳响应 */
    IOT_MQTT_PACKET_DISCONNECT   = 0xE0,  /* 断开连接 */
} iot_mqtt_packet_type_t;

/*===========================================================
 * MQTT QoS 级别
 *===========================================================*/

typedef enum {
    IOT_MQTT_QOS_0 = 0,   /* 最多一次 delivery */
    IOT_MQTT_QOS_1 = 1,   /* 至少一次 delivery */
    IOT_MQTT_QOS_2 = 2,   /* 恰好一次 delivery */
} iot_mqtt_qos_t;

/*===========================================================
 * 连接返回码
 *===========================================================*/

typedef enum {
    IOT_MQTT_CONNACK_ACCEPTED           = 0,    /* 连接被接受 */
    IOT_MQTT_CONNACK_UNACCEPTABLE_PROTO  = 1,    /* 不可接受的协议版本 */
    IOT_MQTT_CONNACK_IDENTIFIER_REJECTED = 2,    /* 客户端标识符被拒绝 */
    IOT_MQTT_CONNACK_SERVER_UNAVAILABLE  = 3,    /* 服务器不可用 */
    IOT_MQTT_CONNACK_BAD_AUTH           = 4,     /* 用户名或密码错误 */
    IOT_MQTT_CONNACK_NOT_AUTHORIZED     = 5,    /* 未授权 */
} iot_mqtt_connack_code_t;

/*===========================================================
 * 订阅返回码
 *===========================================================*/

typedef enum {
    IOT_MQTT_SUBACK_SUCCESS_QOS0 = 0,   /* 订阅成功，最大 QoS 0 */
    IOT_MQTT_SUBACK_SUCCESS_QOS1 = 1,   /* 订阅成功，最大 QoS 1 */
    IOT_MQTT_SUBACK_SUCCESS_QOS2 = 2,   /* 订阅成功，最大 QoS 2 */
    IOT_MQTT_SUBACK_FAILURE      = 0x80, /* 订阅失败 */
} iot_mqtt_suback_code_t;

/*===========================================================
 * 数据包结构定义
 *===========================================================*/

/* CONNECT 数据包 */
typedef struct {
    /* 协议信息（固定） */
    const char* protocol_name;     /* 协议名称，默认 "MQTT" */
    uint8_t protocol_version;      /* 协议版本，默认 4 */

    /* 连接标志 */
    uint8_t clean_session:1;        /* 清理会话 */
    uint8_t will_flag:1;           /* 遗嘱消息标志 */
    uint8_t will_qos:2;            /* 遗嘱 QoS */
    uint8_t will_retain:1;         /* 遗嘱保留 */
    uint8_t password_flag:1;       /* 密码标志 */
    uint8_t username_flag:1;       /* 用户名标志 */

    /* 可变头 */
    uint16_t keepalive;            /* 心跳间隔（秒） */

    /* 负载 */
    const char* client_id;         /* 客户端标识符 */
    const char* will_topic;        /* 遗嘱主题（可选） */
    const char* will_message;      /* 遗嘱消息（可选） */
    const char* username;          /* 用户名（可选） */
    const char* password;          /* 密码（可选） */
} iot_mqtt_packet_connect_t;

/* CONNACK 数据包 */
typedef struct {
    uint8_t session_present:1;      /* 会话存在标志 */
    uint8_t return_code;           /* 连接返回码 */
} iot_mqtt_packet_connack_t;

/* PUBLISH 数据包 */
typedef struct {
    uint8_t dup:1;                 /* 重复标志 */
    iot_mqtt_qos_t qos:2;              /* QoS 级别 */
    uint8_t retain:1;              /* 保留标志 */
    uint16_t packet_id;            /* 数据包标识符（QoS > 0 时） */
    const char* topic;             /* 主题名 */
    const uint8_t* payload;        /* 消息负载 */
    size_t payload_len;            /* 负载长度 */
} iot_mqtt_packet_publish_t;

/* PUBACK 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
} iot_mqtt_packet_puback_t;

/* PUBREC 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
} iot_mqtt_packet_pubrec_t;

/* PUBREL 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
} iot_mqtt_packet_pubrel_t;

/* PUBCOMP 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
} iot_mqtt_packet_pubcomp_t;

/* SUBSCRIBE 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
    /* 负载：主题过滤器 + 请求 QoS 的交替序列 */
    const char** topic_filters;    /* 主题过滤器数组 */
    iot_mqtt_qos_t* requested_qos;     /* 请求的 QoS 数组 */
    size_t topic_count;            /* 主题数量 */
} iot_mqtt_packet_subscribe_t;

/* SUBACK 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
    /* 负载：每个订阅主题对应的返回码数组 */
    uint8_t* return_codes;         /* 返回码数组 */
    size_t return_code_count;      /* 返回码数量 */
} iot_mqtt_packet_suback_t;

/* UNSUBSCRIBE 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
    /* 负载：主题过滤器数组 */
    const char** topic_filters;    /* 主题过滤器数组 */
    size_t topic_count;            /* 主题数量 */
} iot_mqtt_packet_unsubscribe_t;

/* UNSUBACK 数据包 */
typedef struct {
    uint16_t packet_id;            /* 数据包标识符 */
} iot_mqtt_packet_unsuback_t;

/* PINGREQ/PINGRESP/DISCONNECT 数据包（无可变头和负载） */

/*===========================================================
 * 通用数据包（用于解析）
 *===========================================================*/

typedef struct {
    iot_mqtt_packet_type_t type;       /* 数据包类型 */
    uint8_t flags;                 /* 数据包标志 */
    uint32_t remaining_length;      /* 剩余长度 */
    
    union {
        iot_mqtt_packet_connack_t connack;
        iot_mqtt_packet_publish_t publish;
        iot_mqtt_packet_puback_t puback;
        iot_mqtt_packet_pubrec_t pubrec;
        iot_mqtt_packet_pubrel_t pubrel;
        iot_mqtt_packet_pubcomp_t pubcomp;
        iot_mqtt_packet_suback_t suback;
        iot_mqtt_packet_unsuback_t unsuback;
    } packet;
    
    /* 原始数据（用于负载中的动态数据） */
    uint8_t* data;
    size_t data_len;
} iot_mqtt_packet_t;

/*===========================================================
 * 协议包编解码接口
 *===========================================================*/

/**
 * @brief 构建 CONNECT 数据包
 * @param connect CONNECT 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_connect(const iot_mqtt_packet_connect_t* connect, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 CONNACK 数据包（客户端不使用，由服务器构建）
 * @param connack CONNACK 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_connack(const iot_mqtt_packet_connack_t* connack, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PUBLISH 数据包
 * @param publish PUBLISH 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_publish(const iot_mqtt_packet_publish_t* publish, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PUBACK 数据包
 * @param packet_id 数据包标识符
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_puback(uint16_t packet_id, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PUBREC 数据包
 * @param packet_id 数据包标识符
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_pubrec(uint16_t packet_id, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PUBREL 数据包
 * @param packet_id 数据包标识符
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_pubrel(uint16_t packet_id, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PUBCOMP 数据包
 * @param packet_id 数据包标识符
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_pubcomp(uint16_t packet_id, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 SUBSCRIBE 数据包
 * @param subscribe SUBSCRIBE 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_subscribe(const iot_mqtt_packet_subscribe_t* subscribe, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 SUBACK 数据包
 * @param suback SUBACK 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_suback(const iot_mqtt_packet_suback_t* suback, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 UNSUBSCRIBE 数据包
 * @param unsubscribe UNSUBSCRIBE 配置
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_unsubscribe(const iot_mqtt_packet_unsubscribe_t* unsubscribe, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 UNSUBACK 数据包
 * @param packet_id 数据包标识符
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_unsuback(uint16_t packet_id, uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PINGREQ 数据包
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_pingreq(uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 PINGRESP 数据包
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_pingresp(uint8_t* buf, size_t buf_len);

/**
 * @brief 构建 DISCONNECT 数据包
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 数据包长度，失败返回 -1
 */
int iot_mqtt_packet_encode_disconnect(uint8_t* buf, size_t buf_len);

/*===========================================================
 * 协议包解析接口
 *===========================================================*/

/**
 * @brief 解析数据包（头部）
 * @param data 输入数据
 * @param data_len 输入数据长度
 * @param packet 输出数据包结构
 * @param consumed 消费的字节数
 * @return 0 成功，-1 失败，1 数据不完整
 */
int iot_mqtt_packet_decode_header(const uint8_t* data, size_t data_len, 
                              iot_mqtt_packet_t* packet, size_t* consumed);

/**
 * @brief 解析 CONNACK 数据包
 * @param packet 包含数据的包结构
 * @param connack 输出 CONNACK 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_connack(const iot_mqtt_packet_t* packet, iot_mqtt_packet_connack_t* connack);

/**
 * @brief 解析 PUBLISH 数据包
 * @param packet 包含数据的包结构
 * @param publish 输出 PUBLISH 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_publish(const iot_mqtt_packet_t* packet, iot_mqtt_packet_publish_t* publish);

/**
 * @brief 解析 PUBACK 数据包
 * @param packet 包含数据的包结构
 * @param puback 输出 PUBACK 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_puback(const iot_mqtt_packet_t* packet, iot_mqtt_packet_puback_t* puback);

/**
 * @brief 解析 PUBREC 数据包
 * @param packet 包含数据的包结构
 * @param pubrec 输出 PUBREC 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_pubrec(const iot_mqtt_packet_t* packet, iot_mqtt_packet_pubrec_t* pubrec);

/**
 * @brief 解析 PUBREL 数据包
 * @param packet 包含数据的包结构
 * @param pubrel 输出 PUBREL 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_pubrel(const iot_mqtt_packet_t* packet, iot_mqtt_packet_pubrel_t* pubrel);

/**
 * @brief 解析 PUBCOMP 数据包
 * @param packet 包含数据的包结构
 * @param pubcomp 输出 PUBCOMP 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_pubcomp(const iot_mqtt_packet_t* packet, iot_mqtt_packet_pubcomp_t* pubcomp);

/**
 * @brief 解析 SUBACK 数据包
 * @param packet 包含数据的包结构
 * @param suback 输出 SUBACK 结构（需外部释放 return_codes）
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_suback(const iot_mqtt_packet_t* packet, iot_mqtt_packet_suback_t* suback);

/**
 * @brief 解析 UNSUBACK 数据包
 * @param packet 包含数据的包结构
 * @param unsuback 输出 UNSUBACK 结构
 * @return 0 成功，-1 失败
 */
int iot_mqtt_packet_decode_unsuback(const iot_mqtt_packet_t* packet, iot_mqtt_packet_unsuback_t* unsuback);

/*===========================================================
 * 辅助接口
 *===========================================================*/

/**
 * @brief 获取数据包类型的字符串名称
 * @param type 数据包类型
 * @return 字符串名称
 */
const char* iot_mqtt_packet_type_name(iot_mqtt_packet_type_t type);

/**
 * @brief 验证主题名是否符合规范
 * @param topic 主题名
 * @param len 主题名长度
 * @return true 有效，false 无效
 */
bool iot_mqtt_topic_validate(const char* topic, size_t len);

/**
 * @brief 主题过滤器匹配
 * @param filter 主题过滤器（支持 + 和 # 通配符）
 * @param topic 主题名
 * @return true 匹配，false 不匹配
 */
bool iot_mqtt_topic_match(const char* filter, const char* topic);

/**
 * @brief 释放数据包动态分配的内存
 * @param packet 数据包
 */
void iot_mqtt_packet_free(iot_mqtt_packet_t* packet);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_PACKETS_H */
