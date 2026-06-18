/**  @file
  yopen_api_camera.h

  @brief
  

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
16/12/2020        Neo         Init version
=================================================================*/


#ifndef _YOPEN_CAMERA_H_
#define _YOPEN_CAMERA_H_

#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include "yopen_api_common.h"
#include "yopen_type.h"

#ifdef __cplusplus
	 extern "C" {
#endif

/**
 * @defgroup yopen_camera camera功能
 * @{
*/
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/

 typedef enum
 {
   YOPEN_CAMERA_SUCCESS  			= 0,
   YOPEN_CAMERA_INVALID_PARAM_ERR    = 10  | (YOPEN_COMPONENT_CAMERA << 16),
   YOPEN_CAMERA_DEINIT_ERR,
   YOPEN_CAMERA_INIT_ERR,
   YOPEN_CAMERA_CAPTURE_ERR,
   YOPEN_CAMERA_NOT_INIT_ERR,
   YOPEN_CAMERA_GET_VALUE_ERROR 		= 50  | (YOPEN_COMPONENT_CAMERA << 16),
   YOPEN_CAMERA_MEM_ADDR_NULL_ERROR	= 60  | (YOPEN_COMPONENT_CAMERA << 16),
   YOPEN_CAMERA_TASK_ERROR,
   YOPEN_CAMERA_OTHER
} yopen_errcode_camera_e;

/*===========================================================================
 * Struct
 ===========================================================================*/
/** @brief camera结构体*/ 

/*===========================================================================
 * function 
 ===========================================================================*/

/**
 * @brief 摄像头端口枚举
 *
 */
typedef enum
{
	YOPEN_CAMERA_PORT_USP0,
	YOPEN_CAMERA_PORT_USP1,
	YOPEN_CAMERA_PORT_MAX,
} yopen_camera_port_e;

/**
 * @brief 大小端
 *
 */
typedef enum
{
	YOPEN_CAMERA_ENDIAN_LITTLE,
	YOPEN_CAMERA_ENDIAN_BIG,
}	yopen_camera_endian_e;

/**
 * @brief WIRE
 *
 */
typedef enum
{
	YOPEN_CAMERA_WIRE_1,
	YOPEN_CAMERA_WIRE_2,
} yopen_camera_wire_e;

/**
 * @brief 像素大小
 * @note 和buffer大小相关关 8W_Y=320*240  8W_COLOR=320*240*2 30W_Y=640*480 30W_COLOR=640*480*2
 *
 */
typedef enum
{
	YOPE_CAMAMER_8W_Y			= 10, // 8w only y use 10 dma descriptor chain
	YOPE_CAMAMER_8W_COLOR		= 20, // 8w color use 20 dma descriptor chain
	YOPE_CAMAMER_30W_Y			= 40, // 30w only y use 40 dma descriptor chain
	YOPE_CAMAMER_30W_COLOR		= 80, // 30w use 80 dma descriptor chain, 
} yopen_resolution_e;

/**
 * @brief MLCK
 *
 */
typedef enum
{
	YOPE_CAMAMER_6_5_M	= 0,  ///< camera 6.5M HZ
	YOPE_CAMAMER_13_M	= 1,  ///< camera 13M HZ
	YOPE_CAMAMER_25_5_M	= 2,  ///< camera 25.5M HZ
	YOPE_CAMAMER_24_M	= 3,  ///< camera 24M HZ
} yopen_camera_clk_e;

/**
 * @brief SEQ
 *
 */
typedef enum
{
	YOPEN_CAMERA_SEQ_0,
	YOPEN_CAMERA_SEQ_1,
} yopen_camera_seq_e;

/**
 * @brief 采集完成回调函数
 *
 */
typedef void (*yopen_camera_callback)(void);

typedef struct yopen_camera
{
	yopen_camera_port_e port;
	yopen_camera_endian_e endian;
	yopen_camera_wire_e wire;
	yopen_camera_seq_e seq;
	yopen_resolution_e resolution;
	yopen_camera_clk_e clk;
	uint8_t *buffer;
	uint8_t 		cpol;
    uint8_t			cpha;
    uint8_t         ddrMode;
    uint8_t         wordIdSeq;
	uint8_t         yOnly;
    uint8_t         rowScaleRatio;
    uint8_t         colScaleRatio;
    uint8_t         scaleBytes;
    uint8_t         dummyAllowed;
	yopen_camera_callback callback;
} yopen_camera_config_s;


/**
 * @brief 摄像头初始化
 *
 * @param[in] config 摄像头配置
 * @return yopen_errcode_camera_e 摄像头错误码
 */
yopen_errcode_camera_e yopen_camera_init(yopen_camera_config_s *config);


/**
 * @brief 拍照接口
 *
 * @return yopen_errcode_camera_e 摄像头错误码
 */
yopen_errcode_camera_e yopen_camera_capture(void);

/**
 * @brief 摄像头去初始化
 *
 * @return yopen_errcode_camera_e 摄像头错误码
 * 	
 */
yopen_errcode_camera_e yopen_camera_deinit(void);

#ifdef __cplusplus
	 } /*"C" */
#endif
 
#endif /* _YOPEN_CAMERA_H_ */


