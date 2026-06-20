#ifndef MPU6050_H
#define MPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

#define MPU6050_ADDR_PRIMARY       0x68
#define MPU6050_ADDR_SECONDARY     0x69

#define MPU6050_REG_WHO_AM_I       0x75
#define MPU6050_REG_PWR_MGMT_1     0x6B
#define MPU6050_REG_SMPLRT_DIV     0x19
#define MPU6050_REG_CONFIG         0x1A
#define MPU6050_REG_GYRO_CONFIG    0x1B
#define MPU6050_REG_ACCEL_CONFIG   0x1C
#define MPU6050_REG_ACCEL_XOUT_H   0x3B
#define MPU6050_REG_ACCEL_XOUT_L   0x3C
#define MPU6050_REG_ACCEL_YOUT_H   0x3D
#define MPU6050_REG_ACCEL_YOUT_L   0x3E
#define MPU6050_REG_ACCEL_ZOUT_H   0x3F
#define MPU6050_REG_ACCEL_ZOUT_L   0x40
#define MPU6050_REG_TEMP_OUT_H     0x41
#define MPU6050_REG_TEMP_OUT_L     0x42
#define MPU6050_REG_GYRO_XOUT_H    0x43
#define MPU6050_REG_GYRO_XOUT_L    0x44
#define MPU6050_REG_GYRO_YOUT_H    0x45
#define MPU6050_REG_GYRO_YOUT_L    0x46
#define MPU6050_REG_GYRO_ZOUT_H    0x47
#define MPU6050_REG_GYRO_ZOUT_L    0x48

#define MPU6050_ACCEL_RANGE_2G     0x00
#define MPU6050_ACCEL_RANGE_4G     0x08
#define MPU6050_ACCEL_RANGE_8G     0x10
#define MPU6050_ACCEL_RANGE_16G    0x18

#define MPU6050_GYRO_RANGE_250DPS  0x00
#define MPU6050_GYRO_RANGE_500DPS  0x08
#define MPU6050_GYRO_RANGE_1000DPS 0x10
#define MPU6050_GYRO_RANGE_2000DPS 0x18

typedef struct {
    uint8_t addr;
    uint8_t accel_range;
    uint8_t gyro_range;
} mpu6050_config_t;

typedef struct {
    mpu6050_config_t config;
    float accel_scale;
    float gyro_scale;
} mpu6050_priv_t;

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float temperature;
} mpu6050_data_t;

int mpu6050_init(driver_t* drv, const mpu6050_config_t* config);
int mpu6050_read(driver_t* drv, mpu6050_data_t* data);

extern driver_t drv_mpu6050;

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_H */
