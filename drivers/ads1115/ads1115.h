#ifndef ADS1115_H
#define ADS1115_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define ADS1115_ADDR_BASE          0x48

#define ADS1115_REG_CONVERSION     0x00
#define ADS1115_REG_CONFIG         0x01
#define ADS1115_REG_LO_THRESH      0x02
#define ADS1115_REG_HI_THRESH      0x03

#define ADS1115_CFG_OS_MASK        0x8000
#define ADS1115_CFG_OS_SINGLE      0x8000

#define ADS1115_CFG_MUX_MASK       0x7000
#define ADS1115_CFG_MUX_AIN0_AIN1  0x0000
#define ADS1115_CFG_MUX_AIN0_AIN3  0x1000
#define ADS1115_CFG_MUX_AIN1_AIN3  0x2000
#define ADS1115_CFG_MUX_AIN2_AIN3  0x3000
#define ADS1115_CFG_MUX_AIN0_GND   0x4000
#define ADS1115_CFG_MUX_AIN1_GND   0x5000
#define ADS1115_CFG_MUX_AIN2_GND   0x6000
#define ADS1115_CFG_MUX_AIN3_GND   0x7000

#define ADS1115_CFG_PGA_MASK       0x0E00
#define ADS1115_CFG_PGA_6144V      0x0000
#define ADS1115_CFG_PGA_4096V      0x0200
#define ADS1115_CFG_PGA_2048V      0x0400
#define ADS1115_CFG_PGA_1024V      0x0600
#define ADS1115_CFG_PGA_0512V      0x0800
#define ADS1115_CFG_PGA_0256V      0x0A00

#define ADS1115_CFG_MODE_MASK      0x0100
#define ADS1115_CFG_MODE_CONTIN    0x0000
#define ADS1115_CFG_MODE_SINGLE    0x0100

#define ADS1115_CFG_DR_MASK        0x00E0
#define ADS1115_CFG_DR_8SPS        0x0000
#define ADS1115_CFG_DR_16SPS       0x0020
#define ADS1115_CFG_DR_32SPS       0x0040
#define ADS1115_CFG_DR_64SPS       0x0060
#define ADS1115_CFG_DR_128SPS      0x0080
#define ADS1115_CFG_DR_250SPS      0x00A0
#define ADS1115_CFG_DR_475SPS      0x00C0
#define ADS1115_CFG_DR_860SPS      0x00E0

typedef struct {
    uint8_t addr;
    uint8_t mux;
    uint8_t pga;
    uint8_t mode;
    uint8_t dr;
    float vref;
} ads1115_config_t;

typedef struct {
    ads1115_config_t config;
} ads1115_priv_t;

typedef struct {
    int16_t raw;
    float voltage;
} ads1115_data_t;

int ads1115_init(driver_t* drv, const ads1115_config_t* config);
int ads1115_read(driver_t* drv, int channel, ads1115_data_t* data);

extern driver_t drv_ads1115;

#ifdef __cplusplus
}
#endif

#endif /* ADS1115_H */
