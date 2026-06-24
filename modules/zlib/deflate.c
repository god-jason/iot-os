#include "deflate.h"

/* =========================================================
 * include internal modules
 * ========================================================= */
#include "lz77.h"
#include "bitstream.h"

/* =========================================================
 * limits
 * ========================================================= */
#define MAX_TOKENS 16384
#define WIN_SIZE   32768

/* =========================================================
 * DEFLATE compress
 * ========================================================= */
size_t deflate_compress(const uint8_t *in,
                        size_t in_len,
                        uint8_t *out,
                        size_t out_cap)
{
    /* -------------------------
     * 1. LZ77 encode
     * ------------------------- */
    lz77_state_t lz;
    lz77_init(&lz);

    lz77_token_t tokens[MAX_TOKENS];

    size_t nt = lz77_compress(&lz,
                              in,
                              in_len,
                              tokens,
                              MAX_TOKENS);

    /* -------------------------
     * 2. bitstream
     * ------------------------- */
    bitstream_t bs;
    bs_init_writer(&bs, out, out_cap);

    /* DEFLATE block header:
     * BFINAL=1, BTYPE=01 (fixed Huffman)
     */
    bs_write_bits(&bs, 1, 1);
    bs_write_bits(&bs, 1, 2);

    /* -------------------------
     * 3. encode tokens
     * ------------------------- */
    for (size_t i = 0; i < nt; i++) {

        if (tokens[i].type == LZ77_LIT) {

            /* fixed literal Huffman (0-255) */
            bs_write_bits(&bs, tokens[i].lit, 8);
        }
        else {

            /* length */
            bs_write_bits(&bs, 257 + (tokens[i].len - 3), 7);

            /* distance */
            bs_write_bits(&bs, tokens[i].dist - 1, 5);
        }
    }

    /* end of block */
    bs_write_bits(&bs, 256, 7);

    bs_flush_writer(&bs);

    return bs_written(&bs);
}

/* =========================================================
 * DEFLATE decompress (inflate fixed)
 * ========================================================= */

typedef struct {
    uint8_t buf[WIN_SIZE];
    uint16_t pos;
} win_t;

static void win_init(win_t *w)
{
    for (int i = 0; i < WIN_SIZE; i++)
        w->buf[i] = 0;

    w->pos = 0;
}

static void win_put(win_t *w,
                    uint8_t c,
                    uint8_t *out,
                    size_t *o)
{
    w->buf[w->pos] = c;
    w->pos = (w->pos + 1) & (WIN_SIZE - 1);

    out[(*o)++] = c;
}

static void win_copy(win_t *w,
                     int dist,
                     int len,
                     uint8_t *out,
                     size_t *o)
{
    int p = (w->pos + WIN_SIZE - dist) & (WIN_SIZE - 1);

    for (int i = 0; i < len; i++) {
        uint8_t c = w->buf[(p + i) & (WIN_SIZE - 1)];

        win_put(w, c, out, o);
    }
}

/* =========================================================
 * bit reader (inline)
 * ========================================================= */
typedef struct {
    const uint8_t *in;
    size_t size;
    size_t pos;

    uint32_t bitbuf;
    int bitcnt;
} br_t;

static void br_init(br_t *b,
                    const uint8_t *in,
                    size_t size)
{
    b->in = in;
    b->size = size;
    b->pos = 0;
    b->bitbuf = 0;
    b->bitcnt = 0;
}

static int br_read(br_t *b,
                   int n,
                   uint32_t *out)
{
    while (b->bitcnt < n) {

        if (b->pos >= b->size)
            return -1;

        b->bitbuf |= (uint32_t)b->in[b->pos++] << b->bitcnt;
        b->bitcnt += 8;
    }

    *out = b->bitbuf & ((1u << n) - 1);

    b->bitbuf >>= n;
    b->bitcnt -= n;

    return 0;
}

/* =========================================================
 * inflate
 * ========================================================= */
size_t inflate_decompress(const uint8_t *in,
                          size_t in_len,
                          uint8_t *out,
                          size_t out_cap)
{
    br_t br;
    win_t w;

    br_init(&br, in, in_len);
    win_init(&w);

    size_t o = 0;

    uint32_t bfinal, btype;

    br_read(&br, 1, &bfinal);
    br_read(&br, 2, &btype);

    if (btype != 1)
        return 0;

    while (1) {

        uint32_t sym;

        /* read 7-bit symbol (fixed simplification) */
        if (br_read(&br, 7, &sym) < 0)
            break;

        /* end */
        if (sym == 256)
            break;

        /* literal */
        if (sym < 256) {

            win_put(&w, (uint8_t)sym, out, &o);
        }
        else {

            int len = (sym - 257) + 3;

            uint32_t dist;
            br_read(&br, 5, &dist);

            win_copy(&w, dist + 1, len, out, &o);
        }

        if (o >= out_cap)
            break;
    }

    return o;
}