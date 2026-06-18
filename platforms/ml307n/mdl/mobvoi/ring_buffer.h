// Copyright 2017 Mobvoi Inc. All Rights Reserved.
// Author: congfu@mobvoi.com (Cong Fu)

#ifndef SDK_UTILS_RING_BUFFER_H_
#define SDK_UTILS_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
  CONSUME = 1,
  COPY = 2,
  NOBLOCK = 4,
} read_flag;

typedef struct {
  short* base;
  short* read;
  short* write;
  int length;
  int full;
} ring_buffer;

ring_buffer* ring_buffer_init(int length);

void ring_buffer_cleanup(ring_buffer* buffer);

int ring_buffer_read(ring_buffer* buffer, short* data, int size, int flag);

short* ring_buffer_read_by_addr(ring_buffer* buffer, int size);

int ring_buffer_write(ring_buffer* buffer, const short* data, int size);

int ring_buffer_insert_zero(ring_buffer* buffer, int size);

int ring_buffer_revert(ring_buffer* buffer, int size);

void ring_buffer_discard(ring_buffer* buffer, int size);

int ring_buffer_is_full(ring_buffer* buffer);

int ring_buffer_is_empty(ring_buffer* buffer);

int ring_buffer_get_count(ring_buffer* buffer);

void ring_buffer_reset(ring_buffer* buffer);

#ifdef __cplusplus
}
#endif

#endif  // SDK_UTILS_RING_BUFFER_H_
