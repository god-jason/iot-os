#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lz77.h"
#include "bitstream.h"

/* encode fixed huffman block */
void huffman_encode_fixed(bitstream_t *bs,
                          const lz77_token_t *tokens,
                          size_t n);

#ifdef __cplusplus
}
#endif

#endif