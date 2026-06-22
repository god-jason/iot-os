#include "mqtt_client.h"
#include "platform.h"
#include "net.h"

#include <string.h>
#include <stdlib.h>

#define MQTT_DEFAULT_KEEPALIVE      60
#define MQTT_DEFAULT_PORT           1883
#define MQTT_DEFAULT_TIMEOUT        30000
#define MQTT_DEFAULT_SSL_PORT       8883
#define MQTT_MANAGER_POLL_INTERVAL  50
#define MQTT_MAX_RETRY              3
#define MQTT_RETRY_INTERVAL_MS      5000

static mqtt_client_t* s_mqtt_clients = NULL;
static iot_mutex_t s_mqtt_mutex;
static iot_task_t s_mqtt_manager_task;
static bool s_mqtt_running = false;

static int mqtt_client_send_raw(mqtt_client_t* client, const uint8_t* buf, size_t len);
static void mqtt_client_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);
static int mqtt_client_send_connect(mqtt_client_t* client);
static int mqtt_client_process_packet(mqtt_client_t* client);
static void mqtt_client_handle_publish(mqtt_client_t* client, const mqtt_packet_publish_t* publish);
static void mqtt_client_trigger_event(mqtt_client_t* client, mqtt_event_type_t event);

static mqtt_subscribe_entry_t* mqtt_client_subscribe_find(mqtt_client_t* client, const char* topic_filter);
static mqtt_subscribe_entry_t* mqtt_client_subscribe_add(mqtt_client_t* client, const char* topic_filter, mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data);
static void mqtt_client_subscribe_remove(mqtt_client_t* client, const char* topic_filter);
static void mqtt_client_subscribe_destroy_all(mqtt_client_t* client);

static void mqtt_manager_add_client(mqtt_client_t* client);
static void mqtt_manager_remove_client(mqtt_client_t* client);
static void mqtt_manager_thread(void* arg);

static void mqtt_client_send_ping(mqtt_client_t* client);
static void mqtt_client_resend_messages(mqtt_client_t* client);
static void mqtt_client_cleanup(mqtt_client_t* client);

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
        mqtt_client_send_raw(client, buf, len);
    }
    
    mqtt_manager_remove_client(client);
    mqtt_client_cleanup(client);
    
    client->state = MQTT_STATE_DISCONNECTED;
    
    return 0;
}

static void mqtt_client_cleanup(mqtt_client_t* client) {
    if (!client) return;
    
    if (client->sock) {
        net_socket_close(client->sock);
        client->sock = NULL;
    }
    
    client->recv_len = 0;
}

static int mqtt_client_send_raw(mqtt_client_t* client, const uint8_t* buf, size_t len) {
    if (!client || !client->sock) return -1;
    return net_socket_send(client->sock, buf, len);
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
    
    int ret = mqtt_client_send_raw(client, buf, len);
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
    
    int ret = mqtt_client_send_raw(client, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    mqtt_client_subscribe_remove(client, topic_filter);
    
    return 0;
}

static void mqtt_client_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    mqtt_client_t* client = (mqtt_client_t*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            mqtt_client_send_connect(client);
            break;
        }
        
        case NET_EVENT_RECV: {
            const char* buf = net_socket_get_recv_buf((sock_t)sock);
            size_t len = net_socket_get_recv_len((sock_t)sock);
            
            if (len > 0 && client->recv_len + len > client->recv_capacity) {
                size_t new_cap = client->recv_capacity + MQTT_MAX_PACKET_SIZE;
                uint8_t* new_buf = (uint8_t*)iot_realloc(client->recv_buf, new_cap);
                if (!new_buf) return;
                client->recv_buf = new_buf;
                client->recv_capacity = new_cap;
            }
            
            memcpy(client->recv_buf + client->recv_len, buf, len);
            client->recv_len += len;
            net_socket_clear_recv_buf((sock_t)sock);
            
            while (client->recv_len > 0) {
                int ret = mqtt_client_process_packet(client);
                if (ret <= 0) break;
            }
            break;
        }
        
        case NET_EVENT_DISCONNECTED: {
            mqtt_client_cleanup(client);
            client->state = MQTT_STATE_DISCONNECTED;
            mqtt_client_trigger_event(client, MQTT_EVENT_DISCONNECTED);
            break;
        }
        
        case NET_EVENT_ERROR: {
            mqtt_client_cleanup(client);
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            mqtt_client_trigger_event(client, MQTT_EVENT_ERROR);
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
                    client->last_ping_time = iot_get_tick_ms();
                    client->ping_sent_time = 0;
                    
                    mqtt_outgoing_msg_t* msg = client->outgoing_head;
                    while (msg) {
                        mqtt_outgoing_msg_t* next = msg->next;
                        uint8_t buf[MQTT_MAX_PACKET_SIZE];
                        mqtt_packet_publish_t publish = {
                            .dup = true,
                            .qos = msg->qos,
                            .retain = msg->retain,
                            .packet_id = msg->packet_id,
                            .topic = msg->topic,
                            .payload = msg->payload,
                            .payload_len = msg->payload_len
                        };
                        
                        int len = mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
                        if (len > 0) {
                            mqtt_client_send_raw(client, buf, len);
                        }
                        msg->send_time = iot_get_tick_ms();
                        msg->retry_count = 0;
                        msg = next;
                    }
                    
                    if (!client->options.clean_session) {
                        mqtt_subscribe_entry_t* entry = client->subscribe_head;
                        while (entry) {
                            uint8_t buf[512];
                            mqtt_packet_subscribe_t subscribe = {
                                .packet_id = client->next_packet_id++,
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
                    
                    mqtt_client_trigger_event(client, MQTT_EVENT_CONNECTED);
                } else {
                    client->state = MQTT_STATE_ERROR;
                    client->last_error = MQTT_ERR_CONNECT;
                    mqtt_client_trigger_event(client, MQTT_EVENT_ERROR);
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
                mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    mqtt_outgoing_msg_t* msg = *pp;
                    if (msg->packet_id == puback.packet_id && msg->qos == MQTT_QOS_1) {
                        *pp = msg->next;
                        if (msg == client->outgoing_tail) {
                            client->outgoing_tail = *pp;
                        }
                        client->outgoing_count--;
                        if (msg->topic) iot_free(msg->topic);
                        if (msg->payload) iot_free(msg->payload);
                        iot_free(msg);
                        break;
                    }
                    pp = &(*pp)->next;
                }
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
                mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    mqtt_outgoing_msg_t* msg = *pp;
                    if (msg->packet_id == pubcomp.packet_id && msg->qos == MQTT_QOS_2) {
                        *pp = msg->next;
                        if (msg == client->outgoing_tail) {
                            client->outgoing_tail = *pp;
                        }
                        client->outgoing_count--;
                        if (msg->topic) iot_free(msg->topic);
                        if (msg->payload) iot_free(msg->payload);
                        iot_free(msg);
                        break;
                    }
                    pp = &(*pp)->next;
                }
            }
            break;
        }
        
        case MQTT_PACKET_SUBACK: {
            mqtt_client_trigger_event(client, MQTT_EVENT_SUBSCRIBED);
            break;
        }
        
        case MQTT_PACKET_UNSUBACK: {
            mqtt_client_trigger_event(client, MQTT_EVENT_UNSUBSCRIBED);
            break;
        }
        
        case MQTT_PACKET_PINGRESP: {
            client->last_ping_time = iot_get_tick_ms();
            client->ping_sent_time = 0;
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
                entry->callback(publish->topic, publish->payload,
                               publish->payload_len, publish->qos, publish->retain, entry->user_data);
            }
        }
        entry = entry->next;
    }
}

static void mqtt_client_trigger_event(mqtt_client_t* client, mqtt_event_type_t event) {
    if (client && client->event_callback) {
        client->event_callback(client, event, client->event_user_data);
    }
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

static void mqtt_client_send_ping(mqtt_client_t* client) {
    if (!client || !client->sock || client->state != MQTT_STATE_CONNECTED) {
        return;
    }
    
    uint8_t buf[4];
    int len = mqtt_packet_encode_pingreq(buf, sizeof(buf));
    if (len > 0) {
        mqtt_client_send_raw(client, buf, len);
        client->ping_sent_time = iot_get_tick_ms();
    }
}

static void mqtt_client_resend_messages(mqtt_client_t* client) {
    if (!client || !client->sock || client->state != MQTT_STATE_CONNECTED) {
        return;
    }
    
    uint32_t now = iot_get_tick_ms();
    mqtt_outgoing_msg_t** pp = &client->outgoing_head;
    
    while (*pp) {
        mqtt_outgoing_msg_t* msg = *pp;
        
        if (now - msg->send_time > MQTT_RETRY_INTERVAL_MS && msg->retry_count < MQTT_MAX_RETRY) {
            uint8_t buf[MQTT_MAX_PACKET_SIZE];
            mqtt_packet_publish_t publish = {
                .dup = true,
                .qos = msg->qos,
                .retain = msg->retain,
                .packet_id = msg->packet_id,
                .topic = msg->topic,
                .payload = msg->payload,
                .payload_len = msg->payload_len
            };
            
            int len = mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
            if (len > 0) {
                int ret = mqtt_client_send_raw(client, buf, len);
                if (ret > 0) {
                    msg->send_time = now;
                    msg->retry_count++;
                }
            }
        }
        
        if (msg->retry_count >= MQTT_MAX_RETRY) {
            *pp = msg->next;
            if (msg == client->outgoing_tail) {
                client->outgoing_tail = *pp;
            }
            client->outgoing_count--;
            if (msg->topic) iot_free(msg->topic);
            if (msg->payload) iot_free(msg->payload);
            iot_free(msg);
        } else {
            pp = &(*pp)->next;
        }
    }
}

static void mqtt_manager_add_client(mqtt_client_t* client) {
    iot_mutex_lock(s_mqtt_mutex, -1);
    client->next = s_mqtt_clients;
    s_mqtt_clients = client;
    iot_mutex_unlock(s_mqtt_mutex);
}

static void mqtt_manager_remove_client(mqtt_client_t* client) {
    iot_mutex_lock(s_mqtt_mutex, -1);
    if (s_mqtt_clients == client) {
        s_mqtt_clients = client->next;
    } else {
        mqtt_client_t* prev = s_mqtt_clients;
        while (prev && prev->next != client) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = client->next;
        }
    }
    client->next = NULL;
    iot_mutex_unlock(s_mqtt_mutex);
}

static void mqtt_manager_thread(void* arg) {
    while (s_mqtt_running) {
        iot_mutex_lock(s_mqtt_mutex, -1);
        
        mqtt_client_t* client = s_mqtt_clients;
        while (client) {
            mqtt_client_t* next = client->next;
            
            if (client->state == MQTT_STATE_CONNECTED) {
                uint32_t now = iot_get_tick_ms();
                
                if (client->ping_sent_time > 0 && 
                    now - client->ping_sent_time > (uint32_t)client->options.timeout_ms) {
                    client->state = MQTT_STATE_ERROR;
                    client->last_error = MQTT_ERR_KEEPALIVE;
                    mqtt_client_cleanup(client);
                    mqtt_client_trigger_event(client, MQTT_EVENT_ERROR);
                }
                
                if (client->keepalive > 0 && client->ping_sent_time == 0 &&
                    now - client->last_ping_time > (uint32_t)client->keepalive * 1000 / 2) {
                    mqtt_client_send_ping(client);
                }
                
                mqtt_client_resend_messages(client);
            }
            
            if ((client->state == MQTT_STATE_DISCONNECTED || 
                 client->state == MQTT_STATE_ERROR) &&
                client->auto_reconnect) {
                uint32_t now = iot_get_tick_ms();
                if (now - client->last_connect_attempt >= (uint32_t)client->reconnect_interval_ms) {
                    mqtt_client_connect(client, &client->options);
                }
            }
            
            client = next;
        }
        
        iot_mutex_unlock(s_mqtt_mutex);
        
        iot_task_delay(MQTT_MANAGER_POLL_INTERVAL);
    }
}

int mqtt_manager_start(void) {
    s_mqtt_mutex = iot_mutex_create();
    if (!s_mqtt_mutex) {
        return -1;
    }
    
    s_mqtt_running = true;
    s_mqtt_manager_task = iot_task_create("mqtt_mgr", mqtt_manager_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
    if (!s_mqtt_manager_task) {
        iot_mutex_destroy(s_mqtt_mutex);
        s_mqtt_running = false;
        return -1;
    }
    
    return 0;
}

void mqtt_manager_stop(void) {
    s_mqtt_running = false;
    
    iot_mutex_lock(s_mqtt_mutex, -1);
    while (s_mqtt_clients) {
        mqtt_client_t* client = s_mqtt_clients;
        s_mqtt_clients = client->next;
        mqtt_client_disconnect(client);
    }
    iot_mutex_unlock(s_mqtt_mutex);
    
    iot_task_delay(100);
    
    if (s_mqtt_manager_task) {
        iot_task_destroy(s_mqtt_manager_task);
    }
    
    if (s_mqtt_mutex) {
        iot_mutex_destroy(s_mqtt_mutex);
    }
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