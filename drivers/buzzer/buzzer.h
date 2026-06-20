#ifndef BUZZER_H
#define BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

typedef struct {
    int pin;
    int active_high;
} buzzer_config_t;

typedef struct {
    buzzer_config_t config;
} buzzer_priv_t;

int buzzer_init(driver_t* drv, const buzzer_config_t* config);
int buzzer_on(driver_t* drv);
int buzzer_off(driver_t* drv);
int buzzer_beep(driver_t* drv, int duration_ms);

extern driver_t drv_buzzer;

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H */
