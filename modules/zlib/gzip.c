/**
 * @file gzip.c
 * @brief GZIP压缩格式实现
 *
 * 本文件实现GZIP格式的压缩和解压功能。
 * GZIP格式在DEFLATE压缩数据前后添加了固定的头部和尾部，
 * 包含文件名、时间戳、CRC32校验等信息。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "gzip.h"
#include "deflate.h"
#include "adapter.h"
#include <string.h>

/* GZIP格式常量定义 */
#define GZIP_MAGIC1          0x1F    /**< GZIP魔数高字节 */
#define GZIP_MAGIC2          0x8B    /**< GZIP魔数低字节 */
#define GZIP_METHOD_DEFLATE  0x08    /**< 压缩方法：DEFLATE */
#define GZIP_FLAG_FEXTRA     0x04    /**< 标志：包含额外字段 */
#define GZIP_FLAG_FNAME      0x08    /**< 标志：包含原始文件名 */
#define GZIP_FLAG_FCOMMENT   0x10    /**< 标志：包含注释 */

/**
 * @brief 解压GZIP格式数据
 *
 * 解析GZIP头部，跳过可选字段，调用DEFLATE解压核心，
 * 最后验证CRC32校验码。
 *
 * @param src 压缩数据缓冲区
 * @param src_len 压缩数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为解压后实际数据大小
 * @return GZIP_OK成功，其他值失败
 */
int gzip_decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len) {
    if (!src || !dst || !dst_len || src_len < 18) {
        return GZIP_ERR_FORMAT;
    }
    
    const uint8_t *p = src;
    
    /* 验证GZIP魔数和方法 */
    if (p[0] != GZIP_MAGIC1 || p[1] != GZIP_MAGIC2 || p[2] != GZIP_METHOD_DEFLATE) {
        return GZIP_ERR_FORMAT;
    }
    
    /* 读取FLG标志字节 */
    uint8_t flags = p[3];
    p += 10;  /* 跳过固定头部长度 */
    
    /* 跳过可选字段 */
    if (flags & GZIP_FLAG_FEXTRA) {
        /* FEXTRA字段：2字节长度 + 额外数据 */
        uint16_t extra_len = p[0] | (p[1] << 8);
        p += 2 + extra_len;
    }
    
    if (flags & GZIP_FLAG_FNAME) {
        /* FNAME字段：以NULL结尾的原始文件名 */
        while (*p != '\0' && p < src + src_len) p++;
        p++;
    }
    
    if (flags & GZIP_FLAG_FCOMMENT) {
        /* FCOMMENT字段：以NULL结尾的注释 */
        while (*p != '\0' && p < src + src_len) p++;
        p++;
    }
    
    if (flags & 0x02) {
        /* CRC16字段：2字节CRC16校验 */
        p += 2;
    }
    
    /* 计算压缩数据长度（总长度减去8字节尾部） */
    size_t compressed_len = (src + src_len - 8) - p;
    size_t original_dst_len = *dst_len;
    
    /* 调用DEFLATE核心解压 */
    if (zlib_deflate_decompress(p, compressed_len, dst, *dst_len) < 0) {
        return GZIP_ERR_FORMAT;
    }
    
    /* 读取GZIP尾部：CRC32和原始大小 */
    p = src + src_len - 8;
    uint32_t crc32 = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
    uint32_t orig_size = p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
    
    /* 验证CRC32 */
    uint32_t computed_crc = zlib_crc32(0xffffffff, dst, orig_size) ^ 0xffffffff;
    if (computed_crc != crc32) {
        return GZIP_ERR_CRC;
    }
    
    /* 确保输出缓冲区足够大 */
    if (orig_size > original_dst_len) {
        return GZIP_ERR_MEM;
    }
    
    *dst_len = orig_size;
    return GZIP_OK;
}

/**
 * @brief 压缩数据为GZIP格式
 *
 * 首先计算数据的CRC32校验码，然后调用DEFLATE核心压缩，
 * 最后添加GZIP头部和尾部。
 *
 * @param src 源数据缓冲区
 * @param src_len 源数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为实际压缩数据大小
 * @param level 压缩级别（1-9）
 * @return GZIP_OK成功，其他值失败
 */
int gzip_compress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level) {
    if (!src || !dst || !dst_len) {
        return GZIP_ERR_FORMAT;
    }
    
    /* 估算最大压缩大小 */
    size_t max_compressed_len = zlib_deflate_bound(src_len);
    size_t total_len = 10 + max_compressed_len + 8;
    
    if (*dst_len < total_len) {
        *dst_len = total_len;
        return GZIP_ERR_MEM;
    }
    
    uint8_t *p = dst;
    
    /* 写入GZIP头部（10字节） */
    p[0] = GZIP_MAGIC1;
    p[1] = GZIP_MAGIC2;
    p[2] = GZIP_METHOD_DEFLATE;
    p[3] = 0;                     /* FLG：无可选字段 */
    p[4] = 0;                    /* MTIME：修改时间 */
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 0;                    /* XFL：额外标志 */
    p[9] = 0x03;                 /* OS：0x03=Unix */
    p += 10;
    
    /* 计算CRC32校验码 */
    uint32_t crc = zlib_crc32(0xffffffff, src, src_len) ^ 0xffffffff;
    
    /* 压缩数据 */
    size_t compressed_len = *dst_len - 18;
    int ret = zlib_deflate_compress(src, src_len, p, &compressed_len, level);
    if (ret != ZLIB_DEFLATE_OK) {
        return GZIP_ERR_MEM;
    }
    p += compressed_len;
    
    /* 写入CRC32（4字节） */
    *p++ = crc & 0xff;
    *p++ = (crc >> 8) & 0xff;
    *p++ = (crc >> 16) & 0xff;
    *p++ = (crc >> 24) & 0xff;
    
    /* 写入原始大小（4字节） */
    uint32_t size = (uint32_t)src_len;
    *p++ = size & 0xff;
    *p++ = (size >> 8) & 0xff;
    *p++ = (size >> 16) & 0xff;
    *p++ = (size >> 24) & 0xff;
    
    *dst_len = p - dst;
    return GZIP_OK;
}

/**
 * @brief 解压GZIP文件
 *
 * 读取整个GZIP文件到内存，然后调用gzip_decompress进行解压，
 * 最后将解压后的数据写入目标文件。
 *
 * @param src_path 源GZIP文件路径
 * @param dst_path 目标文件路径
 * @return GZIP_OK成功，其他值失败
 */
int gzip_decompress_file(const char *src_path, const char *dst_path) {
    if (!src_path || !dst_path) {
        return GZIP_ERR_FORMAT;
    }
    
    /* 打开源文件 */
    zlib_fs_t src_fd = zlib_fs_open(src_path, ZLIB_FS_RB);
    if (!src_fd) {
        return GZIP_ERR_FILE;
    }
    
    /* 获取文件大小 */
    int32_t file_size = zlib_fs_filesize(src_path);
    if (file_size < 18) {
        zlib_fs_close(src_fd);
        return GZIP_ERR_FORMAT;
    }
    
    /* 分配内存并读取文件 */
    uint8_t *src_buf = (uint8_t *)zlib_malloc(file_size);
    if (!src_buf) {
        zlib_fs_close(src_fd);
        return GZIP_ERR_MEM;
    }
    
    if (zlib_fs_read(src_fd, src_buf, file_size) != file_size) {
        zlib_free(src_buf);
        zlib_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }
    zlib_fs_close(src_fd);
    
    /* 从文件尾部读取预期的解压后大小 */
    const uint8_t *p = src_buf + file_size - 8;
    uint32_t dst_size = p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
    
    /* 分配解压缓冲区 */
    uint8_t *dst_buf = (uint8_t *)zlib_malloc(dst_size);
    if (!dst_buf) {
        zlib_free(src_buf);
        return GZIP_ERR_MEM;
    }
    
    /* 执行解压 */
    size_t out_len = dst_size;
    int ret = gzip_decompress(src_buf, file_size, dst_buf, &out_len);
    if (ret != GZIP_OK) {
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return ret;
    }
    
    /* 写入目标文件 */
    zlib_fs_t dst_fd = zlib_fs_open(dst_path, ZLIB_FS_WB);
    if (!dst_fd) {
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return GZIP_ERR_FILE;
    }
    
    if (zlib_fs_write(dst_fd, dst_buf, dst_size) != dst_size) {
        zlib_fs_close(dst_fd);
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return GZIP_ERR_FILE;
    }
    
    zlib_fs_close(dst_fd);
    zlib_free(dst_buf);
    zlib_free(src_buf);
    
    return GZIP_OK;
}

/**
 * @brief 压缩文件为GZIP格式
 *
 * 读取源文件，使用GZIP格式压缩，然后写入目标文件。
 *
 * @param src_path 源文件路径
 * @param dst_path 目标GZIP文件路径
 * @param level 压缩级别（1-9）
 * @return GZIP_OK成功，其他值失败
 */
int gzip_compress_file(const char *src_path, const char *dst_path, int level) {
    if (!src_path || !dst_path) {
        return GZIP_ERR_FORMAT;
    }
    
    /* 打开源文件 */
    zlib_fs_t src_fd = zlib_fs_open(src_path, ZLIB_FS_RB);
    if (!src_fd) {
        return GZIP_ERR_FILE;
    }
    
    int32_t file_size = zlib_fs_filesize(src_path);
    if (file_size <= 0) {
        zlib_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }
    
    /* 读取源文件数据 */
    uint8_t *src_buf = (uint8_t *)zlib_malloc(file_size);
    if (!src_buf) {
        zlib_fs_close(src_fd);
        return GZIP_ERR_MEM;
    }
    
    if (zlib_fs_read(src_fd, src_buf, file_size) != file_size) {
        zlib_free(src_buf);
        zlib_fs_close(src_fd);
        return GZIP_ERR_FILE;
    }
    zlib_fs_close(src_fd);
    
    /* 分配压缩缓冲区 */
    size_t dst_size = zlib_deflate_bound(file_size) + 18;
    uint8_t *dst_buf = (uint8_t *)zlib_malloc(dst_size);
    if (!dst_buf) {
        zlib_free(src_buf);
        return GZIP_ERR_MEM;
    }
    
    /* 执行压缩 */
    int ret = gzip_compress(src_buf, file_size, dst_buf, &dst_size, level);
    if (ret != GZIP_OK) {
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return ret;
    }
    
    /* 写入目标文件 */
    zlib_fs_t dst_fd = zlib_fs_open(dst_path, ZLIB_FS_WB);
    if (!dst_fd) {
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return GZIP_ERR_FILE;
    }
    
    if (zlib_fs_write(dst_fd, dst_buf, dst_size) != (int32_t)dst_size) {
        zlib_fs_close(dst_fd);
        zlib_free(dst_buf);
        zlib_free(src_buf);
        return GZIP_ERR_FILE;
    }
    
    zlib_fs_close(dst_fd);
    zlib_free(dst_buf);
    zlib_free(src_buf);
    
    return GZIP_OK;
}
