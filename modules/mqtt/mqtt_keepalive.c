/**
 * @file mqtt_keepalive.c
 * @brief MQTT 心跳机制和自动重连实现
 */

#include "mqtt_keepalive.h"
#include "platform.h"

#include <string.h>

void mqtt_keepalive_init(mqtt_keepalive_t* keepalive) {
    if (!keepalive) return;
    memset(keepalive, 0, sizeof(mqtt_keepalive_t));
    keepalive->enabled = false;
    keepalive->ping_timeout_ms = 30000;
}

void mqtt_keepalive_configure(mqtt_keepalive_t* keepalive, int keepalive_sec, int timeout_ms) {
    if (!keepalive) return;
    
    keepalive->enabled = (keepalive_sec > 0);
    keepalive->keepalive_sec = keepalive_sec;
    keepalive->ping_interval_ms = (uint32_t)keepalive_sec * 1000 / 2;
    keepalive->ping_timeout_ms = (timeout_ms > 0) ? (uint32_t)timeout_ms : 30000;
    keepalive->last_ping_time = iot_get_tick_ms();
    keepalive->ping_sent_time = 0;
}

void mqtt_keepalive_reset(mqtt_keepalive_t* keepalive) {
    if (!keepalive) return;
    keepalive->last_ping_time = iot_get_tick_ms();
    keepalive->ping_sent_time = 0;
}

void mqtt_keepalive_mark_ping(mqtt_keepalive_t* keepalive) {
    if (!keepalive) return;
    keepalive->last_ping_time = iot_get_tick_ms();
    keepalive->ping_sent_time = iot_get_tick_ms();
}

void mqtt_keepalive_mark_pong(mqtt_keepalive_t* keepalive) {
    if (!keepalive) return;
    keepalive->last_ping_time = iot_get_tick_ms();
    keepalive->ping_sent_time = 0;
}

bool mqtt_keepalive_check_timeout(mqtt_keepalive_t* keepalive) {
    if (!keepalive || !keepalive->enabled) return false;
    if (keepalive->ping_sent_time == 0) return false;
    
    uint32_t now = iot_get_tick_ms();
    return (now - keepalive->ping_sent_time >= keepalive->ping_timeout_ms);
}

bool mqtt_keepalive_needs_ping(mqtt_keepalive_t* keepalive) {
    if (!keepalive || !keepalive->enabled) return false;
    
    uint32_t now = iot_get_tick_ms();
    return (now - keepalive->last_ping_time >= keepalive->ping_interval_ms);
}

void mqtt_reconnect_init(mqtt_reconnect_t* reconnect) {
    if (!reconnect) return;
    memset(reconnect, 0, sizeof(mqtt_reconnect_t));
    reconnect->enabled = false;
}

void mqtt_reconnect_enable(mqtt_reconnect_t* reconnect, int interval_ms) {
    if (!reconnect) return;
    reconnect->enabled = true;
    reconnect->reconnect_interval_ms = interval_ms;
}

void mqtt_reconnect_disable(mqtt_reconnect_t* reconnect) {
    if (!reconnect) return;
    reconnect->enabled = false;
}

void mqtt_reconnect_mark_attempt(mqtt_reconnect_t* reconnect) {
    if (!reconnect) return;
    reconnect->last_connect_time = iot_get_tick_ms();
}

bool mqtt_reconnect_needs_retry(mqtt_reconnect_t* reconnect) {
    if (!reconnect || !reconnect->enabled) return false;
    
    uint32_t now = iot_get_tick_ms();
    return (now - reconnect->last_connect_time >= (uint32_t)reconnect->reconnect_interval_ms);
}