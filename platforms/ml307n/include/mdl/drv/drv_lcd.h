/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_lcd.h
 *
 * @brief       LCD驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <os.h>
#include <os_def.h>
#include <drv_dma.h>
#if defined(OS_USING_PM)
#include <psm_wakelock.h>
#endif

/**
 * @addtogroup Lcd
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define RGB_5_6_5_VAL(r, g, b)          ((((r) >> 3) << 11) | (((g) >> 2) << 5) | (((b) >> 3) & 0x1f))
#define LCD_SWAP_ENDIAN(val)            ((((val) & 0xff) << 8) | (((val) & 0xff00) >> 8))
#define LCD_RGB565_SWAP_ENDIAN(r, g, b) (LCD_SWAP_ENDIAN(RGB_5_6_5_VAL(r, g, b)))
/*
* RGB_5_6_5_VAL(255, 0, 0); //红色
* RGB_5_6_5_VAL(0, 0, 255); //蓝色
*/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
 * @brief LCD操作句柄
 */
typedef struct
{
  const struct LCD_Res  *pRes;
  
  DMA_ChHandle *transDma;
  DMA_LliDesc  *transDmaCfg;
  uint32_t transLen;
  osThread_t    transThread;
  
  bool inProcess;
  bool inTransData;

  struct osMutex lock;

#if defined(OS_USING_PM)
    WakeLock wakeLock;
#endif
  // High level data & callback;
  void  *userData;
  void (*SendCallback)(void *userData);
} LCD_Handle;

/**
 * @brief LCD传输协议控制
 */
typedef enum {
    LCD_SPI_BIDIR, /*!<SPI data bus signal definition
                    1: bi-dir DIO configuration
                    0: uni-dir DIN/DOUT configuration */
    LCD_SPI_DCX,  /*!<Dedicated D/CX
                    1: dedicated D/CX
                    0: D/CX encoded in DOUT */
    LCD_SAMP_SEL, /*!<Sample condition selection
                    0: sample input data at next rise edge of SCLK
                    1: sample input data at next fall edge of SCLK */
    LCD_READ_DMY_CYC,  /*!<Dummy cycle in read operation
                       0: no dummy cycle in read operation
                       others(1~15):dummy cycle setting in read operation */
    LCD_RGB565_SWAP, /*!<Swap RGB565 hi/lo byte. */
} LCD_ControlW;

/**
 * @brief LCD模拟器RGB格式
 */
typedef enum {
    RGB_4_4_4 = 3,
    RGB_5_6_5 = 5,
    RGB_6_6_6 = 6,
} LCD_EmuRgbFormat;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           初始化LCD控制器
 *
 * @param[in]       handle                     LCD控制器句柄
 *
 * @return          初始化结果.
 * @retval          ==0                        初始化成功
 ************************************************************************************
 */
extern int32_t LCD_Initialize(LCD_Handle *handle);  

/**
 ************************************************************************************
 * @brief           去初始化LCD控制器
 *
 * @param[in]       handle                     LCD控制器句柄
 *
 * @return          无.
 ************************************************************************************
 */
extern void LCD_Uninitialize(LCD_Handle *handle);

/**
 ************************************************************************************
 * @brief           LCD控制器上下电，时钟开关等
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       state                      功耗模式
 *
 * @return          配置结果.
 * @retval          ==0                        配置成功
 *************************************************************************************
 */
extern int32_t LCD_PowerControl(LCD_Handle *handle, DRV_POWER_STATE state);

/**
 *************************************************************************************
 * @brief           配置LCD控制器
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       ctrl                       配置控制字
 * @param[in]       val                        对应的配置值
 *
 * @return          配置结果.
 * @retval          ==0                        配置成功
 *************************************************************************************
 */
extern void    LCD_Control(LCD_Handle *handle, LCD_ControlW ctrl, uint32_t val);

/**
 *************************************************************************************
 * @brief           读LCD参数
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       cmd                        读命令
 * @param[in]       buf                        读取参数的存放地址
 * @param[in]       len                        读取参数长度
 *
 * @return          读取结果.
 * @retval          ==0                        读取成功
 * @note            阻塞方式，直到读取成功后函数返回；
 *************************************************************************************
 */
extern int32_t LCD_ReadParam(LCD_Handle *handle, uint8_t cmd, uint8_t *buf, uint8_t len);

/**
 *************************************************************************************
 * @brief           写LCD参数
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       cmd                        写命令
 * @param[in]       buf                        写参数的存放地址
 * @param[in]       len                        写参数长度
 *
 * @return          写入结果.
 * @retval          ==0                        写入成功
 * @note            阻塞方式，直到写入成功后函数返回；
 *************************************************************************************
 */
extern int32_t LCD_WriteParam(LCD_Handle *handle, uint8_t cmd, uint8_t *buf, uint8_t len);

/**
 *************************************************************************************
 * @brief           发送LCD屏显数据，非阻塞方式
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       cmd                        发数据命令
 * @param[in]       buf                        数据的存放地址
 * @param[in]       len                        数据长度
 *
 * @return          写入结果.
 * @retval          ==0                        写入成功
 * @note            非阻塞方式，未等发送，函数立即返回
 *                  发送完成后，通过handle->SendCallback通知用户
 *************************************************************************************
 */
extern int32_t LCD_SendData(LCD_Handle *handle, uint8_t cmd, uint8_t *buf, uint32_t len);

/**
 *************************************************************************************
 * @brief           查询发送数据长度
 *
 * @param[in]       handle                     LCD控制器句柄
 *
 * @return          DMA搬运已发送数据长度.
 * @retval          >=0    已发送数据长度
 *************************************************************************************
 */
extern int32_t LCD_GetSendCount(LCD_Handle *handle);

/**
 *************************************************************************************
 * @brief           配置LCD模拟器
 *
 * @param[in]       handle                     LCD控制器句柄
 * @param[in]       format                     RGB格式
 * @param[in]       width                      屏宽
 * @param[in]       height                     屏高
 *
 * @return          无
 *************************************************************************************
 */
extern void    LCD_SetEmu(LCD_Handle *handle, LCD_EmuRgbFormat format, uint16_t width, uint16_t height);

#endif /* End of _DRV_LCD_H_*/
/** @} */