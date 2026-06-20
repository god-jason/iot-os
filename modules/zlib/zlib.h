/**
 * @file zlib.h
 * @brief zlib统一入口头文件
 *
 * 本文件整合了所有zlib相关模块的接口，包括：
 * - DEFLATE压缩/解压
 * - GZIP压缩/解压
 * - ZIP归档文件解压
 * - TAR归档文件打包/解包
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef ZLIB_H
#define ZLIB_H

#include "deflate.h"
#include "zip.h"
#include "gzip.h"
#include "tar.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 版本信息 */
#define ZLIB_VERSION_MAJOR    1
#define ZLIB_VERSION_MINOR    0
#define ZLIB_VERSION_PATCH    0
#define ZLIB_VERSION          "1.0.0"

/* 通用错误码 */
#define ZLIB_OK               0   /**< 成功 */
#define ZLIB_ERR_MEM         -1   /**< 内存分配失败 */
#define ZLIB_ERR_BUF         -2   /**< 缓冲区空间不足 */
#define ZLIB_ERR_FORMAT      -3   /**< 格式错误 */
#define ZLIB_ERR_CRC         -4   /**< CRC校验失败 */
#define ZLIB_ERR_FILE        -5   /**< 文件操作失败 */
#define ZLIB_ERR_NOT_FOUND   -6   /**< 文件/条目未找到 */

#ifdef __cplusplus
}
#endif

#endif
