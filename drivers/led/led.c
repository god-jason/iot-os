#include "led.h"
#include "../drivers.h"

static led_priv_t led_priv;

static int led_drv_init(driver_t* drv) {
    led_priv_t* priv = &led_priv;
    priv->config.pin = -1;
    priv->config.active_high = 1;
    drv->priv = priv;
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int led_drv_open(driver_t* drv) {
    led_priv_t* priv = (led_priv_t*)drv->priv;
    driver_gpio_init(priv->config.pin, GPIO_DIR_OUTPUT);
    led_off(drv);
    drv->status = DRIVER_STATUS_OPENED;
    return DRIVER_OK;
}

static int led_drv_close(driver_t* drv) {
    led_priv_t* priv = (led_priv_t*)drv->priv;
    led_off(drv);
    driver_gpio_deinit(priv->config.pin);
    drv->status = DRIVER_STATUS_INITED;
    return DRIVER_OK;
}

static int led_drv_read(driver_t* drv, void* data, size_t len) {
    (void)drv;
    (void)data;
    (void)len;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int led_drv_write(driver_t* drv, const void* data, size_t len) {
    if (len != sizeof(int)) {
        return DRIVER_ERR_INVALID;
    }
    int* state = (int*)data;
    return (*state) ? led_on(drv) : led_off(drv);
}

static int led_drv_ioctl(driver_t* drv, int cmd, void* arg) {
    (void)drv;
    (void)cmd;
    (void)arg;
    return DRIVER_ERR_NOT_SUPPORTED;
}

static int led_drv_deinit(driver_t* drv) {
    drv->status = DRIVER_STATUS_UNINIT;
    return DRIVER_OK;
}

int led_init(driver_t* drv, const led_config_t* config) {
    if (drv->init(drv) != DRIVER_OK) {
        return DRIVER_ERR_INVALID;
    }
    if (config) {
        led_priv_t* priv = (led_priv_t*)drv->priv;
        priv->config = *config;
    }
    return DRIVER_OK;
}

int led_on(driver_t* drv) {
    led_priv_t* priv = (led_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    return DRIVER_OK;
}

int led_off(driver_t* drv) {
    led_priv_t* priv = (led_priv_t*)drv->priv;
    driver_gpio_set_level(priv->config.pin, priv->config.active_high ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    return DRIVER_OK;
}

int led_toggle(driver_t* drv) {
    led_priv_t* priv = (led_priv_t*)drv->priv;
    gpio_level_t level;
    driver_gpio_get_level(priv->config.pin, &level);
    driver_gpio_set_level(priv->config.pin, level == GPIO_LEVEL_HIGH ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    return DRIVER_OK;
}

driver_t drv_led = {
    .name = "led",
    .type = DRIVER_TYPE_IO,
    .status = DRIVER_STATUS_UNINIT,
    .init = led_drv_init,
    .open = led_drv_open,
    .close = led_drv_close,
    .read = led_drv_read,
    .write = led_drv_write,
    .ioctl = led_drv_ioctl,
    .deinit = led_drv_deinit,
    .priv = NULL,
    .next = NULL
};
