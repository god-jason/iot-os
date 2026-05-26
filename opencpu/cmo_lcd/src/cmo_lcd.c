/**
 *************************************************************************************
 * 版权所有 (C) 2023, 南京创芯慧联技术有限公司
 * 保留所有权利。
 *
 * @file        drv_lcd.c
 *
 * @brief       LCD驱动实现.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "os.h"
#include "drv_lcd.h"
#include "lcd_priv.h"
#include "cm_lcd.h"
#include <res_tree.h>



/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define CMD_RDDID                           (0x04)
#define CMD_CASET                           (0x2A)
#define CMD_RASET                           (0x2B)
#define CMD_RAMWR                           (0x2C)
#define CMD_MADCTL                          (0x36)

/************************************************************************************
 *                                 全局变量
 ************************************************************************************/
static LCD_Handle g_lcd_handle = {0};
static bool g_lcd_is_init = FALSE;
static uint8_t g_lcd_cmd = 0;

void lcd_power_control(bool en)
{
    if(en) {
        EXTLDO_Acquire(EXT_LDO_1V8);
        EXTLDO_Acquire(EXT_LDO_2V8);
    }
    else {
        EXTLDO_Release(EXT_LDO_1V8);
        EXTLDO_Release(EXT_LDO_2V8);
    }
}


/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
 /**
 * @brief lcd 驱动初始化
 *
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_init(void)
{
    //static int32_t lcd_init = 0;
    lcd_power_control(TRUE);
    if (g_lcd_is_init == FALSE)
    {
        osMemset(&g_lcd_handle, 0, sizeof(LCD_Handle));
        g_lcd_handle.pRes = DRV_RES(LCD, 0);
        LCD_Initialize(&g_lcd_handle);
        LCD_PowerControl(&g_lcd_handle, DRV_POWER_FULL);
        g_lcd_is_init = TRUE;
    }

#if 0
    if(ST7789V_PIN_RES_DCX(g_st7782vPinCfg) == NULL)
    {
        LCD_Control(&g_lcd_handle, LCD_SPI_DCX, 0);
        LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, 9);
    }
    else
    {
        LCD_Control(&g_lcd_handle, LCD_SPI_DCX, 1);
        LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, 8);
    }
#else
    LCD_Control(&g_lcd_handle, LCD_SPI_DCX, LCD_SPI_DCX_VAL);
    LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, LCD_READ_DMY_CYC_VAL);
#endif

#if 0
    if(ST7789V_PIN_RES_SDO(g_st7782vPinCfg) && ST7789V_PIN_RES_SDI(g_st7782vPinCfg))
    {
        LCD_Control(&g_lcd_handle, LCD_SPI_BIDIR, 0);
    }
    else
    {
        LCD_Control(&g_lcd_handle, LCD_SPI_BIDIR, 1);
    }
#else
    LCD_Control(&g_lcd_handle, LCD_SPI_BIDIR, LCD_SPI_BIDIR_VAL);
#endif
    LCD_Control(&g_lcd_handle, LCD_SAMP_SEL, 1);
    // LCD_Control(&g_lcd_handle, LCD_RGB565_SWAP, 1);

    return;
}

/**
 * @brief lcd 驱动去初始化
 *
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_deinit(void)
{
    if (g_lcd_is_init == TRUE)
    {
        LCD_PowerControl(&g_lcd_handle, DRV_POWER_OFF);
        LCD_Uninitialize(&g_lcd_handle);
        lcd_power_control(FALSE);
        osPrintf("cm_lcd_deinit ok\r\n");
        g_lcd_is_init = FALSE;
    }
    else
    {
        osPrintf("cm_lcd_deinit is deinited\r\n");
    }

}

/**
 * @brief lcd 获取ID
 *
 *
 * @return
 *   > 0 - lcd ID \n
 *   = 0 - 失败
 *
 * @details 只适配ST7735S
 */
uint32_t cm_lcd_read_id(void)
{
    uint32_t id = 0;
    LCD_ReadParam(&g_lcd_handle, CMD_RDDID, (uint8_t *)&id, 3);
    return id;
}

/**
 * @brief 配置时发送cmd
 *
 * @param [in] cmd 需要发送的命令
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_write_cmd(unsigned char cmd)
{
    g_lcd_cmd = cmd;
    LCD_WriteParam(&g_lcd_handle, cmd,NULL, 0);
    //returm LCD_WriteParam(LCD_Handle *handle, uint8_t cmd, uint8_t *buf, uint8_t len);
}
#if 0
/**
 * @brief 配置时发送data
 *
 * @param [in] data 需要发送的数据
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_write_data(unsigned char data)
{
    return LCD_WriteParam(&g_lcd_handle, g_lcd_cmd, &data, 1);
}
#endif

/**
 * @brief 发送刷屏数据
 *
 * @param [in] data 需要发送的数据
 * @param [in] len  数据长度
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_write_buf(unsigned char *data, int len)
{
    return LCD_SendData(&g_lcd_handle, g_lcd_cmd, data, len);
}

#if 1
void cm_lcd_write_cmd_and_data(unsigned char cmd, unsigned char *data, int len)
{
    int ret = LCD_WriteParam(&g_lcd_handle, (uint8_t)cmd, (uint8_t *)data, (uint8_t)len);
    if(ret != 0)
    {
        osPrintf("[CMO_LCD] cm_lcd_write_cmd_and_data err ret %d\r\n",ret);
    }
}
#endif

