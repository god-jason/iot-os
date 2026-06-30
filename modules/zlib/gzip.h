/**
 * @file gzip.h
 * @brief GZIP压缩格式头文件
 *
 * 定义GZIP格式的压缩和解压接口。
 * GZIP是基于DEFLATE算法的通用压缩格式，常用于文件压缩和网络传输。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef GZIP_H
#define GZIP_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GZIP错误码定义 */
#define GZIP_OK              0   /**< 成功 */
#define GZIP_ERR_MEM        -1   /**< 内存分配失败 */
#define GZIP_ERR_FORMAT     -2   /**< GZIP格式错误 */
#define GZIP_ERR_CRC        -3   /**< CRC校验失败 */
#define GZIP_ERR_FILE       -4   /**< 文件操作失败 */

/* GZIP压缩级别定义 */
#define GZIP_COMPRESS_FAST    1   /**< 快速压缩 */
#define GZIP_COMPRESS_DEFAULT 6   /**< 默认压缩 */
#define GZIP_COMPRESS_BEST   9   /**< 最佳压缩 */

/**
 * @brief 解压GZIP格式数据
 * @param src 压缩数据缓冲区
 * @param src_len 压缩数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为解压后实际数据大小
 * @return GZIP_OK成功，其他值失败
 *
 * 解压标准GZIP格式数据，支持带或不带文件名、注释等扩展字段。
 */
int gzip_decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len);

/**
 * @brief 压缩数据为GZIP格式
 * @param src 源数据缓冲区
 * @param src_len 源数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为实际压缩数据大小
 * @param level 压缩级别（1-9）
 * @return GZIP_OK成功，其他值失败
 */
int gzip_compress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level);

/**
 * @brief 估算 GZIP 压缩输出缓冲区大小
 */
size_t gzip_compress_bound(size_t src_len);

/**
 * @brief 解压GZIP文件
 * @param src_path 源GZIP文件路径
 * @param dst_path 目标文件路径
 * @return GZIP_OK成功，其他值失败
 *
 * 读取整个GZIP文件到内存，解压后保存到目标文件。
 */
int gzip_decompress_file(const char *src_path, const char *dst_path);

/**
 * @brief 压缩文件为GZIP格式
 * @param src_path 源文件路径
 * @param dst_path 目标GZIP文件路径
 * @param level 压缩级别（1-9）
 * @return GZIP_OK成功，其他值失败
 */
int gzip_compress_file(const char *src_path, const char *dst_path, int level);

#ifdef __cplusplus
}
#endif

#endif
