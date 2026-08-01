/**
 * @file mqtt_manager.c
 * @brief MQTT 管理器实现
 *
 * 实现 MQTT 管理器的核心功能，包括客户端链表管理、后台轮询线程、
 * 心跳检测、QoS 消息重传、自动重连逻辑、协议包处理等
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "mqtt_manager.h"
#include "mqtt_packets.h"

#include "iot.h"
#include "net.h"
#include "iot_list.h"

#include <string.h>
#include <stdlib.h>

static LIST_HEAD(s_mqtt_clients);
static iot_mutex_t s_mqtt_mutex;
static iot_task_t s_iot_mqtt_manager_task;
static bool s_mqtt_running = false;

static int iot_mqtt_manager_send_raw(iot_mqtt_client_t* client, const uint8_t* buf, size_t len);
static void iot_mqtt_manager_process_packet(iot_mqtt_client_t* client);
static void iot_mqtt_manager_handle_publish(iot_mqtt_client_t* client, const iot_mqtt_packet_publish_t* publish);
static void iot_mqtt_manager_trigger_event(iot_mqtt_client_t* client, iot_mqtt_event_type_t event);

static void iot_mqtt_manager_send_ping(iot_mqtt_client_t* client);
static void iot_mqtt_manager_resend_messages(iot_mqtt_client_t* client);
static void iot_mqtt_manager_check_keepalive(iot_mqtt_client_t* client);
static void iot_mqtt_manager_check_reconnect(iot_mqtt_client_t* client);

static void iot_mqtt_manager_thread(void* arg);

int iot_mqtt_manager_init(void) {
    s_mqtt_mutex = iot_mutex_create();
    if (!s_mqtt_mutex) {
        return -1;
    }

    s_mqtt_running = true;
    s_iot_mqtt_manager_task = iot_task_create("mqtt_mgr", iot_mqtt_manager_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
    if (!s_iot_mqtt_manager_task) {
        iot_mutex_delete(s_mqtt_mutex);
        s_mqtt_running = false;
        return -1;
    }

    return 0;
}

void iot_mqtt_manager_deinit(void) {
    s_mqtt_running = false;

    iot_mutex_lock(s_mqtt_mutex, -1);
    iot_mqtt_client_t* client;
    iot_mqtt_client_t* tmp;
    list_for_each_entry_safe(client, tmp, &s_mqtt_clients, list_node, iot_mqtt_client_t) {
        list_del(&client->list_node);
        iot_mqtt_client_disconnect(client);
    }
    iot_mutex_unlock(s_mqtt_mutex);

    iot_task_delay(100);

    if (s_iot_mqtt_manager_task) {
        iot_task_delete(s_iot_mqtt_manager_task);
    }

    if (s_mqtt_mutex) {
        iot_mutex_delete(s_mqtt_mutex);
    }
}

int iot_mqtt_manager_add_client(iot_mqtt_client_t* client) {
    if (!client) return -1;

    iot_mutex_lock(s_mqtt_mutex, -1);
    list_add_tail(&client->list_node, &s_mqtt_clients);
    iot_mutex_unlock(s_mqtt_mutex);

    return 0;
}

int iot_mqtt_manager_remove_client(iot_mqtt_client_t* client) {
    if (!client) return -1;

    iot_mutex_lock(s_mqtt_mutex, -1);
    if (!list_empty(&client->list_node)) {
        list_del_init(&client->list_node);
    }
    iot_mutex_unlock(s_mqtt_mutex);

    return 0;
}

void iot_mqtt_manager_lock(void) {
    iot_mutex_lock(s_mqtt_mutex, -1);
}

void iot_mqtt_manager_unlock(void) {
    iot_mutex_unlock(s_mqtt_mutex);
}

static void iot_mqtt_manager_send_ping(iot_mqtt_client_t* client) {
    if (!client || !client->sock || client->state != IOT_MQTT_STATE_CONNECTED) {
        return;
    }

    uint8_t buf[4];
    int len = iot_mqtt_packet_encode_pingreq(buf, sizeof(buf));
    if (len > 0) {
        iot_mqtt_manager_send_raw(client, buf, len);
        client->ping_sent_time = iot_get_tick_ms();
    }
}

static void iot_mqtt_manager_resend_messages(iot_mqtt_client_t* client) {
    if (!client || !client->sock || client->state != IOT_MQTT_STATE_CONNECTED) {
        return;
    }

    uint32_t now = iot_get_tick_ms();
    iot_mqtt_outgoing_msg_t** pp = &client->outgoing_head;

    while (*pp) {
        iot_mqtt_outgoing_msg_t* msg = *pp;

        if (now - msg->send_time > IOT_MQTT_RETRY_INTERVAL_MS && msg->retry_count < IOT_MQTT_MAX_RETRY) {
            uint8_t buf[IOT_MQTT_MAX_PACKET_SIZE];
            iot_mqtt_packet_publish_t publish = {
                .dup = true,
                .qos = msg->qos,
                .retain = msg->retain,
                .packet_id = msg->packet_id,
                .topic = msg->topic,
                .payload = msg->payload,
                .payload_len = msg->payload_len
            };

            int len = iot_mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
            if (len > 0) {
                int ret = iot_mqtt_manager_send_raw(client, buf, len);
                if (ret > 0) {
                    msg->send_time = now;
                    msg->retry_count++;
                }
            }
        }

        if (msg->retry_count >= IOT_MQTT_MAX_RETRY) {
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

static void iot_mqtt_manager_check_keepalive(iot_mqtt_client_t* client) {
    if (!client || client->state != IOT_MQTT_STATE_CONNECTED || client->keepalive <= 0) {
        return;
    }

    uint32_t now = iot_get_tick_ms();

    if (client->ping_sent_time > 0) {
        if (now - client->ping_sent_time > (uint32_t)client->options.timeout_ms) {
            client->state = IOT_MQTT_STATE_ERROR;
            client->last_error = IOT_MQTT_ERR_KEEPALIVE;
            iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_ERROR);
            return;
        }
    } else {
        if (now - client->last_ping_time > (uint32_t)client->keepalive * 1000 / 2) {
            iot_mqtt_manager_send_ping(client);
        }
    }
}

static void iot_mqtt_manager_check_reconnect(iot_mqtt_client_t* client) {
    if (!client) return;

    if ((client->state == IOT_MQTT_STATE_DISCONNECTED ||
         client->state == IOT_MQTT_STATE_ERROR) &&
        client->auto_reconnect) {
        uint32_t now = iot_get_tick_ms();
        if (now - client->last_connect_attempt >= (uint32_t)client->reconnect_interval_ms) {
            iot_mqtt_client_connect(client, &client->options);
        }
    }
}

static int iot_mqtt_manager_send_raw(iot_mqtt_client_t* client, const uint8_t* buf, size_t len) {
    if (!client || !client->sock) return -1;
    return iot_net_socket_send(client->sock, buf, len);
}

static void iot_mqtt_manager_process_packet(iot_mqtt_client_t* client) {
    if (!client || client->recv_len < 2) {
        return;
    }

    iot_mqtt_packet_t packet;
    size_t consumed = 0;

    int ret = iot_mqtt_packet_decode_header(client->recv_buf, client->recv_len, &packet, &consumed);
    if (ret < 0) {
        iot_mqtt_packet_free(&packet);
        client->last_error = IOT_MQTT_ERR_PROTOCOL;
        return;
    }

    if (ret == 1) {
        iot_mqtt_packet_free(&packet);
        return;
    }

    switch (packet.type) {
        case IOT_MQTT_PACKET_CONNACK: {
            iot_mqtt_packet_connack_t connack;
            if (iot_mqtt_packet_decode_connack(&packet, &connack) == 0) {
                if (connack.return_code == IOT_MQTT_CONNACK_ACCEPTED) {
                    client->state = IOT_MQTT_STATE_CONNECTED;
                    client->last_ping_time = iot_get_tick_ms();
                    client->ping_sent_time = 0;

                    iot_mqtt_outgoing_msg_t* msg = client->outgoing_head;
                    while (msg) {
                        iot_mqtt_outgoing_msg_t* next = msg->next;
                        uint8_t buf[IOT_MQTT_MAX_PACKET_SIZE];
                        iot_mqtt_packet_publish_t publish = {
                            .dup = true,
                            .qos = msg->qos,
                            .retain = msg->retain,
                            .packet_id = msg->packet_id,
                            .topic = msg->topic,
                            .payload = msg->payload,
                            .payload_len = msg->payload_len
                        };

                        int len = iot_mqtt_packet_encode_publish(&publish, buf, sizeof(buf));
                        if (len > 0) {
                            iot_mqtt_manager_send_raw(client, buf, len);
                        }
                        msg->send_time = iot_get_tick_ms();
                        msg->retry_count = 0;
                        msg = next;
                    }

                    if (!client->options.clean_session) {
                        iot_mqtt_subscribe_entry_t* entry = client->subscribe_head;
                        while (entry) {
                            uint8_t buf[512];
                            const char* topic_filter = entry->topic_filter;
                            iot_mqtt_qos_t qos = entry->qos;
                            iot_mqtt_packet_subscribe_t subscribe = {
                                .packet_id = client->next_packet_id++,
                                .topic_filters = &topic_filter,
                                .requested_qos = &qos,
                                .topic_count = 1
                            };

                            int len = iot_mqtt_packet_encode_subscribe(&subscribe, buf, sizeof(buf));
                            if (len > 0) {
                                iot_mqtt_manager_send_raw(client, buf, len);
                            }
                            entry = entry->next;
                        }
                    }

                    iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_CONNECTED);
                } else {
                    client->state = IOT_MQTT_STATE_ERROR;
                    client->last_error = IOT_MQTT_ERR_CONNECT;
                    iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_ERROR);
                }
            }
            break;
        }

        case IOT_MQTT_PACKET_PUBLISH: {
            iot_mqtt_packet_publish_t publish;
            if (iot_mqtt_packet_decode_publish(&packet, &publish) == 0) {
                iot_mqtt_manager_handle_publish(client, &publish);
            }
            break;
        }

        case IOT_MQTT_PACKET_PUBACK: {
            iot_mqtt_packet_puback_t puback;
            if (iot_mqtt_packet_decode_puback(&packet, &puback) == 0) {
                iot_mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    iot_mqtt_outgoing_msg_t* msg = *pp;
                    if (msg->packet_id == puback.packet_id && msg->qos == IOT_MQTT_QOS_1) {
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

        case IOT_MQTT_PACKET_PUBREC: {
            iot_mqtt_packet_pubrec_t pubrec;
            if (iot_mqtt_packet_decode_pubrec(&packet, &pubrec) == 0) {
                uint8_t buf[8];
                int len = iot_mqtt_packet_encode_pubrel(pubrec.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    iot_mqtt_manager_send_raw(client, buf, len);
                }
            }
            break;
        }

        case IOT_MQTT_PACKET_PUBREL: {
            iot_mqtt_packet_pubrel_t pubrel;
            if (iot_mqtt_packet_decode_pubrel(&packet, &pubrel) == 0) {
                uint8_t buf[8];
                int len = iot_mqtt_packet_encode_pubcomp(pubrel.packet_id, buf, sizeof(buf));
                if (len > 0) {
                    iot_mqtt_manager_send_raw(client, buf, len);
                }
            }
            break;
        }

        case IOT_MQTT_PACKET_PUBCOMP: {
            iot_mqtt_packet_pubcomp_t pubcomp;
            if (iot_mqtt_packet_decode_pubcomp(&packet, &pubcomp) == 0) {
                iot_mqtt_outgoing_msg_t** pp = &client->outgoing_head;
                while (*pp) {
                    iot_mqtt_outgoing_msg_t* msg = *pp;
                    if (msg->packet_id == pubcomp.packet_id && msg->qos == IOT_MQTT_QOS_2) {
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

        case IOT_MQTT_PACKET_SUBACK: {
            iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_SUBSCRIBED);
            break;
        }

        case IOT_MQTT_PACKET_UNSUBACK: {
            iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_UNSUBSCRIBED);
            break;
        }

        case IOT_MQTT_PACKET_PINGRESP: {
            client->last_ping_time = iot_get_tick_ms();
            client->ping_sent_time = 0;
            break;
        }

        case IOT_MQTT_PACKET_DISCONNECT: {
            iot_mqtt_client_disconnect(client);
            break;
        }

        default:
            break;
    }

    iot_mqtt_packet_free(&packet);

    if (consumed > 0 && consumed <= client->recv_len) {
        memmove(client->recv_buf, client->recv_buf + consumed, client->recv_len - consumed);
        client->recv_len -= consumed;
    }
}

static void iot_mqtt_manager_handle_publish(iot_mqtt_client_t* client, const iot_mqtt_packet_publish_t* publish) {
    if (!client || !publish) return;

    if (publish->qos == IOT_MQTT_QOS_1) {
        uint8_t buf[8];
        int len = iot_mqtt_packet_encode_puback(publish->packet_id, buf, sizeof(buf));
        if (len > 0) {
            iot_mqtt_manager_send_raw(client, buf, len);
        }
    } else if (publish->qos == IOT_MQTT_QOS_2) {
        uint8_t buf[8];
        int len = iot_mqtt_packet_encode_pubrec(publish->packet_id, buf, sizeof(buf));
        if (len > 0) {
            iot_mqtt_manager_send_raw(client, buf, len);
        }
    }

    iot_mqtt_subscribe_entry_t* entry = client->subscribe_head;
    while (entry) {
        if (iot_mqtt_topic_match(entry->topic_filter, publish->topic)) {
            if (entry->callback) {
                entry->callback(publish->topic, publish->payload,
                               publish->payload_len, publish->qos, publish->retain, entry->user_data);
            }
        }
        entry = entry->next;
    }
}

static void iot_mqtt_manager_trigger_event(iot_mqtt_client_t* client, iot_mqtt_event_type_t event) {
    if (client && client->event_callback) {
        client->event_callback(client, event, client->event_user_data);
    }
}

static void iot_mqtt_manager_thread(void* arg) {
    while (s_mqtt_running) {
        iot_mutex_lock(s_mqtt_mutex, -1);

        iot_mqtt_client_t* client;
        list_for_each_entry(client, &s_mqtt_clients, list_node, iot_mqtt_client_t) {
            if (client->state == IOT_MQTT_STATE_CONNECTED) {
                iot_mqtt_manager_check_keepalive(client);
                iot_mqtt_manager_resend_messages(client);
            }

            iot_mqtt_manager_check_reconnect(client);
        }

        iot_mutex_unlock(s_mqtt_mutex);

        iot_task_delay(IOT_MQTT_MANAGER_POLL_INTERVAL);
    }
}

void iot_mqtt_manager_on_socket_event(iot_mqtt_client_t* client, iot_net_event_type_t event) {
    if (!client) return;

    switch (event) {
        case IOT_NET_EVENT_CONNECTED: {
            uint8_t buf[512];
            iot_mqtt_packet_connect_t connect = {
                .protocol_name = IOT_MQTT_PROTOCOL_NAME,
                .protocol_version = IOT_MQTT_PROTOCOL_VERSION,
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

            int len = iot_mqtt_packet_encode_connect(&connect, buf, sizeof(buf));
            if (len > 0) {
                iot_mqtt_manager_send_raw(client, buf, len);
            }
            break;
        }

        case IOT_NET_EVENT_RECV: {
            const char* buf = iot_net_socket_get_recv_buf(client->sock);
            size_t len = iot_net_socket_get_recv_len(client->sock);

            if (!client->recv_buf) {
                return;
            }

            if (len > 0 && client->recv_len + len > client->recv_capacity) {
                size_t new_cap = client->recv_capacity + IOT_MQTT_MAX_PACKET_SIZE;
                uint8_t* new_buf = (uint8_t*)iot_realloc(client->recv_buf, new_cap);
                if (!new_buf) return;
                client->recv_buf = new_buf;
                client->recv_capacity = new_cap;
            }

            memcpy(client->recv_buf + client->recv_len, buf, len);
            client->recv_len += len;
            iot_net_socket_clear_recv_buf(client->sock);

            while (client->recv_len > 0) {
                size_t before = client->recv_len;
                iot_mqtt_manager_process_packet(client);
                if (client->recv_len == before) {
                    break;
                }
            }
            break;
        }

        case IOT_NET_EVENT_DISCONNECTED: {
            client->state = IOT_MQTT_STATE_DISCONNECTED;
            iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_DISCONNECTED);
            break;
        }

        case IOT_NET_EVENT_ERROR: {
            client->state = IOT_MQTT_STATE_ERROR;
            client->last_error = IOT_MQTT_ERR_CONNECT;
            iot_mqtt_manager_trigger_event(client, IOT_MQTT_EVENT_ERROR);
            break;
        }

        default:
            break;
    }
}