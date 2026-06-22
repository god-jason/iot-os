/**
 * @file http_gzip.h
 * @brief HTTP Gzip 压缩支持
 *
 * 提供 HTTP 请求的 gzip 压缩和响应解压功能
 */
#ifndef IOT_HTTP_GZIP_H
#define IOT_HTTP_GZIP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

typedef struct http_gzip_ctx http_gzip_ctx_t;

/*===========================================================
 * Gzip 上下文操作
 *===========================================================*/

/**
 * @brief 创建 gzip 解压上下文
 * @return 解压上下文，失败返回 NULL
 */
http_gzip_ctx_t* http_gzip_create(void);

/**
 * @brief 销毁 gzip 解压上下文
 * @param ctx gzip 上下文
 */
void http_gzip_destroy(http_gzip_ctx_t* ctx);

/**
 * @brief 重置 gzip 解压上下文
 * @param ctx gzip 上下文
 * @return 0 成功，-1 失败
 */
int http_gzip_reset(http_gzip_ctx_t* ctx);

/*===========================================================
 * Gzip 解压接口
 *===========================================================*/

/**
 * @brief 解压 gzip 数据
 * @param ctx gzip 上下文
 * @param src 压缩数据
 * @param src_len 压缩数据长度
 * @param dst 输出缓冲区
 * @param dst_cap 输出缓冲区容量
 * @param produced 输出实际解压数据大小
 * @return 0 成功，-1 失败
 */
int http_gzip_decompress(http_gzip_ctx_t* ctx, const uint8_t* src, size_t src_len,
                        uint8_t* dst, size_t dst_cap, size_t* produced);

/**
 * @brief 解压 gzip 数据（分配内存版本）
 * @param src 压缩数据
 * @param src_len 压缩数据长度
 * @param out_len 输出解压后数据长度
 * @return 解压后数据，失败返回 NULL，使用完后需 free
 */
uint8_t* http_gzip_decompress_alloc(const uint8_t* src, size_t src_len, size_t* out_len);

/*===========================================================
 * Gzip 压缩接口
 *===========================================================*/

/**
 * @brief 压缩数据为 gzip 格式
 * @param src 源数据
 * @param src_len 源数据长度
 * @param dst 输出缓冲区
 * @param dst_cap 输出缓冲区容量
 * @param level 压缩级别（1-9）
 * @return 压缩后数据大小，失败返回 -1
 */
int http_gzip_compress(const uint8_t* src, size_t src_len, uint8_t* dst, size_t dst_cap, int level);

/**
 * @brief 压缩数据为 gzip 格式（分配内存版本）
 * @param src 源数据
 * @param src_len 源数据长度
 * @param level 压缩级别（1-9）
 * @param out_len 输出压缩后数据长度
 * @return 压缩后数据，失败返回 NULL，使用完后需 free
 */
uint8_t* http_gzip_compress_alloc(const uint8_t* src, size_t src_len, int level, size_t* out_len);

/*===========================================================
 * HTTP 头处理
 *===========================================================*/

/**
 * @brief 检查响应是否使用 gzip 压缩
 * @param header 响应头
 * @return true 使用 gzip，false 未使用
 */
bool http_gzip_check_response(const char* header);

/**
 * @brief 构建 gzip 请求头
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区大小
 * @return 0 成功，-1 失败
 */
int http_gzip_build_request_header(char* buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_GZIP_H */