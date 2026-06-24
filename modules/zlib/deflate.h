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