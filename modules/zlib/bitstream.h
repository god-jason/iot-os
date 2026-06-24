#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================
 * Bitstream (DEFLATE / ZIP / GZIP compatible)
 * LSB-first
 * ========================================================= */

typedef struct {

    /* ---------- write side ---------- */
    uint8_t *out;
    size_t out_cap;
    size_t out_pos;

    uint32_t w_bitbuf;
    int w_bitcnt;

    /* ---------- read side ---------- */
    const uint8_t *in;
    size_t in_size;
    size_t in_pos;

    uint32_t r_bitbuf;
    int r_bitcnt;

} bitstream_t;

/* =========================
 * init
 * ========================= */
void bs_init_writer(bitstream_t *bs,
                    uint8_t *out,
                    size_t cap);

void bs_init_reader(bitstream_t *bs,
                    const uint8_t *in,
                    size_t size);

/* =========================
 * write API (compress side)
 * ========================= */
void bs_write_bits(bitstream_t *bs,
                   uint32_t value,
                   int bits);

void bs_write_byte(bitstream_t *bs,
                   uint8_t v);

void bs_flush_writer(bitstream_t *bs);

/* =========================
 * read API (decompress side)
 * ========================= */
int bs_read_bits(bitstream_t *bs,
                 int bits,
                 uint32_t *out);

/* =========================
 * utils
 * ========================= */
size_t bs_written(bitstream_t *bs);

#ifdef __cplusplus
}
#endif

#endif