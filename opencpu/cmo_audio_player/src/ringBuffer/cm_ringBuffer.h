#ifndef __MY_RINGBUFFER__
#define __MY_RINGBUFFER__

#include "cm_common_api.h"
#include <stdint.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    int full;
    size_t currentLength;
    cm_mutex_t mutex;
} CircularBuffer;

int initCircularBuffer(CircularBuffer *cb, size_t size);
int uninitCircularBuffer(CircularBuffer *cb);
int wirteByteToCircularBuffer(CircularBuffer *cb, uint8_t data);
int writeCircularBuffer(CircularBuffer *cb, const uint8_t *data, size_t length);
size_t readCircularBuffer(CircularBuffer *cb, uint8_t *data, size_t length);
size_t getCurrentLength(const CircularBuffer *cb);
int seekCircularBuffer(CircularBuffer *cb, size_t offset, size_t pos);


#endif // __MY_RINGBUFFER__
