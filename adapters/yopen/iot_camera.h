/**
 * @file iot_camera.h
 * @brief YOPEN 平台相机适配器头文件
 * @details 提供相机预览和拍照功能封装。
 * @note 暂未支持，需要平台实现
 */

#ifndef IOT_CAMERA_YOPEN_H
#define IOT_CAMERA_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * 相机类型定义
 * ============================================================ */

/** @brief 相机输出格式 */
typedef enum {
    IOT_CAMERA_FMT_YUV422 = 0,       /**< YUV422 */
    IOT_CAMERA_FMT_RGB565 = 1,       /**< RGB565 */
    IOT_CAMERA_FMT_JPEG = 2,         /**< JPEG */
} iot_camera_fmt_t;

/** @brief 相机句柄类型 */
typedef void* iot_camera_handle_t;

/* ============================================================
 * 相机接口（暂未支持）
 * ============================================================ */

/**
 * @brief 打开相机
 * @param[in] fmt 输出格式
 * @param[in] width 宽度
 * @param[in] height 高度
 * @return 相机句柄，失败返回NULL
 */
iot_camera_handle_t iot_camera_open(iot_camera_fmt_t fmt, uint16_t width, uint16_t height);

/**
 * @brief 关闭相机
 * @param[in] handle 相机句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_camera_close(iot_camera_handle_t handle);

/**
 * @brief 开始预览
 * @param[in] handle 相机句柄
 * @param[in] frame_cb 帧数据回调
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_camera_start_preview(iot_camera_handle_t handle,
                                  iot_camera_frame_cb_t frame_cb,
                                  void *user_data);

/**
 * @brief 停止预览
 * @param[in] handle 相机句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_camera_stop_preview(iot_camera_handle_t handle);

/**
 * @brief 拍照
 * @param[in] handle 相机句柄
 * @param[out] buf 图像数据缓冲区
 * @param[out] size 图像数据大小
 * @return 0 成功，<0 失败
 */
int32_t iot_camera_capture(iot_camera_handle_t handle, uint8_t **buf, uint32_t *size);

#endif /* IOT_CAMERA_YOPEN_H */
