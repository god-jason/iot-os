/**
 * @file        cm_camera_sensor.h
 * @brief       相机硬件驱动适配接口
 * @copyright   Copyright @2026 China Mobile IOT. All rights reserved.
 * @author
 * @date
 *
 * @defgroup camera
 * @ingroup camera
 * @{
 */

#ifndef __CMO_CAMERA_H__
#define __CMO_CAMERA_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "cm_camera_sensor.h"
#include "cm_camera.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/


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
 * @brief 获取驱动参数
 *
 * @param
 *
 * @return NULL 失败,   读取到的驱动参数
 *
 * @details 操作相机前需设置
 */
cm_cam_sensor_data_t * cm_camera_sensor_get_data(void);


#if 0
/**
 * @brief i2c读取
 *
 * @param [in] attr         i2c参数
 * @param [in] reg          寄存器地址
 * @param [out] val         读取值
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c读取相机寄存器
 */
int32_t cm_camera_sensor_i2c_read(const cm_cam_sensor_i2c_attr *attr, uint32_t reg, uint32_t *val);
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CMO_CAMERA_H__ */


/** @}*/

