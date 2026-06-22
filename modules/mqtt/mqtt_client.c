/**
 * @file mqtt_client.c
 * @brief MQTT 客户端实现
 *
 * 基于 net_socket 实现的 MQTT 3.1.1 客户端，支持完整的 QoS 0/1/2、自动重连等
 * 支持 SSL/TLS 安全连接（基于 GmSSL）
 */

#include "mqtt_client.h"
#include "platform.h"
#include "net.h"

#include <string.h>
#include <stdlib.h>

#define MQTT_DEFAULT_KEEPALIVE      60
#define MQTT_DEFAULT_PORT           1883
#define MQTT_DEFAULT_TIMEOUT        30000
#define MQTT_DEFAULT_SSL_PORT       8883

struct mqtt_client {
    sock_t sock;
    net_socket_t* socket;
    
    net_ssl_socket_t* ssl_socket;
    
    mqtt_state_t state;
    mqtt_error_t last_error;
    
    mqtt_connect_options_t options;
    
    mqtt_event_t event;
    
    uint16_t next_packet_id;
    
    uint8_t* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    mqtt_publisher_t publisher;
    
    mqtt_subscribe_entry_t* subscribe_head;
    mqtt_subscribe_entry_t* subscribe_tail;
    size_t subscribe_count;
    
    mqtt_reconnect_t reconnect;
    mqtt_keepalive_t keepalive;
};

static int mqtt_client_send_raw(mqtt_client_t* client, const uint8_t* buf, size_t len);
static void mqtt_client_socket_callback(sock_t sock, net_event_type_t event, void* user_data);
static int mqtt_client_send_connect(mqtt_client_t* client);
static int mqtt_client_process_packet(mqtt_client_t* client);
static void mqtt_client_handle_publish(mqtt_client_t* client, const mqtt_packet_publish_t* publish);

static mqtt_subscribe_entry_t* mqtt_client_subscribe_find(mqtt_client_t* client, const char* topic_filter);
static mqtt_subscribe_entry_t* mqtt_client_subscribe_add(mqtt_client_t* client, const char* topic_filter, mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);
static void mqtt_client_subscribe_remove(mqtt_client_t* client, const char* topic_filter);
static void mqtt_client_subscribe_destroy_all(mqtt_client_t* client);

mqtt_client_t* mqtt_client_create(void) {
    mqtt_client_t* client = (mqtt_client_t*)iot_malloc(sizeof(mqtt_client_t));
    if (!client) return NULL;
    
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
        iot_free(client);
        return NULL;
    }
    
    mqtt_publisher_init(&client->publisher);
    mqtt_event_init(&client->event);
    mqtt_keepalive_init(&client->keepalive);
    mqtt_reconnect_init(&client->reconnect);
    
    return client;
}

void mqtt_client_destroy(mqtt_client_t* client) {
    if (!client) return;
    
    mqtt_client_disconnect(client);
    
    if (client->recv_buf) {
        iot_free(client->recv_buf);
    }
    
    mqtt_publisher_deinit(&client->publisher);
    mqtt_client_subscribe_destroy_all(client);
    
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
    
    mqtt_keepalive_configure(&client->keepalive, client->options.keepalive, client->options.timeout_ms);
    
    client->state = MQTT_STATE_CONNECTING;
    
    if (client->options.use_ssl) {
        client->ssl_socket = net_ssl_socket_create(SOCK_TYPE_STREAM,
                                                   (net_ssl_config_t*)client->options.ssl_config,
                                                   mqtt_client_socket_callback, client);
        if (!client->ssl_socket) {
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            return MQTT_ERR_CONNECT;
        }
        
        int ret = net_ssl_connect(client->ssl_socket, options->host, options->port);
        if (ret < 0) {
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            net_ssl_socket_destroy(client->ssl_socket);
            client->ssl_socket = NULL;
            return MQTT_ERR_CONNECT;
        }
    } else {
        client->socket = net_socket_create(SOCK_TYPE_STREAM, mqtt_client_socket_callback, client);
        if (!client->socket) {
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            return MQTT_ERR_CONNECT;
        }
        client->sock = (sock_t)client->socket;
        
        int ret = net_socket_connect(client->sock, options->host, options->port);
        if (ret < 0) {
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            net_socket_close(client->sock);
            client->sock = 0;
            client->socket = NULL;
            return MQTT_ERR_CONNECT;
        }
    }
    
    mqtt_reconnect_mark_attempt(&client->reconnect);
    
    return 0;
}

int mqtt_client_disconnect(mqtt_client_t* client) {
    if (!client) return MQTT_ERR_PARAM;
    
    uint8_t buf[4];
    int len = mqtt_packet_encode_disconnect(buf, sizeof(buf));
    if (len > 0) {
        mqtt_client_send_raw(client, buf, len);
    }
    
    if (client->ssl_socket) {
        net_ssl_close(client->ssl_socket);
        net_ssl_socket_destroy(client->ssl_socket);
        client->ssl_socket = NULL;
    }
    
    if (client->sock) {
        net_socket_close(client->sock);
        client->sock = 0;
        client->socket = NULL;
    }
    
    client->state = MQTT_STATE_DISCONNECTED;
    
    mqtt_publisher_clear_retry_count(&client->publisher);
    
    return 0;
}

static int mqtt_client_send_raw(mqtt_client_t* client, const uint8_t* buf, size_t len) {
    if (!client) return -1;
    
    if (client->ssl_socket) {
        return net_ssl_send(client->ssl_socket, buf, len);
    }
    
    if (!client->sock) return -1;
    return net_socket_send(client->sock, buf, len);
}

int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain) {
    if (!client || !topic || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }
    
    if (!client->ssl_socket && !client->sock) {
        return MQTT_ERR_PARAM;
    }
    
    uint8_t buf[MQTT_MAX_PACKET_SIZE];
    mqtt_packet_publish_t publish = {
        .dup = false,
        .qos = qos,
        .retain = retain,
        .packet_id = (qos > 0) ? mqtt_client_next_packet_id(client) : 0,
        .topic = topic,
        .payload = payload,
        .payload_len = payload_len
    };
    
    int len = mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }
    
    int ret = mqtt_client_send_raw(client, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    if (qos > 0) {
        mqtt_publisher_add_message(&client->publisher, publish.packet_id, qos,
                                   topic, payload, payload_len, retain);
    }
    
    return 0;
}

int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data) {
    if (!client || !topic_filter || !callback || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }
    
    if (!client->ssl_socket && !client->sock) {
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
        .packet_id = mqtt_client_next_packet_id(client),
        .topic_filters = &topic_filter,
        .requested_qos = &qos,
        .topic_count = 1
    };
    
    int len = mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }
    
    int ret = mqtt_client_send_raw(client, buf, len);
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
    
    if (!client->ssl_socket && !client->sock) {
        return MQTT_ERR_PARAM;
    }
    
    uint8_t buf[512];
    mqtt_packet_unsubscribe_t unsubscribe = {
        .packet_id = mqtt_client_next_packet_id(client),
        .topic_filters = &topic_filter,
        .topic_count = 1
    };
    
    int len = mqtt_packet_encode_unsubscribe(&unsubscribe, buf, sizeof(buf));
    if (len < 0) {
        return MQTT_ERR_SEND;
    }
    
    int ret = mqtt_client_send_raw(client, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    mqtt_client_subscribe_remove(client, topic_filter);
    
    return 0;
}

int mqtt_client_subscribe_multiple(mqtt_client_t* client, const char** topic_filters,
                                   const mqtt_qos_t* qos, mqtt_message_callback_t* callbacks,
                                   void** user_data, size_t topic_count) {
    if (!client || !topic_filters || !qos || !callbacks ||
        client->state != MQTT_STATE_CONNECTED || topic_count == 0) {
        return MQTT_ERR_PARAM;
    }
    
    if (!client->ssl_socket && !client->sock) {
        return MQTT_ERR_PARAM;
    }
    
    for (size_t i = 0; i < topic_count; i++) {
        if (!topic_filters[i] || !callbacks[i]) {
            return MQTT_ERR_PARAM;
        }
        
        mqtt_subscribe_entry_t* existing = mqtt_client_subscribe_find(client, topic_filters[i]);
        if (existing) {
            existing->qos = qos[i];
            existing->callback = callbacks[i];
            existing->user_data = user_data ? user_data[i] : NULL;
        } else {
            if (!mqtt_client_subscribe_add(client, topic_filters[i], qos[i],
                                           callbacks[i], user_data ? user_data[i] : NULL)) {
                for (size_t j = 0; j < i; j++) {
                    mqtt_client_subscribe_remove(client, topic_filters[j]);
                }
                return MQTT_ERR_MEMORY;
            }
        }
    }
    
    uint8_t buf[MQTT_MAX_PACKET_SIZE];
    mqtt_packet_subscribe_t subscribe = {
        .packet_id = mqtt_client_next_packet_id(client),
        .topic_filters = topic_filters,
        .requested_qos = qos,
        .topic_count = (uint16_t)topic_count
    };
    
    int len = mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
    if (len < 0) {
        for (size_t i = 0; i < topic_count; i++) {
            mqtt_client_subscribe_remove(client, topic_filters[i]);
        }
        return MQTT_ERR_SEND;
    }
    
    int ret = mqtt_client_send_raw(client, buf, len);
    if (ret < 0) {
        for (size_t i = 0; i < topic_count; i++) {
            mqtt_client_subscribe_remove(client, topic_filters[i]);
        }
        return MQTT_ERR_SEND;
    }
    
    return 0;
}

static void mqtt_client_socket_callback(sock_t sock, net_event_type_t event, void* user_data) {
    mqtt_client_t* client = (mqtt_client_t*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            mqtt_client_send_connect(client);
            break;
        }
        
        case NET_EVENT_RECV: {
            uint8_t buf[4096];
            int len;
            
            if (client->ssl_socket) {
                len = net_ssl_recv(client->ssl_socket, buf, sizeof(buf));
            } else {
                len = net_socket_recv(sock, buf, sizeof(buf));
            }
            
            while (len > 0) {
                if (client->recv_len + len > client->recv_capacity) {
                    size_t new_cap = client->recv_capacity + MQTT_MAX_PACKET_SIZE;
                    uint8_t* new_buf = (uint8_t*)iot_realloc(client->recv_buf, new_cap);
                    if (!new_buf) return;
                    client->recv_buf = new_buf;
                    client->recv_capacity = new_cap;
                }
                
                memcpy(client->recv_buf + client->recv_len, buf, len);
                client->recv_len += len;
                
                while (client->recv_len > 0) {
                    int ret = mqtt_client_process_packet(client);
                    if (ret < 0) break;
                    if (ret == 0) break;
                }
                
                if (client->ssl_socket) {
                    len = net_ssl_recv(client->ssl_socket, buf, sizeof(buf));
                } else {
                    len = net_socket_recv(sock, buf, sizeof(buf));
                }
            }
            break;
        }
        
        case NET_EVENT_DISCONNECTED: {
            if (client->ssl_socket) {
                net_ssl_socket_destroy(client->ssl_socket);
                client->ssl_socket = NULL;
            }
            client->sock = 0;
            client->socket = NULL;
            client->state = MQTT_STATE_DISCONNECTED;
            
            mqtt_event_trigger(&client->event, MQTT_EVENT_DISCONNECTED);
            break;
        }
        
        case NET_EVENT_ERROR: {
            if (client->ssl_socket) {
                net_ssl_socket_destroy(client->ssl_socket);
                client->ssl_socket = NULL;
            }
            client->sock = 0;
            client->socket = NULL;
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            
            mqtt_event_trigger(&client->event, MQTT_EVENT_ERROR);
            break;
        }
        
        default:
            break;
    }
}

static int mqtt_client_send_connect(mqtt_client_t* client) {
    uint8_t buf[512];
    
    mqtt_packet_connect_t connect = {
        .protocol_name = MQTT_PROTOCOL_NAME,
        .protocol_version = MQTT_PROTOCOL_VERSION,
        .clean_session = client->options.clean_session,
        .will_flag = (client->options.will_topic != NULL),
        .will_qos = client->options.will_qos,
        .will_retain = client->options.will_retain,
        .password_flag = (client->options.password != NULL),
        .username_flag = (client->options.username != NULL),
        .keepalive = (uint16_t)client->options.keepalive,
        .client_id = client->options.client_id,
        .will_topic = client->options.will_topic,
        .will_message = client->options.will_message,
        .username = client->options.username,
        .password = client->options.password
    };
    
    int len = mqtt_packet_encode_connect(&connect, buf, sizeof(buf));
    if (len < 0) {
        client->last_error = MQTT_ERR_SEND;
        return -1;
    }
    
    int ret = mqtt_client_send_raw(client, buf, len);
    return (ret > 0) ? 0 : -1;
}

static int mqtt_client_process_packet(mqtt_client_t* client) {
    if (!client || client->recv_len < 2) {
        return 0;
    }
    
    mqtt_packet_t packet;
    size_t consumed = 0;
    
    int ret = mqtt_packet_decode_header(client->recv_buf, client->recv_len, &packet, &consumed);
    if (ret < 0) {
        mqtt_packet_free(&packet);
        client->last_error = MQTT_ERR_PROTOCOL;
        return -1;
    }
    
    if (ret == 1) {
        mqtt_packet_free(&packet);
        return 0;
    }
    
    switch (packet.type) {
        case MQTT_PACKET_CONNACK: {
            mqtt_packet_connack_t connack;
            if (mqtt_packet_decode_connack(&packet, &connack) == 0) {
                if (connack.return_code == MQTT_CONNACK_ACCEPTED) {
                    client->state = MQTT_STATE_CONNECTED;
                    mqtt_keepalive_reset(&client->keepalive);
                    
                    mqtt_publisher_resend_all(&client->publisher,
                                              (int(*)(const uint8_t*,size_t,void*))mqtt_client_send_raw,
                                              (void*)client);
                    
                    if (!client->options.clean_session) {
                        mqtt_subscribe_entry_t* entry = client->subscribe_head;
                        while (entry) {
                            uint8_t buf[512];
                            mqtt_packet_subscribe_t subscribe = {
                                .packet_id = mqtt_client_next_packet_id(client),
                                .topic_filters = &entry->topic_filter,
                                .requested_qos = &entry->qos,
                                .topic_count = 1
                            };
                            
                            int len = mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
                            if (len > 0) {
                                mqtt_client_send_raw(client, buf, len);
                            }
                            entry = entry->next;
                        }
                    }
                    
                    mqtt_event_trigger(&client->event, MQTT_EVENT_CONNECTED);
                } else {
                    client->state = MQTT_STATE_ERROR;
                    client->last_error = MQTT_ERR_CONNECT;
                    
                    mqtt_event_trigger(&client->event, MQTT_EVENT_ERROR);
                }
            }
            break;
        }
        
        case MQTT_PACKET_PUBLISH: {
            mqtt_packet_publish_t publish;
            if (mqtt_packet_decode_publish(&packet, &publish) == 0) {
                mqtt_client_handle_publish(client, &publish);
            }
            break;
        }
        
        case MQTT_PACKET_PUBACK: {
            mqtt_packet_puback_t puback;
            if (mqtt_packet_decode_puback(&packet, &puback) == 0) {
                mqtt_publisher_remove_message(&client->publisher, puback.packet_id, MQTT_QOS_1);
            }
            break;
        }
        
        case MQTT_PACKET_PUBREC: {
            mqtt_packet_pubrec_t pubrec;
            if (mqtt_packet_decode_pubrec(&packet, &pubrec) == 0) {
                uint8_t buf[8];
                int len = mqtt_packet_encode_pubrel(pubrec.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    mqtt_client_send_raw(client, buf, len);
                }
            }
            break;
        }
        
        case MQTT_PACKET_PUBREL: {
            mqtt_packet_pubrel_t pubrel;
            if (mqtt_packet_decode_pubrel(&packet, &pubrel) == 0) {
                uint8_t buf[8];
                int len = mqtt_packet_encode_pubcomp(pubrel.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    mqtt_client_send_raw(client, buf, len);
                }
            }
            break;
        }
        
        case MQTT_PACKET_PUBCOMP: {
            mqtt_packet_pubcomp_t pubcomp;
            if (mqtt_packet_decode_pubcomp(&packet, &pubcomp) == 0) {
                mqtt_publisher_remove_message(&client->publisher, pubcomp.packet_id, MQTT_QOS_2);
            }
            break;
        }
        
        case MQTT_PACKET_SUBACK: {
            break;
        }
        
        case MQTT_PACKET_UNSUBACK: {
            break;
        }
        
        case MQTT_PACKET_PINGRESP: {
            mqtt_keepalive_mark_pong(&client->keepalive);
            break;
        }
        
        case MQTT_PACKET_DISCONNECT: {
            mqtt_client_disconnect(client);
            break;
        }
        
        default:
            break;
    }
    
    mqtt_packet_free(&packet);
    
    memmove(client->recv_buf, client->recv_buf + consumed, client->recv_len - consumed);
    client->recv_len -= consumed;
    
    return 1;
}

static void mqtt_client_handle_publish(mqtt_client_t* client, const mqtt_packet_publish_t* publish) {
    if (!client || !publish) return;
    
    if (publish->qos == MQTT_QOS_1) {
        uint8_t buf[8];
        int len = mqtt_packet_encode_puback(publish->packet_id, buf, sizeof(buf));
        if (len > 0) {
            mqtt_client_send_raw(client, buf, len);
        }
    } else if (publish->qos == MQTT_QOS_2) {
        uint8_t buf[8];
        int len = mqtt_packet_encode_pubrec(publish->packet_id, buf, sizeof(buf));
        if (len > 0) {
            mqtt_client_send_raw(client, buf, len);
        }
    }
    
    mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        if (mqtt_topic_match(entry->topic_filter, publish->topic)) {
            if (entry->callback) {
                entry->callback(entry->user_data, publish->topic,
                               publish->payload, publish->payload_len,
                               publish->qos, publish->retain);
            }
        }
        entry = entry->next;
    }
}

void mqtt_client_poll(mqtt_client_t* client, int timeout_ms) {
    if (!client) return;
    
    /* 心跳超时检测 */
    if (client->state == MQTT_STATE_CONNECTED && mqtt_keepalive_check_timeout(&client->keepalive)) {
        client->last_error = MQTT_ERR_CONNECT;
        mqtt_client_disconnect(client);
        
        mqtt_event_trigger(&client->event, MQTT_EVENT_ERROR);
        return;
    }
    
    /* 心跳 */
    if (client->state == MQTT_STATE_CONNECTED && mqtt_keepalive_needs_ping(&client->keepalive)) {
        uint8_t buf[4];
        int len = mqtt_packet_encode_pingreq(buf, sizeof(buf));
        if (len > 0) {
            mqtt_client_send_raw(client, buf, len);
        }
        mqtt_keepalive_mark_ping(&client->keepalive);
    }
    
    /* 重发消息 */
    mqtt_publisher_resend_messages(&client->publisher,
                                    (int(*)(const uint8_t*,size_t,void*))mqtt_client_send_raw,
                                    (void*)client);
    
    /* 自动重连 */
    if (mqtt_reconnect_needs_retry(&client->reconnect)) {
        mqtt_client_connect(client, &client->options);
    }
    
    /* 处理接收缓冲区中的数据 */
    while (client->recv_len > 0) {
        int ret = mqtt_client_process_packet(client);
        if (ret <= 0) break;
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

void mqtt_client_set_event_callback(mqtt_client_t* client,
                                    mqtt_event_callback_t callback, void* user_data) {
    if (client) {
        mqtt_event_set_callback(&client->event, callback, user_data);
    }
}

void mqtt_client_enable_auto_reconnect(mqtt_client_t* client, int interval_ms) {
    if (client) {
        mqtt_reconnect_enable(&client->reconnect, interval_ms);
    }
}

void mqtt_client_disable_auto_reconnect(mqtt_client_t* client) {
    if (client) {
        mqtt_reconnect_disable(&client->reconnect);
    }
}

void mqtt_client_set_socket(mqtt_client_t* client, sock_t sock) {
    if (client) {
        client->sock = sock;
    }
}

uint16_t mqtt_client_next_packet_id(mqtt_client_t* client) {
    if (!client) return 0;
    uint16_t id = client->next_packet_id++;
    if (client->next_packet_id == 0) {
        client->next_packet_id = 1;
    }
    return id;
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