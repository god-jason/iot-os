#include "lcd1602.h"
#include "../drivers.h"

static lcd1602_priv_t lcd1602_priv;
static driver_i2c_t lcd1602_i2c;

static void lcd1602_send_cmd(lcd1602_priv_t* priv, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    driver_i2c_write_bytes(&lcd1602_i2c, priv->config.addr, buf, 2);
    driver_delay_ms(2);
}

static void lcd1602_send_data(lcd1602_priv_t* priv, uint8_t data) {
    uint8_t buf[2] = {0x40, data};
    driver_i2c_write_bytes(&lcd1602_i2c, priv->config.addr, buf, 2);
}

static int lcd1602_drv_init(driver_t* drv) {
    lcd1602_priv_t* priv = &lcd1602_priv;
    priv->config.addr = LCD1602_ADDR;
    priv->config.cols = 16;
    priv->config.rows = 2;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int lcd1602_drv_open(driver_t* drv) {
    lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;

    driver_i2c_init(&lcd1602_i2c, 0, 400000);

    driver_delay_ms(50);

    lcd1602_send_cmd(priv, LCD1602_CMD_FUNCTION_SET | 0x10);
    driver_delay_ms(5);
    lcd1602_send_cmd(priv, LCD1602_CMD_FUNCTION_SET | 0x10);
    driver_delay_ms(1);
    lcd1602_send_cmd(priv, LCD1602_CMD_FUNCTION_SET | 0x10);

    lcd1602_send_cmd(priv, LCD1602_CMD_FUNCTION_SET | 0x18);
    lcd1602_send_cmd(priv, LCD1602_CMD_DISPLAY_CTRL | 0x08);
    lcd1602_send_cmd(priv, LCD1602_CMD_CLEAR);
    driver_delay_ms(2);
    lcd1602_send_cmd(priv, LCD1602_CMD_ENTRY_MODE | 0x06);
    lcd1602_send_cmd(priv, LCD1602_CMD_DISPLAY_CTRL | 0x0C);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int lcd1602_drv_close(driver_t* drv) {
    lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;
    lcd1602_send_cmd(priv, LCD1602_CMD_DISPLAY_CTRL | 0x00);
    driver_i2c_deinit(&lcd1602_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int lcd1602_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int lcd1602_drv_write(driver_t* drv, const void* data, size_t len) {
    lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;
    const char* str = (const char*)data;
    for (size_t i = 0; i < len; i++) {
        lcd1602_send_data(priv, str[i]);
    }
    return DRIVER_OK;
}

static int lcd1602_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int lcd1602_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int lcd1602_init(driver_t* drv, const lcd1602_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int lcd1602_clear(driver_t* drv) {
    lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;
    lcd1602_send_cmd(priv, LCD1602_CMD_CLEAR);
    driver_delay_ms(2);
    return DRIVER_OK;
}

int lcd1602_set_cursor(driver_t* drv, uint8_t col, uint8_t row) {
    lcd1602_priv_t* priv = (lcd1602_priv_t*)drv->priv;
    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
    uint8_t cmd = LCD1602_CMD_SET_DDRAM | addr;
    lcd1602_send_cmd(priv, cmd);
    return DRIVER_OK;
}

int lcd1602_print(driver_t* drv, const char* str) {
    return drv->write(drv, str, strlen(str));
}

driver_t drv_lcd1602 = {
    .name = "lcd1602",
    .type = DRIVER_TYPE_DISPLAY,
    .status = DRIVER_STATUS_UNINIT,
    .init = lcd1602_drv_init,
    .open = lcd1602_drv_open,
    .close = lcd1602_drv_close,
    .read = lcd1602_drv_read,
    .write = lcd1602_drv_write,
    .ioctl = lcd1602_drv_ioctl,
    .deinit = lcd1602_drv_deinit,
    .priv = NULL,
    .next = NULL
};
