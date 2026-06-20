#include "relay.h"
#include "../drivers.h"

static relay_priv_t relay_priv;

static int relay_drv_init(driver_t* drv) {
    relay_priv_t* priv = &relay_priv;
    priv->config.pin = -1;
    priv->config.active_high = 1;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int relay_drv_open(driver_t* drv) {
    relay_priv_t* priv = (relay_priv_t*)drv->priv;
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    relay_off(drv);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int relay_drv_close(driver_t* drv) {
    relay_priv_t* priv = (relay_priv_t*)drv->priv;
    relay_off(drv);
    driver_gpio_deinit(priv->config.pin);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int relay_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int relay_drv_write(driver_t* drv, const void* data, size_t len) {
    if (len != sizeof(int)) {
        return DRIVER_ERR_INVALID;
    }
    int* state = (int*)data;
    return (*state) ? relay_on(drv) : relay_off(drv);
}

static int relay_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int relay_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int relay_init(driver_t* drv, const relay_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        relay_priv_t* priv = (relay_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int relay_on(driver_t* drv) {
    relay_priv_t* priv = (relay_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    return DRIVER_OK;
}

int relay_off(driver_t* drv) {
    relay_priv_t* priv = (relay_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    return DRIVER_OK;
}

int relay_toggle(driver_t* drv) {
    relay_priv_t* priv = (relay_priv_t*)drv->priv;
    gpio_level_t level;
    driver_gpio_get_level(priv->config.pin, &level);
    driver_gpio_set_level(priv->config.pin, level == GPIO_LEVEL_HIGH ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    return DRIVER_OK;
}

driver_t drv_relay = {
    .name = "relay",
    .type = DRIVER_TYPE_IO,
    .status = DRIVER_STATUS_UNINIT,
    .init = relay_drv_init,
    .open = relay_drv_open,
    .close = relay_drv_close,
    .read = relay_drv_read,
    .write = relay_drv_write,
    .ioctl = relay_drv_ioctl,
    .deinit = relay_drv_deinit,
    .priv = NULL,
    .next = NULL
};
