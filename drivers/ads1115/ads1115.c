#include "ads1115.h"
#include "../drivers.h"

static ads1115_priv_t ads1115_priv;
static driver_i2c_t ads1115_i2c;

static float ads1115_get_vref(uint8_t pga) {
    switch (pga) {
        case ADS1115_CFG_PGA_6144V: return 6.144f;
        case ADS1115_CFG_PGA_4096V: return 4.096f;
        case ADS1115_CFG_PGA_2048V: return 2.048f;
        case ADS1115_CFG_PGA_1024V: return 1.024f;
        case ADS1115_CFG_PGA_0512V: return 0.512f;
        case ADS1115_CFG_PGA_0256V: return 0.256f;
        default:                     return 2.048f;
    }
}

static uint8_t ads1115_get_mux(int channel) {
    switch (channel) {
        case 0: return ADS1115_CFG_MUX_AIN0_GND;
        case 1: return ADS1115_CFG_MUX_AIN1_GND;
        case 2: return ADS1115_CFG_MUX_AIN2_GND;
        case 3: return ADS1115_CFG_MUX_AIN3_GND;
        default: return ADS1115_CFG_MUX_AIN0_GND;
    }
}

static int ads1115_drv_init(driver_t* drv) {
    ads1115_priv_t* priv = &ads1115_priv;
    priv->config.addr = ADS1115_ADDR_BASE;
    priv->config.mux = ADS1115_CFG_MUX_AIN0_GND;
    priv->config.pga = ADS1115_CFG_PGA_2048V;
    priv->config.mode = ADS1115_CFG_MODE_SINGLE;
    priv->config.dr = ADS1115_CFG_DR_128SPS;
    priv->config.vref = ads1115_get_vref(priv->config.pga);
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ads1115_drv_open(driver_t* drv) {
    driver_i2c_init(&ads1115_i2c, 0, 400000);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int ads1115_drv_close(driver_t* drv) {
    driver_i2c_deinit(&ads1115_i2c);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ads1115_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ads1115_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ads1115_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ads1115_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int ads1115_init(driver_t* drv, const ads1115_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        ads1115_priv_t* priv = (ads1115_priv_t*)drv->priv;
        priv->config = *config;
        priv->config.vref = ads1115_get_vref(priv->config.pga);
    }
    return DRIVER_OK;
}

int ads1115_read(driver_t* drv, int channel, ads1115_data_t* data) {
    ads1115_priv_t* priv = (ads1115_priv_t*)drv->priv;

    uint16_t config = ADS1115_CFG_OS_SINGLE;
    config |= ads1115_get_mux(channel);
    config |= priv->config.pga;
    config |= priv->config.mode;
    config |= priv->config.dr;
    config |= 0x0018;

    uint8_t cfg_buf[3] = {ADS1115_REG_CONFIG, (config >> 8) & 0xFF, config & 0xFF};
    driver_i2c_write_bytes(&ads1115_i2c, priv->config.addr, cfg_buf, 3);

    driver_delay_ms(10);

    uint8_t buf[2];
    driver_i2c_read_reg(&ads1115_i2c, priv->config.addr, ADS1115_REG_CONVERSION, buf, 2);

    data->raw = (int16_t)((buf[0] << 8) | buf[1]);
    data->voltage = ((float)data->raw / 32768.0f) * priv->config.vref;

    return DRIVER_OK;
}

driver_t drv_ads1115 = {
    .name = "ads1115",
    .type = DRIVER_TYPE_ADC,
    .status = DRIVER_STATUS_UNINIT,
    .init = ads1115_drv_init,
    .open = ads1115_drv_open,
    .close = ads1115_drv_close,
    .read = ads1115_drv_read,
    .write = ads1115_drv_write,
    .ioctl = ads1115_drv_ioctl,
    .deinit = ads1115_drv_deinit,
    .priv = NULL,
    .next = NULL
};
