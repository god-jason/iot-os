/**
 * @file mqtt_client.c
 * @brief MQTT 客户端实现
 *
 * 基于 net_socket 实现的 MQTT 3.1.1 客户端，支持完整的 QoS 0/1/2、自动重连等
 */

#include "mqtt_client.h"
#include "platform.h"

#include <string.h>
#include <stdlib.h>

/*===========================================================
 * 常量定义
 *===========================================================*/

#define MQTT_DEFAULT_KEEPALIVE      60
#define MQTT_DEFAULT_PORT           1883
#define MQTT_DEFAULT_TIMEOUT        30000
#define MQTT_MAX_RETRY              3
#define MQTT_RETRY_INTERVAL_MS      5000
#define MQTT_MAX_OUTGOING_MSGS      256

/*===========================================================
 * MQTT 客户端结构
 *===========================================================*/

struct mqtt_client {
    sock_t sock;
    net_socket_t* socket;
    
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
    
    bool auto_reconnect;
    int reconnect_interval_ms;
    uint32_t last_connect_time;
    
    uint32_t last_ping_time;
    uint32_t ping_interval_ms;
    
    mqtt_message_callback_t message_callback;
    void* message_user_data;
};

/*===========================================================
 * 静态函数声明
 *===========================================================*/

static void mqtt_client_socket_callback(sock_t sock, net_event_type_t event, void* user_data);
static int mqtt_client_send_connect(mqtt_client_t* client);
static int mqtt_client_process_packet(mqtt_client_t* client);
static void mqtt_client_handle_publish(mqtt_client_t* client, const mqtt_packet_publish_t* publish);
static void mqtt_client_resend_messages(mqtt_client_t* client);

/*===========================================================
 * 客户端创建/销毁
 *===========================================================*/

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
    
    if (client->sock) {
        mqtt_client_disconnect(client);
    }
    
    if (client->recv_buf) {
        iot_free(client->recv_buf);
    }
    
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

/*===========================================================
 * 连接管理
 *===========================================================*/

int mqtt_client_connect(mqtt_client_t* client, const mqtt_connect_options_t* options) {
    if (!client || !options || !options->host || !options->client_id) {
        return MQTT_ERR_PARAM;
    }
    
    /* 关闭已有连接 */
    if (client->sock) {
        net_socket_close(client->sock);
        client->sock = 0;
    }
    
    /* 保存配置 */
    memcpy(&client->options, options, sizeof(mqtt_connect_options_t));
    
    /* 设置默认端口 */
    if (client->options.port == 0) {
        client->options.port = MQTT_DEFAULT_PORT;
    }
    
    /* 设置默认心跳 */
    if (client->options.keepalive == 0) {
        client->options.keepalive = MQTT_DEFAULT_KEEPALIVE;
    }
    
    /* 设置心跳间隔 */
    client->ping_interval_ms = client->options.keepalive * 1000 / 2;
    
    /* 创建 Socket */
    client->state = MQTT_STATE_CONNECTING;
    client->socket = net_socket_create(SOCK_TYPE_STREAM, mqtt_client_socket_callback, client);
    if (!client->socket) {
        client->state = MQTT_STATE_ERROR;
        client->last_error = MQTT_ERR_CONNECT;
        return MQTT_ERR_CONNECT;
    }
    client->sock = (sock_t)client->socket;
    
    /* 连接服务器 */
    int ret = net_socket_connect(client->sock, options->host, options->port);
    if (ret < 0) {
        client->state = MQTT_STATE_ERROR;
        client->last_error = MQTT_ERR_CONNECT;
        return MQTT_ERR_CONNECT;
    }
    
    client->last_connect_time = iot_get_tick_ms();
    
    return 0;
}

int mqtt_client_disconnect(mqtt_client_t* client) {
    if (!client) return MQTT_ERR_PARAM;
    
    /* 发送 DISCONNECT 报文 */
    if (client->sock) {
        uint8_t buf[4];
        int len = mqtt_packet_encode_disconnect(buf, sizeof(buf));
        if (len > 0) {
            net_socket_send(client->sock, buf, len);
        }
        
        net_socket_close(client->sock);
    }
    
    client->sock = 0;
    client->socket = NULL;
    client->state = MQTT_STATE_DISCONNECTED;
    
    /* 清理输出队列 */
    mqtt_outgoing_msg_t* msg = client->outgoing_head;
    while (msg) {
        mqtt_outgoing_msg_t* next = msg->next;
        if (msg->topic) iot_free(msg->topic);
        if (msg->payload) iot_free(msg->payload);
        iot_free(msg);
        msg = next;
    }
    client->outgoing_head = NULL;
    client->outgoing_tail = NULL;
    client->outgoing_count = 0;
    
    return 0;
}

/*===========================================================
 * 消息发布/订阅
 *===========================================================*/

int mqtt_client_publish(mqtt_client_t* client, const char* topic,
                        const uint8_t* payload, size_t payload_len,
                        mqtt_qos_t qos, bool retain) {
    if (!client || !client->sock || !topic || client->state != MQTT_STATE_CONNECTED) {
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
    
    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    /* QoS 1/2 需要记录消息以便重传 */
    if (qos > 0) {
        mqtt_outgoing_msg_t* msg = (mqtt_outgoing_msg_t*)iot_malloc(sizeof(mqtt_outgoing_msg_t));
        if (msg) {
            memset(msg, 0, sizeof(mqtt_outgoing_msg_t));
            msg->packet_id = publish.packet_id;
            msg->qos = qos;
            msg->topic = (char*)iot_strdup(topic);
            if (payload && payload_len > 0) {
                msg->payload = (uint8_t*)iot_malloc(payload_len);
                if (msg->payload) {
                    memcpy(msg->payload, payload, payload_len);
                    msg->payload_len = payload_len;
                }
            }
            msg->retain = retain;
            msg->send_time = iot_get_tick_ms();
            
            /* 添加到队列 */
            msg->next = NULL;
            if (client->outgoing_tail) {
                client->outgoing_tail->next = msg;
                client->outgoing_tail = msg;
            } else {
                client->outgoing_head = client->outgoing_tail = msg;
            }
            client->outgoing_count++;
        }
    }
    
    return 0;
}

int mqtt_client_subscribe(mqtt_client_t* client, const char* topic_filter,
                          mqtt_qos_t qos, mqtt_message_callback_t callback, void* user_data) {
    if (!client || !client->sock || !topic_filter || client->state != MQTT_STATE_CONNECTED) {
        return MQTT_ERR_PARAM;
    }
    
    client->message_callback = callback;
    client->message_user_data = user_data;
    
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
    
    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    return 0;
}

int mqtt_client_unsubscribe(mqtt_client_t* client, const char* topic_filter) {
    if (!client || !client->sock || !topic_filter || client->state != MQTT_STATE_CONNECTED) {
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
    
    int ret = net_socket_send(client->sock, buf, len);
    if (ret < 0) {
        return MQTT_ERR_SEND;
    }
    
    return 0;
}

/*===========================================================
 * Socket 回调处理
 *===========================================================*/

static void mqtt_client_socket_callback(sock_t sock, net_event_type_t event, void* user_data) {
    mqtt_client_t* client = (mqtt_client_t*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            /* 发送 CONNECT 报文 */
            mqtt_client_send_connect(client);
            break;
        }
        
        case NET_EVENT_RECV: {
            /* 接收数据 */
            uint8_t buf[4096];
            int len;
            
            while ((len = net_socket_recv(sock, buf, sizeof(buf))) > 0) {
                /* 扩展缓冲区 */
                if (client->recv_len + len > client->recv_capacity) {
                    size_t new_cap = client->recv_capacity + MQTT_MAX_PACKET_SIZE;
                    uint8_t* new_buf = (uint8_t*)iot_realloc(client->recv_buf, new_cap);
                    if (!new_buf) return;
                    client->recv_buf = new_buf;
                    client->recv_capacity = new_cap;
                }
                
                memcpy(client->recv_buf + client->recv_len, buf, len);
                client->recv_len += len;
                
                /* 处理数据包 */
                while (client->recv_len > 0) {
                    int ret = mqtt_client_process_packet(client);
                    if (ret < 0) break;
                    if (ret == 0) break;  /* 数据不完整 */
                }
            }
            break;
        }
        
        case NET_EVENT_DISCONNECTED: {
            client->sock = 0;
            client->socket = NULL;
            client->state = MQTT_STATE_DISCONNECTED;
            
            if (client->event_callback) {
                client->event_callback(client->event_user_data, MQTT_EVENT_DISCONNECTED);
            }
            break;
        }
        
        case NET_EVENT_ERROR: {
            client->sock = 0;
            client->socket = NULL;
            client->state = MQTT_STATE_ERROR;
            client->last_error = MQTT_ERR_CONNECT;
            
            if (client->event_callback) {
                client->event_callback(client->event_user_data, MQTT_EVENT_ERROR);
            }
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
    
    int ret = net_socket_send(client->sock, buf, len);
    return (ret > 0) ? 0 : -1;
}

static int mqtt_client_process_packet(mqtt_client_t* client) {
    if (!client || client->recv_len < 2) {
        return 0;
    }
    
    /* 解析数据包头部 */
    mqtt_packet_t packet;
    size_t consumed = 0;
    
    int ret = mqtt_packet_decode_header(client->recv_buf, client->recv_len, &packet, &consumed);
    if (ret < 0) {
        /* 协议错误 */
        mqtt_packet_free(&packet);
        client->last_error = MQTT_ERR_PROTOCOL;
        return -1;
    }
    
    if (ret == 1) {
        /* 数据不完整 */
        mqtt_packet_free(&packet);
        return 0;
    }
    
    /* 处理不同类型的报文 */
    switch (packet.type) {
        case MQTT_PACKET_CONNACK: {
            mqtt_packet_connack_t connack;
            if (mqtt_packet_decode_connack(&packet, &connack) == 0) {
                if (connack.return_code == MQTT_CONNACK_ACCEPTED) {
                    client->state = MQTT_STATE_CONNECTED;
                    client->last_ping_time = iot_get_tick_ms();
                    
                    if (client->event_callback) {
                        client->event_callback(client->event_user_data, MQTT_EVENT_CONNECTED);
                    }
                } else {
                    client->state = MQTT_STATE_ERROR;
                    client->last_error = MQTT_ERR_CONNECT;
                    
                    if (client->event_callback) {
                        client->event_callback(client->event_user_data, MQTT_EVENT_ERROR);
                    }
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
                /* 移除 QoS 1 消息 */
                mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    if ((*pp)->packet_id == puback.packet_id && (*pp)->qos == MQTT_QOS_1) {
                        mqtt_outgoing_msg_t* temp = *pp;
                        *pp = temp->next;
                        if (temp == client->outgoing_tail) {
                            client->outgoing_tail = *pp;
                        }
                        client->outgoing_count--;
                        if (temp->topic) iot_free(temp->topic);
                        if (temp->payload) iot_free(temp->payload);
                        iot_free(temp);
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
                /* 发送 PUBREL */
                uint8_t buf[8];
                int len = mqtt_packet_encode_pubrel(pubrec.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    net_socket_send(client->sock, buf, len);
                }
            }
            break;
        }
        
        case MQTT_PACKET_PUBREL: {
            mqtt_packet_pubrel_t pubrel;
            if (mqtt_packet_decode_pubrel(&packet, &pubrel) == 0) {
                /* 发送 PUBCOMP */
                uint8_t buf[8];
                int len = mqtt_packet_encode_pubcomp(pubrel.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    net_socket_send(client->sock, buf, len);
                }
                
                /* 移除 QoS 2 消息（完成） */
                mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    if ((*pp)->packet_id == pubrel.packet_id && (*pp)->qos == MQTT_QOS_2) {
                        mqtt_outgoing_msg_t* temp = *pp;
                        *pp = temp->next;
                        if (temp == client->outgoing_tail) {
                            client->outgoing_tail = *pp;
                        }
                        client->outgoing_count--;
                        if (temp->topic) iot_free(temp->topic);
                        if (temp->payload) iot_free(temp->payload);
                        iot_free(temp);
                        break;
                    }
                    pp = &(*pp)->next;
                }
            }
            break;
        }
        
        case MQTT_PACKET_PUBCOMP: {
            mqtt_packet_pubcomp_t pubcomp;
            if (mqtt_packet_decode_pubcomp(&packet, &pubcomp) == 0) {
                /* 移除 QoS 2 消息（完成） */
                mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    if ((*pp)->packet_id == pubcomp.packet_id) {
                        mqtt_outgoing_msg_t* temp = *pp;
                        *pp = temp->next;
                        if (temp == client->outgoing_tail) {
                            client->outgoing_tail = *pp;
                        }
                        client->outgoing_count--;
                        if (temp->topic) iot_free(temp->topic);
                        if (temp->payload) iot_free(temp->payload);
                        iot_free(temp);
                        break;
                    }
                    pp = &(*pp)->next;
                }
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
            client->last_ping_time = iot_get_tick_ms();
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
    
    /* 移动缓冲区 */
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
            net_socket_send(client->sock, buf, len);
        }
    } else if (publish->qos == MQTT_QOS_2) {
        uint8_t buf[8];
        int len = mqtt_packet_encode_pubrec(publish->packet_id, buf, sizeof(buf));
        if (len > 0) {
            net_socket_send(client->sock, buf, len);
        }
    }
    
    if (client->message_callback) {
        client->message_callback(client->message_user_data, publish->topic,
                                publish->payload, publish->payload_len,
                                publish->qos, publish->retain);
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
        
        /* 检查是否需要重传 */
        if (now - msg->send_time > MQTT_RETRY_INTERVAL_MS && msg->retry_count < MQTT_MAX_RETRY) {
            /* 重发消息 */
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
                int ret = net_socket_send(client->sock, buf, len);
                if (ret > 0) {
                    msg->send_time = now;
                    msg->retry_count++;
                }
            }
        }
        
        /* 移除超过最大重试次数的消息 */
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

/*===========================================================
 * 客户端轮询和状态管理
 *===========================================================*/

void mqtt_client_poll(mqtt_client_t* client, int timeout_ms) {
    if (!client) return;
    
    uint32_t now = iot_get_tick_ms();
    
    /* 心跳 */
    if (client->state == MQTT_STATE_CONNECTED && client->options.keepalive > 0) {
        if (now - client->last_ping_time >= client->ping_interval_ms) {
            uint8_t buf[4];
            int len = mqtt_packet_encode_pingreq(buf, sizeof(buf));
            if (len > 0) {
                net_socket_send(client->sock, buf, len);
            }
            client->last_ping_time = now;
        }
    }
    
    /* 重发消息 */
    mqtt_client_resend_messages(client);
    
    /* 自动重连 */
    if (client->auto_reconnect && client->state == MQTT_STATE_DISCONNECTED) {
        if (now - client->last_connect_time >= (uint32_t)client->reconnect_interval_ms) {
            mqtt_client_connect(client, &client->options);
        }
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
