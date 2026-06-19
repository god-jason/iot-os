#ifndef IOT_QUEUE_H
#define IOT_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* iot_queue_t;

#define IOT_WAIT_FOREVER 0xFFFFFFFFU

iot_queue_t iot_queue_create(int msg_num, int msg_size);

bool iot_queue_send(iot_queue_t queue, const void* data, uint32_t timeout_ms);

bool iot_queue_recv(iot_queue_t queue, void* data, uint32_t timeout_ms);

void iot_queue_delete(iot_queue_t queue);

bool iot_queue_is_empty(iot_queue_t queue);

int iot_queue_get_count(iot_queue_t queue);

#ifdef __cplusplus
}
#endif

#endif