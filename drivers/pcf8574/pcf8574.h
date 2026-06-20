#ifndef PCF8574_H
#define PCF8574_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define PCF8574_ADDR_BASE          0x20

typedef struct {
    uint8_t addr;
    uint8_t output;
} pcf8574_priv_t;

int pcf8574_init(driver_t* drv, uint8_t addr);
int pcf8574_set_port(driver_t* drv, uint8_t value);
int pcf8574_get_port(driver_t* drv, uint8_t* value);
int pcf8574_set_pin(driver_t* drv, uint8_t pin, int level);
int pcf8574_get_pin(driver_t* drv, uint8_t pin, int* level);

extern driver_t drv_pcf8574;

#ifdef __cplusplus
}
#endif

#endif /* PCF8574_H */
