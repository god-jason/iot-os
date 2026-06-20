#include "ds18b20.h"
#include "../drivers.h"

static ds18b20_priv_t ds18b20_priv;

static int ds18b20_reset(ds18b20_priv_t* priv) {
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_LOW);
    driver_delay_us(480);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
    driver_delay_us(60);

    driver_gpio_init(priv->config.pin, GPIO_DIR_INPUT);
    driver_delay_us(60);

    int level;
    driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&level);
    driver_delay_us(420);

    return (level == GPIO_LEVEL_LOW) ? DRIVER_OK : DRIVER_ERR_TIMEOUT;
}

static void ds18b20_write_bit(ds18b20_priv_t* priv, int bit) {
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_LOW);

    if (bit) {
        driver_delay_us(6);
        driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
        driver_delay_us(64);
    } else {
        driver_delay_us(60);
        driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
        driver_delay_us(10);
    }
}

static int ds18b20_read_bit(ds18b20_priv_t* priv) {
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_LOW);
    driver_delay_us(6);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
    driver_delay_us(9);

    driver_gpio_init(priv->config.pin, GPIO_DIR_INPUT);
    driver_delay_us(50);

    int level;
    driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&level);
    driver_delay_us(55);

    return level;
}

static void ds18b20_write_byte(ds18b20_priv_t* priv, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit(priv, (byte >> i) & 1);
    }
}

static uint8_t ds18b20_read_byte(ds18b20_priv_t* priv) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte |= (ds18b20_read_bit(priv) << i);
    }
    return byte;
}

static int ds18b20_drv_init(driver_t* drv) {
    ds18b20_priv_t* priv = &ds18b20_priv;
    priv->config.pin = -1;
    memset(priv->config.rom, 0, sizeof(priv->config.rom));
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ds18b20_drv_open(driver_t* drv) {
    ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int ds18b20_drv_close(driver_t* drv) {
    ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;
    driver_gpio_deinit(priv->config.pin);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int ds18b20_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(ds18b20_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;
    ds18b20_data_t* out = (ds18b20_data_t*)data;

    if (ds18b20_reset(priv) != DRIVER_OK) {
        return DRIVER_ERR_TIMEOUT;
    }

    ds18b20_write_byte(priv, DS18B20_CMD_SKIP_ROM);
    ds18b20_write_byte(priv, DS18B20_CMD_READ_SCRATCH);

    uint8_t scratch[9];
    for (int i = 0; i < 9; i++) {
        scratch[i] = ds18b20_read_byte(priv);
    }

    uint8_t crc = 0;
    for (int i = 0; i < 8; i++) {
        crc ^= scratch[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
        }
    }

    if (crc != scratch[8]) {
        return DRIVER_ERR_CRC;
    }

    int16_t temp_raw = (int16_t)((scratch[1] << 8) | scratch[0]);
    out->temperature = (float)temp_raw / 16.0f;

    return DRIVER_OK;
}

static int ds18b20_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ds18b20_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int ds18b20_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int ds18b20_init(driver_t* drv, const ds18b20_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int ds18b20_read(driver_t* drv, ds18b20_data_t* data) {
    ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;

    if (ds18b20_reset(priv) != DRIVER_OK) {
        return DRIVER_ERR_TIMEOUT;
    }

    ds18b20_write_byte(priv, DS18B20_CMD_SKIP_ROM);
    ds18b20_write_byte(priv, DS18B20_CMD_CONVERT_T);

    driver_delay_ms(750);

    return drv->read(drv, data, sizeof(ds18b20_data_t));
}

int ds18b20_convert(driver_t* drv) {
    ds18b20_priv_t* priv = (ds18b20_priv_t*)drv->priv;

    if (ds18b20_reset(priv) != DRIVER_OK) {
        return DRIVER_ERR_TIMEOUT;
    }

    ds18b20_write_byte(priv, DS18B20_CMD_SKIP_ROM);
    ds18b20_write_byte(priv, DS18B20_CMD_CONVERT_T);

    return DRIVER_OK;
}

driver_t drv_ds18b20 = {
    .name = "ds18b20",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = ds18b20_drv_init,
    .open = ds18b20_drv_open,
    .close = ds18b20_drv_close,
    .read = ds18b20_drv_read,
    .write = ds18b20_drv_write,
    .ioctl = ds18b20_drv_ioctl,
    .deinit = ds18b20_drv_deinit,
    .priv = NULL,
    .next = NULL
};
