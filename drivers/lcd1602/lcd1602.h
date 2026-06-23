#ifndef LCD1602_H
#define LCD1602_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

#define LCD1602_ADDR               0x27

#define LCD1602_CMD_CLEAR          0x01
#define LCD1602_CMD_HOME           0x02
#define LCD1602_CMD_ENTRY_MODE     0x04
#define LCD1602_CMD_DISPLAY_CTRL   0x08
#define LCD1602_CMD_CURSOR_SHIFT   0x10
#define LCD1602_CMD_FUNCTION_SET   0x20
#define LCD1602_CMD_SET_CGRAM     0x40
#define LCD1602_CMD_SET_DDRAM     0x80

#define LCD1602_LINE1_ADDR         0x00
#define LCD1602_LINE2_ADDR         0x40

typedef struct {
    uint8_t addr;
    uint8_t cols;
    uint8_t rows;
} lcd1602_config_t;

typedef struct {
    lcd1602_config_t config;
} lcd1602_priv_t;

int lcd1602_init(driver_t* drv, const lcd1602_config_t* config);
int lcd1602_clear(driver_t* drv);
int lcd1602_set_cursor(driver_t* drv, uint8_t col, uint8_t row);
int lcd1602_write_char(driver_t* drv, char ch);
int lcd1602_write_string(driver_t* drv, const char* str);

extern driver_t drv_lcd1602;

#ifdef __cplusplus
}
#endif

#endif /* LCD1602_H */
