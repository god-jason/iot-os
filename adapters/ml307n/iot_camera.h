/**
 * @file iot_camera.h
 * @brief ML307N 平台相机(Camera)适配器头文件
 * @details 封装 ML307N 平台 cm_camera 接口，提供统一的相机操作宏定义。
 *          支持预览、拍照等功能，编译时零开销映射到底层函数。
 */

#ifndef IOT_CAMERA_ML307N_H
#define IOT_CAMERA_ML307N_H

#include "../../iot_types.h"
#include "cm_camera.h"

/** @brief 相机输出格式枚举重定义 */
#define iot_camera_out_fmt_t        cm_camera_out_fmt_e

/** @brief 相机预览配置结构体重定义 */
#define iot_camera_preview_cfg_t    cm_camera_preview_cfg_t

/** @brief 相机拍照配置结构体重定义 */
#define iot_camera_capture_cfg_t    cm_camera_capture_cfg_t

/** @brief 相机拍照缓冲区结构体重定义 */
#define iot_camera_capture_buf_t    cm_camera_capture_img_buf_t

/**
 * @brief 获取底层驱动输出图像大小
 * @param[out] width  图像宽度
 * @param[out] height 图像高度
 * @return 0 成功，-1 失败
 */
#define iot_camera_get_info(width, height) \
    cm_camera_get_info((uint32_t *)(width), (uint32_t *)(height))

/**
 * @brief 打开相机
 * @return 0 成功，-1 失败
 */
#define iot_camera_open() \
    cm_camera_open()

/**
 * @brief 关闭相机
 * @return 0 成功，-1 失败
 */
#define iot_camera_close() \
    cm_camera_close()

/**
 * @brief 开始预览
 * @param[in] cfg 预览配置参数（包含输出格式和数据回调函数）
 * @return 0 成功，-1 失败
 * @note 回调函数中可调用 iot_camera_preview_stop 关闭预览，但不可调用 iot_camera_close
 */
#define iot_camera_preview_start(cfg) \
    cm_camera_preview_start((cm_camera_preview_cfg_t *)(cfg))

/**
 * @brief 停止预览
 * @return 0 成功，-1 失败
 */
#define iot_camera_preview_stop() \
    cm_camera_preview_stop()

/**
 * @brief 拍照
 * @param[in] cfg 拍照配置参数（包含输出格式、文件名、内存缓冲区）
 * @return 0 成功，-1 失败
 * @note 数据自动保存至指定文件或输出到内存缓冲区
 */
#define iot_camera_capture(cfg) \
    cm_camera_capture_take((cm_camera_capture_cfg_t *)(cfg))

#endif /* IOT_CAMERA_ML307N_H */
