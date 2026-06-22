/**
 * @file http_form.h
 * @brief HTTP 表单数据处理工具
 *
 * 提供 application/x-www-form-urlencoded 和 multipart/form-data 编码/解码功能
 */
#ifndef IOT_HTTP_FORM_H
#define IOT_HTTP_FORM_H

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
    char name[128];
    char value[512];
} http_form_param_t;

typedef struct {
    char name[128];
    char filename[256];
    char content_type[64];
    char* data;
    size_t data_len;
} http_form_file_t;

typedef struct {
    char boundary[128];
    char* data;
    size_t data_len;
    size_t data_capacity;
} http_form_multipart_t;

/*===========================================================
 * URL 编码表单接口 (application/x-www-form-urlencoded)
 *===========================================================*/

/**
 * @brief 编码单个表单参数为 URL 编码格式
 * @param name 参数名
 * @param value 参数值
 * @param encoded 输出缓冲区
 * @param encoded_len 缓冲区大小
 * @return 0 成功，-1 失败
 */
int http_form_encode_param(const char* name, const char* value, char* encoded, size_t encoded_len);

/**
 * @brief 编码多个表单参数为 URL 编码格式
 * @param params 参数数组
 * @param param_count 参数数量
 * @param encoded 输出缓冲区
 * @param encoded_len 缓冲区大小
 * @return 0 成功，-1 失败
 */
int http_form_encode_params(const http_form_param_t* params, size_t param_count,
                           char* encoded, size_t encoded_len);

/**
 * @brief 编码多个表单参数（分配内存版本）
 * @param params 参数数组
 * @param param_count 参数数量
 * @return 编码后字符串，失败返回 NULL，使用完后需 free
 */
char* http_form_encode_params_alloc(const http_form_param_t* params, size_t param_count);

/**
 * @brief 解码 URL 编码的表单数据
 * @param encoded URL 编码的表单数据
 * @param params 输出参数数组
 * @param max_params 最大参数数量
 * @return 实际解析的参数数量
 */
size_t http_form_decode_params(const char* encoded, http_form_param_t* params, size_t max_params);

/**
 * @brief 根据名称查找表单参数值
 * @param params 参数数组
 * @param param_count 参数数量
 * @param name 参数名
 * @return 参数值，未找到返回 NULL
 */
const char* http_form_get_param(const http_form_param_t* params, size_t param_count, const char* name);

/*===========================================================
 * Multipart 表单接口 (multipart/form-data)
 *===========================================================*/

/**
 * @brief 创建 multipart 表单编码器
 * @param boundary 分隔符（可为空，自动生成）
 * @return 编码器句柄，失败返回 NULL
 */
http_form_multipart_t* http_form_multipart_create(const char* boundary);

/**
 * @brief 销毁 multipart 表单编码器
 * @param mp 编码器句柄
 */
void http_form_multipart_destroy(http_form_multipart_t* mp);

/**
 * @brief 添加文本字段
 * @param mp 编码器句柄
 * @param name 字段名
 * @param value 字段值
 * @return 0 成功，-1 失败
 */
int http_form_multipart_add_field(http_form_multipart_t* mp, const char* name, const char* value);

/**
 * @brief 添加文件字段
 * @param mp 编码器句柄
 * @param name 字段名
 * @param filename 文件名
 * @param content_type MIME 类型
 * @param data 文件数据
 * @param data_len 数据长度
 * @return 0 成功，-1 失败
 */
int http_form_multipart_add_file(http_form_multipart_t* mp, const char* name, const char* filename,
                                const char* content_type, const void* data, size_t data_len);

/**
 * @brief 添加文件字段（从文件路径读取）
 * @param mp 编码器句柄
 * @param name 字段名
 * @param filename 文件名
 * @param content_type MIME 类型
 * @param file_path 文件路径
 * @return 0 成功，-1 失败
 */
int http_form_multipart_add_file_path(http_form_multipart_t* mp, const char* name, 
                                      const char* filename, const char* content_type,
                                      const char* file_path);

/**
 * @brief 获取编码后的 multipart 数据
 * @param mp 编码器句柄
 * @param data 输出数据指针（可选）
 * @param len 输出数据长度（可选）
 * @return 数据指针，失败返回 NULL
 */
const char* http_form_multipart_get_data(http_form_multipart_t* mp, const char** data, size_t* len);

/**
 * @brief 获取 Content-Type 头（包含 boundary）
 * @param mp 编码器句柄
 * @param content_type 输出缓冲区
 * @param len 缓冲区大小
 * @return 0 成功，-1 失败
 */
int http_form_multipart_get_content_type(http_form_multipart_t* mp, char* content_type, size_t len);

/**
 * @brief 获取当前编码数据的大小
 * @param mp 编码器句柄
 * @return 编码数据大小
 */
size_t http_form_multipart_get_size(http_form_multipart_t* mp);

/*===========================================================
 * 工具函数
 *===========================================================*/

/**
 * @brief 根据文件扩展名猜测 MIME 类型
 * @param filename 文件名
 * @return MIME 类型字符串
 */
const char* http_form_guess_content_type(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_FORM_H */