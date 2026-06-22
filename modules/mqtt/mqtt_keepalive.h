/**
 * @file mqtt_keepalive.h
 * @brief MQTT 心跳机制和自动重连
 *
 * 负责 PINGREQ 发送、PINGRESP 超时检测及断线自动重连逻辑
 */
#ifndef IOT_MQTT_KEEPALIVE_H
#define IOT_MQTT_KEEPALIVE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mqtt_client mqtt_client_t;

typedef struct {
    bool enabled;
    int keepalive_sec;
    uint32_t ping_interval_ms;
    uint32_t ping_timeout_ms;
    uint32_t last_ping_time;
    uint32_t ping_sent_time;
} mqtt_keepalive_t;

typedef struct {
    bool enabled;
    int reconnect_interval_ms;
    uint32_t last_connect_time;
} mqtt_reconnect_t;

void mqtt_keepalive_init(mqtt_keepalive_t* keepalive);
void mqtt_keepalive_configure(mqtt_keepalive_t* keepalive, int keepalive_sec, int timeout_ms);
void mqtt_keepalive_reset(mqtt_keepalive_t* keepalive);
void mqtt_keepalive_mark_ping(mqtt_keepalive_t* keepalive);
void mqtt_keepalive_mark_pong(mqtt_keepalive_t* keepalive);
bool mqtt_keepalive_check_timeout(mqtt_keepalive_t* keepalive);
bool mqtt_keepalive_needs_ping(mqtt_keepalive_t* keepalive);

void mqtt_reconnect_init(mqtt_reconnect_t* reconnect);
void mqtt_reconnect_enable(mqtt_reconnect_t* reconnect, int interval_ms);
void mqtt_reconnect_disable(mqtt_reconnect_t* reconnect);
void mqtt_reconnect_mark_attempt(mqtt_reconnect_t* reconnect);
bool mqtt_reconnect_needs_retry(mqtt_reconnect_t* reconnect);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MQTT_KEEPALIVE_H */