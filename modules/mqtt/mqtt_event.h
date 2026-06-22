/**
 * @file mqtt_event.h
 * @brief MQTT 事件处理
 *
 * 管理连接、断开、消息接收等事件的回调分发
 */
#ifndef IOT_MQTT_EVENT_H
#define IOT_MQTT_EVENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "mqtt_packets.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MQTT_EVENT_CONNECTED     = 0,
    MQTT_EVENT_DISCONNECTED  = 1,
    MQTT_EVENT_ERROR         = 2,
} mqtt_event_type_t;

typedef void (*mqtt_event_callback_t)(void* user_data, mqtt_event_type_t event);

typedef void (*mqtt_message_callback_t)(void* user_data, const char* topic,
                                       const uint8_t* payload, size_t payload_len,
                                       mqtt_qos_t qos, bool retain);

typedef struct mqtt_event mqtt_event_t;

struct mqtt_event {
    mqtt_event_callback_t event_callback;
    void* event_user_data;
};

void mqtt_event_init(mqtt_event_t* event);
void mqtt_event_set_callback(mqtt_event_t* event, mqtt_event_callback_t callback, void* user_data);
void mqtt_event_trigger(mqtt_event_t* event, mqtt_event_type_t event_type);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_EVENT_H */