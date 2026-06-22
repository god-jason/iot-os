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

typedef enum {
    MQTT_EVENT_CONNECTED      = 0,
    MQTT_EVENT_DISCONNECTED   = 1,
    MQTT_EVENT_MESSAGE        = 2,
    MQTT_EVENT_ERROR          = 3,
    MQTT_EVENT_SUBSCRIBED     = 4,
    MQTT_EVENT_UNSUBSCRIBED   = 5,
} mqtt_event_type_t;

typedef void (*mqtt_message_callback_t)(const char* topic, const uint8_t* payload,
                                       size_t payload_len, mqtt_qos_t qos, bool retain, void* user_data);

typedef void (*mqtt_event_callback_t)(mqtt_client_t* client, mqtt_event_type_t event, void* user_data);

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
    
    bool use_ssl;
    net_ssl_config_t ssl_config;
} mqtt_connect_options_t;

typedef struct mqtt_subscribe_entry mqtt_subscribe_entry_t;

struct mqtt_subscribe_entry {
    char* topic_filter;
    mqtt_qos_t qos;
    mqtt_message_callback_t callback;
    void* user_data;
    mqtt_subscribe_entry_t* next;
};

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

typedef struct mqtt_client mqtt_client_t;

struct mqtt_client {
    sock_t sock;
    mqtt_state_t state;
    mqtt_error_t last_error;
    
    mqtt_connect_options_t options;
    
    mqtt_event_callback_t event_callback;
    void* event_user_data;
    
    uint16_t next_packet_id;
    
    uint8_t* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    mqtt_outgoing_msg_t* outgoing_head;
    mqtt_outgoing_msg_t* outgoing_tail;
    size_t outgoing_count;
    
    mqtt_subscribe_entry_t* subscribe_head;
    mqtt_subscribe_entry_t* subscribe_tail;
    size_t subscribe_count;
    
    bool auto_reconnect;
    int reconnect_interval_ms;
    uint32_t last_connect_attempt;
    
    int keepalive;
    uint32_t last_ping_time;
    uint32_t ping_sent_time;
    
    mqtt_client_t* next;
};

mqtt_client_t* mqtt_client_create(void);
void mqtt_client_destroy(mqtt_client_t* client);

int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options);
int mqtt_client_disconnect(mqtt_client_t* client);

int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain);

int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);

int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter);

void mqtt_client_set_event_callback(mqtt_client_t* client,
                                    mqtt_event_callback_t callback, void* user_data);

mqtt_state_t mqtt_client_get_state(mqtt_client_t* client);
mqtt_error_t mqtt_client_get_error(mqtt_client_t* client);

void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms);
void mqtt_client_disable_auto_reconnect(mqtt_client_t* client);

bool mqtt_client_is_connected(mqtt_client_t* client);

int mqtt_manager_start(void);
void mqtt_manager_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_CLIENT_H */