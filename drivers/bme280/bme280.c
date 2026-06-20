#include "bme280.h"
#include "../drivers.h"

static bme280_priv_t bme280_priv;

static int bme280_read_calib(bme280_priv_t* priv);
static int32_t bme280_compensate_temperature(bme280_priv_t* priv, int32_t adc_T);
static uint32_t bme280_compensate_pressure(bme280_priv_t* priv, int32_t adc_P);
static uint32_t bme280_compensate_humidity(bme280_priv_t* priv, int32_t adc_H);

static int bme280_drv_init(driver_t* drv) {
    bme280_priv_t* priv = &bme280_priv;
    priv->config.addr = BME280_ADDR_PRIMARY;
    priv->config.osrs_t = 1;
    priv->config.osrs_p = 1;
    priv->config.osrs_h = 1;
    priv->config.mode = 3;
    priv->config.t_sb = 5;
    priv->config.filter = 0;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int bme280_drv_open(driver_t* drv) {
    bme280_priv_t* priv = (bme280_priv_t*)drv->priv;
    uint8_t id;

    driver_i2c_read(priv->config.addr, BME280_REG_ID, &id, 1);
    if (id != 0x60) {
        return DRIVER_ERR_I2C;
    }

    driver_i2c_write(priv->config.addr, BME280_REG_RESET, &(uint8_t){BME280_RESET_VALUE}, 1);
    driver_delay_ms(10);

    if (bme280_read_calib(priv) != DRIVER_OK) {
        return DRIVER_ERR_I2C;
    }

    uint8_t ctrl_hum = priv->config.osrs_h;
    driver_i2c_write(priv->config.addr, BME280_REG_CTRL_HUM, &ctrl_hum, 1);

    uint8_t ctrl_meas = (priv->config.osrs_t << 5) | (priv->config.osrs_p << 2) | priv->config.mode;
    driver_i2c_write(priv->config.addr, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);

    uint8_t config = (priv->config.t_sb << 5) | (priv->config.filter << 2);
    driver_i2c_write(priv->config.addr, BME280_REG_CONFIG, &config, 1);

    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int bme280_drv_close(driver_t* drv) {
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int bme280_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(bme280_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    bme280_priv_t* priv = (bme280_priv_t*)drv->priv;
    bme280_data_t* out = (bme280_data_t*)data;
    uint8_t buf[8];

    driver_i2c_read(priv->config.addr, BME280_REG_PRESS_MSB, buf, 8);

    int32_t adc_P = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    int32_t adc_T = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
    int32_t adc_H = (buf[6] << 8) | buf[7];

    out->temperature = (float)bme280_compensate_temperature(priv, adc_T) / 100.0f;
    out->pressure = (float)bme280_compensate_pressure(priv, adc_P) / 100.0f;
    out->humidity = (float)bme280_compensate_humidity(priv, adc_H) / 1024.0f;

    return DRIVER_OK;
}

static int bme280_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int bme280_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int bme280_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

static int bme280_read_calib(bme280_priv_t* priv) {
    uint8_t buf[24];

    driver_i2c_read(priv->config.addr, 0x88, buf, 24);

    priv->calib.dig_T1 = (uint16_t)((buf[1] << 8) | buf[0]);
    priv->calib.dig_T2 = (int16_t)((buf[3] << 8) | buf[2]);
    priv->calib.dig_T3 = (int16_t)((buf[5] << 8) | buf[4]);
    priv->calib.dig_P1 = (uint16_t)((buf[7] << 8) | buf[6]);
    priv->calib.dig_P2 = (int16_t)((buf[9] << 8) | buf[8]);
    priv->calib.dig_P3 = (int16_t)((buf[11] << 8) | buf[10]);
    priv->calib.dig_P4 = (int16_t)((buf[13] << 8) | buf[12]);
    priv->calib.dig_P5 = (int16_t)((buf[15] << 8) | buf[14]);
    priv->calib.dig_P6 = (int16_t)((buf[17] << 8) | buf[16]);
    priv->calib.dig_P7 = (int16_t)((buf[19] << 8) | buf[18]);
    priv->calib.dig_P8 = (int16_t)((buf[21] << 8) | buf[20]);
    priv->calib.dig_P9 = (int16_t)((buf[23] << 8) | buf[22]);

    driver_i2c_read(priv->config.addr, 0xA1, &priv->calib.dig_H1, 1);

    driver_i2c_read(priv->config.addr, 0xE1, buf, 7);

    priv->calib.dig_H2 = (int16_t)((buf[1] << 8) | buf[0]);
    priv->calib.dig_H3 = buf[2];
    priv->calib.dig_H4 = (int16_t)((buf[3] << 4) | (buf[4] & 0x0F));
    priv->calib.dig_H5 = (int16_t)((buf[5] << 4) | ((buf[4] >> 4) & 0x0F));
    priv->calib.dig_H6 = (int8_t)buf[6];

    return DRIVER_OK;
}

static int32_t bme280_compensate_temperature(bme280_priv_t* priv, int32_t adc_T) {
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)priv->calib.dig_T1 << 1))) * ((int32_t)priv->calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)priv->calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)priv->calib.dig_T1))) >> 12) * ((int32_t)priv->calib.dig_T3)) >> 14;
    priv->t_fine = var1 + var2;
    T = (priv->t_fine * 5 + 128) >> 8;

    return T;
}

static uint32_t bme280_compensate_pressure(bme280_priv_t* priv, int32_t adc_P) {
    int64_t var1, var2, p;

    var1 = ((int64_t)priv->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)priv->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)priv->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)priv->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)priv->calib.dig_P3) >> 8) + ((var1 * (int64_t)priv->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)priv->calib.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)priv->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)priv->calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)priv->calib.dig_P7) << 4);

    return (uint32_t)p;
}

static uint32_t bme280_compensate_humidity(bme280_priv_t* priv, int32_t adc_H) {
    int32_t v_x1_u32r;

    v_x1_u32r = (priv->t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)priv->calib.dig_H4) << 20) - (((int32_t)priv->calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)priv->calib.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)priv->calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)priv->calib.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)priv->calib.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (uint32_t)(v_x1_u32r >> 12);
}

int bme280_init(driver_t* drv, const bme280_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        bme280_priv_t* priv = (bme280_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int bme280_read(driver_t* drv, bme280_data_t* data) {
    return drv->read(drv, data, sizeof(bme280_data_t));
}

int bme280_reset(driver_t* drv) {
    bme280_priv_t* priv = (bme280_priv_t*)drv->priv;
    uint8_t val = BME280_RESET_VALUE;
    return driver_i2c_write(priv->config.addr, BME280_REG_RESET, &val, 1);
}

driver_t drv_bme280 = {
    .name = "bme280",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = bme280_drv_init,
    .open = bme280_drv_open,
    .close = bme280_drv_close,
    .read = bme280_drv_read,
    .write = bme280_drv_write,
    .ioctl = bme280_drv_ioctl,
    .deinit = bme280_drv_deinit,
    .priv = NULL,
    .next = NULL
};
