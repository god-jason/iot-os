#include "iot_queue.h"
#include "iot.h"
#include "iot_log.h"

/* 使用 iot_queue.h 中定义的 IOT_WAIT_FOREVER */

typedef struct {
    void* buffer;
    int msg_num;
    int msg_size;
    int head;
    int tail;
    int count;
    iot_mutex_t mutex;
    iot_sem_t sem_empty;
    iot_sem_t sem_full;
} iot_queue_internal_t;

iot_queue_t iot_queue_create(int msg_num, int msg_size)
{
    LOG_DEBUG("queue create: msg_num=%d, msg_size=%d", msg_num, msg_size);
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)iot_malloc(sizeof(iot_queue_internal_t));
    if (q == NULL) {
        LOG_ERROR("queue create failed: malloc error");
        return NULL;
    }
    
    q->buffer = iot_malloc(msg_num * msg_size);
    if (q->buffer == NULL) {
        LOG_ERROR("queue create failed: buffer malloc error");
        iot_free(q);
        return NULL;
    }
    
    q->msg_num = msg_num;
    q->msg_size = msg_size;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    
    q->mutex = iot_mutex_create();
    if (q->mutex == NULL) {
        LOG_ERROR("queue create failed: mutex create error");
        iot_free(q->buffer);
        iot_free(q);
        return NULL;
    }
    
    q->sem_empty = iot_sem_create(msg_num, msg_num);
    if (q->sem_empty == NULL) {
        LOG_ERROR("queue create failed: sem_empty create error");
        iot_mutex_delete(q->mutex);
        iot_free(q->buffer);
        iot_free(q);
        return NULL;
    }
    
    q->sem_full = iot_sem_create(msg_num, 0);
    if (q->sem_full == NULL) {
        LOG_ERROR("queue create failed: sem_full create error");
        iot_sem_delete(q->sem_empty);
        iot_mutex_delete(q->mutex);
        iot_free(q->buffer);
        iot_free(q);
        return NULL;
    }
    
    LOG_INFO("queue created successfully");
    return (iot_queue_t)q;
}

bool iot_queue_send(iot_queue_t queue, const void* data, uint32_t timeout_ms)
{
    if (queue == NULL || data == NULL) {
        LOG_ERROR("queue send: invalid param");
        return false;
    }
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)queue;
    
    LOG_TRACE("queue send: timeout=%u", timeout_ms);
    
    if (timeout_ms == IOT_WAIT_FOREVER) {
        iot_sem_wait(q->sem_empty);
    } else {
        if (!iot_sem_wait_timeout(q->sem_empty, timeout_ms)) {
            LOG_WARN("queue send: timeout");
            return false;
        }
    }
    
    iot_mutex_lock(q->mutex, IOT_WAIT_FOREVER);
    
    int offset = q->head * q->msg_size;
    memcpy((char*)q->buffer + offset, data, q->msg_size);
    q->head = (q->head + 1) % q->msg_num;
    q->count++;
    
    iot_mutex_unlock(q->mutex);
    
    iot_sem_post(q->sem_full);
    
    return true;
}

bool iot_queue_recv(iot_queue_t queue, void* data, uint32_t timeout_ms)
{
    if (queue == NULL || data == NULL) {
        LOG_ERROR("queue recv: invalid param");
        return false;
    }
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)queue;
    
    LOG_TRACE("queue recv: timeout=%u", timeout_ms);
    
    if (timeout_ms == IOT_WAIT_FOREVER) {
        iot_sem_wait(q->sem_full);
    } else {
        if (!iot_sem_wait_timeout(q->sem_full, timeout_ms)) {
            LOG_TRACE("queue recv: timeout");
            return false;
        }
    }
    
    iot_mutex_lock(q->mutex, IOT_WAIT_FOREVER);
    
    int offset = q->tail * q->msg_size;
    memcpy(data, (char*)q->buffer + offset, q->msg_size);
    q->tail = (q->tail + 1) % q->msg_num;
    q->count--;
    
    iot_mutex_unlock(q->mutex);
    
    iot_sem_post(q->sem_empty);
    
    return true;
}

void iot_queue_delete(iot_queue_t queue)
{
    if (queue == NULL) {
        LOG_WARN("queue delete: null pointer");
        return;
    }
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)queue;
    
    LOG_DEBUG("queue delete: count=%d", q->count);
    
    iot_sem_delete(q->sem_full);
    iot_sem_delete(q->sem_empty);
    iot_mutex_delete(q->mutex);
    iot_free(q->buffer);
    iot_free(q);
    
    LOG_INFO("queue deleted");
}

bool iot_queue_is_empty(iot_queue_t queue)
{
    if (queue == NULL) {
        return true;
    }
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)queue;
    
    iot_mutex_lock(q->mutex, IOT_WAIT_FOREVER);
    bool empty = (q->count == 0);
    iot_mutex_unlock(q->mutex);
    
    return empty;
}

int iot_queue_get_count(iot_queue_t queue)
{
    if (queue == NULL) {
        return 0;
    }
    
    iot_queue_internal_t* q = (iot_queue_internal_t*)queue;
    
    iot_mutex_lock(q->mutex, IOT_WAIT_FOREVER);
    int count = q->count;
    iot_mutex_unlock(q->mutex);
    
    return count;
}