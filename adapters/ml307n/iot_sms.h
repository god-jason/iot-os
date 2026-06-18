/**
 * @file iot_sms.h
 * @brief ML307N 平台短信(SMS)适配器头文件
 * @details 封装 ML307N 平台 cm_sms 接口，提供统一的短信操作宏定义。
 *          支持发送短信、设置短信中心号码、设置编码格式等功能。
 */

#ifndef IOT_SMS_ML307N_H
#define IOT_SMS_ML307N_H

#include "../../iot_types.h"
#include "cm_sms.h"

/** @brief 短信编码格式枚举重定义 */
#define iot_sms_charset_t       cm_sms_charset_e

/** @brief 短信存储位置枚举重定义 */
#define iot_sms_storage_t       cm_sms_storage_e

/** @brief 短信状态枚举重定义 */
#define iot_sms_status_t        cm_sms_status_e

/** @brief 短信信息结构体重定义 */
#define iot_sms_info_t          cm_sms_info_t

/**
 * @brief 设置短信编码格式
 * @param[in] charset 编码格式枚举值
 * @return 0 成功，-1 失败
 */
#define iot_sms_set_charset(charset) \
    cm_sms_set_charset((cm_sms_charset_e)(charset))

/**
 * @brief 获取短信编码格式
 * @return 编码格式枚举值
 */
#define iot_sms_get_charset() \
    cm_sms_get_charset()

/**
 * @brief 设置短信中心号码
 * @param[in] sca 短信中心号码字符串（不超过20字节）
 * @return 0 成功，-1 失败
 */
#define iot_sms_set_sca(sca) \
    cm_sms_set_sca((char *)(sca))

/**
 * @brief 获取短信中心号码
 * @param[out] sca 短信中心号码缓冲区（>=21字节）
 * @return >=0 号码字符串长度，<0 失败
 */
#define iot_sms_get_sca(sca) \
    cm_sms_get_sca((char *)(sca))

/**
 * @brief 发送短信
 * @param[in] phone_num 目标手机号码
 * @param[in] text      短信内容
 * @return 0 成功，-1 失败
 */
#define iot_sms_send(phone_num, text) \
    cm_sms_send((char *)(phone_num), (char *)(text))

/**
 * @brief 删除指定位置的短信
 * @param[in] storage 存储位置枚举值
 * @param[in] index   短信索引
 * @return 0 成功，-1 失败
 */
#define iot_sms_delete(storage, index) \
    cm_sms_delete((cm_sms_storage_e)(storage), (uint32_t)(index))

/**
 * @brief 读取指定位置的短信
 * @param[in]  storage 存储位置枚举值
 * @param[in]  index   短信索引
 * @param[out] info    短信信息结构体指针
 * @return 0 成功，-1 失败
 */
#define iot_sms_read(storage, index, info) \
    cm_sms_read((cm_sms_storage_e)(storage), (uint32_t)(index), (cm_sms_info_t *)(info))

#endif /* IOT_SMS_ML307N_H */
