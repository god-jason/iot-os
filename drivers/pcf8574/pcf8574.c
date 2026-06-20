#include "pcf8574.h"
#include "../drivers.h"

static pcf8574_priv_t pcf8574_priv;

static int pcf8574_drv_init(driver_t* drv) {
    pcf8574_priv_t* priv = &pcf8574_priv;
    priv->addr = PCF8574_ADDR_BASE;
    priv->output = 0xFF;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int pcf8574_drv_open(driver_t* drv) {
    pcf8574_priv_t* priv = (pcf8574_priv_t*)drv->priv;
    driver_i2c_write_bytes(priv->addr, &priv->output, 1);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int pcf8574_drv_close(driver_t* drv) {
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int pcf8574_drv_read(driver_t* drv, void* data, size_t len) {
    if (len != 1) {
        return DRIVER_ERR_INVALID;
    }

    pcf8574_priv_t* priv = (pcf8574_priv_t*)drv->priv;
    return driver_i2c_read_bytes(priv->addr, (uint8_t*)data, 1);
}

static int pcf8574_drv_write(driver_t* drv, const void* data, size_t len) {
    if (len != 1) {
        return DRIVER_ERR_INVALID;
    }

    pcf8574_priv_t* priv = (pcf8574_priv_t*)drv->priv;
    priv->output = *(const uint8_t*)data;
    return driver_i2c_write_bytes(priv->addr, (uint8_t*)data, 1);
}

static int pcf8574_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int pcf8574_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int pcf8574_init(driver_t* drv, uint8_t addr) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    pcf8574_priv_t* priv = (pcf8574_priv_t*)drv->priv;
    priv->addr = addr;
    return DRIVER_OK;
}

int pcf8574_set_port(driver_t* drv, uint8_t value) {
    return drv->write(drv, &value, 1);
}

int pcf8574_get_port(driver_t* drv, uint8_t* value) {
    return drv->read(drv, value, 1);
}

int pcf8574_set_pin(driver_t* drv, uint8_t pin, int level) {
    if (pin > 7) {
        return DRIVER_ERR_INVALID;
    }

    pcf8574_priv_t* priv = (pcf8574_priv_t*)drv->priv;
    if (level) {
        priv->output |= (1 << pin);
    } else {
        priv->output &= ~(1 << pin);
    }
    return driver_i2c_write_bytes(priv->addr, &priv->output, 1);
}

int pcf8574_get_pin(driver_t* drv, uint8_t pin, int* level) {
    if (pin > 7) {
        return DRIVER_ERR_INVALID;
    }

    uint8_t value;
    if (drv->read(drv, &value, 1) != DRIVER_OK) {
        return DRIVER_ERR_I2C;
    }
    *level = (value >> pin) & 1;
    return DRIVER_OK;
}

driver_t drv_pcf8574 = {
    .name = "pcf8574",
    .type = DRIVER_TYPE_IO,
    .status = DRIVER_STATUS_UNINIT,
    .init = pcf8574_drv_init,
    .open = pcf8574_drv_open,
    .close = pcf8574_drv_close,
    .read = pcf8574_drv_read,
    .write = pcf8574_drv_write,
    .ioctl = pcf8574_drv_ioctl,
    .deinit = pcf8574_drv_deinit,
    .priv = NULL,
    .next = NULL
};
