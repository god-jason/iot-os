/**
 * @file mqtt_client.c
 * @brief MQTT 客户端实现
 *
 * 实现 MQTT 客户端的核心功能，包括连接管理、消息收发、订阅处理、
 * QoS 消息队列管理、自动重连机制等
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "mqtt_client.h"
#include "mqtt_manager.h"
#include "mqtt_packets.h"

#include "iot.h"
#include "net.h"
#include "iot_log.h"
#include "iot_list.h"

#include <string.h>
#include <stdlib.h>

#define IOT_MQTT_DEFAULT_KEEPALIVE      60
#define IOT_MQTT_DEFAULT_PORT           1883
#define IOT_MQTT_DEFAULT_TIMEOUT        30000
#define IOT_MQTT_DEFAULT_SSL_PORT       8883

static void iot_mqtt_client_socket_callback(iot_net_socket_t* sock, iot_net_event_type_t event, void* user_data);

static iot_mqtt_subscribe_entry_t* iot_mqtt_client_subscribe_find(iot_mqtt_client_t* client, const char* topic_filter);
static iot_mqtt_subscribe_entry_t* iot_mqtt_client_subscribe_add(iot_mqtt_client_t* client, const char* topic_filter, iot_mqtt_qos_t qos, iot_mqtt_message_callback_t callback, void* user_data);
static void iot_mqtt_client_subscribe_remove(iot_mqtt_client_t* client, const char* topic_filter);
static void iot_mqtt_client_subscribe_destroy_all(iot_mqtt_client_t* client);
static void iot_mqtt_client_options_free(iot_mqtt_client_t* client);
static int iot_mqtt_client_options_copy(iot_mqtt_client_t* client, const iot_mqtt_connect_options_t* src);

iot_mqtt_client_t* iot_mqtt_client_create(void) {
    iot_mqtt_client_t* client = (iot_mqtt_client_t*)iot_malloc(sizeof(iot_mqtt_client_t));
    if (!client) {
        LOG_ERROR("mqtt client create failed: out of memory");
        return NULL;
    }

    memset(client, 0, sizeof(iot_mqtt_client_t));
    list_init(&client->list_node);
    client->state = IOT_MQTT_STATE_DISCONNECTED;
    client->next_packet_id = 1;
    client->options.keepalive = IOT_MQTT_DEFAULT_KEEPALIVE;
    client->options.port = IOT_MQTT_DEFAULT_PORT;
    client->options.clean_session = true;
    client->options.timeout_ms = IOT_MQTT_DEFAULT_TIMEOUT;
    client->options.use_ssl = false;

    client->recv_capacity = IOT_MQTT_MAX_PACKET_SIZE;
    client->recv_buf = (uint8_t*)iot_malloc(client->recv_capacity);
    if (!client->recv_buf) {
        LOG_ERROR("mqtt client create failed: recv buffer alloc error");
        iot_free(client);
        return NULL;
    }

    LOG_INFO("mqtt client created");
    return client;
}

void iot_mqtt_client_destroy(iot_mqtt_client_t* client) {
    if (!client) return;

    iot_mqtt_client_disconnect(client);

    if (client->recv_buf) {
        iot_free(client->recv_buf);
    }

    iot_mqtt_client_subscribe_destroy_all(client);
    iot_mqtt_client_options_free(client);

    iot_mqtt_outgoing_msg_t* msg = client->outgoing_head;
    while (msg) {
        iot_mqtt_outgoing_msg_t* next = msg->next;
        if (msg->topic) iot_free(msg->topic);
        if (msg->payload) iot_free(msg->payload);
        iot_free(msg);
        msg = next;
    }

    iot_free(client);
}

int iot_mqtt_client_connect(iot_mqtt_client_t* client, const iot_mqtt_connect_options_t* options) {
    if (!client || !options || !options->host || !options->client_id) {
        return IOT_MQTT_ERR_PARAM;
    }

    iot_mqtt_client_disconnect(client);

    if (iot_mqtt_client_options_copy(client, options) != 0) {
        client->state = IOT_MQTT_STATE_ERROR;
        client->last_error = IOT_MQTT_ERR_MEMORY;
        return IOT_MQTT_ERR_MEMORY;
    }

    if (client->options.port == 0) {
        client->options.port = client->options.use_ssl ? IOT_MQTT_DEFAULT_SSL_PORT : IOT_MQTT_DEFAULT_PORT;
    }

    if (client->options.keepalive == 0) {
        client->options.keepalive = IOT_MQTT_DEFAULT_KEEPALIVE;
    }

    client->keepalive = client->options.keepalive;
    client->state = IOT_MQTT_STATE_CONNECTING;
    client->last_connect_attempt = iot_get_tick_ms();

    const iot_net_ssl_config_t* ssl_config = client->options.use_ssl ? &client->options.ssl_config : NULL;
    client->sock = iot_net_socket_create(IOT_NET_SOCK_STREAM, ssl_config, iot_mqtt_client_socket_callback, client);
    if (!client->sock) {
        client->state = IOT_MQTT_STATE_ERROR;
        client->last_error = IOT_MQTT_ERR_CONNECT;
        return IOT_MQTT_ERR_CONNECT;
    }

    int ret = iot_net_socket_connect(client->sock, client->options.host, client->options.port);
    if (ret < 0) {
        client->state = IOT_MQTT_STATE_ERROR;
        client->last_error = IOT_MQTT_ERR_CONNECT;
        iot_net_socket_close(client->sock);
        client->sock = NULL;
        return IOT_MQTT_ERR_CONNECT;
    }

    iot_mqtt_manager_add_client(client);

    return 0;
}

int iot_mqtt_client_disconnect(iot_mqtt_client_t* client) {
    if (!client) return IOT_MQTT_ERR_PARAM;

    uint8_t buf[4];
    int len = iot_mqtt_packet_encode_disconnect(buf, sizeof(buf));
    if (len > 0 && client->sock) {
        iot_net_socket_send(client->sock, buf, len);
    }

    iot_mqtt_manager_remove_client(client);

    if (client->sock) {
        iot_net_socket_close(client->sock);
        client->sock = NULL;
    }

    client->recv_len = 0;
    client->state = IOT_MQTT_STATE_DISCONNECTED;

    return 0;
}

int iot_mqtt_client_publish(iot_mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        iot_mqtt_qos_t qos, bool retain) {
    if (!client || !topic || client->state != IOT_MQTT_STATE_CONNECTED) {
        return IOT_MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return IOT_MQTT_ERR_PARAM;
    }

    uint8_t buf[IOT_MQTT_MAX_PACKET_SIZE];
    iot_mqtt_packet_publish_t publish = {
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

    int len = iot_mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
    if (len < 0) {
        return IOT_MQTT_ERR_SEND;
    }

    int ret = iot_net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return IOT_MQTT_ERR_SEND;
    }

    if (qos > 0) {
        iot_mqtt_outgoing_msg_t* msg = (iot_mqtt_outgoing_msg_t*)iot_malloc(sizeof(iot_mqtt_outgoing_msg_t));
        if (!msg) return IOT_MQTT_ERR_MEMORY;

        memset(msg, 0, sizeof(iot_mqtt_outgoing_msg_t));
        msg->packet_id = publish.packet_id;
        msg->qos = qos;
        msg->topic = (char*)iot_strdup(topic);
        if (!msg->topic) {
            iot_free(msg);
            return IOT_MQTT_ERR_MEMORY;
        }

        if (payload && payload_len > 0) {
            msg->payload = (uint8_t*)iot_malloc(payload_len);
            if (!msg->payload) {
                iot_free(msg->topic);
                iot_free(msg);
                return IOT_MQTT_ERR_MEMORY;
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

int iot_mqtt_client_subscribe(iot_mqtt_client_t* client, const char* topic_filter,
                          iot_mqtt_qos_t qos, iot_mqtt_message_callback_t callback, void* user_data) {
    if (!client || !topic_filter || !callback || client->state != IOT_MQTT_STATE_CONNECTED) {
        return IOT_MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return IOT_MQTT_ERR_PARAM;
    }

    iot_mqtt_subscribe_entry_t* existing = iot_mqtt_client_subscribe_find(client, topic_filter);
    if (existing) {
        existing->qos = qos;
        existing->callback = callback;
        existing->user_data = user_data;
    } else {
        if (!iot_mqtt_client_subscribe_add(client, topic_filter, qos, callback, user_data)) {
            return IOT_MQTT_ERR_MEMORY;
        }
    }

    uint8_t buf[512];
    iot_mqtt_packet_subscribe_t subscribe = {
        .packet_id = client->next_packet_id++,
        .topic_filters = &topic_filter,
        .requested_qos = &qos,
        .topic_count = 1
    };

    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }

    int len = iot_mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
    if (len < 0) {
        return IOT_MQTT_ERR_SEND;
    }

    int ret = iot_net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        iot_mqtt_client_subscribe_remove(client, topic_filter);
        return IOT_MQTT_ERR_SEND;
    }

    return 0;
}

int iot_mqtt_client_unsubscribe(iot_mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter || client->state != IOT_MQTT_STATE_CONNECTED) {
        return IOT_MQTT_ERR_PARAM;
    }

    if (!client->sock) {
        return IOT_MQTT_ERR_PARAM;
    }

    uint8_t buf[512];
    iot_mqtt_packet_unsubscribe_t unsubscribe = {
        .packet_id = client->next_packet_id++,
        .topic_filters = &topic_filter,
        .topic_count = 1
    };

    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }

    int len = iot_mqtt_packet_encode_unsubscribe(&unsubscribe, buf, sizeof(buf));
    if (len < 0) {
        return IOT_MQTT_ERR_SEND;
    }

    int ret = iot_net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return IOT_MQTT_ERR_SEND;
    }

    iot_mqtt_client_subscribe_remove(client, topic_filter);

    return 0;
}

static void iot_mqtt_client_socket_callback(iot_net_socket_t* sock, iot_net_event_type_t event, void* user_data) {
    iot_mqtt_client_t* client = (iot_mqtt_client_t*)user_data;
    if (!client) return;

    iot_mqtt_manager_on_socket_event(client, event);
}

void iot_mqtt_client_set_event_callback(iot_mqtt_client_t* client,
                                    iot_mqtt_event_callback_t callback, void* user_data) {
    if (client) {
        client->event_callback = callback;
        client->event_user_data = user_data;
    }
}

void iot_mqtt_client_enable_auto_reconnect(iot_mqtt_client_t* client, int interval_ms) {
    if (client) {
        client->auto_reconnect = true;
        client->reconnect_interval_ms = interval_ms;
    }
}

void iot_mqtt_client_disable_auto_reconnect(iot_mqtt_client_t* client) {
    if (client) {
        client->auto_reconnect = false;
    }
}

iot_mqtt_state_t iot_mqtt_client_get_state(iot_mqtt_client_t* client) {
    return client ? client->state : IOT_MQTT_STATE_DISCONNECTED;
}

iot_mqtt_error_t iot_mqtt_client_get_error(iot_mqtt_client_t* client) {
    return client ? client->last_error : IOT_MQTT_ERR_PARAM;
}

bool iot_mqtt_client_is_connected(iot_mqtt_client_t* client) {
    return client && client->state == IOT_MQTT_STATE_CONNECTED;
}

int iot_mqtt_manager_start(void) {
    return iot_mqtt_manager_init();
}

void iot_mqtt_manager_stop(void) {
    iot_mqtt_manager_deinit();
}

static iot_mqtt_subscribe_entry_t* iot_mqtt_client_subscribe_find(iot_mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter) return NULL;
    iot_mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

static iot_mqtt_subscribe_entry_t* iot_mqtt_client_subscribe_add(iot_mqtt_client_t* client, const char* topic_filter,
                                                          iot_mqtt_qos_t qos, iot_mqtt_message_callback_t callback, void* user_data) {
    if (!client || !topic_filter || !callback) return NULL;

    iot_mqtt_subscribe_entry_t* entry = (iot_mqtt_subscribe_entry_t*)iot_malloc(sizeof(iot_mqtt_subscribe_entry_t));
    if (!entry) return NULL;

    memset(entry, 0, sizeof(iot_mqtt_subscribe_entry_t));
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

static void iot_mqtt_client_subscribe_remove(iot_mqtt_client_t* client, const char* topic_filter) {
    if (!client || !topic_filter) return;

    iot_mqtt_subscribe_entry_t** pp = &client->subscribe_head;
    while (*pp) {
        iot_mqtt_subscribe_entry_t* entry = *pp;
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

static void iot_mqtt_client_subscribe_destroy_all(iot_mqtt_client_t* client) {
    if (!client) return;

    iot_mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        iot_mqtt_subscribe_entry_t* next = entry->next;
        iot_free(entry->topic_filter);
        iot_free(entry);
        entry = next;
    }
    client->subscribe_head = NULL;
    client->subscribe_tail = NULL;
    client->subscribe_count = 0;
}

static char* mqtt_strdup_opt(const char* s)
{
    if (!s) {
        return NULL;
    }
    size_t n = strlen(s) + 1;
    char* p = (char*)iot_malloc(n);
    if (p) {
        memcpy(p, s, n);
    }
    return p;
}

static void iot_mqtt_client_options_free(iot_mqtt_client_t* client)
{
    if (!client) {
        return;
    }

    iot_free((void*)client->options.host);
    iot_free((void*)client->options.client_id);
    iot_free((void*)client->options.username);
    iot_free((void*)client->options.password);
    iot_free((void*)client->options.will_topic);
    iot_free((void*)client->options.will_message);
    memset(&client->options, 0, sizeof(client->options));
}

static int iot_mqtt_client_options_copy(iot_mqtt_client_t* client, const iot_mqtt_connect_options_t* src)
{
    iot_mqtt_connect_options_t copy = *src;

    copy.host = mqtt_strdup_opt(src->host);
    copy.client_id = mqtt_strdup_opt(src->client_id);
    copy.username = mqtt_strdup_opt(src->username);
    copy.password = mqtt_strdup_opt(src->password);
    copy.will_topic = mqtt_strdup_opt(src->will_topic);
    copy.will_message = mqtt_strdup_opt(src->will_message);

    if ((src->host && !copy.host) ||
        (src->client_id && !copy.client_id) ||
        (src->username && !copy.username) ||
        (src->password && !copy.password) ||
        (src->will_topic && !copy.will_topic) ||
        (src->will_message && !copy.will_message)) {
        iot_free((void*)copy.host);
        iot_free((void*)copy.client_id);
        iot_free((void*)copy.username);
        iot_free((void*)copy.password);
        iot_free((void*)copy.will_topic);
        iot_free((void*)copy.will_message);
        return -1;
    }

    iot_mqtt_client_options_free(client);
    client->options = copy;
    return 0;
}