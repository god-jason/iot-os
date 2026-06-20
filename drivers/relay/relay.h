#ifndef RELAY_H
#define RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

typedef struct {
    int pin;
    int active_high;
} relay_config_t;

typedef struct {
    relay_config_t config;
} relay_priv_t;

int relay_init(driver_t* drv, const relay_config_t* config);
int relay_on(driver_t* drv);
int relay_off(driver_t* drv);
int relay_toggle(driver_t* drv);

extern driver_t drv_relay;

#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */
