#ifndef SSD1306_H
#define SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define SSD1306_ADDR              0x3C
#define SSD1306_WIDTH             128
#define SSD1306_HEIGHT            64
#define SSD1306_BUFFER_SIZE       (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

#define SSD1306_REG_CMD           0x00
#define SSD1306_REG_DATA          0x40

#define SSD1306_CMD_SET_CONTRAST  0x81
#define SSD1306_CMD_DISPLAY_RAM   0xA4
#define SSD1306_CMD_DISPLAY_ALLON 0xA5
#define SSD1306_CMD_NORMAL_DISP   0xA6
#define SSD1306_CMD_INVERT_DISP   0xA7
#define SSD1306_CMD_DISPLAY_OFF   0xAE
#define SSD1306_CMD_DISPLAY_ON    0xAF

#define SSD1306_CMD_SET_MEM_ADDR_MODE 0x20
#define SSD1306_CMD_SET_COL_ADDR      0x21
#define SSD1306_CMD_SET_PAGE_ADDR     0x22

#define SSD1306_CMD_SET_START_LINE    0x40
#define SSD1306_CMD_SET_SEG_REMAP     0xA0
#define SSD1306_CMD_SET_MUX_RATIO     0xA8
#define SSD1306_CMD_SET_COM_OUT_DIR   0xC0
#define SSD1306_CMD_SET_DISPLAY_OFFSET 0xD3
#define SSD1306_CMD_SET_COM_PIN_CFG   0xDA
#define SSD1306_CMD_SET_DISPLAY_CLOCK 0xD5
#define SSD1306_CMD_SET_PRECHARGE     0xD9
#define SSD1306_CMD_SET_VCOM_DETECT   0xDB

typedef struct {
    uint8_t addr;
    uint8_t width;
    uint8_t height;
    uint8_t buffer[SSD1306_BUFFER_SIZE];
} ssd1306_priv_t;

int ssd1306_init(driver_t* drv);
int ssd1306_clear(driver_t* drv);
int ssd1306_draw_pixel(driver_t* drv, int x, int y, int color);
int ssd1306_draw_char(driver_t* drv, int x, int y, char ch, uint8_t* font);
int ssd1306_draw_string(driver_t* drv, int x, int y, const char* str, uint8_t* font);
int ssd1306_update(driver_t* drv);

extern driver_t drv_ssd1306;

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H */
