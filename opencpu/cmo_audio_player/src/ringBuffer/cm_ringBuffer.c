#include "cm_common_api.h"
#include "os.h"

#include "cm_ringBuffer.h"

int initCircularBuffer(CircularBuffer *cb, size_t size)
{
    cb->buffer = (uint8_t *)cm_malloc(size * sizeof(uint8_t));
    if (cb->buffer == NULL) {
        return -1;
    }

    cb->capacity = size;
    cb->head = 0;
    cb->tail = 0;
    cb->full = 0;
    cb->currentLength = 0;

    cb->mutex = cm_mutex_create();
    if (cb->mutex == NULL) {
        cm_free(cb->buffer);
        cb->buffer = NULL;
        return -1;
    }

    return 0;
}

int wirteByteToCircularBuffer(CircularBuffer *cb, uint8_t data)
{
    if (cb->full) {
        return -1;
    }

    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % cb->capacity;

    cb->currentLength++;

    if (cb->head == cb->tail) {
        cb->full = 1;
    } else {

    }

    return 0;
}

int writeCircularBuffer(CircularBuffer *cb, const uint8_t *data, size_t length)
{
    cm_mutex_take(cb->mutex);

    if (length > (cb->capacity - cb->currentLength)) {
        cm_mutex_release(cb->mutex);
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        if (wirteByteToCircularBuffer(cb, data[i]) != 0) {
            cm_mutex_release(cb->mutex);
            return -1;
        }
    }

    cm_mutex_release(cb->mutex);

    return length;
}

size_t readCircularBuffer(CircularBuffer *cb, uint8_t *data, size_t length)
{
    cm_mutex_take(cb->mutex);

    size_t available = cb->full ? cb->capacity : (cb->head >= cb->tail ? cb->head - cb->tail : cb->capacity - (cb->tail - cb->head));

    size_t readData = length >= available ? available : length;

    for (size_t i = 0; i < readData; ++i) {
        data[i] = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % cb->capacity;
    }

    cb->full = 0;
    cb->currentLength -= readData;

    cm_mutex_release(cb->mutex);

    return readData;
}

size_t getCurrentLength(const CircularBuffer *cb)
{
    cm_mutex_take(cb->mutex);

    size_t currentLength = cb->currentLength;

    cm_mutex_release(cb->mutex);

    return currentLength;
}

int uninitCircularBuffer(CircularBuffer *cb)
{
    if (cb == NULL) {
        return -1;
    }

    if (cb->buffer != NULL) {
        cm_free(cb->buffer);
    }

    if (cb->mutex != NULL) {
        cm_mutex_delete(cb->mutex);
    }

    return 0;
}

int seekCircularBuffer(CircularBuffer *cb, size_t offset, size_t pos)
{
    cm_mutex_take(cb->mutex);

    size_t available = cb->full ? cb->capacity : (cb->head >= cb->tail ? cb->head - cb->tail : cb->capacity - (cb->tail - cb->head));

    if (available < offset) {
        cm_mutex_release(cb->mutex);
        return -1;
    }

    if (pos == 0) {
        cb->tail = offset % cb->capacity;
        if (offset > cb->tail) {
            cb->currentLength -= (offset - cb->tail);
        } else {
            cb->currentLength += (cb->tail - offset);
        }
    } else if (pos == 1) {
        cb->tail = (cb->tail + offset) % cb->capacity;
        cb->currentLength -= offset;
    } else {
        cm_mutex_release(cb->mutex);
        return -1;
    }

    cm_mutex_release(cb->mutex);

    return 0;
}


