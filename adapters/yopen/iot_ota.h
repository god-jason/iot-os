/**
 * @file iot_ota.h
 * @brief YOPEN 平台 OTA 升级适配器头文件
 * @details 提供 OTA 固件升级功能封装，支持状态回调、进度回调等。
 * @note 暂未支持，需要平台实现
 */

#ifndef IOT_OTA_YOPEN_H
#define IOT_OTA_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * OTA 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 初始化OTA模块
 * @param[in] status_cb 状态回调
 * @param[in] progress_cb 进度回调
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_ota_init(iot_ota_status_cb_t status_cb,
                     iot_ota_progress_cb_t progress_cb,
                     void *user_data);

/**
 * @brief 开始OTA下载
 * @param[in] url 固件URL
 * @return 0 成功，<0 失败
 */
int32_t iot_ota_start(const char *url);

/**
 * @brief 停止OTA下载
 * @return 0 成功，<0 失败
 */
int32_t iot_ota_stop(void);

/**
 * @brief 执行OTA升级
 * @return 0 成功，<0 失败
 */
int32_t iot_ota_execute(void);

/**
 * @brief 获取OTA状态
 * @return OTA状态
 */
iot_ota_state_t iot_ota_get_state(void);

#endif /* IOT_OTA_YOPEN_H */
