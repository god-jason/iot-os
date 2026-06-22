/**
 * @file http_url.h
 * @brief HTTP URL 解析与编码工具
 *
 * 提供 URL 解析、URL 编码/解码等功能
 */
#ifndef IOT_HTTP_URL_H
#define IOT_HTTP_URL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

typedef struct {
    char scheme[16];      /* 协议，如 "http"、"https" */
    char host[256];      /* 主机名 */
    uint16_t port;       /* 端口号 */
    char path[512];      /* 路径 */
    char query[512];     /* 查询参数 */
    char fragment[128];  /* 锚点 */
} http_url_t;

/*===========================================================
 * URL 解析接口
 *===========================================================*/

/**
 * @brief 解析 URL 字符串
 * @param url URL 字符串
 * @param parsed 输出参数，解析结果
 * @return 0 成功，-1 失败
 */
int http_url_parse(const char* url, http_url_t* parsed);

/**
 * @brief 从 URL 构建查询参数
 * @param url URL 字符串
 * @return 查询参数字符串，失败或无查询参数返回 NULL，使用完后需 free
 */
char* http_url_get_query(const char* url);

/**
 * @brief 根据键名获取查询参数值
 * @param query 查询参数字符串
 * @param key 键名
 * @return 值字符串，未找到返回 NULL，使用完后需 free
 */
char* http_url_get_param(const char* query, const char* key);

/**
 * @brief 获取 URL 编码后的字符串长度
 * @param str 原始字符串
 * @return 编码后字符串长度
 */
size_t http_url_encode_len(const char* str);

/**
 * @brief URL 编码
 * @param src 原始字符串
 * @param dst 输出缓冲区
 * @param dst_len 缓冲区大小
 * @return 编码后字符串，失败返回 NULL
 */
char* http_url_encode(const char* src, char* dst, size_t dst_len);

/**
 * @brief URL 编码（分配内存版本）
 * @param str 原始字符串
 * @return 编码后字符串，失败返回 NULL，使用完后需 free
 */
char* http_url_encode_alloc(const char* str);

/**
 * @brief 获取 URL 解码后的字符串长度
 * @param str 编码后的字符串
 * @return 解码后字符串长度
 */
size_t http_url_decode_len(const char* str);

/**
 * @brief URL 解码
 * @param src 编码后的字符串
 * @param dst 输出缓冲区
 * @param dst_len 缓冲区大小
 * @return 解码后字符串，失败返回 NULL
 */
char* http_url_decode(const char* src, char* dst, size_t dst_len);

/**
 * @brief URL 解码（分配内存版本）
 * @param str 编码后的字符串
 * @return 解码后字符串，失败返回 NULL，使用完后需 free
 */
char* http_url_decode_alloc(const char* str);

/**
 * @brief 构建 URL 字符串
 * @param url 输出缓冲区
 * @param url_len 缓冲区大小
 * @param scheme 协议
 * @param host 主机名
 * @param port 端口（0 表示使用默认端口）
 * @param path 路径
 * @param query 查询参数（可选）
 * @return 0 成功，-1 失败
 */
int http_url_build(char* url, size_t url_len, const char* scheme, const char* host,
                  uint16_t port, const char* path, const char* query);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_URL_H */