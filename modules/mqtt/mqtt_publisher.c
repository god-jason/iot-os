/**
 * @file mqtt_publisher.c
 * @brief MQTT 消息发送队列和 QoS 处理实现
 */

#include "mqtt_publisher.h"
#include "platform.h"

#include <string.h>
#include <stdlib.h>

void mqtt_publisher_init(mqtt_publisher_t* publisher) {
    if (!publisher) return;
    memset(publisher, 0, sizeof(mqtt_publisher_t));
}

void mqtt_publisher_deinit(mqtt_publisher_t* publisher) {
    if (!publisher) return;
    
    mqtt_outgoing_msg_t* msg = publisher->outgoing_head;
    while (msg) {
        mqtt_outgoing_msg_t* next = msg->next;
        if (msg->topic) iot_free(msg->topic);
        if (msg->payload) iot_free(msg->payload);
        iot_free(msg);
        msg = next;
    }
    
    memset(publisher, 0, sizeof(mqtt_publisher_t));
}

int mqtt_publisher_add_message(mqtt_publisher_t* publisher, uint16_t packet_id,
                               mqtt_qos_t qos, const char* topic,
                               const uint8_t* payload, size_t payload_len,
                               bool retain) {
    if (!publisher || !topic || qos == MQTT_QOS_0) {
        return -1;
    }
    
    if (publisher->outgoing_count >= MQTT_MAX_OUTGOING_MSGS) {
        return -1;
    }
    
    mqtt_outgoing_msg_t* msg = (mqtt_outgoing_msg_t*)iot_malloc(sizeof(mqtt_outgoing_msg_t));
    if (!msg) return -1;
    
    memset(msg, 0, sizeof(mqtt_outgoing_msg_t));
    msg->packet_id = packet_id;
    msg->qos = qos;
    msg->topic = (char*)iot_strdup(topic);
    if (!msg->topic) {
        iot_free(msg);
        return -1;
    }
    
    if (payload && payload_len > 0) {
        msg->payload = (uint8_t*)iot_malloc(payload_len);
        if (!msg->payload) {
            iot_free(msg->topic);
            iot_free(msg);
            return -1;
        }
        memcpy(msg->payload, payload, payload_len);
        msg->payload_len = payload_len;
    }
    msg->retain = retain;
    msg->send_time = iot_get_tick_ms();
    
    msg->next = NULL;
    if (publisher->outgoing_tail) {
        publisher->outgoing_tail->next = msg;
        publisher->outgoing_tail = msg;
    } else {
        publisher->outgoing_head = publisher->outgoing_tail = msg;
    }
    publisher->outgoing_count++;
    
    return 0;
}

void mqtt_publisher_remove_message(mqtt_publisher_t* publisher, uint16_t packet_id, mqtt_qos_t qos) {
    if (!publisher) return;
    
    mqtt_outgoing_msg_t** pp = &publisher->outgoing_head;
    while (*pp) {
        mqtt_outgoing_msg_t* msg = *pp;
        if (msg->packet_id == packet_id && msg->qos == qos) {
            *pp = msg->next;
            if (msg == publisher->outgoing_tail) {
                publisher->outgoing_tail = *pp;
            }
            publisher->outgoing_count--;
            if (msg->topic) iot_free(msg->topic);
            if (msg->payload) iot_free(msg->payload);
            iot_free(msg);
            break;
        }
        pp = &(*pp)->next;
    }
}

void mqtt_publisher_resend_messages(mqtt_publisher_t* publisher,
                                    int (*send_func)(const uint8_t*, size_t, void*),
                                    void* send_arg) {
    if (!publisher || !send_func) return;
    
    uint32_t now = iot_get_tick_ms();
    mqtt_outgoing_msg_t** pp = &publisher->outgoing_head;
    
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
                int ret = send_func(buf, len, send_arg);
                if (ret > 0) {
                    msg->send_time = now;
                    msg->retry_count++;
                }
            }
        }
        
        if (msg->retry_count >= MQTT_MAX_RETRY) {
            *pp = msg->next;
            if (msg == publisher->outgoing_tail) {
                publisher->outgoing_tail = *pp;
            }
            publisher->outgoing_count--;
            if (msg->topic) iot_free(msg->topic);
            if (msg->payload) iot_free(msg->payload);
            iot_free(msg);
        } else {
            pp = &(*pp)->next;
        }
    }
}

void mqtt_publisher_clear_retry_count(mqtt_publisher_t* publisher) {
    if (!publisher) return;
    
    mqtt_outgoing_msg_t* msg = publisher->outgoing_head;
    while (msg) {
        msg->retry_count = 0;
        msg = msg->next;
    }
}

void mqtt_publisher_resend_all(mqtt_publisher_t* publisher,
                               int (*send_func)(const uint8_t*, size_t, void*),
                               void* send_arg) {
    if (!publisher || !send_func) return;
    
    uint32_t now = iot_get_tick_ms();
    mqtt_outgoing_msg_t* msg = publisher->outgoing_head;
    
    while (msg) {
        if (msg->qos != MQTT_QOS_0) {
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
                send_func(buf, len, send_arg);
            }
            msg->send_time = now;
            msg->retry_count = 0;
        }
        msg = msg->next;
    }
}

size_t mqtt_publisher_get_message_count(mqtt_publisher_t* publisher) {
    return publisher ? publisher->outgoing_count : 0;
}