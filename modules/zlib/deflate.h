/**
 * @file deflate.h
 * @brief DEFLATE 压缩/解压接口
 *
 * 提供基于 miniz 库的 DEFLATE 格式压缩和解压功能
 * 支持原始 DEFLATE 数据的压缩和解压操作
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef DEFLATE_H
#define DEFLATE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================
 * compress (raw deflate fixed)
 * ========================= */
size_t deflate_compress(const uint8_t *in,
                        size_t in_len,
                        uint8_t *out,
                        size_t out_cap);

/* =========================
 * decompress (inflate fixed)
 * ========================= */
size_t inflate_decompress(const uint8_t *in,
                          size_t in_len,
                          uint8_t *out,
                          size_t out_cap);

#ifdef __cplusplus
}
#endif

#endif