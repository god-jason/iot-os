/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_edma.h
 *
 * @brief       EDMA驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRIVER_EDMA_H_
#define DRIVER_EDMA_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>
#include <os_def.h>

/**
 * @addtogroup Edma
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
// EDMA flags
#define EDMA_FLAG_INITIALIZED ((uint8_t)(1U))
#define EDMA_FLAG_POWERED     ((uint8_t)(1U << 1))

/****** EDMA Event *****/
#define EDMA_EVENT_DONE    (1UL << 0) /// Done
#define EDMA_EVENT_WRERROR (1UL << 1) /// Write Error
#define EDMA_EVENT_RDERROR (1UL << 2) /// Read Error

/****** EDMA Control Codes *****/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
  IDLE = 0,
  WORK,
  WAIT,
} EDMA_STATE;

/**
 * @brief  上层回调函数
 */
typedef void (*USER_Callback)(void *cbinfo); ///< Pointer to \ref EDMA_SignalEvent : Signal EDMA Event.

/**
 * @brief  call back info
 */
typedef struct
{
  osList_t      listNode;
  USER_Callback cb;   // user
  void         *param;
  uint32_t      index_num;
} callBack_Info;

/**
 * @brief  param format
 */
typedef struct
{
  uint32_t para_num : 15; /*搬运长度*/
  uint32_t reserved0 : 16;
  uint32_t int_enable : 1;
} index_CFG;

/**
 * @brief  Index format
 */
typedef struct
{
  uint32_t  source_addr; /*源地址*/
  index_CFG cfg;
  uint32_t  dest_addr; /*目的地址*/
} index_format;

/**
\brief Access structure of EDMA_INFO.
*/
typedef struct _EDMA_INFO
{
  osList_t      listCallBack;
  uint8_t       flags;      // Current EDMA flags
  int32_t       readIndex;  // edma read
  int32_t       writeIndex; // wirte to edma
  uint32_t      indexTotal;
  index_format *indexBuf;     // point to base index buffer
  EDMA_STATE    state;        // EDMA state
  index_format *addIndexAddr; // index buffer ptr
} EDMA_INFO;

/**
\brief Access structure of EDMA_Handle.
*/
typedef struct
{
  EDMA_Res_t pRes;
  EDMA_INFO  info;
  void      *userData;
} EDMA_Handle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief          添加index
 *
 * @param[in]       index               用户配置的索引
 * @param[in]       count               索引的个数
 * @param[in]       cb                  用户的回调
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER    添加失败
 *                  count                成功添加
 ************************************************************************************
 */
int32_t EDMA_AddIndex(index_format *index, int32_t count, callBack_Info *cb);

/**
 ************************************************************************************
 * @brief           获取EDMA控制器状态
 *
 * @param[in]       EDMA            EDMA控制器句柄
 *
 * @return          EDMA_STATE
 * @retval          IDLE            空闲状态
                    WORK            工作状态
                    WAIT            等待状态
 ************************************************************************************
 */
EDMA_STATE EDMA_GetState();
#endif

/** @} */