/**
 * @file iot_fota.h
 * @brief YOPEN 平台 FOTA 固件升级适配器头文件
 * @details 提供 FOTA 固件升级功能封装，支持下载、校验、升级等。
 * @note 暂未支持，需要平台实现
 */

#ifndef IOT_FOTA_YOPEN_H
#define IOT_FOTA_H

#include "../../iot_types.h"

/* ============================================================
 * FOTA 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 初始化FOTA模块
 * @param[in] result_cb 升级结果回调
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_fota_init(iot_fota_result_cb_t result_cb, void *user_data);

/**
 * @brief 开始FOTA升级
 * @param[in] url 固件URL
 * @return 0 成功，<0 失败
 */
int32_t iot_fota_start(const char *url);

/**
 * @brief 取消FOTA升级
 * @return 0 成功，<0 失败
 */
int32_t iot_fota_cancel(void);

/**
 * @brief 获取FOTA升级进度
 * @param[out] current 当前进度(字节)
 * @param[out] total 总进度(字节)
 * @return 0 成功，<0 失败
 */
int32_t iot_fota_get_progress(uint32_t *current, uint32_t *total);

/**
 * @brief 获取FOTA当前阶段
 * @return FOTA阶段
 */
iot_fota_stage_t iot_fota_get_stage(void);

#endif /* IOT_FOTA_YOPEN_H */
