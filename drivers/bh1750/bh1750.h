#ifndef BH1750_H
#define BH1750_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define BH1750_ADDR_PRIMARY       0x23
#define BH1750_ADDR_SECONDARY     0x5C

#define BH1750_REG_CONTROL        0x00
#define BH1750_REG_TIMING         0x01
#define BH1750_REG_MEASURE_HIGH   0x20
#define BH1750_REG_MEASURE_HIGH2  0x21
#define BH1750_REG_MEASURE_LOW    0x23
#define BH1750_REG_POWER_DOWN     0x00
#define BH1750_REG_POWER_ON       0x01

typedef struct {
    uint8_t addr;
    uint8_t mode;
    uint8_t mtreg;
} bh1750_config_t;

typedef struct {
    bh1750_config_t config;
} bh1750_priv_t;

typedef struct {
    float lux;
} bh1750_data_t;

int bh1750_init(driver_t* drv, const bh1750_config_t* config);
int bh1750_read(driver_t* drv, bh1750_data_t* data);

extern driver_t drv_bh1750;

#ifdef __cplusplus
}
#endif

#endif /* BH1750_H */
