/**
 * @file iot_camera.c
 * @brief ML307N 平台相机适配器实现
 * @details 基于 cm_camera 接口实现相机功能封装，
 *          提供统一的相机操作函数。
 */

#include "iot_camera.h"

/**
 * @brief 获取底层驱动输出图像大小
 * @param[out] width  图像宽度
 * @param[out] height 图像高度
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_get_info(uint32_t *width, uint32_t *height)
{
    return cm_camera_get_info((uint32_t *)width, (uint32_t *)height);
}

/**
 * @brief 打开相机
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_open(void)
{
    return cm_camera_open();
}

/**
 * @brief 关闭相机
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_close(void)
{
    return cm_camera_close();
}

/**
 * @brief 开始预览
 * @param[in] cfg 预览配置参数（包含输出格式和数据回调函数）
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_preview_start(const iot_camera_preview_cfg_t *cfg)
{
    return cm_camera_preview_start((cm_camera_preview_cfg_t *)cfg);
}

/**
 * @brief 停止预览
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_preview_stop(void)
{
    return cm_camera_preview_stop();
}

/**
 * @brief 拍照
 * @param[in] cfg 拍照配置参数（包含输出格式、文件名、内存缓冲区）
 * @return 0 成功，-1 失败
 */
int32_t iot_camera_capture(const iot_camera_capture_cfg_t *cfg)
{
    return cm_camera_capture_take((cm_camera_capture_cfg_t *)cfg);
}