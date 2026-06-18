
/**  @file
  ycom_adc.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
#ifndef _YOPEN_ADC_H
#define _YOPEN_ADC_H
/*> include header files here*/

#include <stdio.h>
#include "yopen_api_common.h"
#include "yopen_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
*  Enumeration Definition
*========================================================================*/

/****************************  error code about ADC    ***************************/
typedef enum
{
  YOPEN_LCD_SUCCESS  			= 0,
  YOPEN_LCD_INVALID_PARAM_ERR    = 10  | (YOPEN_COMPONENT_LCD << 16),
  YOPEN_LCD_GET_VALUE_ERROR 		= 50  | (YOPEN_COMPONENT_LCD << 16),
  YOPEN_LCD_MEM_ADDR_NULL_ERROR	= 60  | (YOPEN_COMPONENT_LCD << 16),
  YOPEN_LCD_TASK_ERROR,
  YOPEN_LCD_OTHER
} yopen_errcode_lcd_e;

typedef enum {
  SPI_3W_I,       /**< 3线SPI接口类型I */
  SPI_3W_II,      /**< 3线SPI接口类型II */
  SPI_4W_I,       /**< 4线SPI接口类型I */
  SPI_4W_II,      /**< 4线SPI接口类型II */
  MSPI_4W_II,     /**< 多路4线SPI接口类型II */
  INTERFACE_8080, /**< 8080并行接口 */
  LCD_IF_MAX    
} lcd_interface_type_t;

// LCD刷新结束回调函数
typedef void (*yopen_lcd_refresh_callback_t)(void);

typedef struct 
{
  /* data */
  uint8_t     bpp; //LCD像素深度，单位bit 12位rgb444  16位rgb565  18位rgb666 24位rgb888
  uint16_t    width; //LCD宽度，单位像素
  uint16_t    height; //LCD高度，单位像素
  uint16_t    xoffset; //LCD水平偏移，单位像素
  uint16_t    yoffset; //LCD垂直偏移，单位像素
  yopen_lcd_refresh_callback_t refreshCallback; //LCD刷新结束回调函数
  uint32_t freq; //频率，单位Hz
  lcd_interface_type_t interfaceType; //接口类型
}yopen_lcd_config_s;


/**
  @brief  LCD初始化函数
  @param  config 配置参数
  @return 错误码
*/
yopen_errcode_lcd_e yopen_lcd_init(yopen_lcd_config_s *config);

/**
  @brief  LCD更新函数
  @param  buff 要更新的数据
  @param  length 数据长度
  @return 错误码
*/
BOOL yopen_lcd_update(void *buff, uint32_t length);

/**
  @brief  LCD发送命令和数据函数
  @param  cmd 命令
  @param  data 数据 如果没有数据，传NULL
  @param  dataLen 数据长度 如果没有数据，传0
  @return 错误码
*/
void yopen_lcd_cmd_data_send(uint8_t cmd, uint8_t *data, uint8_t dataLen);

#ifdef __cplusplus
} /*"C" */
#endif

#endif




