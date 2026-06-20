#include "buzzer.h"
#include "../drivers.h"

static buzzer_priv_t buzzer_priv;

static int buzzer_drv_init(driver_t* drv) {
    buzzer_priv_t* priv = &buzzer_priv;
    priv->config.pin = -1;
    priv->config.active_high = 1;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int buzzer_drv_open(driver_t* drv) {
    buzzer_priv_t* priv = (buzzer_priv_t*)drv->priv;
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    buzzer_off(drv);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int buzzer_drv_close(driver_t* drv) {
    buzzer_priv_t* priv = (buzzer_priv_t*)drv->priv;
    buzzer_off(drv);
    driver_gpio_deinit(priv->config.pin);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int buzzer_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int buzzer_drv_write(driver_t* drv, const void* data, size_t len) {
    if (len != sizeof(int)) {
        return DRIVER_ERR_INVALID;
    }
    int* state = (int*)data;
    return (*state) ? buzzer_on(drv) : buzzer_off(drv);
}

static int buzzer_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int buzzer_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int buzzer_init(driver_t* drv, const buzzer_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        buzzer_priv_t* priv = (buzzer_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int buzzer_on(driver_t* drv) {
    buzzer_priv_t* priv = (buzzer_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    return DRIVER_OK;
}

int buzzer_off(driver_t* drv) {
    buzzer_priv_t* priv = (buzzer_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    return DRIVER_OK;
}

int buzzer_beep(driver_t* drv, int duration_ms) {
    buzzer_on(drv);
    driver_delay_ms(duration_ms);
    buzzer_off(drv);
    return DRIVER_OK;
}

driver_t drv_buzzer = {
    .name = "buzzer",
    .type = DRIVER_TYPE_IO,
    .status = DRIVER_STATUS_UNINIT,
    .init = buzzer_drv_init,
    .open = buzzer_drv_open,
    .close = buzzer_drv_close,
    .read = buzzer_drv_read,
    .write = buzzer_drv_write,
    .ioctl = buzzer_drv_ioctl,
    .deinit = buzzer_drv_deinit,
    .priv = NULL,
    .next = NULL
};
