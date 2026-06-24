#include "crc32.h"

/* =========================================================
 * CRC32 IEEE 802.3 / PKZIP
 * polynomial: 0xEDB88320 (reversed)
 * ========================================================= */

/* =========================
 * table (fast version)
 * ========================= */
static uint32_t crc_table[256];
static int table_init = 0;

/* generate table */
static void crc32_init_table(void)
{
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;

        for (int j = 0; j < 8; j++) {
            if (c & 1)
                c = (c >> 1) ^ 0xEDB88320u;
            else
                c >>= 1;
        }

        crc_table[i] = c;
    }

    table_init = 1;
}

/* =========================
 * update (streaming)
 * ========================= */
uint32_t crc32_update(uint32_t crc,
                      const uint8_t *data,
                      size_t len)
{
    if (!table_init)
        crc32_init_table();

    crc = crc ^ 0xFFFFFFFFu;

    while (len--) {
        uint8_t idx = (crc ^ *data++) & 0xFF;
        crc = (crc >> 8) ^ crc_table[idx];
    }

    return crc ^ 0xFFFFFFFFu;
}

/* =========================
 * one-shot
 * ========================= */
uint32_t crc32(const uint8_t *data,
               size_t len)
{
    return crc32_update(0xFFFFFFFFu, data, len);
}