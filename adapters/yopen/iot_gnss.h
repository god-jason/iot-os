/**
 * @file iot_gnss.h
 * @brief YOPEN 平台 GNSS 定位适配器头文件
 * @details 提供 GNSS 定位功能封装，支持定位信息获取和 NMEA 数据回调。
 * @note 暂未支持，需要平台实现
 */

#ifndef IOT_GNSS_YOPEN_H
#define IOT_GNSS_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * GNSS 类型定义
 * ============================================================ */

/** @brief GNSS卫星系统类型 */
typedef enum {
    IOT_GNSS_TYPE_GPS = 0,            /**< GPS */
    IOT_GNSS_TYPE_BDS = 1,             /**< 北斗 */
    IOT_GNSS_TYPE_GLO = 2,            /**< GLONASS */
    IOT_GNSS_TYPE_GAL = 3,            /**< 伽利略 */
    IOT_GNSS_TYPE_QZSS = 4,           /**< QZSS */
} iot_gnss_type_t;

/** @brief GNSS定位模式 */
typedef enum {
    IOT_GNSS_MODE_SINGLE = 0,         /**< 单点定位 */
    IOT_GNSS_MODE_DGPS = 1,           /**< 差分定位 */
    IOT_GNSS_MODE_FIXED = 2,         /**< 固定解 */
} iot_gnss_fix_mode_t;

/* ============================================================
 * GNSS 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 初始化GNSS模块
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_init(void);

/**
 * @brief 去初始化GNSS模块
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_deinit(void);

/**
 * @brief 启动GNSS定位
 * @param[in] type 卫星系统类型
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_start(iot_gnss_type_t type);

/**
 * @brief 停止GNSS定位
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_stop(void);

/**
 * @brief 获取定位结果
 * @param[out] location 定位信息结构体
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_get_location(iot_gnss_location_t *location);

/**
 * @brief 注册NMEA回调
 * @param[in] cb NMEA回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_gnss_register_nmea_cb(iot_gnss_nmea_cb_t cb, void *user_data);

#endif /* IOT_GNSS_YOPEN_H */
