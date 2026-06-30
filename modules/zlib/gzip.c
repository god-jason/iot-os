/**
 * @file gzip.c
 * @brief GZIP 压缩/解压（miniz 封装）
 */

#include "gzip.h"
#include "miniz.h"
#include "iot.h"
#include <string.h>
#include <stdio.h>

#define GZIP_MAGIC1          0x1F
#define GZIP_MAGIC2          0x8B
#define GZIP_METHOD_DEFLATE  0x08
#define GZIP_FLAG_FEXTRA     0x04
#define GZIP_FLAG_FNAME      0x08
#define GZIP_FLAG_FCOMMENT   0x10

static size_t gzip_raw_deflate(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_cap, int level)
{
    int flags = tdefl_create_comp_flags_from_zip_params(level, -MZ_DEFAULT_WINDOW_BITS, 0);
    return tdefl_compress_mem_to_mem(dst, dst_cap, src, src_len, flags);
}

static size_t gzip_raw_inflate(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_cap)
{
    return tinfl_decompress_mem_to_mem(dst, dst_cap, src, src_len, 0);
}

static const uint8_t *gzip_skip_header(const uint8_t *src, size_t src_len, size_t *compressed_len)
{
    const uint8_t *p = src;

    if (!src || src_len < 18 || compressed_len == NULL) {
        return NULL;
    }

    if (p[0] != GZIP_MAGIC1 || p[1] != GZIP_MAGIC2 || p[2] != GZIP_METHOD_DEFLATE) {
        return NULL;
    }

    {
        uint8_t flags = p[3];
        p += 10;

        if (flags & GZIP_FLAG_FEXTRA) {
            uint16_t extra_len;
            if ((size_t)(p - src) + 2 > src_len) {
                return NULL;
            }
            extra_len = (uint16_t)(p[0] | (p[1] << 8));
            p += 2 + extra_len;
        }

        if (flags & GZIP_FLAG_FNAME) {
            while (p < src + src_len && *p != '\0') {
                p++;
            }
            if (p >= src + src_len) {
                return NULL;
            }
            p++;
        }

        if (flags & GZIP_FLAG_FCOMMENT) {
            while (p < src + src_len && *p != '\0') {
                p++;
            }
            if (p >= src + src_len) {
                return NULL;
            }
            p++;
        }

        if (flags & 0x02) {
            p += 2;
        }
    }

    if ((size_t)(src + src_len - p) < 8) {
        return NULL;
    }

    *compressed_len = (size_t)((src + src_len - 8) - p);
    return p;
}

int gzip_decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len)
{
    size_t compressed_len = 0;
    const uint8_t *payload;
    size_t out_len;
    const uint8_t *tail;
    uint32_t crc32;
    uint32_t orig_size;
    uint32_t computed_crc;

    if (!src || !dst || !dst_len) {
        return GZIP_ERR_FORMAT;
    }

    payload = gzip_skip_header(src, src_len, &compressed_len);
    if (!payload || compressed_len == 0) {
        return GZIP_ERR_FORMAT;
    }

    out_len = gzip_raw_inflate(payload, compressed_len, dst, *dst_len);
    if (out_len == 0) {
        return GZIP_ERR_FORMAT;
    }

    tail = src + src_len - 8;
    crc32 = (uint32_t)(tail[0] | (tail[1] << 8) | (tail[2] << 16) | (tail[3] << 24));
    orig_size = (uint32_t)(tail[4] | (tail[5] << 8) | (tail[6] << 16) | (tail[7] << 24));

    computed_crc = (uint32_t)mz_crc32(MZ_CRC32_INIT, dst, out_len) ^ 0xffffffffU;
    if (computed_crc != crc32 || (uint32_t)out_len != orig_size) {
        return GZIP_ERR_CRC;
    }

    *dst_len = out_len;
    return GZIP_OK;
}

static size_t gzip_compress_bound_size(size_t src_len)
{
    mz_ulong bound = mz_compressBound((mz_ulong)src_len);
    size_t total = (size_t)bound + 18;

    if (total < src_len + 128) {
        total = src_len + 128;
    }
    return total;
}

size_t gzip_compress_bound(size_t src_len)
{
    return gzip_compress_bound_size(src_len);
}

int gzip_compress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level)
{
    uint8_t *p;
    uint32_t crc;
    size_t compressed;
    size_t bound;
    uint32_t size;

    if (!src || !dst || !dst_len) {
        return GZIP_ERR_FORMAT;
    }

    if (level < 1) {
        level = GZIP_COMPRESS_FAST;
    }
    if (level > 9) {
        level = GZIP_COMPRESS_BEST;
    }

    bound = gzip_compress_bound_size(src_len);
    if (*dst_len < bound) {
        return GZIP_ERR_MEM;
    }

    p = dst;
    p[0] = GZIP_MAGIC1;
    p[1] = GZIP_MAGIC2;
    p[2] = GZIP_METHOD_DEFLATE;
    p[3] = 0;
    memset(p + 4, 0, 6);
    p[9] = 0x03;
    p += 10;

    crc = (uint32_t)mz_crc32(MZ_CRC32_INIT, src, src_len) ^ 0xffffffffU;
    compressed = gzip_raw_deflate(src, src_len, p, *dst_len - 18, level);
    if (compressed == 0) {
        return GZIP_ERR_MEM;
    }
    p += compressed;

    *p++ = (uint8_t)(crc & 0xff);
    *p++ = (uint8_t)((crc >> 8) & 0xff);
    *p++ = (uint8_t)((crc >> 16) & 0xff);
    *p++ = (uint8_t)((crc >> 24) & 0xff);

    size = (uint32_t)src_len;
    *p++ = (uint8_t)(size & 0xff);
    *p++ = (uint8_t)((size >> 8) & 0xff);
    *p++ = (uint8_t)((size >> 16) & 0xff);
    *p++ = (uint8_t)((size >> 24) & 0xff);

    *dst_len = (size_t)(p - dst);
    return GZIP_OK;
}

int gzip_decompress_file(const char *src_path, const char *dst_path)
{
    iot_fs_file_t src_fd;
    int32_t file_size;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    size_t out_len;
    int ret;
    iot_fs_file_t dst_fd;
    const uint8_t *tail;
    uint32_t dst_size;

    if (!src_path || !dst_path) {
        return GZIP_ERR_FORMAT;
    }

    src_fd = iot_fs_open(src_path, IOT_FS_RB);
    if (!src_fd) {
        return GZIP_ERR_FILE;
    }

    file_size = iot_fs_filesize(src_path);
    if (file_size < 18) {
        iot_fs_close(src_fd);
        return GZIP_ERR_FORMAT;
    }

    src_buf = (uint8_t *)iot_malloc((size_t)file_size);
    if (!src_buf) {
        iot_fs_close(src_fd);
        return GZIP_ERR_MEM;
    }

    if (iot_fs_read(src_fd, src_buf, (size_t)file_size) != file_size) {
        iot_free(src_buf);
        iot_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }
    iot_fs_close(src_fd);

    tail = src_buf + file_size - 8;
    dst_size = (uint32_t)(tail[4] | (tail[5] << 8) | (tail[6] << 16) | (tail[7] << 24));

    dst_buf = (uint8_t *)iot_malloc(dst_size);
    if (!dst_buf) {
        iot_free(src_buf);
        return GZIP_ERR_MEM;
    }

    out_len = dst_size;
    ret = gzip_decompress(src_buf, (size_t)file_size, dst_buf, &out_len);
    iot_free(src_buf);
    if (ret != GZIP_OK) {
        iot_free(dst_buf);
        return ret;
    }

    dst_fd = iot_fs_open(dst_path, IOT_FS_WB);
    if (!dst_fd) {
        iot_free(dst_buf);
        return GZIP_ERR_FILE;
    }

    if (iot_fs_write(dst_fd, dst_buf, out_len) != (int32_t)out_len) {
        iot_fs_close(dst_fd);
        iot_free(dst_buf);
        return GZIP_ERR_FILE;
    }

    iot_fs_close(dst_fd);
    iot_free(dst_buf);
    return GZIP_OK;
}

int gzip_compress_file(const char *src_path, const char *dst_path, int level)
{
    iot_fs_file_t src_fd;
    int32_t file_size;
    uint8_t *src_buf;
    uint8_t *dst_buf;
    size_t dst_size;
    int ret;
    iot_fs_file_t dst_fd;

    if (!src_path || !dst_path) {
        return GZIP_ERR_FORMAT;
    }

    src_fd = iot_fs_open(src_path, IOT_FS_RB);
    if (!src_fd) {
        return GZIP_ERR_FILE;
    }

    file_size = iot_fs_filesize(src_path);
    if (file_size <= 0) {
        iot_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }

    src_buf = (uint8_t *)iot_malloc((size_t)file_size);
    if (!src_buf) {
        iot_fs_close(src_fd);
        return GZIP_ERR_MEM;
    }

    if (iot_fs_read(src_fd, src_buf, (size_t)file_size) != file_size) {
        iot_free(src_buf);
        iot_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }
    iot_fs_close(src_fd);

    dst_size = gzip_compress_bound_size((size_t)file_size);
    dst_buf = (uint8_t *)iot_malloc(dst_size);
    if (!dst_buf) {
        iot_free(src_buf);
        return GZIP_ERR_MEM;
    }

    ret = gzip_compress(src_buf, (size_t)file_size, dst_buf, &dst_size, level);
    iot_free(src_buf);
    if (ret != GZIP_OK) {
        iot_free(dst_buf);
        return ret;
    }

    dst_fd = iot_fs_open(dst_path, IOT_FS_WB);
    if (!dst_fd) {
        iot_free(dst_buf);
        return GZIP_ERR_FILE;
    }

    if (iot_fs_write(dst_fd, dst_buf, dst_size) != (int32_t)dst_size) {
        iot_fs_close(dst_fd);
        iot_free(dst_buf);
        return GZIP_ERR_FILE;
    }

    iot_fs_close(dst_fd);
    iot_free(dst_buf);
    return GZIP_OK;
}
