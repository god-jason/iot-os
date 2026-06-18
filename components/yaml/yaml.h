/**
 * @file yaml.h
 * @brief YAML与cJSON相互转换库
 *
 * 本模块提供YAML格式与cJSON对象之间的相互转换功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef YAML_H
#define YAML_H

#include "cJSON.h"
#include "iot_base.h"

/**
 * @brief cJSON对象转换为YAML字符串
 * @param json cJSON对象
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @return 成功返回输出的字符数(不含终止符)，失败返回-1
 */
int yaml_encode_cjson(const cJSON *json, char *out, int out_size);

/**
 * @brief cJSON对象转换为YAML字符串(动态分配)
 * @param json cJSON对象
 * @return 分配的YAML字符串，需要free释放;失败返回NULL
 */
char* yaml_encode_cjson_dup(const cJSON *json);

/**
 * @brief YAML字符串转换为cJSON对象
 * @param yaml YAML格式字符串
 * @return 分配的cJSON对象，需要cJSON_Delete释放;失败返回NULL
 */
cJSON* yaml_decode_cjson(const char *yaml);

#endif /* YAML_H */
