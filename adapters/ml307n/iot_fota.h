/**
 * @file iot_fota.h
 * @brief ML307N 平台 FOTA(固件空中升级)适配器头文件
 * @details 封装 ML307N 平台 cm_fota 接口，提供统一的 FOTA 操作宏定义。
 *          支持设置升级URL、重启时间、执行升级等功能。
 */

#ifndef IOT_FOTA_ML307N_H
#define IOT_FOTA_ML307N_H

#include "../../iot_types.h"
#include "cm_fota.h"

/** @brief FOTA错误码枚举重定义 */
#define iot_fota_error_t            cm_fota_error_e

/** @brief FOTA阶段枚举重定义 */
#define iot_fota_stage_t            cm_fota_stage_e

/** @brief FOTA信息结构体重定义 */
#define iot_fota_info_t             cm_fota_info_t

/** @brief FOTA重启时间结构体重定义 */
#define iot_fota_reboot_time_t      cm_fota_reboot_time

/** @brief FOTA结果回调函数类型重定义 */
#define iot_fota_result_callback_t  cm_fota_result_callback

/**
 * @brief 设置FOTA服务器URL（HTTP/HTTPS）
 * @param[in] url 服务器URL，最大256字节，必须指定协议类型
 * @return 0 成功，其他失败
 * @note 仅支持HTTP（含HTTPS无证书校验）下载方式
 */
#define iot_fota_set_url(url) \
    cm_fota_set_url((char *)(url))

/**
 * @brief 设置FOTA失败时的下一次重启时间
 * @param[in] time 重启时间结构体指针
 * @return 0 成功，其他失败
 * @note 定时起始时间为尝试FOTA升级的时间，非检测到失败的时间
 */
#define iot_fota_set_reboot_time(time) \
    cm_fota_set_reboot_time((cm_fota_reboot_time *)(time))

/**
 * @brief 读取FOTA服务器配置
 * @param[out] cfg FOTA信息结构体指针
 */
#define iot_fota_read_config(cfg) \
    cm_fota_read_config((cm_fota_info_t *)(cfg))

/**
 * @brief 注册FOTA结果回调函数
 * @param[in] cb 回调函数指针
 * @note 仅在大系统执行FOTA升级任务预校验时回调，出错时才会触发
 */
#define iot_fota_register_callback(cb) \
    cm_fota_res_callback_register((cm_fota_result_callback)(cb))

/**
 * @brief 执行FOTA升级
 * @return 0 成功，<0 失败
 * @note 一次FOTA期间模组会重启4次，需要数分钟时间。
 *       建议在OpenCPU程序运行至少5秒后再执行。
 */
#define iot_fota_exec_upgrade() \
    cm_fota_exec_upgrade()

#endif /* IOT_FOTA_ML307N_H */
