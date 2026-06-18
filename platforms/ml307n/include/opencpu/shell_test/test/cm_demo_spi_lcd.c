/**
 *  @file    cm_demo_spi_lcd.c
 *  @brief   OpenCPU spi_lcd示例
 *  @copyright copyright © 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot1325
 *  @date 2025/03/25
 */

 /****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cm_iomux.h"
#include "cm_spi.h"
#include "cm_demo_uart.h"
#include "cm_demo_spi_lcd.h"
#include "cm_gpio.h"
#include "cm_wifi.h"
#include "cm_mem.h"
#include "cm_spi_lcd.h"
#include "qrcode.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
 
#define CM_SPI_LCD_DEMO_LOG   cm_demo_printf

#ifndef min
#define min(A,B) ((A) <= (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) < (B) ? (B) : (A))
#endif

/* 选择使用的spi设备和gpio */
#define LCD_SPI_DEV     CM_SPI_DEV_0
#define LCD_DCX_GPIO    CM_GPIO_NUM_50
#define LCD_DCX_PIN     CM_IOMUX_PIN_214
#define LCD_RST_GPIO    CM_GPIO_NUM_49
#define LCD_RST_PIN     CM_IOMUX_PIN_213
#define LCD_BKL_GPIO    CM_GPIO_NUM_16
#define LCD_BKL_PIN     CM_IOMUX_PIN_209

/* 选择生成二维码的参数，详细说明在third-party\QRCode\QRCode\README.md */
#define LCD_QR_SIZE     57
#define LCD_QR_VER      10
#define LCD_QR_ERROR    ECC_HIGH

/* 配置屏幕分辨率 */
#define LCD_WIDTH  280
#define LCD_HEIGHT 240

#define PSM_MOD_WLAN_SECURITY  "wlan_security"

/****************************************************************************
 * Private Types
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
 
 
/****************************************************************************
 * Private Data
 ****************************************************************************/
 
static uint16_t s_lcdbuffer[LCD_WIDTH * LCD_HEIGHT] = {0};
static QRCode ricmoo = {0};
static uint8_t ricmooBytes[LCD_QR_SIZE * LCD_QR_SIZE] = {0};
static char wifiinfo[128] = {0};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* 配置lcd所需要的gpio引脚的功能复用 */
static void __cm_spi_lcd_gpio_init(void)
{
    cm_iomux_set_pin_func(LCD_RST_PIN, CM_IOMUX_FUNC_FUNCTION3);
    cm_iomux_set_pin_func(LCD_DCX_PIN, CM_IOMUX_FUNC_FUNCTION3);
    cm_iomux_set_pin_func(LCD_BKL_PIN, CM_IOMUX_FUNC_FUNCTION1);

    cm_gpio_cfg_t cfg = {
        .direction = CM_GPIO_DIRECTION_OUTPUT,
        .pull = CM_GPIO_PULL_DOWN
    };
    cm_gpio_init(LCD_DCX_GPIO, &cfg);
    
    cm_gpio_init(LCD_RST_GPIO, &cfg); 
    
    cm_gpio_init(LCD_BKL_GPIO, &cfg);
    
    cm_gpio_set_level(LCD_BKL_GPIO, CM_GPIO_LEVEL_LOW);
}

/* 配置显示区域 */
static void __cm_spi_lcd_test_set_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    cm_spi_lcd_write_cmd(0x2a);
    cm_spi_lcd_write_data(x_start >> 8);
    cm_spi_lcd_write_data(x_start);
    cm_spi_lcd_write_data(x_end >> 8);
    cm_spi_lcd_write_data(x_end);
    cm_spi_lcd_write_cmd(0x2b);
    cm_spi_lcd_write_data(y_start >> 8);
    cm_spi_lcd_write_data(y_start);
    cm_spi_lcd_write_data(y_end >> 8);
    cm_spi_lcd_write_data(y_end);

    cm_spi_lcd_write_cmd(0x2c);
}

/* 刷屏 */
static void __cm_spi_lcd_test_show_color(void)
{
    uint16_t width = LCD_WIDTH;
    uint16_t height = LCD_HEIGHT;

    /* 以四个像素点表示一位二维码数据，并且将二维码图片移至屏幕中间 */
    int offset = (LCD_HEIGHT/2 - LCD_QR_SIZE) * LCD_WIDTH + LCD_WIDTH/2 - LCD_QR_SIZE;
    uint8_t x = 0, y = 0;
    
    for (y = 0; y < ricmoo.size; y++)
    {
        for (x = 0; x < ricmoo.size; x++)
        {
            if (qrcode_getModule(&ricmoo, x, y))
            {
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width)] = 0x0000;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + width] = 0x0000;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + 1] = 0x0000;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + 1 + width] = 0x0000;
            }
            else
            {
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width)] = 0xffff;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + width] = 0xffff;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + 1] = 0xffff;
                s_lcdbuffer[offset + (x * 2) + ((y * 2) * width) + 1 + width] = 0xffff;
            }
        }
    }

    __cm_spi_lcd_test_set_area(0, 0, width - 1, height - 1);

    cm_spi_lcd_write_buf((uint8_t *)s_lcdbuffer, height * width * 2);
}

static void __cm_spi_lcd_read_id(void)
{
    uint8_t id_buf[4] = {0};
    cm_spi_lcd_read_id(0x04, id_buf, 4);
    CM_SPI_LCD_DEMO_LOG("lcd read id: %x %x %x %x", id_buf[0], id_buf[1], id_buf[2], id_buf[3]);
}

static void __cm_spi_lcd_test(void)
{
    cm_spi_lcd_init(LCD_SPI_DEV, LCD_DCX_GPIO, LCD_RST_GPIO);

    /* 在cm_spi_lcd_init执行之后再进行GPIO初始化 */
    __cm_spi_lcd_gpio_init();

    cm_gpio_set_level(LCD_RST_GPIO, CM_GPIO_LEVEL_LOW);
    
    osDelay(4);
    
    cm_gpio_set_level(LCD_RST_GPIO, CM_GPIO_LEVEL_HIGH);
    
    osDelay(24);

    __cm_spi_lcd_read_id();
    cm_spi_lcd_write_cmd(0x11); //sleep out
    osDelay(120);

    cm_spi_lcd_write_cmd(0x36); //RGB顺序，显示方向等设置由此命令调整
    cm_spi_lcd_write_data(0x60);
    
    cm_spi_lcd_write_cmd(0x3A);  
    cm_spi_lcd_write_data(0x65);

    cm_spi_lcd_write_cmd(0xB2);
    cm_spi_lcd_write_data(0x0C);
    cm_spi_lcd_write_data(0x0C);
    cm_spi_lcd_write_data(0x00);
    cm_spi_lcd_write_data(0x33);
    cm_spi_lcd_write_data(0x33);
    cm_spi_lcd_write_cmd(0xB7);
    cm_spi_lcd_write_data(0x72);

    cm_spi_lcd_write_cmd(0xBB);
    cm_spi_lcd_write_data(0x3D);
    cm_spi_lcd_write_cmd(0xC0);
    cm_spi_lcd_write_data(0x2C);
    cm_spi_lcd_write_cmd(0xC2);
    cm_spi_lcd_write_data(0x01);
    cm_spi_lcd_write_cmd(0xC3);
    cm_spi_lcd_write_data(0x19);
    cm_spi_lcd_write_cmd(0xC4);
    cm_spi_lcd_write_data(0x20);
    cm_spi_lcd_write_cmd(0xC6);
    cm_spi_lcd_write_data(0x0F);
    cm_spi_lcd_write_cmd(0xD0);
    cm_spi_lcd_write_data(0xA4);
    cm_spi_lcd_write_data(0xA1);
   
    cm_spi_lcd_write_cmd(0xE0);
    cm_spi_lcd_write_data(0xD0);
    cm_spi_lcd_write_data(0x04);
    cm_spi_lcd_write_data(0x0D);
    cm_spi_lcd_write_data(0x11);
    cm_spi_lcd_write_data(0x13);
    cm_spi_lcd_write_data(0x2B);
    cm_spi_lcd_write_data(0x3F);
    cm_spi_lcd_write_data(0x54);
    cm_spi_lcd_write_data(0x4C);
    cm_spi_lcd_write_data(0x18);
    cm_spi_lcd_write_data(0x0D);
    cm_spi_lcd_write_data(0x0B);
    cm_spi_lcd_write_data(0x1F);
    cm_spi_lcd_write_data(0x23);
    
    cm_spi_lcd_write_cmd(0xE1);
    cm_spi_lcd_write_data(0xD0);
    cm_spi_lcd_write_data(0x04);
    cm_spi_lcd_write_data(0x0C);
    cm_spi_lcd_write_data(0x11);
    cm_spi_lcd_write_data(0x13);
    cm_spi_lcd_write_data(0x2C);
    cm_spi_lcd_write_data(0x3F);
    cm_spi_lcd_write_data(0x44);
    cm_spi_lcd_write_data(0x51);
    cm_spi_lcd_write_data(0x2F);
    cm_spi_lcd_write_data(0x1F);
    cm_spi_lcd_write_data(0x1F);
    cm_spi_lcd_write_data(0x20);
    cm_spi_lcd_write_data(0x23);

    __cm_spi_lcd_test_show_color();

    cm_spi_lcd_write_cmd(0x29);  //display on
}

static uint32_t __hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    else
    {
        return 0;  // 非法字符按0处理
    }
}

static int __hex_to_ascii(const char *hex_str, char *result, int result_len)
{
    if (hex_str == NULL || result == NULL)
    {
        return -1;  // 参数无效
    }

    int hex_len = strlen(hex_str);

    if (hex_len % 4 != 0)
    {
        return -2;  // 输入长度非法
    }

    // 计算目标字符串长度（每组保留2个字符）
    int required_len = hex_len / 2;

    if (result_len < required_len + 1)
    {
        return -3;  // 缓冲区不足
    }

    int i = 0;
    int j = 0;
    for (i = 0, j = 0; i < hex_len; i += 4, j++)
    {
        // 提取每组的后两位字符（去掉前导00）
        char high_char = hex_str[i + 2];
        char low_char = hex_str[i + 3];

        // 将两位十六进制字符转换为数值
        unsigned int high = __hex_char_to_int(high_char);
        unsigned int low = __hex_char_to_int(low_char);

        // 组合成ASCII码值
        unsigned int codepoint = (high << 4) | low;

        // 存储结果
        result[j] = (char)codepoint;
    }

    result[required_len] = '\0';  // 添加字符串结束符

    return 0;  // 成功
}

static int __get_wifi_ssid_str(char *ssid_buf, int buf_len)
{
    char *str = psm_get_wrapper(PSM_MOD_WLAN_SECURITY, NULL, "ssid");
    __hex_to_ascii(str, ssid_buf, buf_len);
    cm_free(str);
    return 0;
}
/****************************************************************************
 * Public Functions
 ****************************************************************************/

void cm_test_spi_lcd(EmbeddedCli *cli, char *args, void *context)
{
    char wifi_ssid[64] = {0};
    __get_wifi_ssid_str(wifi_ssid, 64);
    char * wifi_psk = psm_get_wrapper(PSM_MOD_WLAN_SECURITY, NULL, "WPA-PSK.key");
    CM_SPI_LCD_DEMO_LOG("wifi ssid:%s, psk:%s\n", wifi_ssid, wifi_psk);

    snprintf(wifiinfo, sizeof(wifiinfo), "WIFI:T:WPA;S:%s;P:%s;", wifi_ssid, wifi_psk);
    qrcode_initText(&ricmoo, ricmooBytes, LCD_QR_VER, LCD_QR_ERROR, (const char *)wifiinfo);
    __cm_spi_lcd_test();
    return;
}

