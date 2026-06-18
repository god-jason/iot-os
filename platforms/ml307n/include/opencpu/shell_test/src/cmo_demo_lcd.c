#ifdef CM_DEMO_LCD_SUPPORT
#ifdef OS_USING_SHELL
/**
 * @file        cm_demo_lcd.c
 * @brief       OpenCPU LCD测试例程，当前仅在ST7735S上调试通过
 * @copyright   Copyright © 2023 China Mobile IOT. All rights reserved.
 */


 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>
#include <string.h>
#include "cm_lcd.h"
#include "cm_iomux.h"
#include "cm_gpio.h"
#include "cm_mem.h"
#include <os.h>
#include <cmsis_os2.h>
#include <nr_micro_shell.h>





/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
 #define cm_demo_printf osPrintf

#define SPILCD_TE_PIN               CM_IOMUX_PIN_39                          // SWD0_SWDIO
#define SPILCD_BACKLIGHT_PIN        CM_IOMUX_PIN_54                          // PD_GPIO_2
#define SPILCD_RESET_PIN            CM_IOMUX_PIN_38                          // SWD0_SWCLK
// #define SPILCD_DCX_PIN              CM_IOMUX_PIN_28                          // PD_GPIO_6
#define SPILCD_CSX_PIN              CM_IOMUX_PIN_87                          // PD_GPIO_12
#define SPILCD_SCL_PIN              CM_IOMUX_PIN_76                          // PD_GPIO_13
#define SPILCD_SDI_PIN              CM_IOMUX_PIN_86                          // PD_GPIO_14
#define SPILCD_SDO_PIN              CM_IOMUX_PIN_77                          // PD_GPIO_15

/****************************************************************************
 * Private Types
 ****************************************************************************/



/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
//LCD_Handle g_lcd_handle = {0};

/****************************************************************************
 * Private Functions
 ****************************************************************************/


static void cm_st7789v_pin_mux_init(void)
{
    cm_iomux_set_pin_func(SPILCD_CSX_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_33_MUX_SPI_LCD_CSX
    cm_iomux_set_pin_func(SPILCD_SCL_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_34_MUX_SPI_LCD_SCL
    cm_iomux_set_pin_func(SPILCD_SDO_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_36_MUX_SPI_LCD_SDO
    cm_iomux_set_pin_func(SPILCD_SDI_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_35_MUX_SPI_LCD_SDI
    cm_iomux_set_pin_func(SPILCD_BACKLIGHT_PIN, CM_IOMUX_FUNC_FUNCTION0); //PIN_23_MUX_GPIO
    cm_gpio_set_level(SPILCD_BACKLIGHT_PIN, 0);
    cm_gpio_set_direction(SPILCD_BACKLIGHT_PIN, CM_GPIO_DIRECTION_OUTPUT);
    cm_iomux_set_pin_func(SPILCD_RESET_PIN, CM_IOMUX_FUNC_FUNCTION0); //SWD0_SWCLK
    cm_gpio_set_level(SPILCD_RESET_PIN, 0);
    cm_gpio_set_direction(SPILCD_RESET_PIN, CM_GPIO_DIRECTION_OUTPUT);

}
static void cm_st7789v_pin_mux_deinit(void)
{
    //cm_gpio_set_level(SPILCD_RESET_PIN, 1);
    cm_gpio_set_level(SPILCD_CSX_PIN, 1);
}

static void cm_lcd_test_gpio_init(cm_gpio_num_e test_gpio)
{
    cm_gpio_cfg_t cfg = {
        .direction = CM_GPIO_DIRECTION_OUTPUT,
        .pull = CM_GPIO_PULL_DOWN
    };
    cm_gpio_init(test_gpio, &cfg);
}

static void cm_lcd_test_backlight_on(cm_gpio_num_e test_gpio)
{
    cm_gpio_set_level(test_gpio, CM_GPIO_LEVEL_HIGH);
}

static void cm_lcd_test_backlight_off(cm_gpio_num_e test_gpio)
{
    cm_gpio_set_level(test_gpio, CM_GPIO_LEVEL_LOW);
}

static void st7789v_reset(void)
{
    cm_lcd_test_gpio_init(SPILCD_RESET_PIN);
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_HIGH);
    osDelay(50);
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_LOW);
    osDelay(100);
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_HIGH);
    osDelay(150);
}


#if 1
static void cm_lcd_test_set_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    unsigned char data[4] = {0};

    data[0] = 0x00;
    data[1] = x_start & 0xFF;
    data[2] = 0x00;
    data[3] = x_end & 0xFF;
    cm_lcd_write_cmd_and_data(0x2a, data, 4);
    data[0] = 0x00;
    data[1] = y_start & 0xFF;
    data[2] = 0x00;
    data[3] = y_end & 0xFF;
    cm_lcd_write_cmd_and_data(0x2b, data, 4);

    //cm_lcd_write_cmd(0x2c);
}
uint16_t s_lcdbuffer[128 * 128];
static void cm_lcd_show(uint16_t const color)
{
    uint8_t width = 128, height = 128;
    int i, j;

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            *(s_lcdbuffer + i + j * width) = color;
        }
    }

    cm_lcd_test_set_area(0, 0, width - 1, height - 1);

    //cm_lcd_write_cmd_and_data(0x2c, (unsigned char *)s_lcdbuffer, (int)(height * width * 2));
    cm_demo_printf("cm_lcd_test_set_area end\r\n");
    cm_lcd_write_cmd(0x2c); //  memory write将数据从MCU存入帧内存

    cm_lcd_write_buf((uint8_t *)s_lcdbuffer, height * width * 2);
}
static void cm_lcd_test_show_color(void)
{
    //int i, j;
    //uint8_t width = 128, height = 128;
    uint16_t const red    = 0xF800;     //(0x1F << 11) | (0x00 << 5) | 0x00;
    uint16_t const green  = 0x07E0;     //(0x00 << 11) | (0x3F << 5) | 0x00;
    uint16_t const blue   = 0x001F;     //(0x00 << 11) | (0x00 << 5) | 0x1F;
    uint16_t const black  = 0x0000;     //(0x00 << 11) | (0x00 << 5) | 0x00;
    uint16_t const white  = 0xFFFF;     //(0x1F << 11) | (0x3F << 5) | 0x1F;

    uint16_t color_bar[] = {
        blue, green, red, black, white,
    };
    cm_lcd_show(blue);
    osDelay(1000);
    cm_lcd_show(green);
    osDelay(1000);
    cm_lcd_show(red);
    osDelay(1000);
    cm_lcd_show(black);
    osDelay(1000);
    cm_lcd_show(white);
    osDelay(1000);
    cm_demo_printf("cm_lcd_write_buf end\r\n");
}


void cm_lcd_test_init(void)
{
    unsigned char data[16]={0};
    st7789v_reset();
    cm_st7789v_pin_mux_init();
    cm_lcd_test_backlight_on(SPILCD_BACKLIGHT_PIN);

    //cm_lcd_test_gpio_init(CM_GPIO_NUM_0); // 以GPIO0为例\

    cm_demo_printf("cm_lcd_init start ... \r\n");
    cm_lcd_init();

    uint32_t id = cm_lcd_read_id();
    cm_demo_printf("id =  %x cm_lcd_init end \r\n",id);

    cm_demo_printf("cm_lcd_write_cmd_and_data start ... \r\n");
#if 0
    cm_lcd_write_cmd_and_data(0x11, NULL, 0); //sleep out
    osDelay(120);
    cm_demo_printf("cm_lcd_write_cmd_and_data end \r\n");

    data[0] = 0x05;
    data[1] = 0x3A;
    data[2] = 0x3A;
    cm_lcd_write_cmd_and_data(0xB1, data, 3);

    data[0] = 0x05;
    data[1] = 0x3A;
    data[2] = 0x3A;
    cm_lcd_write_cmd_and_data(0xB2, data, 3);

    data[0] = 0x05;
    data[1] = 0x3A;
    data[2] = 0x3A;
    data[3] = 0x05;
    data[4] = 0x3A;
    data[5] = 0x3A;
    cm_lcd_write_cmd_and_data(0xB3, data, 6);

    data[0] = 0x03;
    cm_lcd_write_cmd_and_data(0xB4, data, 1); //column inversion

    data[0] = 0x62;
    data[1] = 0x02;
    data[2] = 0x04;
    cm_lcd_write_cmd_and_data(0xC0, data, 3);

    data[0] = 0xC0;
    cm_lcd_write_cmd_and_data(0xC1, data, 1);

    data[0] = 0x0D;
    data[1] = 0x00;
    cm_lcd_write_cmd_and_data(0xC2, data, 2);

    data[0] = 0x8D;
    data[1] = 0x6A;
    cm_lcd_write_cmd_and_data(0xC3, data, 2);

    data[0] = 0x8D;
    data[1] = 0xEE;
    cm_lcd_write_cmd_and_data(0xC4, data, 2);

    data[0] = 0x12;
    cm_lcd_write_cmd_and_data(0xC5, data, 1); //vcom

    data[0] = 0x00;
    cm_lcd_write_cmd_and_data(0x36, data, 1); //cm_lcd_write_data(0x68);////MX,MV,RGB mode

    osDelay(20);
    data[0] = 0x03;
    data[1] = 0x1B;
    data[2] = 0x12;
    data[3] = 0x11;
    data[4] = 0x3F;
    data[5] = 0x3A;
    data[6] = 0x32;
    data[7] = 0x34;
    data[8] = 0x2F;
    data[9] = 0x2B;
    data[10] = 0x30;
    data[11] = 0x3A;
    data[12] = 0x00;
    data[13] = 0x01;
    data[14] = 0x02;
    data[15] = 0x05;
    cm_lcd_write_cmd_and_data(0xE0, data, 16);

    data[0] = 0x03;
    data[1] = 0x1B;
    data[2] = 0x12;
    data[3] = 0x11;
    data[4] = 0x32;
    data[5] = 0x2F;
    data[6] = 0x2A;
    data[7] = 0x2F;
    data[8] = 0x2E;
    data[9] = 0x2C;
    data[10] = 0x35;
    data[11] = 0x3F;
    data[12] = 0x00;
    data[13] = 0x00;
    data[14] = 0x01;
    data[15] = 0x05;
    cm_lcd_write_cmd_and_data(0xE1, data, 16);

    data[0] = 0x05;
    cm_lcd_write_cmd_and_data(0x3A, data, 1); //65K mode
    data[0] = 0x00;
    cm_lcd_write_cmd_and_data(0x35, data, 1);

    cm_lcd_write_cmd_and_data(0x29, NULL, 0); //开启显示
#endif
    cm_lcd_write_cmd_and_data(0x11,NULL, 0);    //sleep out
    osDelay(120);

    data[0] = 0xA0;
    cm_lcd_write_cmd_and_data(0x36, data, 1);   //控制内存访问方式

    data[0]=0x05;
    cm_lcd_write_cmd_and_data(0x3a, data, 1);   //65K mode

    data[0]=0x00;
    data[1]=0xe8;   //ST7789V_RGB_MODE
    cm_lcd_write_cmd_and_data(0xb0, data, 2);

    data[0]=0x0c;
    data[1]=0x0c;
    data[2]=0x0;
    data[3]=0x33;
    data[4]=0x33;
    cm_lcd_write_cmd_and_data(0xb2, data, 5);

    data[0]=0x75;
    cm_lcd_write_cmd_and_data(0xb7, data, 1);

    data[0]=0x13;
    cm_lcd_write_cmd_and_data(0xbb, data, 1);

    data[0]=0x2c;
    cm_lcd_write_cmd_and_data(0xc0, data, 1);

    data[0]=0x01;
    cm_lcd_write_cmd_and_data(0xc2, data, 1);

    data[0]=0x11;
    cm_lcd_write_cmd_and_data(0xc3, data, 1);

    data[0]=0x2c;
    cm_lcd_write_cmd_and_data(0xc5, data, 1);

    data[0]=0x20;
    cm_lcd_write_cmd_and_data(0xc4, data, 1);

    data[0]=0x0f;
    cm_lcd_write_cmd_and_data(0xc6, data, 1);

    data[0]=0xa4;
    data[1]=0xa1;
    cm_lcd_write_cmd_and_data(0xd0, data, 2);

    data[0]=0xD0;
    data[1]=0x1A;
    data[2]=0x1E;
    data[3]=0x0A;
    data[4]=0x0A;
    data[5]=0x27;
    data[6]=0x3B;
    data[7]=0x44;
    data[8]=0x4A;
    data[9]=0x2B;
    data[10]=0x16;
    data[11]=0x15;
    data[12]=0x1A;
    data[13]=0x1E;
    //增加两个无效字节构成4字节对齐，否则颜色显示异常
    data[14]=0xFF;
    data[15]=0xFF;
    cm_lcd_write_cmd_and_data(0xe0, data, 16);


    data[0]=0xD0;
    data[1]=0x1A;
    data[2]=0x1E;
    data[3]=0x0A;
    data[4]=0x0A;
    data[5]=0x27;
    data[6]=0x3A;
    data[7]=0x43;
    data[8]=0x49;
    data[9]=0x2B;
    data[10]=0x16;
    data[11]=0x15;
    data[12]=0x1A;
    data[13]=0x1D;
    //增加两个无效字节构成4字节对齐，否则颜色显示异常
    data[14]=0xFF;
    data[15]=0xFF;
    cm_lcd_write_cmd_and_data(0xe1, data, 16);

    cm_lcd_write_cmd_and_data(0x29, NULL, 0);//开启显示
    osDelay(200);

    cm_lcd_test_show_color();

    osDelay(2000);

}

void SHELL_testLcd(unsigned char **cmd, int len)
{
    cm_demo_printf("cm_lcd test start ... \r\n");
    cm_lcd_test_init();

    cm_demo_printf("cm_lcd test SUCCESS \r\n");
}
NR_SHELL_CMD_EXPORT(cm_lcd, SHELL_testLcd);


void SHELL_deinit_Lcd(unsigned char **cmd, int len)
{
    cm_demo_printf("cm_lcd test start ... \r\n");
    cm_lcd_test_backlight_off(SPILCD_BACKLIGHT_PIN);
    cm_lcd_write_cmd_and_data(0x28, NULL, 0);//关闭显示
    osDelay(100);
    cm_lcd_deinit();

    cm_demo_printf("cm_lcd test end \r\n");
}
NR_SHELL_CMD_EXPORT(cm_de_lcd, SHELL_deinit_Lcd);

#endif
#endif
#endif

