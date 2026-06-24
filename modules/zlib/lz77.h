#ifndef LZ77_H
#define LZ77_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LZ77_WINDOW_SIZE 32768
#define LZ77_MIN_MATCH   3
#define LZ77_MAX_MATCH   258

/* =========================
 * token (for Huffman)
 * ========================= */
typedef enum {
    LZ77_LIT,
    LZ77_MATCH
} lz77_type_t;

typedef struct {
    lz77_type_t type;

    uint8_t lit;

    uint16_t len;
    uint16_t dist;
} lz77_token_t;

/* =========================
 * state (compress + decompress shared)
 * ========================= */
typedef struct {
    uint8_t window[LZ77_WINDOW_SIZE];
    uint16_t pos;
} lz77_state_t;

/* =========================
 * API
 * ========================= */

/* init */
void lz77_init(lz77_state_t *s);

/* compress */
size_t lz77_compress(lz77_state_t *s,
                     const uint8_t *in,
                     size_t in_len,
                     lz77_token_t *out,
                     size_t max_tokens);

/* decompress */
size_t lz77_decompress(lz77_state_t *s,
                       const lz77_token_t *in,
                       size_t in_len,
                       uint8_t *out,
                       size_t out_cap);

#ifdef __cplusplus
}
#endif

#endif