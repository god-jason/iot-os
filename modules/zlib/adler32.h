#ifndef ADLER32_H
#define ADLER32_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* zlib standard Adler-32 */
#define ADLER32_INIT 1u

/* update */
uint32_t adler32_update(uint32_t adler,
                        const uint8_t *data,
                        size_t len);

/* one-shot */
uint32_t adler32(const uint8_t *data,
                 size_t len);

#ifdef __cplusplus
}
#endif

#endif