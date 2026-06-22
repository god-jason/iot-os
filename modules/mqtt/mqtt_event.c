/**
 * @file mqtt_event.c
 * @brief MQTT 事件处理实现
 */

#include "mqtt_event.h"

#include <string.h>

void mqtt_event_init(mqtt_event_t* event) {
    if (!event) return;
    memset(event, 0, sizeof(mqtt_event_t));
}

void mqtt_event_set_callback(mqtt_event_t* event, mqtt_event_callback_t callback, void* user_data) {
    if (!event) return;
    event->event_callback = callback;
    event->event_user_data = user_data;
}

void mqtt_event_trigger(mqtt_event_t* event, mqtt_event_type_t event_type) {
    if (!event || !event->event_callback) return;
    event->event_callback(event->event_user_data, event_type);
}