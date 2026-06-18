/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
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
#include "cm_spi_lcd.h"
#include "cm_lcd.h"
#include <res_tree.h>

#define CM_SPI_LCD_DISPLAY_CMD      0x2C

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
 * @brief spi_lcd驱动初始化
 *
 * @param [in] cfg  spi_lcd配置
 *
 * @return
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 对底层驱动进行初始化，初始化前请先完成对应GPIO引脚的功能复用
 */
int32_t cm_spi_lcd_init(cm_spi_lcd_cfg_t *cfg)
{
    //static int32_t lcd_init = 0;
    cm_gpio_cfg_t key_cfg = {
        .direction = CM_GPIO_DIRECTION_OUTPUT,
        .pull = CM_GPIO_PULL_DOWN
    };
    if(cfg == NULL)
    {
        osPrintf("cfg NULL");
        return -1;
    }
    if(cfg->spi_dev != CM_SPI_DEV_2)
    {
        osPrintf("cfg->spi_lcd_dev err:%d", cfg->spi_dev);
        return -2;
    }
    if((cfg->dcx_gpio != CM_GPIO_NUM_8) && (cfg->dcx_gpio != CM_GPIO_NUM_27) && (cfg->dcx_gpio != CM_GPIO_NUM_28) && (cfg->dcx_gpio != CM_GPIO_NUM_MAX))
    {
        osPrintf("cfg->dcx_gpio err:%d", cfg->dcx_gpio);
        return -3;
    }
    if(cfg->rst_gpio >= CM_GPIO_NUM_MAX)
    {
        osPrintf("cfg->rst_gpio err:%d", cfg->rst_gpio);
        return -4;
    }

    cm_gpio_init(cfg->rst_gpio, &key_cfg);
    cm_gpio_set_level(cfg->rst_gpio, CM_GPIO_LEVEL_HIGH);
    osDelay(50);
    cm_gpio_set_level(cfg->rst_gpio, CM_GPIO_LEVEL_LOW);
    osDelay(100);
    cm_gpio_set_level(cfg->rst_gpio, CM_GPIO_LEVEL_HIGH);
    osDelay(150);

    lcd_power_control(TRUE);
    if (g_lcd_is_init == FALSE)
    {
        osMemset(&g_lcd_handle, 0, sizeof(LCD_Handle));
        g_lcd_handle.pRes = DRV_RES(LCD, 0);
        LCD_Initialize(&g_lcd_handle);
        LCD_PowerControl(&g_lcd_handle, DRV_POWER_FULL);
        g_lcd_is_init = TRUE;
    }

    if(cfg->dcx_gpio == CM_GPIO_NUM_MAX)
    {
        LCD_Control(&g_lcd_handle, LCD_SPI_DCX, 0);
        LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, 9);
    }
    else
    {
        cm_gpio_init(cfg->dcx_gpio, &key_cfg);
        LCD_Control(&g_lcd_handle, LCD_SPI_DCX, 1);
        LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, 8);
    }

    LCD_Control(&g_lcd_handle, LCD_SPI_BIDIR, 0);
    LCD_Control(&g_lcd_handle, LCD_SAMP_SEL, 1);

    return 0;
}

/**
 * @brief spi_lcd驱动去初始化
 *
 * @return
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 释放底层驱动资源
 */
int32_t cm_spi_lcd_deinit(void)
{
    if (g_lcd_is_init == TRUE)
    {
        LCD_PowerControl(&g_lcd_handle, DRV_POWER_OFF);
        LCD_Uninitialize(&g_lcd_handle);
        lcd_power_control(FALSE);
        osPrintf("cm_lcd_deinit ok\r\n");
        g_lcd_is_init = FALSE;
        return 0;
    }
    else
    {
        osPrintf("cm_lcd_deinit is deinited\r\n");
        return -1;
    }
}

/**
 * @brief spi_lcd获取ID
 *
 * @param [in]  cmd     读取id的命令数组
 * @param [in]  cmd_len 命令长度
 * @param [out] id_buf  保存id的缓存地址
 * @param [in]  buf_len 缓存buf长度
 *
 * @return
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details cmd传入命令数组，若读取命令只有一个字节，则cmd_len传入1即可
 */
int32_t cm_spi_lcd_read_id(uint8_t *cmd, uint32_t cmd_len, uint8_t *id_buf, uint32_t buf_len)
{
    if ((cmd == NULL) || (cmd_len != 1) || (buf_len == 0) || (id_buf == NULL))
    {
        return -1;
    }
    return LCD_ReadParam(&g_lcd_handle, *cmd, id_buf, buf_len);
}

/**
 * @brief 发送命令
 *
 * @param [in] cmd  命令数组
 * @param [in] len  命令长度
 *
 * @return
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 可用于连续发送多个命令，cmd传入命令数组，len传入要发送的命令个数，如果要发送单个命令，len传入1即可
            目前仅支持单命令发送，len需要传入1
 */
int32_t cm_spi_lcd_write_cmd(uint8_t *cmd, uint32_t len)
{
    if ((len != 1) || (cmd == NULL))
    {
        return -1;
    }
    osPrintf("cm_spi_lcd_write_cmd [%02X]\r\n",*cmd);
    g_lcd_cmd = *cmd;
    return LCD_WriteParam(&g_lcd_handle, g_lcd_cmd, NULL, 0);
}

/**
 * @brief 发送数据
 *
 * @param [in] data 需要发送的数据
 * @param [in] len  数据长度
 *
 * @return
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 可用于连续发送数据，data传入数据，len传入数据长度
            通过前一次cm_spi_lcd_write_cmd传入的命令来判断数据类型
            前一次传入命令为0x2C时，此函数传入的数据被认定为屏显数据
            前一次传入命令不为0x2C时，此函数传入的数据被认定为参数数据
 */
int32_t cm_spi_lcd_write_data(uint8_t *data, uint32_t len)
{
    if ((len == 0) || (data == NULL))
    {
        return -1;
    }
    if (g_lcd_cmd == CM_SPI_LCD_DISPLAY_CMD)
    {
        return LCD_SendData(&g_lcd_handle, g_lcd_cmd, data, len);
    }
    else
    {
        return LCD_WriteParam(&g_lcd_handle, g_lcd_cmd, data, len);
    }
}

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

    LCD_Control(&g_lcd_handle, LCD_SPI_DCX, LCD_SPI_DCX_VAL);
    LCD_Control(&g_lcd_handle, LCD_READ_DMY_CYC, LCD_READ_DMY_CYC_VAL);

    LCD_Control(&g_lcd_handle, LCD_SPI_BIDIR, LCD_SPI_BIDIR_VAL);
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

void cm_lcd_write_cmd_and_data(unsigned char cmd, unsigned char *data, int len)
{
    int ret = LCD_WriteParam(&g_lcd_handle, (uint8_t)cmd, (uint8_t *)data, (uint8_t)len);
    if(ret != 0)
    {
        osPrintf("[CMO_LCD] cm_lcd_write_cmd_and_data err ret %d\r\n",ret);
    }
}


