#include "lz77.h"
#include <string.h>

/* =========================================================
 * init
 * ========================================================= */
void lz77_init(lz77_state_t *s)
{
    memset(s->window, 0, LZ77_WINDOW_SIZE);
    s->pos = 0;
}

/* =========================================================
 * window write
 * ========================================================= */
static inline void win_put(lz77_state_t *s, uint8_t c)
{
    s->window[s->pos] = c;
    s->pos = (s->pos + 1) & (LZ77_WINDOW_SIZE - 1);
}

/* =========================================================
 * window read
 * ========================================================= */
static inline uint8_t win_get(lz77_state_t *s, uint16_t p)
{
    return s->window[p & (LZ77_WINDOW_SIZE - 1)];
}

/* =========================================================
 * find best match (simple but correct)
 * ========================================================= */
static void find_best(lz77_state_t *s,
                      const uint8_t *in,
                      size_t i,
                      size_t len,
                      uint16_t *best_len,
                      uint16_t *best_dist)
{
    *best_len = 0;
    *best_dist = 0;

    if (i + LZ77_MIN_MATCH >= len)
        return;

    for (uint16_t dist = 1; dist < LZ77_WINDOW_SIZE; dist++) {

        uint16_t p = (s->pos + LZ77_WINDOW_SIZE - dist)
                     & (LZ77_WINDOW_SIZE - 1);

        uint16_t l = 0;

        while (l < LZ77_MAX_MATCH &&
               i + l < len)
        {
            if (in[i + l] != s->window[(p + l) & (LZ77_WINDOW_SIZE - 1)])
                break;

            l++;
        }

        if (l >= LZ77_MIN_MATCH && l > *best_len) {
            *best_len = l;
            *best_dist = dist;

            if (l == LZ77_MAX_MATCH)
                return;
        }
    }
}

/* =========================================================
 * compress
 * ========================================================= */
size_t lz77_compress(lz77_state_t *s,
                     const uint8_t *in,
                     size_t in_len,
                     lz77_token_t *out,
                     size_t max_tokens)
{
    size_t i = 0, t = 0;

    while (i < in_len && t < max_tokens) {

        uint16_t bl = 0, bd = 0;

        find_best(s, in, i, in_len, &bl, &bd);

        if (bl >= LZ77_MIN_MATCH) {

            out[t].type = LZ77_MATCH;
            out[t].len = bl;
            out[t].dist = bd;

            for (int k = 0; k < bl; k++)
                win_put(s, in[i + k]);

            i += bl;
            t++;
        }
        else {

            out[t].type = LZ77_LIT;
            out[t].lit = in[i];

            win_put(s, in[i]);

            i++;
            t++;
        }
    }

    return t;
}

/* =========================================================
 * decompress
 * ========================================================= */
size_t lz77_decompress(lz77_state_t *s,
                       const lz77_token_t *in,
                       size_t in_len,
                       uint8_t *out,
                       size_t out_cap)
{
    size_t o = 0;

    for (size_t i = 0; i < in_len; i++) {

        if (o >= out_cap)
            break;

        if (in[i].type == LZ77_LIT) {

            uint8_t c = in[i].lit;

            win_put(s, c);
            out[o++] = c;
        }
        else {

            uint16_t p = (s->pos + LZ77_WINDOW_SIZE - in[i].dist)
                         & (LZ77_WINDOW_SIZE - 1);

            for (int k = 0; k < in[i].len; k++) {

                uint8_t c = s->window[(p + k) & (LZ77_WINDOW_SIZE - 1)];

                win_put(s, c);
                out[o++] = c;

                if (o >= out_cap)
                    break;
            }
        }
    }

    return o;
}