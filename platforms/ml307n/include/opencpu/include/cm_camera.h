/**
 * @file        cm_camera.h
 * @brief       相机接口
 * @copyright   Copyright @2023 China Mobile IOT. All rights reserved.
 * @author
 * @date        2023/03/13
 *
 * @defgroup camera
 * @ingroup camera
 * @{
 */

#ifndef __CM_CAMERA_H__
#define __CM_CAMERA_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/


/**
 * @brief error code
 */
#define     CM_CAM_ERR_OK                   0
#define     CM_CAM_ERR_UNKNOW               -1
#define     CM_CAM_ERR_INVALID_PARA         -2
#define     CM_CAM_ERR_NOT_MEMORY           -3
#define     CM_CAM_ERR_FILE_OP_FAIL         -4
#define     CM_CAM_ERR_WRONG_CFG_DATA       -5
#define     CM_CAM_ERR_WRONG_STATE          -6
#define     CM_CAM_ERR_NOT_OPEN             -7
#define     CM_CAM_ERR_NO_DEV               -8
#define     CM_CAM_ERR_INIT_FAIL            -9
#define     CM_CAM_ERR_STOP_FAIL            -10
#define     CM_CAM_ERR_JPEG_OP_FAIL         -11
#define     CM_CAM_ERR_CAPTURE_TAKE_FAIL    -12



/**
 * @brief camera数据输出类型
 *
 * @details 支持CAM_OUT_FMT_RGB565和CAM_OUT_FMT_JPEG,
 *
 */
typedef enum
{
    CAM_OUT_FMT_YUV422_YUYV = 0,
    CAM_OUT_FMT_YUV422_YVYU,
    CAM_OUT_FMT_YUV422_UYVY,
    CAM_OUT_FMT_YUV422_VYUY,
    CAM_OUT_FMT_YUV420_NV12 = 6,
    CAM_OUT_FMT_RAW8 = 10,
    CAM_OUT_FMT_RGB565,
    CAM_OUT_FMT_JPEG,
    CAM_OUT_FMT_MAX,
}cm_camera_out_fmt_e;

/**
 * @brief camera预览接口配置参数
 */
typedef struct {
    cm_camera_out_fmt_e recordFormat;                               /*!< 输出数据类型 */
    void (*enqueueCamRecordBuffer)(void *pBuf, uint32_t len);       /*!< 输出数据回调函数 */
    void (*flushCamRecordBuffers)(void);                            /*!< 输出关闭回调 */
}cm_camera_preview_cfg_t;


/**
 * @brief camerap拍照数据读取参数
 *
 * @details 拍照数据存储成文件时, buf_size设置为0;
 * 拍照图像的数据需要输出时,buf_size设置为非0，拍照后图像数据的存储地址通过p_buf输出，使用完数据后需要free,
 * 否则会有内存泄露问题.
 *
 */
typedef struct {
    uint8_t *p_buf;
    uint32_t buf_size;
}cm_camera_capture_img_buf_t;

/**
 * @brief camera拍照接口配置参数
 *
 */
typedef struct {
    cm_camera_out_fmt_e recordFormat;                               /*!< 输出数据类型 */
    uint8_t *file_name;                                             /*!< 输出文件目录和文件名        */
    cm_camera_capture_img_buf_t image;                              /*!< 输出到内存，见cm_camera_capture_img_buf_t描述*/
}cm_camera_capture_cfg_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief 获取底层驱动输出大小
 *
 * @param [out] width       宽度
 * @param [out] height      高度
 *
 * @return  0 成功；-1 失败
 *
 * @details 获取底层驱动输出大小
 */
int32_t cm_camera_get_info(uint32_t *width, uint32_t *height);

/**
 * @brief 打开相机
 *
 * @return  0 成功；-1 失败
 *
 * @details 打开相机
 */
int32_t cm_camera_open(void);

/**
 * @brief 关闭相机
 *
 * @return  0 成功；-1 失败
 *
 * @details 关闭相机
 */
int32_t cm_camera_close(void);

/**
 * @brief 开始预览
 *
 * @param [in] cfg          配置参数
 *
 * @return  0 成功；-1 失败
 *
 * @details 开始预览，数据通过cfg中设置的回调函数返回，回调函数中可
 * 调用cm_camera_preview_stop关闭预览，但不可调用cm_camera_close
 */
int32_t cm_camera_preview_start(cm_camera_preview_cfg_t *cfg);

/**
 * @brief 关闭预览
 *
 * @return  0 成功；-1 失败
 *
 * @details 关闭预览
 */
int32_t cm_camera_preview_stop(void);

/**
 * @brief 拍照
 *
 * @param [in] cfg          配置参数
 *
 * @return  0 成功；-1 失败
 *
 * @details 拍照，数据自动保存至指定文件
 */
int32_t cm_camera_capture_take(cm_camera_capture_cfg_t *cfg);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_CAMERA_H__ */


/** @}*/


