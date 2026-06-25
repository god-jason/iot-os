/**
 * @file http_gzip.c
 * @brief HTTP Gzip 压缩支持（基于 iot_zlib/miniz）
 */

#include "http_gzip.h"
#include "../zlib/gzip.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) && !defined(strncasecmp)
#define strncasecmp _strnicmp
#endif

struct http_gzip_ctx {
    int initialized;
};

http_gzip_ctx_t* http_gzip_create(void)
{
    http_gzip_ctx_t* ctx = (http_gzip_ctx_t*)malloc(sizeof(http_gzip_ctx_t));
    if (!ctx) {
        return NULL;
    }
    ctx->initialized = 1;
    return ctx;
}

void http_gzip_destroy(http_gzip_ctx_t* ctx)
{
    free(ctx);
}

int http_gzip_reset(http_gzip_ctx_t* ctx)
{
    if (!ctx) {
        return -1;
    }
    ctx->initialized = 1;
    return 0;
}

int http_gzip_decompress(http_gzip_ctx_t* ctx, const uint8_t* src, size_t src_len,
                         uint8_t* dst, size_t dst_cap, size_t* produced)
{
    size_t out_len;

    if (!ctx || !src || !dst) {
        return -1;
    }

    out_len = dst_cap;
    if (gzip_decompress(src, src_len, dst, &out_len) != GZIP_OK) {
        return -1;
    }

    if (produced) {
        *produced = out_len;
    }
    return 0;
}

uint8_t* http_gzip_decompress_alloc(const uint8_t* src, size_t src_len, size_t* out_len)
{
    size_t bound;
    uint8_t* dst;
    size_t dst_len;

    if (!src || src_len == 0) {
        return NULL;
    }

    bound = src_len * 4;
    if (bound < 4096) {
        bound = 4096;
    }

    dst = (uint8_t*)malloc(bound);
    if (!dst) {
        return NULL;
    }

    dst_len = bound;
    if (gzip_decompress(src, src_len, dst, &dst_len) != GZIP_OK) {
        free(dst);
        return NULL;
    }

    if (out_len) {
        *out_len = dst_len;
    }

    {
        uint8_t* result = (uint8_t*)realloc(dst, dst_len);
        return result ? result : dst;
    }
}

int http_gzip_compress(const uint8_t* src, size_t src_len, uint8_t* dst, size_t dst_cap, int level)
{
    size_t dst_len;

    if (!src || !dst || src_len == 0 || dst_cap == 0) {
        return -1;
    }

    dst_len = dst_cap;
    if (gzip_compress(src, src_len, dst, &dst_len, level) != GZIP_OK) {
        return -1;
    }

    return (int)dst_len;
}

uint8_t* http_gzip_compress_alloc(const uint8_t* src, size_t src_len, int level, size_t* out_len)
{
    size_t bound;
    uint8_t* dst;
    size_t dst_len;

    if (!src || src_len == 0) {
        return NULL;
    }

    bound = src_len * 4 + 128;
    dst = (uint8_t*)malloc(bound);
    if (!dst) {
        return NULL;
    }

    dst_len = bound;
    if (gzip_compress(src, src_len, dst, &dst_len, level) != GZIP_OK) {
        free(dst);
        return NULL;
    }

    if (out_len) {
        *out_len = dst_len;
    }

    {
        uint8_t* result = (uint8_t*)realloc(dst, dst_len);
        return result ? result : dst;
    }
}

bool http_gzip_check_response(const char* header)
{
    const char* p;

    if (!header) {
        return false;
    }

    p = header;
    while (*p) {
        const char* line_end = strstr(p, "\r\n");
        if (!line_end) {
            line_end = p + strlen(p);
        }

        if ((size_t)(line_end - p) >= 17 &&
            strncasecmp(p, "Content-Encoding:", 17) == 0) {
            const char* encoding = p + 17;
            while (*encoding == ' ' || *encoding == '\t') {
                encoding++;
            }
            return (strncasecmp(encoding, "gzip", 4) == 0);
        }

        if (*line_end == '\0') {
            break;
        }
        p = line_end + 2;
    }

    return false;
}

int http_gzip_build_request_header(char* buf, size_t buf_len)
{
    if (!buf || buf_len == 0) {
        return -1;
    }

    snprintf(buf, buf_len, "Accept-Encoding: gzip, deflate\r\n");
    return 0;
}
