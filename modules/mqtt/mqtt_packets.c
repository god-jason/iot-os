/**
 * @file mqtt_packets.c
 * @brief MQTT 3.1.1 协议包编解码实现
 */

#include "mqtt_packets.h"
#include "iot.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*===========================================================
 * 内部辅助函数
 *===========================================================*/

/**
 * @brief 编码 16 位无符号整数（大端序）
 */
static uint16_t encode_uint16(uint8_t* buf, uint16_t value) {
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
    return 2;
}

/**
 * @brief 解码 16 位无符号整数（大端序）
 */
static uint16_t decode_uint16(const uint8_t* buf) {
    return ((uint16_t)buf[0] << 8) | buf[1];
}

/**
 * @brief 编码字符串
 */
static uint16_t encode_string(uint8_t* buf, const char* str) {
    uint16_t len = strlen(str);
    encode_uint16(buf, len);
    memcpy(buf + 2, str, len);
    return len + 2;
}

/**
 * @brief 解码字符串到缓冲区
 */
static uint16_t decode_string(const uint8_t* buf, char* str, size_t max_len) {
    uint16_t len = decode_uint16(buf);
    if (len >= max_len) len = max_len - 1;
    memcpy(str, buf + 2, len);
    str[len] = '\0';
    return len + 2;
}

/**
 * @brief 解码字符串（动态分配）
 */
static char* decode_string_dup(const uint8_t* buf) {
    uint16_t len = decode_uint16(buf);
    char* str = (char*)iot_malloc(len + 1);
    if (str) {
        memcpy(str, buf + 2, len);
        str[len] = '\0';
    }
    return str;
}

/**
 * @brief 编码可变长度整数（Remaining Length）
 */
static uint16_t encode_remaining_length(uint8_t* buf, uint32_t len) {
    uint16_t pos = 0;
    uint32_t remaining = len;
    
    do {
        uint8_t byte = remaining % 128;
        remaining /= 128;
        if (remaining > 0) byte |= 0x80;
        buf[pos++] = byte;
    } while (remaining > 0);
    
    return pos;
}

/**
 * @brief 解码可变长度整数
 */
static uint32_t decode_remaining_length(const uint8_t* buf, uint16_t* consumed) {
    uint32_t len = 0;
    uint16_t pos = 0;
    uint8_t byte;
    uint32_t multiplier = 1;
    
    do {
        byte = buf[pos++];
        len += (byte & 0x7F) * multiplier;
        multiplier *= 128;
        if (multiplier > 128 * 128 * 128 * 128) {
            /* 可变长度编码最大 4 字节 */
            *consumed = pos;
            return 0;
        }
    } while (byte & 0x80);
    
    *consumed = pos;
    return len;
}

/*===========================================================
 * 协议包构建实现
 *===========================================================*/

int mqtt_packet_encode_connect(const mqtt_packet_connect_t* connect, uint8_t* buf, size_t buf_len) {
    if (!connect || !buf || buf_len < 12) return -1;
    
    uint16_t pos = 0;
    
    /* 固定头：数据包类型 */
    buf[pos++] = MQTT_PACKET_CONNECT;
    
    /* 计算剩余长度（先占位） */
    uint8_t* remaining_len_pos = buf + pos;
    pos++;
    
    /* 可变头：协议名 */
    pos += encode_string(buf + pos, connect->protocol_name ? connect->protocol_name : MQTT_PROTOCOL_NAME);
    
    /* 可变头：协议级别 */
    buf[pos++] = connect->protocol_version ? connect->protocol_version : MQTT_PROTOCOL_VERSION;
    
    /* 连接标志 */
    uint8_t connect_flags = 0;
    if (connect->username_flag) connect_flags |= 0x80;
    if (connect->password_flag) connect_flags |= 0x40;
    if (connect->will_retain) connect_flags |= 0x20;
    connect_flags |= (connect->will_qos & 0x03) << 3;
    if (connect->will_flag) connect_flags |= 0x04;
    if (connect->clean_session) connect_flags |= 0x02;
    buf[pos++] = connect_flags;
    
    /* 可变头：心跳间隔 */
    pos += encode_uint16(buf + pos, connect->keepalive);
    
    /* 负载：客户端标识符 */
    if (connect->client_id) {
        pos += encode_string(buf + pos, connect->client_id);
    } else {
        /* 空的客户端标识符，长度为 0 */
        buf[pos++] = 0;
        buf[pos++] = 0;
    }
    
    /* 负载：遗嘱消息 */
    if (connect->will_flag) {
        if (connect->will_topic) {
            pos += encode_string(buf + pos, connect->will_topic);
        } else {
            buf[pos++] = 0;
            buf[pos++] = 0;
        }
        if (connect->will_message) {
            pos += encode_string(buf + pos, connect->will_message);
        } else {
            buf[pos++] = 0;
            buf[pos++] = 0;
        }
    }
    
    /* 负载：用户名 */
    if (connect->username_flag && connect->username) {
        pos += encode_string(buf + pos, connect->username);
    }
    
    /* 负载：密码 */
    if (connect->password_flag && connect->password) {
        pos += encode_string(buf + pos, connect->password);
    }
    
    /* 填充剩余长度 */
    uint32_t remaining_len = pos - 2;
    uint16_t len_bytes = encode_remaining_length(remaining_len_pos, remaining_len);
    
    if (len_bytes > 1) {
        memmove(remaining_len_pos + len_bytes, remaining_len_pos + 1, remaining_len);
        pos += len_bytes - 1;
    }
    
    return pos;
}

int mqtt_packet_encode_connack(const mqtt_packet_connack_t* connack, uint8_t* buf, size_t buf_len) {
    if (!connack || !buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_CONNACK;
    buf[1] = 2;  /* 剩余长度固定为 2 */
    buf[2] = connack->session_present & 0x01;
    buf[3] = connack->return_code & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_publish(const mqtt_packet_publish_t* publish, uint8_t* buf, size_t buf_len) {
    if (!publish || !buf || !publish->topic) return -1;
    
    uint16_t pos = 0;
    
    /* 固定头：数据包类型 + 标志 */
    uint8_t cmd = MQTT_PACKET_PUBLISH;
    cmd |= (publish->dup & 0x01) << 3;
    cmd |= (publish->qos & 0x03) << 1;
    cmd |= (publish->retain & 0x01);
    buf[pos++] = cmd;
    
    /* 计算剩余长度 */
    uint8_t* remaining_len_pos = buf + pos;
    pos++;
    
    /* 可变头：主题名 */
    pos += encode_string(buf + pos, publish->topic);
    
    /* 可变头：数据包标识符（QoS > 0 时） */
    if (publish->qos > 0) {
        pos += encode_uint16(buf + pos, publish->packet_id);
    }
    
    /* 负载：消息内容 */
    if (publish->payload && publish->payload_len > 0) {
        if (pos + publish->payload_len > buf_len) return -1;
        memcpy(buf + pos, publish->payload, publish->payload_len);
        pos += publish->payload_len;
    }
    
    /* 填充剩余长度 */
    uint32_t remaining_len = pos - 2;
    uint16_t len_bytes = encode_remaining_length(remaining_len_pos, remaining_len);
    
    if (len_bytes > 1) {
        memmove(remaining_len_pos + len_bytes, remaining_len_pos + 1, remaining_len);
        pos += len_bytes - 1;
    }
    
    return pos;
}

int mqtt_packet_encode_puback(uint16_t packet_id, uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_PUBACK;
    buf[1] = 2;
    buf[2] = (packet_id >> 8) & 0xFF;
    buf[3] = packet_id & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_pubrec(uint16_t packet_id, uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_PUBREC;
    buf[1] = 2;
    buf[2] = (packet_id >> 8) & 0xFF;
    buf[3] = packet_id & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_pubrel(uint16_t packet_id, uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_PUBREL | 0x02;  /* PUBREL 固定标志为 10 */
    buf[1] = 2;
    buf[2] = (packet_id >> 8) & 0xFF;
    buf[3] = packet_id & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_pubcomp(uint16_t packet_id, uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_PUBCOMP;
    buf[1] = 2;
    buf[2] = (packet_id >> 8) & 0xFF;
    buf[3] = packet_id & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_subscribe(const mqtt_packet_subscribe_t* subscribe, uint8_t* buf, size_t buf_len) {
    if (!subscribe || !buf || !subscribe->topic_filters || subscribe->topic_count == 0) return -1;
    
    uint16_t pos = 0;
    
    /* 固定头：数据包类型 + 标志（SUBSCRIBE 固定标志为 0010） */
    buf[pos++] = MQTT_PACKET_SUBSCRIBE | 0x02;
    
    /* 计算剩余长度 */
    uint8_t* remaining_len_pos = buf + pos;
    pos++;
    
    /* 可变头：数据包标识符 */
    pos += encode_uint16(buf + pos, subscribe->packet_id);
    
    /* 负载：主题过滤器 + 请求 QoS */
    for (size_t i = 0; i < subscribe->topic_count; i++) {
        pos += encode_string(buf + pos, subscribe->topic_filters[i]);
        buf[pos++] = subscribe->requested_qos[i] & 0x03;
    }
    
    /* 填充剩余长度 */
    uint32_t remaining_len = pos - 2;
    uint16_t len_bytes = encode_remaining_length(remaining_len_pos, remaining_len);
    
    if (len_bytes > 1) {
        memmove(remaining_len_pos + len_bytes, remaining_len_pos + 1, remaining_len);
        pos += len_bytes - 1;
    }
    
    return pos;
}

int mqtt_packet_encode_suback(const mqtt_packet_suback_t* suback, uint8_t* buf, size_t buf_len) {
    if (!suback || !buf || buf_len < 4) return -1;
    
    uint16_t pos = 0;
    
    buf[pos++] = MQTT_PACKET_SUBACK;
    
    /* 计算剩余长度 */
    uint8_t* remaining_len_pos = buf + pos;
    pos++;
    
    /* 可变头：数据包标识符 */
    pos += encode_uint16(buf + pos, suback->packet_id);
    
    /* 负载：返回码 */
    for (size_t i = 0; i < suback->return_code_count; i++) {
        buf[pos++] = suback->return_codes[i];
    }
    
    /* 填充剩余长度 */
    uint32_t remaining_len = pos - 2;
    uint16_t len_bytes = encode_remaining_length(remaining_len_pos, remaining_len);
    
    if (len_bytes > 1) {
        memmove(remaining_len_pos + len_bytes, remaining_len_pos + 1, remaining_len);
        pos += len_bytes - 1;
    }
    
    return pos;
}

int mqtt_packet_encode_unsubscribe(const mqtt_packet_unsubscribe_t* unsubscribe, uint8_t* buf, size_t buf_len) {
    if (!unsubscribe || !buf || !unsubscribe->topic_filters || unsubscribe->topic_count == 0) return -1;
    
    uint16_t pos = 0;
    
    /* 固定头：数据包类型 + 标志（UNSUBSCRIBE 固定标志为 0010） */
    buf[pos++] = MQTT_PACKET_UNSUBSCRIBE | 0x02;
    
    /* 计算剩余长度 */
    uint8_t* remaining_len_pos = buf + pos;
    pos++;
    
    /* 可变头：数据包标识符 */
    pos += encode_uint16(buf + pos, unsubscribe->packet_id);
    
    /* 负载：主题过滤器 */
    for (size_t i = 0; i < unsubscribe->topic_count; i++) {
        pos += encode_string(buf + pos, unsubscribe->topic_filters[i]);
    }
    
    /* 填充剩余长度 */
    uint32_t remaining_len = pos - 2;
    uint16_t len_bytes = encode_remaining_length(remaining_len_pos, remaining_len);
    
    if (len_bytes > 1) {
        memmove(remaining_len_pos + len_bytes, remaining_len_pos + 1, remaining_len);
        pos += len_bytes - 1;
    }
    
    return pos;
}

int mqtt_packet_encode_unsuback(uint16_t packet_id, uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 4) return -1;
    
    buf[0] = MQTT_PACKET_UNSUBACK;
    buf[1] = 2;
    buf[2] = (packet_id >> 8) & 0xFF;
    buf[3] = packet_id & 0xFF;
    
    return 4;
}

int mqtt_packet_encode_pingreq(uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 2) return -1;
    
    buf[0] = MQTT_PACKET_PINGREQ;
    buf[1] = 0;
    
    return 2;
}

int mqtt_packet_encode_pingresp(uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 2) return -1;
    
    buf[0] = MQTT_PACKET_PINGRESP;
    buf[1] = 0;
    
    return 2;
}

int mqtt_packet_encode_disconnect(uint8_t* buf, size_t buf_len) {
    if (!buf || buf_len < 2) return -1;
    
    buf[0] = MQTT_PACKET_DISCONNECT;
    buf[1] = 0;
    
    return 2;
}

/*===========================================================
 * 协议包解析实现
 *===========================================================*/

int mqtt_packet_decode_header(const uint8_t* data, size_t data_len,
                              mqtt_packet_t* packet, size_t* consumed) {
    if (!data || !packet || !consumed || data_len < 2) {
        return -1;
    }
    
    memset(packet, 0, sizeof(mqtt_packet_t));
    
    /* 解析固定头 */
    uint8_t first_byte = data[0];
    packet->type = (mqtt_packet_type_t)(first_byte & 0xF0);
    packet->flags = first_byte & 0x0F;
    
    /* 解析剩余长度 */
    uint16_t len_consumed;
    packet->remaining_length = decode_remaining_length(data + 1, &len_consumed);
    
    /* 计算完整数据包长度 */
    size_t total_len = 1 + len_consumed + packet->remaining_length;
    
    if (data_len < total_len) {
        *consumed = 0;
        return 1;  /* 数据不完整 */
    }
    
    /* 复制可变头和负载数据 */
    if (packet->remaining_length > 0) {
        packet->data = (uint8_t*)iot_malloc(packet->remaining_length);
        if (!packet->data) {
            return -1;
        }
        memcpy(packet->data, data + 1 + len_consumed, packet->remaining_length);
        packet->data_len = packet->remaining_length;
    }
    
    *consumed = total_len;
    return 0;
}

int mqtt_packet_decode_connack(const mqtt_packet_t* packet, mqtt_packet_connack_t* connack) {
    if (!packet || !connack || packet->type != MQTT_PACKET_CONNACK) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    connack->session_present = packet->data[0] & 0x01;
    connack->return_code = packet->data[1];
    
    return 0;
}

int mqtt_packet_decode_publish(const mqtt_packet_t* packet, mqtt_packet_publish_t* publish) {
    if (!packet || !publish || packet->type != MQTT_PACKET_PUBLISH) {
        return -1;
    }
    
    memset(publish, 0, sizeof(mqtt_packet_publish_t));
    
    /* 解析标志 */
    publish->dup = (packet->flags >> 3) & 0x01;
    publish->qos = (mqtt_qos_t)((packet->flags >> 1) & 0x03);
    publish->retain = packet->flags & 0x01;
    
    /* 解析主题名 */
    publish->topic = decode_string_dup(packet->data);
    if (!publish->topic) return -1;
    
    size_t pos = strlen(publish->topic) + 2;  /* 字符串长度 + 2 字节长度字段 */
    
    /* 解析数据包标识符 */
    if (publish->qos > 0) {
        if (pos + 2 > packet->data_len) return -1;
        publish->packet_id = decode_uint16(packet->data + pos);
        pos += 2;
    }
    
    /* 解析负载 */
    if (pos < packet->data_len) {
        publish->payload = packet->data + pos;
        publish->payload_len = packet->data_len - pos;
    }
    
    return 0;
}

int mqtt_packet_decode_puback(const mqtt_packet_t* packet, mqtt_packet_puback_t* puback) {
    if (!packet || !puback || packet->type != MQTT_PACKET_PUBACK) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    puback->packet_id = decode_uint16(packet->data);
    return 0;
}

int mqtt_packet_decode_pubrec(const mqtt_packet_t* packet, mqtt_packet_pubrec_t* pubrec) {
    if (!packet || !pubrec || packet->type != MQTT_PACKET_PUBREC) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    pubrec->packet_id = decode_uint16(packet->data);
    return 0;
}

int mqtt_packet_decode_pubrel(const mqtt_packet_t* packet, mqtt_packet_pubrel_t* pubrel) {
    if (!packet || !pubrel || packet->type != MQTT_PACKET_PUBREL) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    pubrel->packet_id = decode_uint16(packet->data);
    return 0;
}

int mqtt_packet_decode_pubcomp(const mqtt_packet_t* packet, mqtt_packet_pubcomp_t* pubcomp) {
    if (!packet || !pubcomp || packet->type != MQTT_PACKET_PUBCOMP) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    pubcomp->packet_id = decode_uint16(packet->data);
    return 0;
}

int mqtt_packet_decode_suback(const mqtt_packet_t* packet, mqtt_packet_suback_t* suback) {
    if (!packet || !suback || packet->type != MQTT_PACKET_SUBACK) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    suback->packet_id = decode_uint16(packet->data);
    
    suback->return_code_count = packet->remaining_length - 2;
    suback->return_codes = (uint8_t*)iot_malloc(suback->return_code_count);
    if (!suback->return_codes) return -1;
    
    memcpy(suback->return_codes, packet->data + 2, suback->return_code_count);
    
    return 0;
}

int mqtt_packet_decode_unsuback(const mqtt_packet_t* packet, mqtt_packet_unsuback_t* unsuback) {
    if (!packet || !unsuback || packet->type != MQTT_PACKET_UNSUBACK) {
        return -1;
    }
    
    if (packet->remaining_length < 2) return -1;
    
    unsuback->packet_id = decode_uint16(packet->data);
    return 0;
}

/*===========================================================
 * 辅助函数实现
 *===========================================================*/

const char* mqtt_packet_type_name(mqtt_packet_type_t type) {
    switch (type) {
        case MQTT_PACKET_CONNECT:     return "CONNECT";
        case MQTT_PACKET_CONNACK:     return "CONNACK";
        case MQTT_PACKET_PUBLISH:     return "PUBLISH";
        case MQTT_PACKET_PUBACK:      return "PUBACK";
        case MQTT_PACKET_PUBREC:      return "PUBREC";
        case MQTT_PACKET_PUBREL:      return "PUBREL";
        case MQTT_PACKET_PUBCOMP:     return "PUBCOMP";
        case MQTT_PACKET_SUBSCRIBE:   return "SUBSCRIBE";
        case MQTT_PACKET_SUBACK:      return "SUBACK";
        case MQTT_PACKET_UNSUBSCRIBE: return "UNSUBSCRIBE";
        case MQTT_PACKET_UNSUBACK:    return "UNSUBACK";
        case MQTT_PACKET_PINGREQ:     return "PINGREQ";
        case MQTT_PACKET_PINGRESP:    return "PINGRESP";
        case MQTT_PACKET_DISCONNECT:  return "DISCONNECT";
        default:                       return "UNKNOWN";
    }
}

bool mqtt_topic_validate(const char* topic, size_t len) {
    if (!topic || len == 0 || len > MQTT_MAX_TOPIC_LEN) {
        return false;
    }
    
    /* 主题名不能包含通配符 */
    for (size_t i = 0; i < len; i++) {
        if (topic[i] == '+' || topic[i] == '#') {
            return false;
        }
    }
    
    /* 不能以 / 开头，除非就是单独的 / */
    if (len > 1 && topic[0] == '/' && topic[1] == '/') {
        return false;
    }
    
    return true;
}

bool mqtt_topic_match(const char* filter, const char* topic) {
    if (!filter || !topic) return false;
    
    const char* f = filter;
    const char* t = topic;
    
    while (*f && *t) {
        if (*f == '#') {
            /* # 匹配任意多级 */
            return true;
        }
        
        if (*f == '+') {
            /* + 匹配单级 */
            while (*t && *t != '/') t++;
            f++;
            continue;
        }
        
        if (*f != *t) {
            return false;
        }
        
        f++;
        t++;
    }
    
    /* 处理末尾 */
    if (*f == '#') return true;
    if (*f == '\0' && *t == '\0') return true;
    if (*f == '+' && (*t == '\0' || *t == '/')) return true;
    
    return false;
}

void mqtt_packet_free(mqtt_packet_t* packet) {
    if (!packet) return;
    
    if (packet->type == MQTT_PACKET_PUBLISH && packet->packet.publish.topic) {
        iot_free((void*)packet->packet.publish.topic);
    }
    
    if (packet->type == MQTT_PACKET_SUBACK && packet->packet.suback.return_codes) {
        iot_free(packet->packet.suback.return_codes);
    }
    
    if (packet->data) {
        iot_free(packet->data);
    }
    
    memset(packet, 0, sizeof(mqtt_packet_t));
}
