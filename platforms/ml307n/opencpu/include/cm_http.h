/**
 * @file        cm_http_client.h
 * @brief       HTTP协议客户端接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By chenxy
 * @date        2021/05/21
 *
 * @defgroup http http
 * @ingroup NP
 * @{
 */


#ifndef __CM_HTTP_H__
#define __CM_HTTP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup httpclient_define Define
  * @{
  */

#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

#ifndef bool
#define bool int
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/**HTTP默认端口*/
#define HTTP_DEFAULT_PORT   80

/**HTTPS默认端口*/
#define HTTPS_DEFAULT_PORT 443

/**HTTP可创建实例个数*/
#define HTTPCLIENT_CTX_MAX_NUM                    4

/**HTTP连接超时最大时间*/
#define HTTPCLIENT_CONNECT_TIMEOUT_MAXTIME        180

/**HTTP请求响应超时最大时间*/
#define HTTPCLIENT_WAITRSP_TIMEOUT_MAXTIME        60

/**HTTP连接超时默认时间*/
#define HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT        60

/**HTTP请求响应超时默认时间*/
#define HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT        0

/**
 * @}
 */

/** @defgroup httpclient_enum Enum
 *  @{
 */

/**HTTP请求类型*/
typedef enum {
    HTTPCLIENT_REQUEST_NONE = 0,
    HTTPCLIENT_REQUEST_GET,
    HTTPCLIENT_REQUEST_POST,
    HTTPCLIENT_REQUEST_PUT,
    HTTPCLIENT_REQUEST_DELETE,
    HTTPCLIENT_REQUEST_HEAD,
    HTTPCLIENT_REQUEST_MAX
} cm_httpclient_request_type_e;

/**HTTP回调事件*/
typedef enum{
    CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND=1,        /*!< 请求启动成功事件 */
    CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND,              /*!< 接收到报头事件 */
    CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND,             /*!< 接收到消息体事件 */
    CM_HTTP_CALLBACK_EVENT_RSP_END_IND,                 /*!< 请求响应结束事件 */
    CM_HTTP_CALLBACK_EVENT_ERROR_IND,                   /*!< 请求失败事件 */
}cm_httpclient_callback_event_e;

/**HTTP异常状态码*/
typedef enum{
    CM_HTTP_EVENT_CODE_DNS_FAIL=1,                      /*!< DNS解析失败 */
    CM_HTTP_EVENT_CODE_CONNECT_FAIL,                    /*!< 连接服务器失败 */
    CM_HTTP_EVENT_CODE_CONNECT_TIMEOUT,                 /*!< 连接超时 */
    CM_HTTP_EVENT_CODE_SSL_CONNECT_FAIL,                /*!< SSL握手失败 */
    CM_HTTP_EVENT_CODE_CONNECT_BREAK,                   /*!< 连接异常断开 */
    CM_HTTP_EVENT_CODE_WAITRSP_TIMEOUT,                 /*!< 等待响应超时 */
    CM_HTTP_EVENT_CODE_DATA_PARSE_FAIL,                 /*!< 数据解析失败 */
    CM_HTTP_EVENT_CODE_CACHR_NOT_ENOUGH,                /*!< 缓存空间不足 */
    CM_HTTP_EVENT_CODE_DATA_DROP,                       /*!< 数据丢包 */
    CM_HTTP_EVENT_CODE_WRITE_FILE_FAIL,                 /*!< 写文件失败 */
    CM_HTTP_EVENT_CODE_UNKNOWN=255,                     /*!< 未知错误 */
}cm_httpclient_error_event_e;

/**HTTP结果码*/
typedef enum{
    CM_HTTP_RET_CODE_OK = 0,                            /*!< 成功 */
    CM_HTTP_RET_CODE_OPERATION_NOT_ALLOWED = 3,         /*!< 操作不被允许 3 */
    CM_HTTP_RET_CODE_MALLOC_FAIL = 23,                  /*!< 内存分配失败 23 */
    CM_HTTP_RET_CODE_PARAM_ERROR = 50,                  /*!< 参数错误 50 */
    CM_HTTP_RET_CODE_UNKNOWN_ERROR = 650,               /*!< 未知错误 650 */
    CM_HTTP_RET_CODE_NO_MORE_FREE_CLIENT = 651,         /*!< 没有空闲客户端 651 */
    CM_HTTP_RET_CODE_CLIENT_NOT_CREATE = 652,           /*!< 客户端未创建 652 */
    CM_HTTP_RET_CODE_CLIENT_IS_BUSY = 653,              /*!< 客户端忙 653 */
    CM_HTTP_RET_CODE_URL_PARSE_FAIL = 654,              /*!< URL解析失败 654 */
    CM_HTTP_RET_CODE_SSL_NOT_ENABLE = 655,              /*!< SSL未使能 655 */
    CM_HTTP_RET_CODE_CONNECT_FAIL = 656,                /*!< 连接失败 656 */
    CM_HTTP_RET_CODE_SEND_DATA_FAIL = 657,              /*!< 数据发送失败 657 */
    CM_HTTP_RET_CODE_OPEN_FILE_FAIL = 658,              /*!< 打开文件失败 658 */
} cm_httpclient_ret_code_e;

/**
 * @}
 */

/** @defgroup httpclient_struct Struct
  * @{
  */

/**HTTP实例句柄*/
typedef void *cm_httpclient_handle_t;

/**HTTP回调*/
/*
* @param [in] client_handle                         客户端句柄
* @param [in] event                                 回调消息事件
* @param [in] param                                 事件相关对应响应数据
*
* CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND         NULL
* CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND             cm_httpclient_callback_rsp_header_param_t
* CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND            cm_httpclient_callback_rsp_content_param_t
* CM_HTTP_CALLBACK_EVENT_RSP_END_IND                NULL
* CM_HTTP_CALLBACK_EVENT_ERROR_IND                  cm_httpclient_error_event_e
*/
typedef void (*cm_httpclient_event_callback_func)(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param);

/**HTTP相关回调函数*/
typedef struct {
    uint16_t response_code;
    uint16_t response_header_len;
    const uint8_t *response_header;
}cm_httpclient_callback_rsp_header_param_t;

/**HTTP相关回调函数*/
typedef struct {
    uint32_t total_len;	                /*!< 数据总长度（chunked模式时为0） */
    uint32_t sum_len;                   /*!< 已接收长度 */
    uint32_t current_len;               /*!< 当前包长度 */
    const uint8_t *response_content;    /*!< 数据 */
}cm_httpclient_callback_rsp_content_param_t;

/**HTTP可配参数*/
typedef struct {
    uint8_t ssl_enable;             /*!< 是否使用HTTPS */
    int32_t ssl_id;                 /*!< ssl 索引号 */
    uint8_t cid;                    /*!< PDP索引 */
    uint8_t conn_timeout;           /*!< 连接超时时间 */
    uint8_t rsp_timeout;            /*!< 响应超时时间 */
    uint8_t dns_priority;           /*!< dns解析优先级 0：使用全局优先级。1：v4优先。2：v6优先 */
}cm_httpclient_cfg_t;

/**HTTP 同步接口输入参数*/
typedef struct {
    cm_httpclient_request_type_e method;    /*!< 请求类型 */
    const uint8_t *path;                    /*!< 请求路径 */
    uint32_t content_length;                /*!< 数据长度 */
    uint8_t *content;                       /*!< 数据 */
}cm_httpclient_sync_param_t;

/**HTTP 同步接口响应数据*/
typedef struct {
    uint32_t response_code;                 /*!< 请求成功时的响应结果码 */
    uint32_t response_header_len;           /*!< 响应报头长度 */
    uint32_t response_content_len;          /*!< 响应消息体长度 */
    uint8_t *response_header;               /*!< 响应报头（内部分配空间，使用完后可调用cm_httpclient_sync_free_data释放） */
    uint8_t *response_content;              /*!< 响应消息体（内部分配空间，使用完后可调用cm_httpclient_sync_free_data释放） */
}cm_httpclient_sync_response_t;

/**
 * @}
 */

/**
 * @brief uri编码(不对字母数字以及-_.!~*'();/?:@&=+$,#进行编码)
 *
 * @param [in] src          原始数据
 * @param [in] len          原始数据长度
 *
 * @return 编码数据/NULL 失败
 *
 * @details uri编码，不对字母数字以及-_.!~*'();/?:@&=+$,#进行编码，返回值为内部分配空间使用完后，需自行释放。不支持中文域名解析。
 */
uint8_t *cm_httpclient_uri_encode(const uint8_t *src, const uint32_t len);

/**
 * @brief uri编码(不对字母数字以及-_.!~*'()进行编码)
 *
 * @param [in] src          原始数据
 * @param [in] len          原始数据长度
 *
 * @return 编码数据/NULL 失败
 *
 * @details uri编码，不对字母数字以及-_.!~*'()进行编码，返回值为内部分配空间使用完后，需自行释放。不支持中文域名解析。
 */
uint8_t *cm_httpclient_uri_encode_component(const uint8_t *src, const uint32_t len);

/**
 * @brief 创建客户端实例
 *
 * @param [in] url              服务器地址（服务器地址url需要填写完整，例如（服务器url仅为格式示例）"https://39.106.55.200:80"）
 * @param [in] callback         客户端相关回调函数（使用cm_httpclient_sync_request()同步接口可忽略该参数，传NULL即可）
 * @param [out] handle          实例句柄
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 创建客户端实例
 */
cm_httpclient_ret_code_e cm_httpclient_create(const uint8_t *url, cm_httpclient_event_callback_func callback, cm_httpclient_handle_t *handle);

/**
 * @brief 删除客户端实例
 *
 * @param [in] handle         客户端实例句柄
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 本接口会将close socket（HTTP）操作发送至eloop模块中让其执行close socket操作。本接口返回成功代表操作已发送至eloop中，不代表已完成实例删除操作。用户连续两次调用本接口时建议中间保证100ms的延时。
 */
cm_httpclient_ret_code_e cm_httpclient_delete(cm_httpclient_handle_t handle);

/**
 * @brief 检测是否在执行请求
 *
 * @param [in] client           客户端实例句柄
 *
 * @return false 未执行请求/ true 正在执行请求
 *
 * @details 检测是否在执行请求
 */
bool cm_httpclient_is_busy(cm_httpclient_handle_t handle);

/**
 * @brief 客户端参数设置
 *
 * @param [in] handle         客户端实例句柄
 * @param [in] cfg            客户端配置
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 客户端参数设置，创建实例后设置,请求过程中不可设置；服务器应答重定向信息的情况下，采用cm_httpclient_set_cfg()配置的参数连接重定向的服务器
 */
cm_httpclient_ret_code_e cm_httpclient_set_cfg(cm_httpclient_handle_t handle, cm_httpclient_cfg_t cfg);

/**
 * @brief 终止http连接
 *
 * @param [in] handle         客户端实例句柄
 *
 * @return
 *
 * @details 终止http连接
 */
void cm_httpclient_terminate(cm_httpclient_handle_t handle);

/**
 * @brief 设置通用报头
 *
 * @param [in] handle           客户端实例句柄
 * @param [in] header           报头内容
 * @param [in] header_len       长度
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 设置通用报头，实例期间有效，不设置时，发送请求时将自动添加默认报头。
 * 直接引用header指针，不做拷贝，需保证请求过程中不做修改和释放，如外部释放，也
 * 需调用cm_httpclient_custom_header_free接口释放内部设置。
 */
cm_httpclient_ret_code_e cm_httpclient_custom_header_set(cm_httpclient_handle_t handle, uint8_t *header, uint16_t header_len);

/**
 * @brief 释放通用报头
 *
 * @param [in] handle           客户端实例句柄
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 重置客户端内部指针为NULL、长度为0。
 */
cm_httpclient_ret_code_e cm_httpclient_custom_header_free(cm_httpclient_handle_t handle);

/**
 * @brief 设置特定报头
 *
 * @param [in] handle           客户端实例句柄
 * @param [in] header           报头内容
 * @param [in] header_len       长度
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 设置特定报头，用于设置报头中，在不同请求时存在变化的字段，
 * 如果通用报头中同样存在该字段，将在发送请求时自动替换为当前设置值。
 * 直接引用header指针，不做拷贝，需保证请求过程中不做修改和释放，如外部释放，也
 * 需调用cm_httpclient_specific_header_free接口释放内部设置。
 */
cm_httpclient_ret_code_e cm_httpclient_specific_header_set(cm_httpclient_handle_t handle, uint8_t *header, uint16_t header_len);

/**
 * @brief 释放特定报头
 *
 * @param [in] handle           客户端实例句柄
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details  重置客户端内部指针为NULL、长度为0。
 */
cm_httpclient_ret_code_e cm_httpclient_specific_header_free(cm_httpclient_handle_t handle);

/**
 * @brief 开始发送请求
 *
 * @param [in] handle           客户端实例句柄
 * @param [in] method           请求类型
 * @param [in] path             请求路径
 * @param [in] chunked          是否为chunked模式
 * @param [in] content_length   content数据总长度（chunked模式时无效）
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 开始发送请求，主要为建立连接，并发送报头，将在request_start_func回调中指示开始完成
 * ，后续可发送content数据。
 */
cm_httpclient_ret_code_e cm_httpclient_request_start(cm_httpclient_handle_t handle, cm_httpclient_request_type_e method, const uint8_t *path,
bool chunked, uint32_t content_length);

/**
 * @brief 发送消息体
 *
 * @param [in] handle           客户端实例句柄
 * @param [in] content          消息体内容
 * @param [in] content_len      消息体长度
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 发送消息体，执行request_start_func回调后可使用。 \n
 *          socket发送缓存空间64240，单次发送超过64240长度数据时可能会造成缓存区溢出从而导致发送失败。
 */
cm_httpclient_ret_code_e cm_httpclient_request_send(cm_httpclient_handle_t handle, const uint8_t *content, uint32_t content_len);

/**
 * @brief 请求发送结束
 *
 * @param [in] handle           客户端实例句柄
 *
 * @return 0 成功/其他 失败（见cm_httpclient_ret_code_e）
 *
 * @details 请求发送结束，chunked模式下将发送结束包，普通模式下无实际效果，可不执行。
 */
cm_httpclient_ret_code_e cm_httpclient_request_end(cm_httpclient_handle_t handle);

/**
 * @brief 获取响应结果码
 *
 * @param [in] handle           客户端实例句柄
 *
 * @return -1 失败/其他 结果码
 *
 * @details 获取响应结果码。
 */
int32_t cm_httpclient_get_response_code(cm_httpclient_handle_t handle);

/**
 * @brief 解析报头
 *
 * @param [in] header           需解析的报头
 * @param [in] key              需要找到的字段key
 * @param [out] value           需要找到的字段数据
 *
 * @return 解析到的数据长度
 *
 * @details 解析报头，用于解析接收到的报头，按键值对进行解析，输入相应key，返回对应value，
 * value输出的为header中的指针位置，未单独分配空间。
 */
uint32_t cm_httpclient_parse_header(const uint8_t *header, const uint8_t *key, uint8_t **value);

/**
 * @brief 发送请求(同步接口)
 *
 * @param [in] handle           客户端实例句柄
 * @param [in] param            输入参数（见cm_httpclient_sync_param_t）
 * @param [out] response        响应结果（见cm_httpclient_sync_response_t）
 *
 * @return 见cm_httpclient_ret_code_e
 *
 * @details 发送http请求同步接口，只可用于非chunk模式发送，需先创建实例并完成相关参数设置，
 *响应结果中的数据未内部动态分配空间，使用完后可通过cm_httpclient_sync_free_data接口释放，
 *下次请求时也会自动释放。 \n
 *          该接口对内存资源开销较大，包括任务使用的内存、数据下载和拷贝使用的缓存区、ssl组件使用的内存，下载大文件可能会因内存不足产生异常，不同平台异常表现不同。 \n
 *          用户下载大文件时，建议使用HTTP异步接口或通过分段下载的方式多次使用HTTP同步接口进行分段下载。 \n
 *          socket发送缓存空间64240，单次发送超过64240长度数据时可能会造成缓存区溢出从而导致发送失败。
 */
cm_httpclient_ret_code_e cm_httpclient_sync_request(cm_httpclient_handle_t handle, cm_httpclient_sync_param_t param, 
        cm_httpclient_sync_response_t *response);

/**
 * @brief 释放响应数据(同步接口)
 *
 * @param [in] handle           客户端实例句柄
 *
 * @return 
 *
 * @details 释放响应数据。
 */
void cm_httpclient_sync_free_data(cm_httpclient_handle_t handle);


/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __CM_HTTP_H__ */


