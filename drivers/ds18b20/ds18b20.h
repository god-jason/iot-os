#ifndef DS18B20_H
#define DS18B20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

#define DS18B20_CMD_CONVERT_T      0x44
#define DS18B20_CMD_READ_SCRATCH   0xBE
#define DS18B20_CMD_WRITE_SCRATCH  0x4E
#define DS18B20_CMD_COPY_SCRATCH   0x48
#define DS18B20_CMD_RECALL_E2      0xB8
#define DS18B20_CMD_READ_PWR       0xB4
#define DS18B20_CMD_SEARCH_ROM     0xF0
#define DS18B20_CMD_READ_ROM       0x33
#define DS18B20_CMD_MATCH_ROM      0x55
#define DS18B20_CMD_SKIP_ROM       0xCC

typedef struct {
    int pin;
    uint8_t rom[8];
} ds18b20_config_t;

typedef struct {
    ds18b20_config_t config;
} ds18b20_priv_t;

typedef struct {
    float temperature;
} ds18b20_data_t;

int ds18b20_init(driver_t* drv, const ds18b20_config_t* config);
int ds18b20_read(driver_t* drv, ds18b20_data_t* data);
int ds18b20_convert(driver_t* drv);

extern driver_t drv_ds18b20;

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_H */
