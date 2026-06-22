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
#include "mqtt_publisher.h"
#include "mqtt_keepalive.h"
#include "mqtt_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MQTT_ERR_SUCCESS         = 0,
    MQTT_ERR_CONNECT         = -1,
    MQTT_ERR_SEND           = -2,
    MQTT_ERR_RECV           = -3,
    MQTT_ERR_TIMEOUT        = -4,
    MQTT_ERR_PROTOCOL       = -5,
    MQTT_ERR_QOS            = -6,
    MQTT_ERR_KEEPALIVE      = -7,
    MQTT_ERR_MEMORY         = -8,
    MQTT_ERR_PARAM          = -9,
} mqtt_error_t;

typedef enum {
    MQTT_STATE_DISCONNECTED  = 0,
    MQTT_STATE_CONNECTING    = 1,
    MQTT_STATE_CONNECTED     = 2,
    MQTT_STATE_ERROR         = 3,
} mqtt_state_t;

typedef struct {
    const char* host;
    uint16_t port;
    const char* client_id;
    const char* username;
    const char* password;
    int keepalive;
    bool clean_session;
    
    const char* will_topic;
    const char* will_message;
    mqtt_qos_t will_qos;
    bool will_retain;
    
    int timeout_ms;
} mqtt_connect_options_t;

typedef struct mqtt_subscribe_entry mqtt_subscribe_entry_t;

struct mqtt_subscribe_entry {
    char* topic_filter;
    mqtt_qos_t qos;
    mqtt_message_callback_t callback;
    void* user_data;
    mqtt_subscribe_entry_t* next;
};

typedef struct mqtt_client mqtt_client_t;

mqtt_client_t* mqtt_client_create(void);
void mqtt_client_destroy(mqtt_client_t* client);

int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options);
int mqtt_client_disconnect(mqtt_client_t* client);

int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain);

int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);

int mqtt_client_subscribe_multiple(mqtt_client_t* client, const char** topic_filters,
                                   const mqtt_qos_t* qos, mqtt_message_callback_t* callbacks,
                                   void** user_data, size_t topic_count);

int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter);

void mqtt_client_set_event_callback(mqtt_client_t* client,
                                    mqtt_event_callback_t callback, void* user_data);

mqtt_state_t mqtt_client_get_state(mqtt_client_t* client);
mqtt_error_t mqtt_client_get_error(mqtt_client_t* client);

void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms);
void mqtt_client_disable_auto_reconnect(mqtt_client_t* client);

void mqtt_client_poll(mqtt_client_t* client, int timeout_ms);
bool mqtt_client_is_connected(mqtt_client_t* client);

void mqtt_client_set_socket(mqtt_client_t* client, sock_t sock);
uint16_t mqtt_client_next_packet_id(mqtt_client_t* client);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_CLIENT_H */