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
#include "cm_spi_lcd.h"
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

#define CMD_RDDID                           (0x04)

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
    cm_iomux_set_pin_func(SPILCD_RESET_PIN, CM_IOMUX_FUNC_FUNCTION4); //SWD0_SWCLK
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
    uint8_t cmd[1] = {0};

    cmd[0] = 0x2a;
    data[0] = 0x00;
    data[1] = x_start & 0xFF;
    data[2] = 0x00;
    data[3] = x_end & 0xFF;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 4);

    cmd[0] = 0x2b;
    data[0] = 0x00;
    data[1] = y_start & 0xFF;
    data[2] = 0x00;
    data[3] = y_end & 0xFF;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 4);

    //cm_lcd_write_cmd(0x2c);
}
uint16_t s_lcdbuffer[128 * 128];
static void cm_lcd_show(uint16_t const color)
{
    uint8_t width = 128, height = 128;
    int i, j;
    uint8_t cmd[1] = {0};

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
    cmd[0] = 0x2c;
    cm_spi_lcd_write_cmd(cmd, 1);//  memory write将数据从MCU存入帧内存
    cm_spi_lcd_write_data((uint8_t *)s_lcdbuffer, height * width * 2);
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
    cm_demo_printf("cm_spi_lcd_write_data end\r\n");
}


void cm_lcd_test_init(void)
{
    unsigned char data[16]={0};
    uint8_t cmd[1] = {0};
    cm_st7789v_pin_mux_init();
    cm_lcd_test_backlight_on(SPILCD_BACKLIGHT_PIN);

    //cm_lcd_test_gpio_init(CM_GPIO_NUM_0); // 以GPIO0为例\

    cm_demo_printf("cm_lcd_init start ... \r\n");
    cm_spi_lcd_cfg_t cfg = {
        .spi_dev = CM_SPI_DEV_2,
        .dcx_gpio = CM_GPIO_NUM_MAX,
        .rst_gpio = SPILCD_RESET_PIN
    };
    int32_t ret = cm_spi_lcd_init(&cfg);
    if (ret != 0)
    {
        cm_demo_printf("cm_spi_lcd_init error:%d \r\n",ret);
        return;
    }

    uint8_t id_cmd[1] = {CMD_RDDID};
    uint8_t id_buf[4] = {0};
    ret = cm_spi_lcd_read_id(id_cmd, 1, id_buf, 4);
    if (ret != 0)
    {
        cm_demo_printf("cm_spi_lcd_read_id error:%d \r\n",ret);
        return;
    }
    cm_demo_printf("id =  %x cm_lcd_init end \r\n",*((uint32_t*)id_buf));

    cm_demo_printf("cm_lcd_write_cmd_and_data start ... \r\n");
    cmd[0] = 0x11;
    cm_spi_lcd_write_cmd(cmd, 1);    //sleep out
    osDelay(120);

    cmd[0] = 0x36;
    data[0] = 0xA0;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);   //控制内存访问方式

    cmd[0] = 0x3a;
    data[0]=0x05;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);   //65K mode

    cmd[0] = 0xb0;
    data[0]=0x00;
    data[1]=0xe8;   //ST7789V_RGB_MODE
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 2);

    cmd[0] = 0xb2;
    data[0]=0x0c;
    data[1]=0x0c;
    data[2]=0x0;
    data[3]=0x33;
    data[4]=0x33;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 5);

    cmd[0] = 0xb7;
    data[0]=0x75;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xbb;
    data[0]=0x13;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc0;
    data[0]=0x2c;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc2;
    data[0]=0x01;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc3;
    data[0]=0x11;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc5;
    data[0]=0x2c;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc4;
    data[0]=0x20;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xc6;
    data[0]=0x0f;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 1);

    cmd[0] = 0xd0;
    data[0]=0xa4;
    data[1]=0xa1;
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 2);

    cmd[0] = 0xe0;
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
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 16);


    cmd[0] = 0xe1;
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
    cm_spi_lcd_write_cmd(cmd, 1);
    cm_spi_lcd_write_data(data, 16);

    cmd[0] = 0x29;
    cm_spi_lcd_write_cmd(cmd, 1);
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
    uint8_t lcd_cmd[1] = {0};
    cm_demo_printf("cm_lcd test start ... \r\n");
    cm_lcd_test_backlight_off(SPILCD_BACKLIGHT_PIN);
    lcd_cmd[0] = 0x28;
    cm_spi_lcd_write_cmd(lcd_cmd, 1);
    osDelay(100);
    cm_spi_lcd_deinit();

    cm_demo_printf("cm_lcd test end \r\n");
}
NR_SHELL_CMD_EXPORT(cm_de_lcd, SHELL_deinit_Lcd);

#endif
#endif
#endif

