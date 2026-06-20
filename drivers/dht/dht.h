#ifndef DHT_H
#define DHT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define DHT_TYPE_DHT11            11
#define DHT_TYPE_DHT22            22

typedef struct {
    int pin;
    int type;
} dht_config_t;

typedef struct {
    dht_config_t config;
} dht_priv_t;

typedef struct {
    float temperature;
    float humidity;
} dht_data_t;

int dht_init(driver_t* drv, const dht_config_t* config);
int dht_read(driver_t* drv, dht_data_t* data);

extern driver_t drv_dht11;
extern driver_t drv_dht22;

#ifdef __cplusplus
}
#endif

#endif /* DHT_H */
