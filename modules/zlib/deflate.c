/**
 * @file deflate.c
 * @brief DEFLATE 压缩/解压实现（miniz 封装）
 *
 * 基于 miniz 库实现 DEFLATE 格式的压缩和解压功能
 * 提供原始 DEFLATE 数据压缩和解压的便捷接口
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "deflate.h"
#include "miniz.h"

size_t iot_deflate_compress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_cap)
{
    mz_ulong bound;
    mz_ulong dest_len;

    if (!in || !out || in_len == 0 || out_cap == 0) {
        return 0;
    }

    bound = mz_compressBound((mz_ulong)in_len);
    if (out_cap < bound) {
        return 0;
    }

    dest_len = (mz_ulong)out_cap;
    if (mz_compress2(out, &dest_len, in, (mz_ulong)in_len, MZ_DEFAULT_COMPRESSION) != MZ_OK) {
        return 0;
    }

    return (size_t)dest_len;
}

size_t iot_inflate_decompress(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_cap)
{
    mz_ulong dest_len;

    if (!in || !out || in_len == 0 || out_cap == 0) {
        return 0;
    }

    dest_len = (mz_ulong)out_cap;
    if (mz_uncompress(out, &dest_len, in, (mz_ulong)in_len) != MZ_OK) {
        return 0;
    }

    return (size_t)dest_len;
}
