#ifndef HMC5883L_H
#define HMC5883L_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../drivers.h"

#define HMC5883L_ADDR_PRIMARY       0x1E

#define HMC5883L_REG_CONFIG_A      0x00
#define HMC5883L_REG_CONFIG_B      0x01
#define HMC5883L_REG_MODE          0x02
#define HMC5883L_REG_OUT_X_MSB     0x03
#define HMC5883L_REG_OUT_X_LSB     0x04
#define HMC5883L_REG_OUT_Z_MSB     0x05
#define HMC5883L_REG_OUT_Z_LSB     0x06
#define HMC5883L_REG_OUT_Y_MSB     0x07
#define HMC5883L_REG_OUT_Y_LSB     0x08
#define HMC5883L_REG_STATUS        0x09
#define HMC5883L_REG_ID_A          0x0A
#define HMC5883L_REG_ID_B          0x0B
#define HMC5883L_REG_ID_C          0x0C

#define HMC5883L_MODE_CONTINUOUS   0x00
#define HMC5883L_MODE_SINGLE       0x01
#define HMC5883L_MODE_IDLE         0x02

#define HMC5883L_SCALE_088GAUSS    0x00
#define HMC5883L_SCALE_13GAUSS     0x01
#define HMC5883L_SCALE_19GAUSS     0x02
#define HMC5883L_SCALE_25GAUSS     0x03
#define HMC5883L_SCALE_40GAUSS     0x04
#define HMC5883L_SCALE_47GAUSS     0x05
#define HMC5883L_SCALE_56GAUSS     0x06
#define HMC5883L_SCALE_81GAUSS     0x07

typedef struct {
    uint8_t addr;
    uint8_t mode;
    uint8_t scale;
    float sensitivity;
} hmc5883l_config_t;

typedef struct {
    hmc5883l_config_t config;
} hmc5883l_priv_t;

typedef struct {
    float x;
    float y;
    float z;
} hmc5883l_data_t;

int hmc5883l_init(driver_t* drv, const hmc5883l_config_t* config);
int hmc5883l_read(driver_t* drv, hmc5883l_data_t* data);

extern driver_t drv_hmc5883l;

#ifdef __cplusplus
}
#endif

#endif /* HMC5883L_H */
