#include "bh1750.h"
#include "../drivers.h"

static bh1750_priv_t bh1750_priv;
static driver_i2c_t bh1750_i2c;

static int bh1750_drv_init(driver_t* drv) {
    bh1750_priv_t* priv = &bh1750_priv;
    priv->config.addr = BH1750_ADDR_PRIMARY;
    priv->config.mode = BH1750_REG_MEASURE_HIGH;
    priv->config.mtreg = 69;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int bh1750_drv_open(driver_t* drv) {
    bh1750_priv_t* priv = (bh1750_priv_t*)drv->priv;

    driver_i2c_init(&bh1750_i2c, 0, 400000);

    uint8_t power_on = BH1750_REG_POWER_ON;
    driver_i2c_write_bytes(&bh1750_i2c, priv->config.addr, &power_on, 1);
    driver_delay_ms(10);

    driver_i2c_write_bytes(&bh1750_i2c, priv->config.addr, &priv->config.mode, 1);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int bh1750_drv_close(driver_t* drv) {
    bh1750_priv_t* priv = (bh1750_priv_t*)drv->priv;
    uint8_t power_down = BH1750_REG_POWER_DOWN;
    driver_i2c_write_bytes(&bh1750_i2c, priv->config.addr, &power_down, 1);
    driver_i2c_deinit(&bh1750_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int bh1750_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(bh1750_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    bh1750_priv_t* priv = (bh1750_priv_t*)drv->priv;
    bh1750_data_t* out = (bh1750_data_t*)data;
    uint8_t buf[2];

    driver_delay_ms(180);
    driver_i2c_read_bytes(&bh1750_i2c, priv->config.addr, buf, 2);

    uint16_t raw = (buf[0] << 8) | buf[1];
    float factor = (1.0f / 1.2f) * (69.0f / (float)priv->config.mtreg);
    out->lux = (float)raw * factor;

    return DRIVER_OK;
}

static int bh1750_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int bh1750_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int bh1750_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int bh1750_init(driver_t* drv, const bh1750_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        bh1750_priv_t* priv = (bh1750_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int bh1750_read(driver_t* drv, bh1750_data_t* data) {
    return drv->read(drv, data, sizeof(bh1750_data_t));
}

driver_t drv_bh1750 = {
    .name = "bh1750",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = bh1750_drv_init,
    .open = bh1750_drv_open,
    .close = bh1750_drv_close,
    .read = bh1750_drv_read,
    .write = bh1750_drv_write,
    .ioctl = bh1750_drv_ioctl,
    .deinit = bh1750_drv_deinit,
    .priv = NULL,
    .next = NULL
};
