#include "dht.h"
#include "../drivers.h"

static dht_priv_t dht11_priv;
static dht_priv_t dht22_priv;

static int dht_read_raw(dht_priv_t* priv, uint8_t* buf) {
    uint8_t bits[5] = {0};
    uint8_t cnt = 7;
    uint8_t idx = 0;
    int i;

    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_LOW);
    driver_delay_ms(priv->config.type == DHT_TYPE_DHT11 ? 18 : 1);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
    driver_delay_us(30);

    driver_gpio_init(priv->config.pin, GPIO_DIR_INPUT);

    uint32_t timeout = 1000;
    while (driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&i) == DRIVER_OK && i == GPIO_LEVEL_LOW && timeout--) {
        driver_delay_us(1);
    }
    if (timeout == 0) return DRIVER_ERR_TIMEOUT;

    timeout = 1000;
    while (driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&i) == DRIVER_OK && i == GPIO_LEVEL_HIGH && timeout--) {
        driver_delay_us(1);
    }
    if (timeout == 0) return DRIVER_ERR_TIMEOUT;

    for (i = 0; i < 40; i++) {
        timeout = 1000;
        while (driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&i) == DRIVER_OK && i == GPIO_LEVEL_LOW && timeout--) {
            driver_delay_us(1);
        }
        if (timeout == 0) return DRIVER_ERR_TIMEOUT;

        uint32_t t = 0;
        timeout = 1000;
        while (driver_gpio_get_level(priv->config.pin, (gpio_level_t*)&i) == DRIVER_OK && i == GPIO_LEVEL_HIGH && timeout--) {
            driver_delay_us(1);
            t++;
        }
        if (timeout == 0) return DRIVER_ERR_TIMEOUT;

        if (t > 40) bits[idx] |= (1 << cnt);

        if (cnt == 0) {
            cnt = 7;
            idx++;
        } else {
            cnt--;
        }
    }

    buf[0] = bits[0];
    buf[1] = bits[1];
    buf[2] = bits[2];
    buf[3] = bits[3];
    buf[4] = bits[4];

    return DRIVER_OK;
}

static int dht_drv_init(driver_t* drv) {
    dht_priv_t* priv = (dht_priv_t*)drv->priv;
    if (!priv) {
        return DRIVER_ERR_INVALID;
    }
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int dht_drv_open(driver_t* drv) {
    dht_priv_t* priv = (dht_priv_t*)drv->priv;
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    driver_gpio_set_level(priv->config.pin, GPIO_LEVEL_HIGH);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int dht_drv_close(driver_t* drv) {
    dht_priv_t* priv = (dht_priv_t*)drv->priv;
    driver_gpio_deinit(priv->config.pin);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int dht_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != sizeof(dht_data_t)) {
        return DRIVER_ERR_INVALID;
    }

    dht_priv_t* priv = (dht_priv_t*)drv->priv;
    dht_data_t* out = (dht_data_t*)data;
    uint8_t buf[5];

    if (dht_read_raw(priv, buf) != DRIVER_OK) {
        return DRIVER_ERR_TIMEOUT;
    }

    if (buf[4] != ((buf[0] + buf[1] + buf[2] + buf[3]) & 0xFF)) {
        return DRIVER_ERR_CRC;
    }

    if (priv->config.type == DHT_TYPE_DHT11) {
        out->humidity = (float)buf[0];
        out->temperature = (float)buf[2];
    } else {
        out->humidity = (float)((buf[0] << 8) | buf[1]) / 10.0f;
        out->temperature = (float)((buf[2] << 8) | buf[3]) / 10.0f;
    }

    return DRIVER_OK;
}

static int dht_drv_write(driver_t* drv, const void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int dht_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int dht_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int dht_init(driver_t* drv, const dht_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        dht_priv_t* priv = (dht_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int dht_read(driver_t* drv, dht_data_t* data) {
    return drv->read(drv, data, sizeof(dht_data_t));
}

static int dht11_drv_init(driver_t* drv) {
    dht_priv_t* priv = &dht11_priv;
    priv->config.type = DHT_TYPE_DHT11;
    priv->config.pin = -1;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int dht22_drv_init(driver_t* drv) {
    dht_priv_t* priv = &dht22_priv;
    priv->config.type = DHT_TYPE_DHT22;
    priv->config.pin = -1;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

driver_t drv_dht11 = {
    .name = "dht11",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = dht11_drv_init,
    .open = dht_drv_open,
    .close = dht_drv_close,
    .read = dht_drv_read,
    .write = dht_drv_write,
    .ioctl = dht_drv_ioctl,
    .deinit = dht_drv_deinit,
    .priv = NULL,
    .next = NULL
};

driver_t drv_dht22 = {
    .name = "dht22",
    .type = DRIVER_TYPE_SENSOR,
    .status = DRIVER_STATUS_UNINIT,
    .init = dht22_drv_init,
    .open = dht_drv_open,
    .close = dht_drv_close,
    .read = dht_drv_read,
    .write = dht_drv_write,
    .ioctl = dht_drv_ioctl,
    .deinit = dht_drv_deinit,
    .priv = NULL,
    .next = NULL
};
