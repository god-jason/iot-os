/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_pin.h
 *
 * @brief       pin驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef _DRV_PIN_H
#define _DRV_PIN_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>
#include "drv_gpio.h"

/**
 * @addtogroup PinMux
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
  
 /**
   * @brief gpio pull up or pull down
   */
typedef enum
{
  PULL_NONE,
  PULL_DOWN,
  PULL_UP,
} PIN_PL;

 /**
   * @brief pin driver capablity
   */
typedef enum
{
  PIN_DRV_0,
  PIN_DRV_1,
  PIN_DRV_2,
  PIN_DRV_3,
} PIN_DR;

 /**
   * @brief pin slew rate
   */
typedef enum
{
  SLEW_RATE_FAST,
  SLEW_RATE_LOW,
} PIN_SLEW_RATE;

/**
   * @brief PIN input or output
   */
typedef enum
{
  PIN_INPUT_DISABLE,
  PIN_INPUT_ENABLE,
} PIN_IE; 

/**
   * @brief PIN multi mux res
   */
typedef struct PIN_MultiMux
{
    const struct PIN_Res *res;
    uint8_t gpioMux;
    uint8_t specMux;
} PIN_MultiMux_t;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           设置pin复用
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       func           复用功能
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetMux(const struct PIN_Res *pResource, uint8_t func);

/**
 ************************************************************************************
 * @brief           获取pin复用
 *
 * @param[in]       pResource      PIN资源句柄
 *
 * @return          复用功能
 * @retval          uint32_t
 ************************************************************************************
*/
uint32_t PIN_GetMux(const struct PIN_Res *pResource);

/**
 ************************************************************************************
 * @brief           获取pin上下拉
 *
 * @param[in]       pResource      PIN资源句柄
 *
 * @return          获取上拉、下拉、浮空
 * @retval          PIN_PL
 ************************************************************************************
*/
PIN_PL PIN_GetPull(const struct PIN_Res *pResource);

/**
 ************************************************************************************
 * @brief           设置施密特触发
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       is             是否使能施密特触发
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetIS(const struct PIN_Res *pResource, uint8_t is);

/**
 ************************************************************************************
 * @brief           设置上下拉使能
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       pull           上下拉
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetPull(const struct PIN_Res *pResource, PIN_PL pull);

/**
 ************************************************************************************
 * @brief           设置驱动能力
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       drvCap         驱动等级
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetDrCap(const struct PIN_Res *pResource, PIN_DR drvCap);

/**
 ************************************************************************************
 * @brief           设置slew rate
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       drvCap         slew rate
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetSlewRate(const struct PIN_Res *pResource, PIN_SLEW_RATE slewRate);


/**
 ************************************************************************************
 * @brief           设置输入还是输出
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       ie             input enable
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PIN_SetIE(const struct PIN_Res *pResource, PIN_IE ie);

#endif
/** @} */