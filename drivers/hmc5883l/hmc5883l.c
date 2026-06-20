#include "hmc5883l.h"
#include "../drivers.h"

static hmc5883l_priv_t hmc5883l_priv;
static driver_i2c_t hmc5883l_i2c;

static float hmc5883l_get_sensitivity(uint8_t scale) {
    switch (scale) {
        case HMC5883L_SCALE_088GAUSS: return 1370.0f;
        case HMC5883L_SCALE_13GAUSS:  return 1090.0f;
        case HMC5883L_SCALE_19GAUSS:  return 820.0f;
        case HMC5883L_SCALE_25GAUSS:  return 660.0f;
        case HMC5883L_SCALE_40GAUSS:  return 440.0f;
        case HMC5883L_SCALE_47GAUSS:  return 390.0f;
        case HMC5883L_SCALE_56GAUSS:  return 330.0f;
        case HMC5883L_SCALE_81GAUSS:  return 230.0f;
        default:                      return 1090.0f;
    }
}

static int hmc5883l_drv_init(driver_t* drv) {
    hmc5883l_priv_t* priv = &hmc5883l_priv;
    priv->config.addr = HMC5883L_ADDR_PRIMARY;
    priv->config.mode = HMC5883L_MODE_CONTINUOUS;
    priv->config.scale = HMC5883L_SCALE_13GAUSS;
    priv->config.sensitivity = hmc5883l_get_sensitivity(priv->config.scale);
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int hmc5883l_drv_open(driver_t* drv) {
    hmc5883l_priv_t* priv = (hmc5883l_priv_t*)drv->priv;

    driver_i2c_init(&hmc5883l_i2c, 0, 400000);

    uint8_t id[3];
    driver_i2c_read_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_ID_A, id, 3);
    if (id[0] != 0x48 || id[1] != 0x34 || id[2] != 0x33) {
        return DRIVER_ERR_I2C;
    }

    uint8_t config_a = 0x70;
    driver_i2c_write_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_CONFIG_A, &config_a, 1);

    uint8_t config_b = (priv->config.scale << 5);
    driver_i2c_write_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_CONFIG_B, &config_b, 1);

    driver_i2c_write_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_MODE, &priv->config.mode, 1);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int hmc5883l_drv_close(driver_t* drv) {
    hmc5883l_priv_t* priv = (hmc5883l_priv_t*)drv->priv;
    uint8_t mode = HMC5883L_MODE_IDLE;
    driver_i2c_write_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_MODE, &mode, 1);
    driver_i2c_deinit(&hmc5883l_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int hmc5883l_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(hmc5883l_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    hmc5883l_priv_t* priv = (hmc5883l_priv_t*)drv->priv;
    hmc5883l_data_t* out = (hmc5883l_data_t*)data;
    uint8_t buf[6];

    driver_i2c_read_reg(&hmc5883l_i2c, priv->config.addr, HMC5883L_REG_OUT_X_MSB, buf, 6);

    int16_t x_raw = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t z_raw = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t y_raw = (int16_t)((buf[4] << 8) | buf[5]);

    out->x = (float)x_raw / priv->config.sensitivity;
    out->y = (float)y_raw / priv->config.sensitivity;
    out->z = (float)z_raw / priv->config.sensitivity;

    return DRIVER_OK;
}

static int hmc5883l_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int hmc5883l_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int hmc5883l_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int hmc5883l_init(driver_t* drv, const hmc5883l_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        hmc5883l_priv_t* priv = (hmc5883l_priv_t*)drv->priv;
        priv->config = *config;
        priv->config.sensitivity = hmc5883l_get_sensitivity(priv->config.scale);
    }
    return DRIVER_OK;
}

int hmc5883l_read(driver_t* drv, hmc5883l_data_t* data) {
    return drv->read(drv, data, sizeof(hmc5883l_data_t));
}

driver_t drv_hmc5883l = {
    .name = "hmc5883l",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = hmc5883l_drv_init,
    .open = hmc5883l_drv_open,
    .close = hmc5883l_drv_close,
    .read = hmc5883l_drv_read,
    .write = hmc5883l_drv_write,
    .ioctl = hmc5883l_drv_ioctl,
    .deinit = hmc5883l_drv_deinit,
    .priv = NULL,
    .next = NULL
};
