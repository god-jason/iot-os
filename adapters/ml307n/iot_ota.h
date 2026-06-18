/**
 * @file iot_ota.h
 * @brief ML307N 平台 OTA(空中固件升级)适配器头文件
 * @details 封装 ML307N 平台 cm_ota 接口，提供统一的 OTA 操作宏定义。
 *          支持固件下载、写入、校验、触发升级等功能。
 */

#ifndef IOT_OTA_ML307N_H
#define IOT_OTA_ML307N_H

#include "../../iot_types.h"
#include "cm_ota.h"

/** @brief OTA状态枚举重定义 */
#define iot_ota_status_t            cm_ota_status_e

/** @brief OTA结果枚举重定义 */
#define iot_ota_result_t            cm_ota_result_e

/** @brief OTA信息结构体重定义 */
#define iot_ota_info_t              cm_ota_info_t

/** @brief OTA状态信息结构体重定义 */
#define iot_ota_status_info_t       cm_ota_status_info_t

/** @brief OTA下载数据回调函数类型重定义 */
#define iot_ota_download_callback_t cm_ota_download_callback

/** @brief OTA状态回调函数类型重定义 */
#define iot_ota_status_callback_t   cm_ota_status_callback

/**
 * @brief 初始化OTA下载
 * @param[in] url      升级包URL
 * @param[in] cb       下载数据回调函数
 * @param[in] cb_param 回调用户参数
 * @return 0 成功，-1 失败
 */
#define iot_ota_init(url, cb, cb_param) \
    cm_ota_init((char *)(url), (cm_ota_download_callback)(cb), (void *)(cb_param))

/**
 * @brief 开始OTA下载
 * @return 0 成功，-1 失败
 */
#define iot_ota_start() \
    cm_ota_start()

/**
 * @brief 取消OTA下载
 * @return 0 成功，-1 失败
 */
#define iot_ota_cancel() \
    cm_ota_cancel()

/**
 * @brief 获取OTA下载状态
 * @param[out] info 状态信息结构体指针
 * @return 0 成功，-1 失败
 */
#define iot_ota_get_status(info) \
    cm_ota_get_status((cm_ota_status_info_t *)(info))

/**
 * @brief 注册OTA状态回调函数
 * @param[in] cb 状态回调函数指针
 */
#define iot_ota_register_status_callback(cb) \
    cm_ota_register_status_callback((cm_ota_status_callback)(cb))

/**
 * @brief 写入OTA固件数据
 * @param[in] data 固件数据指针
 * @param[in] len  数据长度
 * @return 0 成功，-1 失败
 */
#define iot_ota_write_data(data, len) \
    cm_ota_write_data((uint8_t *)(data), (uint32_t)(len))

/**
 * @brief 校验OTA固件完整性
 * @return 0 成功，-1 失败
 */
#define iot_ota_verify() \
    cm_ota_verify()

/**
 * @brief 触发OTA升级
 * @return 0 成功，-1 失败
 */
#define iot_ota_trigger_upgrade() \
    cm_ota_trigger_upgrade()

#endif /* IOT_OTA_ML307N_H */
