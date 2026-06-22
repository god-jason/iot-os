/**
 * @file mqtt_publisher.h
 * @brief MQTT 消息发送队列和 QoS 处理
 *
 * 负责管理 MQTT 客户端的发送队列，处理 QoS 1/2 消息的重传与确认机制
 */
#ifndef IOT_MQTT_PUBLISHER_H
#define IOT_MQTT_PUBLISHER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "mqtt_packets.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_MAX_RETRY              3
#define MQTT_RETRY_INTERVAL_MS      5000
#define MQTT_MAX_OUTGOING_MSGS      256

typedef struct mqtt_outgoing_msg mqtt_outgoing_msg_t;

struct mqtt_outgoing_msg {
    uint16_t packet_id;
    mqtt_qos_t qos;
    char* topic;
    uint8_t* payload;
    size_t payload_len;
    bool retain;
    int retry_count;
    uint32_t send_time;
    mqtt_outgoing_msg_t* next;
};

typedef struct {
    mqtt_outgoing_msg_t* outgoing_head;
    mqtt_outgoing_msg_t* outgoing_tail;
    size_t outgoing_count;
} mqtt_publisher_t;

void mqtt_publisher_init(mqtt_publisher_t* publisher);
void mqtt_publisher_deinit(mqtt_publisher_t* publisher);

int mqtt_publisher_add_message(mqtt_publisher_t* publisher, uint16_t packet_id,
                               mqtt_qos_t qos, const char* topic,
                               const uint8_t* payload, size_t payload_len,
                               bool retain);

void mqtt_publisher_remove_message(mqtt_publisher_t* publisher, uint16_t packet_id, mqtt_qos_t qos);

void mqtt_publisher_resend_messages(mqtt_publisher_t* publisher, 
                                    int (*send_func)(const uint8_t*, size_t, void*),
                                    void* send_arg);

void mqtt_publisher_clear_retry_count(mqtt_publisher_t* publisher);

void mqtt_publisher_resend_all(mqtt_publisher_t* publisher,
                                int (*send_func)(const uint8_t*, size_t, void*),
                                void* send_arg);

size_t mqtt_publisher_get_message_count(mqtt_publisher_t* publisher);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_PUBLISHER_H */