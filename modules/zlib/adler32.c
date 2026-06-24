#include "adler32.h"

/* =========================================================
 * Adler-32 checksum (zlib compatible)
 * A = sum of bytes
 * B = sum of A
 * mod = 65521 (largest prime < 2^16)
 * ========================================================= */

#define MOD_ADLER 65521u

/* =========================
 * streaming update
 * ========================= */
uint32_t adler32_update(uint32_t adler,
                        const uint8_t *data,
                        size_t len)
{
    uint32_t a = adler & 0xFFFF;
    uint32_t b = (adler >> 16) & 0xFFFF;

    while (len) {

        /* block processing to avoid overflow */
        size_t n = len > 5552 ? 5552 : len;

        len -= n;

        while (n--) {
            a += *data++;
            b += a;
        }

        a %= MOD_ADLER;
        b %= MOD_ADLER;
    }

    return (b << 16) | a;
}

/* =========================
 * one-shot
 * ========================= */
uint32_t adler32(const uint8_t *data,
                 size_t len)
{
    return adler32_update(1u, data, len);
}