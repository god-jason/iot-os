/**
 * @file http_gzip.c
 * @brief HTTP Gzip 压缩支持实现
 */

#include "http_gzip.h"
#include "../zlib/zlib.h"
#include "../zlib/gzip.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*===========================================================
 * 内部结构
 *===========================================================*/

struct http_gzip_ctx {
    void* zstream;
    uint8_t* pending_buf;
    size_t pending_len;
    size_t pending_cap;
    int initialized;
    int finished;
};

/*===========================================================
 * zlib 适配层接口
 *===========================================================*/

typedef struct {
    void* opaque;
    void* (*alloc)(void*, unsigned, unsigned);
    void (*free)(void*, void*);
} zlib_alloc_func;

typedef struct {
    int (*init)(void* strm, int level);
    int (*deflate)(void* strm, int flush);
    int (*inflate)(void* strm, int flush);
    int (*end)(void* strm);
    const char* (*msg)(void* strm);
} zlib_funcs;

typedef struct {
    void* strm;
    zlib_funcs funcs;
    uint8_t* next_in;
    uint8_t* next_out;
    unsigned avail_in;
    unsigned avail_out;
} z_stream_t;

static int zlib_inflate_init(void** strm, int windowbits) {
    (void)windowbits;
    
    z_stream_t* zs = (z_stream_t*)malloc(sizeof(z_stream_t));
    if (!zs) return -1;
    
    memset(zs, 0, sizeof(z_stream_t));
    
    int ret = zlib_deflate_decompress(NULL, 0, NULL, 0);
    (void)ret;
    
    zs->strm = malloc(128);
    if (!zs->strm) {
        free(zs);
        return -1;
    }
    memset(zs->strm, 0, 128);
    
    *strm = zs;
    return 0;
}

static int zlib_inflate(void* strm, int flush) {
    z_stream_t* zs = (z_stream_t*)strm;
    if (!zs) return -1;
    
    size_t src_len = zs->avail_in;
    size_t dst_len = zs->avail_out;
    
    int ret = zlib_deflate_decompress(zs->next_in, src_len, zs->next_out, dst_len);
    
    if (ret < 0) {
        return ret;
    }
    
    zs->next_in += src_len - zs->avail_in;
    zs->avail_in = 0;
    zs->next_out += ret;
    zs->avail_out -= ret;
    
    return 0;
}

static int zlib_inflate_end(void* strm) {
    z_stream_t* zs = (z_stream_t*)strm;
    if (!zs) return -1;
    
    if (zs->strm) free(zs->strm);
    free(zs);
    return 0;
}

static int zlib_deflate_init(void** strm, int level) {
    (void)level;
    
    z_stream_t* zs = (z_stream_t*)malloc(sizeof(z_stream_t));
    if (!zs) return -1;
    
    memset(zs, 0, sizeof(z_stream_t));
    
    zs->strm = malloc(128);
    if (!zs->strm) {
        free(zs);
        return -1;
    }
    memset(zs->strm, 0, 128);
    
    *strm = zs;
    return 0;
}

static int http_zlib_deflate_compress(void* strm, const uint8_t* src, size_t src_len,
                                  uint8_t* dst, size_t dst_cap, int level, size_t* produced) {
    size_t dst_len = dst_cap;
    (void)strm;
    int ret = zlib_deflate_compress(src, src_len, dst, &dst_len, level);
    
    if (ret == 0) {
        if (produced) *produced = dst_len;
        return 0;
    }
    return ret;
}

static int zlib_deflate_end(void* strm) {
    z_stream_t* zs = (z_stream_t*)strm;
    if (!zs) return -1;
    
    if (zs->strm) free(zs->strm);
    free(zs);
    return 0;
}

/*===========================================================
 * Gzip 上下文操作
 *===========================================================*/

http_gzip_ctx_t* http_gzip_create(void) {
    http_gzip_ctx_t* ctx = (http_gzip_ctx_t*)malloc(sizeof(http_gzip_ctx_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(http_gzip_ctx_t));
    
    if (zlib_inflate_init(&ctx->zstream, 15 + 32) != 0) {
        free(ctx);
        return NULL;
    }
    
    ctx->pending_cap = 4096;
    ctx->pending_buf = (uint8_t*)malloc(ctx->pending_cap);
    if (!ctx->pending_buf) {
        zlib_inflate_end(ctx->zstream);
        free(ctx);
        return NULL;
    }
    
    ctx->initialized = 1;
    return ctx;
}

void http_gzip_destroy(http_gzip_ctx_t* ctx) {
    if (!ctx) return;
    
    if (ctx->zstream) {
        zlib_inflate_end(ctx->zstream);
    }
    if (ctx->pending_buf) {
        free(ctx->pending_buf);
    }
    free(ctx);
}

int http_gzip_reset(http_gzip_ctx_t* ctx) {
    if (!ctx) return -1;
    
    if (ctx->zstream) {
        zlib_inflate_end(ctx->zstream);
    }
    
    if (zlib_inflate_init(&ctx->zstream, 15 + 32) != 0) {
        return -1;
    }
    
    ctx->pending_len = 0;
    ctx->finished = 0;
    
    return 0;
}

/*===========================================================
 * Gzip 解压
 *===========================================================*/

int http_gzip_decompress(http_gzip_ctx_t* ctx, const uint8_t* src, size_t src_len,
                        uint8_t* dst, size_t dst_cap, size_t* produced) {
    if (!ctx || !src || !dst) return -1;
    
    if (produced) *produced = 0;
    
    z_stream_t* zs = (z_stream_t*)ctx->zstream;
    if (!zs) return -1;
    
    zs->next_in = (uint8_t*)src;
    zs->avail_in = (unsigned)src_len;
    zs->next_out = dst;
    zs->avail_out = (unsigned)dst_cap;
    
    int ret = zlib_inflate(zs, 0);
    
    if (produced) {
        *produced = dst_cap - zs->avail_out;
    }
    
    return (ret == 0 || zs->avail_out == 0) ? 0 : -1;
}

uint8_t* http_gzip_decompress_alloc(const uint8_t* src, size_t src_len, size_t* out_len) {
    if (!src || src_len == 0) return NULL;
    
    size_t bound = src_len * 4;
    if (bound < 4096) bound = 4096;
    
    uint8_t* dst = (uint8_t*)malloc(bound);
    if (!dst) return NULL;
    
    size_t dst_len = bound;
    int ret = gzip_decompress(src, src_len, dst, &dst_len);
    
    if (ret != 0) {
        free(dst);
        return NULL;
    }
    
    if (out_len) *out_len = dst_len;
    
    uint8_t* result = (uint8_t*)realloc(dst, dst_len);
    return result ? result : dst;
}

/*===========================================================
 * Gzip 压缩
 *===========================================================*/

int http_gzip_compress(const uint8_t* src, size_t src_len, uint8_t* dst, size_t dst_cap, int level) {
    if (!src || !dst || src_len == 0 || dst_cap == 0) return -1;
    
    size_t dst_len = dst_cap;
    int ret = gzip_compress(src, src_len, dst, &dst_len, level);
    
    return (ret == 0) ? (int)dst_len : -1;
}

uint8_t* http_gzip_compress_alloc(const uint8_t* src, size_t src_len, int level, size_t* out_len) {
    if (!src || src_len == 0) return NULL;
    
    size_t bound = zlib_deflate_bound(src_len) + 64;
    
    uint8_t* dst = (uint8_t*)malloc(bound);
    if (!dst) return NULL;
    
    size_t dst_len = bound;
    int ret = gzip_compress(src, src_len, dst, &dst_len, level);
    
    if (ret != 0) {
        free(dst);
        return NULL;
    }
    
    if (out_len) *out_len = dst_len;
    
    uint8_t* result = (uint8_t*)realloc(dst, dst_len);
    return result ? result : dst;
}

/*===========================================================
 * HTTP 头处理
 *===========================================================*/

bool http_gzip_check_response(const char* header) {
    if (!header) return false;
    
    const char* encoding = strstr(header, "Content-Encoding:");
    if (!encoding) return false;
    
    encoding = strchr(encoding, ':');
    if (!encoding) return false;
    encoding++;
    
    while (*encoding == ' ' || *encoding == '\t') encoding++;
    
    return (strncmp(encoding, "gzip", 4) == 0);
}

int http_gzip_build_request_header(char* buf, size_t buf_len) {
    if (!buf || buf_len == 0) return -1;
    
    snprintf(buf, buf_len, "Accept-Encoding: gzip, deflate\r\n");
    return 0;
}