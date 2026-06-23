#include "mqtt_client.h"
#include "mqtt_manager.h"
#include "mqtt_packets.h"

#include "iot.h"
#include "net.h"
#include "iot_log.h"

#include <string.h>
#include <stdlib.h>

#define MQTT_DEFAULT_KEEPALIVE      60
#define MQTT_DEFAULT_PORT           1883
#define MQTT_DEFAULT_TIMEOUT        30000
#define MQTT_DEFAULT_SSL_PORT       8883

static void mqtt_client_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);

static mqtt_subscribe_entry_t* mqtt_client_subscribe_find(mqtt_client_t* client, const char* topic_filter);
static mqtt_subscribe_entry_t* mqtt_client_subscribe_add(mqtt_client_t* client, const char* topic_filter, mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);
static void mqtt_client_subscribe_remove(mqtt_client_t* client, const char* topic_filter);
static void mqtt_client_subscribe_destroy_all(mqtt_client_t* client);

mqtt_client_t* mqtt_client_create(void) {
    mqtt_client_t* client = (mqtt_client_t*)iot_malloc(sizeof(mqtt_client_t));
    if (!client) {
        LOG_ERROR("mqtt client create failed: out of memory");
        return NULL;
    }

    memset(client, 0, sizeof(mqtt_client_t));
    client->state = MQTT_STATE_DISCONNECTED;
    client->next_packet_id = 1;
    client->options.keepalive = MQTT_DEFAULT_KEEPALIVE;
    client->options.port = MQTT_DEFAULT_PORT;
    client->options.clean_session = true;
    client->options.timeout_ms = MQTT_DEFAULT_TIMEOUT;
    client->options.use_ssl = false;

    client->recv_capacity = MQTT_MAX_PACKET_SIZE;
    client->recv_buf = (uint8_t*)iot_malloc(client->recv_capacity);
    if (!client->recv_buf) {
        LOG_ERROR("mqtt client create failed: recv buffer alloc error");
        iot_free(client);
        return NULL;
    }

    LOG_INFO("mqtt client created");
    return client;
}

void mqtt_client_destroy(mqtt_client_t* client) {
    if (!client) return;

    mqtt_client_disconnect(client);

    if (client->recv_buf) {
        iot_free(client->recv_buf);
    }

    mqtt_client_subscribe_destroy_all(client);

    mqtt_outgoing_msg_t* msg = client->outgoing_head;
    while (msg) {
        mqtt_outgoing_msg_t* next = msg->next;
        if (msg->topic) iot_free(msg->topic);
        if (msg->payload) iot_free(msg->payload);
        iot_free(msg);
        msg = next;
    }

    iot_free(client);
}

int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options) {
    if (!client || !options || !options->host || !options->client_id) {
        return MQTT_ERR_PARAM;
    }

    mqtt_client_disconnect(client);

    memcpy(&client->options, options, sizeof(mqtt_connect_options_t));

    if (client->options.port == 0) {
        client->options.port = client->options.use_ssl ? MQTT_DEFAULT_SSL_PORT : MQTT_DEFAULT_PORT;
    }

    if (client->options.keepalive == 0) {
        client->options.keepalive = MQTT_DEFAULT_KEEPALIVE;
    }

    client->keepalive = client->options.keepalive;
    client->state = MQTT_STATE_CONNECTING;
    client->last_connect_attempt = iot_get_tick_ms();

    const net_ssl_config_t* ssl_config = client->options.use_ssl ? &client->options.ssl_config : NULL;
    client->sock = net_socket_create(SOCK_TYPE_STREAM, ssl_config, mqtt_client_socket_callback, client);
    if (!client->sock) {
        client->state = MQTT_STATE_ERROR;
        client->last_error = MQTT_ERR_CONNECT;
        return MQTT_ERR_CONNECT;
    }

    int ret = net_socket_connect(client->sock, client->options.host, client->options.port);
    if (ret < 0) {
        client->state = MQTT_STATE_ERROR;
        client->last_error = MQTT_ERR_CONNECT;
        net_socket_close(client->sock);
        client->sock = NULL;
        return MQTT_ERR_CONNECT;
    }

    mqtt_manager_add_client(client);

    return 0;
}

int mqtt_client_disconnect(mqtt_client_t* client) {
    if (!client) return MQTT_ERR_PARAM;

    uint8_t buf[4];
    int len = mqtt_packet_encode_disconnect(buf, sizeof(buf));
    if (len > 0 && client->sock) {
        net_socket_send(client->sock, buf, len);
    }

    mqtt_manager_remove_client(client);

    if (client->sock) {
        net_socket_close(client->sock);
        client->sock = NULL;
    }

    client->recv_len = 0;
    client->state = MQTT_STATE_DISCONNECTED;

    return 0;
}

int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain) {
    if (!client || !topic || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return MQTT_ERR_PARAM;
    }

    uint8_t buf[MQTT_MAX_PACKET_SIZE];
    mqtt_packet_publish_t publish = {
        .dup = false,
        .qos = qos,
        .retain = retain,
        .packet_id = (qos > 0) ? (client->next_packet_id++) : 0,
        .topic = topic,
        .payload = payload,
        .payload_len = payload_len
    };

    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }

    int len = mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }

    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }

    if (qos > 0) {
        mqtt_outgoing_msg_t* msg = (mqtt_outgoing_msg_t*)iot_malloc(sizeof(mqtt_outgoing_msg_t));
        if (!msg) return MQTT_ERR_MEMORY;

        memset(msg, 0, sizeof(mqtt_outgoing_msg_t));
        msg->packet_id = publish.packet_id;
        msg->qos = qos;
        msg->topic = (char*)iot_strdup(topic);
        if (!msg->topic) {
            iot_free(msg);
            return MQTT_ERR_MEMORY;
        }

        if (payload && payload_len > 0) {
            msg->payload = (uint8_t*)iot_malloc(payload_len);
            if (!msg->payload) {
                iot_free(msg->topic);
                iot_free(msg);
                return MQTT_ERR_MEMORY;
            }
            memcpy(msg->payload, payload, payload_len);
            msg->payload_len = payload_len;
        }
        msg->retain = retain;
        msg->send_time = iot_get_tick_ms();

        msg->next = NULL;
        if (client->outgoing_tail) {
            client->outgoing_tail->next = msg;
            client->outgoing_tail = msg;
        } else {
            client->outgoing_head = client->outgoing_tail = msg;
        }
        client->outgoing_count++;
    }

    return 0;
}

int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data) {
    if (!client || !topic_filter || !callback || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return MQTT_ERR_PARAM;
    }

    mqtt_subscribe_entry_t* existing = mqtt_client_subscribe_find(client, topic_filter);
    if (existing) {
        existing->qos = qos;
        existing->callback = callback;
        existing->user_data = user_data;
    } else {
        if (!mqtt_client_subscribe_add(client, topic_filter, qos, callback, user_data)) {
            return MQTT_ERR_MEMORY;
        }
    }

    uint8_t buf[512];
    mqtt_packet_subscribe_t subscribe = {
        .packet_id = client->next_packet_id++,
        .topic_filters = &topic_filter,
        .requested_qos = &qos,
        .topic_count = 1
    };

    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }

    int len = mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }

    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        mqtt_client_subscribe_remove(client, topic_filter);
        return MQTT_ERR_SEND;
    }

    return 0;
}

int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return MQTT_ERR_PARAM;
    }

    uint8_t buf[512];
    mqtt_packet_unsubscribe_t unsubscribe = {
        .packet_id = client->next_packet_id++,
        .topic_filters = &topic_filter,
        .topic_count = 1
    };

    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }

    int len = mqtt_packet_encode_unsubscribe(&unsubscribe, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }

    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }

    mqtt_client_subscribe_remove(client, topic_filter);

    return 0;
}

static void mqtt_client_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    mqtt_client_t* client = (mqtt_client_t*)user_data;
    if (!client) return;

    mqtt_manager_on_socket_event(client, event);
}

void mqtt_client_set_event_callback(mqtt_client_t* client,
                                    mqtt_event_callback_t callback, void* user_data) {
    if (client) {
        client->event_callback = callback;
        client->event_user_data = user_data;
    }
}

void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms) {
    if (client) {
        client->auto_reconnect = true;
        client->reconnect_interval_ms = interval_ms;
    }
}

void mqtt_client_disable_auto_reconnect(mqtt_client_t* client) {
    if (client) {
        client->auto_reconnect = false;
    }
}

mqtt_state_t mqtt_client_get_state(mqtt_client_t* client) {
    return client ? client->state : MQTT_STATE_DISCONNECTED;
}

mqtt_error_t mqtt_client_get_error(mqtt_client_t* client) {
    return client ? client->last_error : MQTT_ERR_PARAM;
}

bool mqtt_client_is_connected(mqtt_client_t* client) {
    return client && client->state == MQTT_STATE_CONNECTED;
}

int mqtt_manager_start(void) {
    return mqtt_manager_init();
}

void mqtt_manager_stop(void) {
    mqtt_manager_deinit();
}

static mqtt_subscribe_entry_t* mqtt_client_subscribe_find(mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter) return NULL;
    mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

static mqtt_subscribe_entry_t* mqtt_client_subscribe_add(mqtt_client_t* client, const char* topic_filter,
                                                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data) {
    if (!client || !topic_filter || !callback) return NULL;

    mqtt_subscribe_entry_t* entry = (mqtt_subscribe_entry_t*)iot_malloc(sizeof(mqtt_subscribe_entry_t));
    if (!entry) return NULL;

    memset(entry, 0, sizeof(mqtt_subscribe_entry_t));
    entry->topic_filter = (char*)iot_strdup(topic_filter);
    if (!entry->topic_filter) {
        iot_free(entry);
        return NULL;
    }
    entry->qos = qos;
    entry->callback = callback;
    entry->user_data = user_data;

    if (!client->subscribe_head) {
        client->subscribe_head = entry;
        client->subscribe_tail = entry;
    } else {
        client->subscribe_tail->next = entry;
        client->subscribe_tail = entry;
    }
    client->subscribe_count++;

    return entry;
}

static void mqtt_client_subscribe_remove(mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter) return;

    mqtt_subscribe_entry_t** pp = &client->subscribe_head;
    while (*pp) {
        mqtt_subscribe_entry_t* entry = *pp;
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            *pp = entry->next;
            if (entry == client->subscribe_tail) {
                client->subscribe_tail = *pp;
            }
            client->subscribe_count--;
            iot_free(entry->topic_filter);
            iot_free(entry);
            break;
        }
        pp = &(*pp)->next;
    }
}

static void mqtt_client_subscribe_destroy_all(mqtt_client_t* client) {
    if (!client) return;

    mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        mqtt_subscribe_entry_t* next = entry->next;
        iot_free(entry->topic_filter);
        iot_free(entry);
        entry = next;
    }
    client->subscribe_head = NULL;
    client->subscribe_tail = NULL;
    client->subscribe_count = 0;
}