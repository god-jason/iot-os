/**
 * @file deflate.h
 * @brief DEFLATE压缩算法头文件
 *
 * 定义ADLER32校验、CRC32校验和DEFLATE压缩/解压接口。
 * DEFLATE是ZIP、GZIP等格式的核心压缩算法，结合了LZ77和Huffman编码。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef ZLIB_DEFLATE_H
#define ZLIB_DEFLATE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ADLER32算法常量定义 */
#define ZLIB_ADLER32_BASE    65521   /**< ADLER32算法的模数（素数） */
#define ZLIB_ADLER32_NMAX    5552    /**< ADLER32每次处理的最大字节数 */

/**
 * @brief 计算数据的ADLER32校验码
 * @param adler 初始校验值（通常为1）
 * @param buf 数据缓冲区指针
 * @param len 数据长度（字节数）
 * @return 计算后的校验码
 *
 * ADLER32是一种快速校验算法，比CRC32快但可靠性略低，
 * 常用于zlib压缩格式的数据校验。
 */
uint32_t zlib_adler32(uint32_t adler, const uint8_t *buf, size_t len);

/**
 * @brief 计算数据的CRC32校验码
 * @param crc 初始校验值（通常为0xFFFFFFFF）
 * @param buf 数据缓冲区指针
 * @param len 数据长度（字节数）
 * @return 计算后的校验码（会进行最终翻转）
 *
 * CRC32是一种可靠的循环冗余校验算法，
 * 用于ZIP、GZIP等格式的数据完整性校验。
 */
uint32_t zlib_crc32(uint32_t crc, const uint8_t *buf, size_t len);

/* DEFLATE错误码定义 */
#define ZLIB_DEFLATE_OK          0   /**< 成功 */
#define ZLIB_DEFLATE_ERR_MEM    -1   /**< 内存分配失败 */
#define ZLIB_DEFLATE_ERR_BUF    -2   /**< 缓冲区空间不足 */

/* 压缩级别定义 */
#define ZLIB_DEFLATE_LEVEL_FAST    1   /**< 快速压缩级别 */
#define ZLIB_DEFLATE_LEVEL_DEFAULT 6   /**< 默认压缩级别（平衡速度和压缩率） */
#define ZLIB_DEFLATE_LEVEL_BEST   9   /**< 最佳压缩级别（最高压缩率） */

/**
 * @brief 计算压缩后的最大可能大小
 * @param src_len 源数据长度
 * @return 压缩后最大可能的字节数
 *
 * 用于在压缩前估算所需的目标缓冲区大小，
 * 避免压缩过程中缓冲区溢出。
 */
size_t zlib_deflate_bound(size_t src_len);

/**
 * @brief 使用DEFLATE算法压缩数据
 * @param src 源数据缓冲区
 * @param src_len 源数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为实际压缩数据大小
 * @param level 压缩级别（1-9）
 * @return ZLIB_DEFLATE_OK成功，其他值失败
 *
 * 使用DEFLATE算法（LZ77+Huffman编码）对数据进行压缩。
 */
int zlib_deflate_compress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level);

/**
 * @brief 使用DEFLATE算法解压数据
 * @param src 压缩数据缓冲区
 * @param src_len 压缩数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 目标缓冲区大小
 * @return 正数解压后实际数据大小，负数表示失败
 */
int zlib_deflate_decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len);

#ifdef __cplusplus
}
#endif

#endif
