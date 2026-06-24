#include "huffman.h"

/* =========================================================
 * DEFLATE Fixed Huffman Encoder
 * (literal/length + distance)
 * ========================================================= */

/* =========================
 * literal encoding (fixed table)
 * ========================= */
static inline void emit_lit(bitstream_t *bs, uint16_t c)
{
    /* 0-143 : 8 bits 00110000 + c */
    if (c <= 143) {
        bs_write(bs, 0x30 + c, 8);
    }
    /* 144-255 : 9 bits */
    else if (c <= 255) {
        bs_write(bs, 0x190 + (c - 144), 9);
    }
}

/* =========================
 * length code (DEFLATE spec)
 * ========================= */
static inline int length_code(int len, int *eb, int *ev)
{
    if (len <= 10) { *eb=0; *ev=0; return 257 + (len - 3); }
    if (len <= 18) { *eb=1; *ev=len - 11; return 265; }
    if (len <= 34) { *eb=2; *ev=len - 19; return 269; }
    if (len <= 66) { *eb=3; *ev=len - 35; return 273; }
    if (len <= 130){ *eb=4; *ev=len - 67; return 277; }
    if (len <= 257){ *eb=5; *ev=len - 131; return 281; }

    *eb = 0;
    *ev = 0;
    return 285;
}

/* =========================
 * distance code
 * ========================= */
static inline int dist_code(int d, int *eb, int *ev)
{
    if (d <= 4) { *eb=0; *ev=0; return d - 1; }

    if (d <= 10){ *eb=5; *ev=d - 5; return 4; }
    if (d <= 18){ *eb=5; *ev=d - 11; return 5; }
    if (d <= 34){ *eb=5; *ev=d - 19; return 6; }
    if (d <= 66){ *eb=6; *ev=d - 35; return 7; }
    if (d <=130){ *eb=7; *ev=d - 67; return 8; }

    *eb = 8;
    *ev = d - 131;
    return 9;
}

/* =========================
 * emit match
 * ========================= */
static inline void emit_match(bitstream_t *bs,
                              int len,
                              int dist)
{
    int eb1, ev1;
    int eb2, ev2;

    int lc = length_code(len, &eb1, &ev1);
    int dc = dist_code(dist, &eb2, &ev2);

    /* length */
    bs_write(bs, lc, 7);
    if (eb1) bs_write(bs, ev1, eb1);

    /* distance */
    bs_write(bs, dc, 5);
    if (eb2) bs_write(bs, ev2, eb2);
}

/* =========================
 * main encoder
 * ========================= */
void huffman_encode_fixed(bitstream_t *bs,
                          const lz77_token_t *tokens,
                          size_t n)
{
    /* DEFLATE block header:
     * BFINAL = 1
     * BTYPE  = 01 (fixed Huffman)
     */
    bs_write(bs, 1, 1);
    bs_write(bs, 1, 2);

    for (size_t i = 0; i < n; i++) {

        if (tokens[i].type == LZ77_TOKEN_LIT) {

            emit_lit(bs, tokens[i].lit);
        }
        else {

            emit_match(bs,
                       tokens[i].len,
                       tokens[i].dist);
        }
    }

    /* end of block (256) */
    bs_write(bs, 256, 7);
}