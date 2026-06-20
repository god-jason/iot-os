#include "ssd1306.h"
#include "../drivers.h"

static ssd1306_priv_t ssd1306_priv;
static driver_i2c_t ssd1306_i2c;

static void ssd1306_send_cmd(ssd1306_priv_t* priv, uint8_t cmd) {
    uint8_t buf[2] = {SSD1306_REG_CMD, cmd};
    driver_i2c_write_bytes(&ssd1306_i2c, priv->addr, buf, 2);
}

static void ssd1306_send_data(ssd1306_priv_t* priv, const uint8_t* data, size_t len) {
    uint8_t buf[SSD1306_BUFFER_SIZE + 1];
    buf[0] = SSD1306_REG_DATA;
    memcpy(buf + 1, data, len);
    driver_i2c_write_bytes(&ssd1306_i2c, priv->addr, buf, len + 1);
}

static int ssd1306_drv_init(driver_t* drv) {
    ssd1306_priv_t* priv = &ssd1306_priv;
    priv->addr = SSD1306_ADDR;
    priv->width = SSD1306_WIDTH;
    priv->height = SSD1306_HEIGHT;
    memset(priv->buffer, 0, sizeof(priv->buffer));
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ssd1306_drv_open(driver_t* drv) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;

    driver_i2c_init(&ssd1306_i2c, 0, 400000);

    ssd1306_send_cmd(priv, SSD1306_CMD_DISPLAY_OFF);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_DISPLAY_CLOCK);
    ssd1306_send_cmd(priv, 0x80);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_MUX_RATIO);
    ssd1306_send_cmd(priv, priv->height - 1);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_DISPLAY_OFFSET);
    ssd1306_send_cmd(priv, 0x00);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_START_LINE | 0x00);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_SEG_REMAP | 0x01);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_COM_OUT_DIR | 0x08);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_COM_PIN_CFG);
    ssd1306_send_cmd(priv, priv->height == 64 ? 0x12 : 0x02);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_CONTRAST);
    ssd1306_send_cmd(priv, 0xFF);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_PRECHARGE);
    ssd1306_send_cmd(priv, 0xF1);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_VCOM_DETECT);
    ssd1306_send_cmd(priv, 0x40);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_MEM_ADDR_MODE);
    ssd1306_send_cmd(priv, 0x00);
    ssd1306_send_cmd(priv, SSD1306_CMD_DISPLAY_RAM);
    ssd1306_send_cmd(priv, SSD1306_CMD_NORMAL_DISP);
    ssd1306_send_cmd(priv, SSD1306_CMD_DISPLAY_ON);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int ssd1306_drv_close(driver_t* drv) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;
    ssd1306_send_cmd(priv, SSD1306_CMD_DISPLAY_OFF);
    driver_i2c_deinit(&ssd1306_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ssd1306_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ssd1306_drv_write(driver_t* drv, const void* data, size_t len) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;
    if (len > sizeof(priv->buffer)) {
        return DRIVER_ERR_INVALID;
    }
    memcpy(priv->buffer, data, len);
    return DRIVER_OK;
}

static int ssd1306_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ssd1306_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int ssd1306_init(driver_t* drv) {
    return drv->init(drv);
}

int ssd1306_clear(driver_t* drv) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;
    memset(priv->buffer, 0, sizeof(priv->buffer));
    return DRIVER_OK;
}

int ssd1306_draw_pixel(driver_t* drv, int x, int y, int color) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;

    if (x < 0 || x >= priv->width || y < 0 || y >= priv->height) {
        return DRIVER_ERR_INVALID;
    }

    int page = y / 8;
    int bit = y % 8;

    if (color) {
        priv->buffer[page * priv->width + x] |= (1 << bit);
    } else {
        priv->buffer[page * priv->width + x] &= ~(1 << bit);
    }

    return DRIVER_OK;
}

int ssd1306_draw_char(driver_t* drv, int x, int y, char ch, uint8_t* font) {
    (void)drv;
    (void)x;
    (void)y;
    (void)ch;
    (void)font;
    return DRIVER_ERR_NOT_SUPPORTED;
}

int ssd1306_draw_string(driver_t* drv, int x, int y, const char* str, uint8_t* font) {
    (void)drv;
    (void)x;
    (void)y;
    (void)str;
    (void)font;
    return DRIVER_ERR_NOT_SUPPORTED;
}

int ssd1306_update(driver_t* drv) {
    ssd1306_priv_t* priv = (ssd1306_priv_t*)drv->priv;

    ssd1306_send_cmd(priv, SSD1306_CMD_SET_COL_ADDR);
    ssd1306_send_cmd(priv, 0);
    ssd1306_send_cmd(priv, priv->width - 1);
    ssd1306_send_cmd(priv, SSD1306_CMD_SET_PAGE_ADDR);
    ssd1306_send_cmd(priv, 0);
    ssd1306_send_cmd(priv, (priv->height / 8) - 1);

    ssd1306_send_data(priv, priv->buffer, sizeof(priv->buffer));

    return DRIVER_OK;
}

driver_t drv_ssd1306 = {
    .name = "ssd1306",
    .type = DRIVER_TYPE_DISPLAY,
    .status = DRIVER_STATUS_UNINIT,
    .init = ssd1306_drv_init,
    .open = ssd1306_drv_open,
    .close = ssd1306_drv_close,
    .read = ssd1306_drv_read,
    .write = ssd1306_drv_write,
    .ioctl = ssd1306_drv_ioctl,
    .deinit = ssd1306_drv_deinit,
    .priv = NULL,
    .next = NULL
};
