#include "mpu6050.h"
#include "../drivers.h"

static mpu6050_priv_t mpu6050_priv;
static driver_i2c_t mpu6050_i2c;

static float mpu6050_get_accel_scale(uint8_t range) {
    switch (range) {
        case MPU6050_ACCEL_RANGE_2G:  return 16384.0f;
        case MPU6050_ACCEL_RANGE_4G:  return 8192.0f;
        case MPU6050_ACCEL_RANGE_8G:  return 4096.0f;
        case MPU6050_ACCEL_RANGE_16G: return 2048.0f;
        default:                      return 16384.0f;
    }
}

static float mpu6050_get_gyro_scale(uint8_t range) {
    switch (range) {
        case MPU6050_GYRO_RANGE_250DPS:  return 131.0f;
        case MPU6050_GYRO_RANGE_500DPS:  return 65.5f;
        case MPU6050_GYRO_RANGE_1000DPS: return 32.8f;
        case MPU6050_GYRO_RANGE_2000DPS: return 16.4f;
        default:                         return 131.0f;
    }
}

static int mpu6050_drv_init(driver_t* drv) {
    mpu6050_priv_t* priv = &mpu6050_priv;
    priv->config.addr = MPU6050_ADDR_PRIMARY;
    priv->config.accel_range = MPU6050_ACCEL_RANGE_2G;
    priv->config.gyro_range = MPU6050_GYRO_RANGE_250DPS;
    priv->accel_scale = mpu6050_get_accel_scale(priv->config.accel_range);
    priv->gyro_scale = mpu6050_get_gyro_scale(priv->config.gyro_range);
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int mpu6050_drv_open(driver_t* drv) {
    mpu6050_priv_t* priv = (mpu6050_priv_t*)drv->priv;
    uint8_t id;

    driver_i2c_init(&mpu6050_i2c, 0, 400000);

    driver_i2c_read_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_WHO_AM_I, &id, 1);
    if (id != 0x68) {
        return DRIVER_ERR_I2C;
    }

    uint8_t pwr_mgmt = 0x00;
    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_PWR_MGMT_1, &pwr_mgmt, 1);
    driver_delay_ms(100);

    uint8_t smplrt_div = 0x07;
    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_SMPLRT_DIV, &smplrt_div, 1);

    uint8_t config = 0x06;
    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_CONFIG, &config, 1);

    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_GYRO_CONFIG, &priv->config.gyro_range, 1);
    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_ACCEL_CONFIG, &priv->config.accel_range, 1);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int mpu6050_drv_close(driver_t* drv) {
    mpu6050_priv_t* priv = (mpu6050_priv_t*)drv->priv;
    uint8_t pwr_mgmt = 0x40;
    driver_i2c_write_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_PWR_MGMT_1, &pwr_mgmt, 1);
    driver_i2c_deinit(&mpu6050_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int mpu6050_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(mpu6050_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    mpu6050_priv_t* priv = (mpu6050_priv_t*)drv->priv;
    mpu6050_data_t* out = (mpu6050_data_t*)data;
    uint8_t buf[14];

    driver_i2c_read_reg(&mpu6050_i2c, priv->config.addr, MPU6050_REG_ACCEL_XOUT_H, buf, 14);

    int16_t ax_raw = (buf[0] << 8) | buf[1];
    int16_t ay_raw = (buf[2] << 8) | buf[3];
    int16_t az_raw = (buf[4] << 8) | buf[5];
    int16_t temp_raw = (buf[6] << 8) | buf[7];
    int16_t gx_raw = (buf[8] << 8) | buf[9];
    int16_t gy_raw = (buf[10] << 8) | buf[11];
    int16_t gz_raw = (buf[12] << 8) | buf[13];

    out->ax = (float)ax_raw / priv->accel_scale;
    out->ay = (float)ay_raw / priv->accel_scale;
    out->az = (float)az_raw / priv->accel_scale;
    out->temperature = (float)temp_raw / 340.0f + 36.53f;
    out->gx = (float)gx_raw / priv->gyro_scale;
    out->gy = (float)gy_raw / priv->gyro_scale;
    out->gz = (float)gz_raw / priv->gyro_scale;

    return DRIVER_OK;
}

static int mpu6050_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int mpu6050_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int mpu6050_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int mpu6050_init(driver_t* drv, const mpu6050_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        mpu6050_priv_t* priv = (mpu6050_priv_t*)drv->priv;
        priv->config = *config;
        priv->accel_scale = mpu6050_get_accel_scale(priv->config.accel_range);
        priv->gyro_scale = mpu6050_get_gyro_scale(priv->config.gyro_range);
    }
    return DRIVER_OK;
}

int mpu6050_read(driver_t* drv, mpu6050_data_t* data) {
    return drv->read(drv, data, sizeof(mpu6050_data_t));
}

driver_t drv_mpu6050 = {
    .name = "mpu6050",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = mpu6050_drv_init,
    .open = mpu6050_drv_open,
    .close = mpu6050_drv_close,
    .read = mpu6050_drv_read,
    .write = mpu6050_drv_write,
    .ioctl = mpu6050_drv_ioctl,
    .deinit = mpu6050_drv_deinit,
    .priv = NULL,
    .next = NULL
};
