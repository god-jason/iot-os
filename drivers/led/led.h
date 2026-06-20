#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

typedef struct {
    int pin;
    int active_high;
} led_config_t;

typedef struct {
    led_config_t config;
} led_priv_t;

int led_init(driver_t* drv, const led_config_t* config);
int led_on(driver_t* drv);
int led_off(driver_t* drv);
int led_toggle(driver_t* drv);

extern driver_t drv_led;

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
