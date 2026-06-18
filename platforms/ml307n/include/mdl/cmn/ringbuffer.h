/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t in;   /*!< Define the write pointer.               */
    uint32_t out;  /*!< Define the read pointer.                */
    uint32_t mask; /*!< Define the write and read pointer mask. */
    void *pool;    /*!< Define the memory pointer.              */
} os_ringbuffer_t;

extern int os_ringbuffer_init(os_ringbuffer_t *rb, void *pool, uint32_t size);
extern void os_ringbuffer_reset(os_ringbuffer_t *rb);
extern void os_ringbuffer_reset_read(os_ringbuffer_t *rb);

extern uint32_t os_ringbuffer_get_size(os_ringbuffer_t *rb);
extern uint32_t os_ringbuffer_get_used(os_ringbuffer_t *rb);
extern uint32_t os_ringbuffer_get_free(os_ringbuffer_t *rb);

extern bool os_ringbuffer_check_full(os_ringbuffer_t *rb);
extern bool os_ringbuffer_check_empty(os_ringbuffer_t *rb);

extern bool os_ringbuffer_write_byte(os_ringbuffer_t *rb, uint8_t byte);
extern bool os_ringbuffer_overwrite_byte(os_ringbuffer_t *rb, uint8_t byte);
extern bool os_ringbuffer_peek_byte(os_ringbuffer_t *rb, uint8_t *byte);
extern bool os_ringbuffer_read_byte(os_ringbuffer_t *rb, uint8_t *byte);
extern bool os_ringbuffer_drop_byte(os_ringbuffer_t *rb);

extern uint32_t os_ringbuffer_write(os_ringbuffer_t *rb, void *data, uint32_t size);
extern uint32_t os_ringbuffer_overwrite(os_ringbuffer_t *rb, void *data, uint32_t size);
extern uint32_t os_ringbuffer_peek(os_ringbuffer_t *rb, void *data, uint32_t size);
extern uint32_t os_ringbuffer_read(os_ringbuffer_t *rb, void *data, uint32_t size);
extern uint32_t os_ringbuffer_drop(os_ringbuffer_t *rb, uint32_t size);

extern void *os_ringbuffer_linear_write_setup(os_ringbuffer_t *rb, uint32_t *size);
extern void *os_ringbuffer_linear_read_setup(os_ringbuffer_t *rb, uint32_t *size);
extern uint32_t os_ringbuffer_linear_write_done(os_ringbuffer_t *rb, uint32_t size);
extern uint32_t os_ringbuffer_linear_read_done(os_ringbuffer_t *rb, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
