/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_gpio.h
 *
 * @brief       GPIO驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef _DRV_GPIO_H
#define _DRV_GPIO_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>


/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/**
 * @addtogroup Gpio
 */

/**@{*/

/**
 * @brief pin rising sample
 */
#define PIN_IRQ_MODE_RISING         0x00
/**
 * @brief pin falling sample
 */
#define PIN_IRQ_MODE_FALLING        0x01
/**
 * @brief pin falling and rising sample
 */
#define PIN_IRQ_MODE_RISING_FALLING 0x02
/**
 * @brief pin high level sample
 */
#define PIN_IRQ_MODE_HIGH_LEVEL     0x03
/**
 * @brief pin low level sample
 */
#define PIN_IRQ_MODE_LOW_LEVEL      0x04


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

 /**
   * @brief gpio direction
   */
typedef enum
{
  GPIO_INPUT  = 0x0,
  GPIO_OUTPUT = 0x1,
} GPIO_DIR;

 /**
   * @brief gpio level
   */
typedef enum
{
  GPIO_LOW  = 0,
  GPIO_HIGH = 1,
} GPIO_LEVEL;
  

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           设置GPIO方向
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       dir            方向
 *
 * @return          void
 * @retval          null
 ************************************************************************************
 */
void GPIO_SetDir(const struct PIN_Res *pResource, GPIO_DIR dir);

/**
 ************************************************************************************
 * @brief           获取GPIO方向
 *
 * @param[in]       pResource      PIN资源句柄
 *
 * @return          GPIO_DIR
 * @retval          GPIO_INPUT or GPIO_OUTPUT      方向
 ************************************************************************************
*/
GPIO_DIR GPIO_GetDir(const struct PIN_Res *pResource);

/**
 ************************************************************************************
 * @brief           获取GPIO电平
 *
 * @param[in]       pResource      PIN资源句柄
 *
 * @return          GPIO_LEVEL 
 * @retval          
 ************************************************************************************
*/
GPIO_LEVEL GPIO_Read(const struct PIN_Res *pResource);


/**
 ************************************************************************************
 * @brief           设置GPIO输出电平
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       level          电平
 *
 * @return          void
 * @retval          
 ************************************************************************************
*/
void GPIO_Write(const struct PIN_Res *pResource, GPIO_LEVEL level);

/**
 ************************************************************************************
 * @brief           注册GPIO中断
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       mode           中断触发方式
 * @param[in]       hdr            中断处理函数
 * @param[in]       args           中断处理入参
 *
 * @return          GPIO_LEVEL 
 * @retval          
 ************************************************************************************
*/
int32_t GPIO_AttachIrq(const struct PIN_Res *pResource, uint32_t mode, void (*hdr)(void *args), void *args);

/**
 ************************************************************************************
 * @brief           Detach GPIO中断
 *
 * @param[in]       pResource      PIN资源句柄

 *
 * @return          void 
 * @retval           
 ************************************************************************************
*/
void GPIO_DetachIrq(const struct PIN_Res *pResource);

/**
 ************************************************************************************
 * @brief           修改 GPIO中断模式
 *
 * @param[in]       pResource      PIN资源句柄

 *
 * @return          int32_t 
 * @retval           
 ************************************************************************************
*/
int32_t GPIO_ModifyIrqMode(const struct PIN_Res *pResource, uint32_t mode);
/**
 ************************************************************************************
 * @brief           使能GPIO中断
 *
 * @param[in]       pResource      PIN资源句柄
 * @param[in]       enable         使能
 *
 * @return          int32_t 
 * @retval          OS_ENOSYS     未attach
 *                  osError      错误
 *                  osOK        成功
 ************************************************************************************
*/
int32_t GPIO_IrqEnable(const struct PIN_Res *pResource, uint32_t enable);

/** @} */

#endif
