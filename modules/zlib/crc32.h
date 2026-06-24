#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化值（ZIP标准） */
#define CRC32_INIT 0xFFFFFFFFu
#define CRC32_XOROUT 0xFFFFFFFFu

/* 计算CRC32 */
uint32_t crc32_update(uint32_t crc,
                      const uint8_t *data,
                      size_t len);

/* 一步计算 */
uint32_t crc32(const uint8_t *data,
               size_t len);

#ifdef __cplusplus
}
#endif

#endif