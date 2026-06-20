/**
 * @file url.h
 * @brief URL 解析和操作库
 *
 * 提供 URL 解析、编码、解码和构建功能
 */
#ifndef IOT_URL_H
#define IOT_URL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* URL 组件结构 */
typedef struct {
    char* scheme;       /* 协议方案 (如 http, https) */
    char* host;         /* 主机名 */
    int port;           /* 端口号 */
    char* path;         /* 路径 */
    char* query;        /* 查询字符串 */
    char* fragment;     /* 片段标识符 */
    char* username;     /* 用户名 */
    char* password;     /* 密码 */
} iot_url_t;

/* URL 编码选项 */
typedef struct {
    int encode_space_as_plus;  /* 空格编码为 + 号 (application/x-www-form-urlencoded) */
    int encode uppercase_hex;  /* 十六进制字符大写 */
} iot_url_encode_opt_t;

/* 默认编码选项 */
#define URL_ENCODE_DEFAULT {0, 1}

/*===========================================================
 * URL 解析接口
 *===========================================================*/

/**
 * @brief 解析 URL 字符串
 * @param url_str URL 字符串
 * @return URL 结构体指针，失败返回 NULL
 * @note 返回的指针需要用 iot_url_free 释放
 */
iot_url_t* iot_url_parse(const char* url_str);

/**
 * @brief 释放 URL 结构体
 * @param url URL 结构体指针
 */
void iot_url_free(iot_url_t* url);

/**
 * @brief 复制 URL 结构体
 * @param url 原始 URL 结构体
 * @return 复制后的 URL 结构体，失败返回 NULL
 */
iot_url_t* iot_url_copy(const iot_url_t* url);

/*===========================================================
 * URL 构建接口
 *===========================================================*/

/**
 * @brief 从组件构建 URL 字符串
 * @param url URL 结构体
 * @return 分配的 URL 字符串，失败返回 NULL
 * @note 返回的指针需要用 iot_free 释放
 */
char* iot_url_build(const iot_url_t* url);

/**
 * @brief 获取默认端口
 * @param scheme 协议方案
 * @return 默认端口号，未知协议返回 0
 */
int iot_url_default_port(const char* scheme);

/**
 * @brief 检查 URL 是否为绝对路径
 * @param url URL 结构体
 * @return true 是绝对路径，false 不是
 */
bool iot_url_is_absolute(const iot_url_t* url);

/*===========================================================
 * URL 编码/解码接口
 *===========================================================*/

/**
 * @brief URL 编码 (percent encoding)
 * @param str 待编码字符串
 * @param len 字符串长度，-1 表示自动计算
 * @param opt 编码选项，NULL 使用默认选项
 * @return 编码后的字符串，失败返回 NULL
 * @note 返回的指针需要用 iot_free 释放
 */
char* iot_url_encode(const char* str, int len, const iot_url_encode_opt_t* opt);

/**
 * @brief URL 解码 (percent decoding)
 * @param str 待解码字符串
 * @param len 字符串长度，-1 表示自动计算
 * @param opt 解码选项，NULL 使用默认选项
 * @return 解码后的字符串，失败返回 NULL
 * @note 返回的指针需要用 iot_free 释放
 */
char* iot_url_decode(const char* str, int len, const iot_url_encode_opt_t* opt);

/**
 * @brief 查询参数编码 (application/x-www-form-urlencoded)
 * @param str 待编码字符串
 * @return 编码后的字符串，失败返回 NULL
 * @note 返回的指针需要用 iot_free 释放
 */
char* iot_url_encode_query(const char* str);

/**
 * @brief 查询参数解码
 * @param str 待解码字符串
 * @return 解码后的字符串，失败返回 NULL
 * @note 返回的指针需要用 iot_free 释放
 */
char* iot_url_decode_query(const char* str);

#ifdef __cplusplus
}
#endif

#endif /* IOT_URL_H */
