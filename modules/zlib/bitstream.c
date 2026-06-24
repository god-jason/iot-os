#include "bitstream.h"
#include <string.h>

/* =========================================================
 * INIT
 * ========================================================= */

void bs_init_writer(bitstream_t *bs,
                    uint8_t *out,
                    size_t cap)
{
    memset(bs, 0, sizeof(*bs));

    bs->out = out;
    bs->out_cap = cap;
    bs->out_pos = 0;
}

void bs_init_reader(bitstream_t *bs,
                    const uint8_t *in,
                    size_t size)
{
    memset(bs, 0, sizeof(*bs));

    bs->in = in;
    bs->in_size = size;
    bs->in_pos = 0;
}

/* =========================================================
 * WRITE (LSB-first)
 * ========================================================= */

void bs_write_bits(bitstream_t *bs,
                   uint32_t value,
                   int bits)
{
    bs->w_bitbuf |= (value << bs->w_bitcnt);
    bs->w_bitcnt += bits;

    while (bs->w_bitcnt >= 8) {

        if (bs->out_pos < bs->out_cap) {
            bs->out[bs->out_pos++] =
                (uint8_t)(bs->w_bitbuf & 0xFF);
        }

        bs->w_bitbuf >>= 8;
        bs->w_bitcnt -= 8;
    }
}

/* =========================
 * fast byte write
 * ========================= */
void bs_write_byte(bitstream_t *bs,
                   uint8_t v)
{
    if (bs->w_bitcnt == 0) {
        if (bs->out_pos < bs->out_cap)
            bs->out[bs->out_pos++] = v;
        return;
    }

    bs_write_bits(bs, v, 8);
}

/* =========================
 * flush writer
 * ========================= */
void bs_flush_writer(bitstream_t *bs)
{
    while (bs->w_bitcnt > 0) {

        if (bs->out_pos < bs->out_cap) {
            bs->out[bs->out_pos++] =
                (uint8_t)(bs->w_bitbuf & 0xFF);
        }

        bs->w_bitbuf >>= 8;
        bs->w_bitcnt -= 8;
    }

    bs->w_bitbuf = 0;
    bs->w_bitcnt = 0;
}

/* =========================================================
 * READ (LSB-first)
 * ========================================================= */

int bs_read_bits(bitstream_t *bs,
                 int bits,
                 uint32_t *out)
{
    while (bs->r_bitcnt < bits) {

        if (bs->in_pos >= bs->in_size)
            return -1;

        bs->r_bitbuf |=
            (uint32_t)bs->in[bs->in_pos++] << bs->r_bitcnt;

        bs->r_bitcnt += 8;
    }

    *out = bs->r_bitbuf & ((1u << bits) - 1);

    bs->r_bitbuf >>= bits;
    bs->r_bitcnt -= bits;

    return 0;
}

/* =========================================================
 * utils
 * ========================================================= */

size_t bs_written(bitstream_t *bs)
{
    return bs->out_pos;
}