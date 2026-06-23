#ifndef DRIVERS_H
#define DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/*===========================================================
 * 平台扩展接口
 *===========================================================*/
#include "iot.h"
#include "iot_ext.h"

/*===========================================================
 * 驱动类型枚举
 *===========================================================*/

typedef enum {
    DRIVER_TYPE_UNKNOWN = 0,
    DRIVER_TYPE_SENSOR,
    DRIVER_TYPE_DISPLAY,
    DRIVER_TYPE_IO,
    DRIVER_TYPE_ADC,
    DRIVER_TYPE_COMM
} driver_type_t;

/*===========================================================
 * 传感器类型枚举
 *===========================================================*/

typedef enum {
    SENSOR_TYPE_UNKNOWN = 0,
    SENSOR_TYPE_TEMPERATURE,
    SENSOR_TYPE_HUMIDITY,
    SENSOR_TYPE_PRESSURE,
    SENSOR_TYPE_LIGHT,
    SENSOR_TYPE_ACCELEROMETER,
    SENSOR_TYPE_GYROSCOPE,
    SENSOR_TYPE_MAGNETOMETER,
    SENSOR_TYPE_GAS,
    SENSOR_TYPE_PROXIMITY
} sensor_type_t;

/*===========================================================
 * 驱动状态
 *===========================================================*/

typedef enum {
    DRIVER_STATUS_UNINIT = 0,
    DRIVER_STATUS_INITED,
    DRIVER_STATUS_OPENED,
    DRIVER_STATUS_ERROR
} driver_status_t;

/*===========================================================
 * 通用驱动结构体
 *===========================================================*/

typedef struct driver driver_t;

struct driver {
    const char* name;
    driver_type_t type;
    driver_status_t status;
    int (*init)(driver_t* drv);
    int (*open)(driver_t* drv);
    int (*close)(driver_t* drv);
    int (*read)(driver_t* drv, void* data, size_t len);
    int (*write)(driver_t* drv, const void* data, size_t len);
    int (*ioctl)(driver_t* drv, int cmd, void* arg);
    int (*deinit)(driver_t* drv);
    void* priv;
    driver_t* next;
};

/*===========================================================
 * 传感器数据结构体
 *===========================================================*/

typedef struct {
    sensor_type_t type;
    uint64_t timestamp;
    union {
        float temperature;
        float humidity;
        float pressure;
        float light;
        struct { float x; float y; float z; } accel;
        struct { float x; float y; float z; } gyro;
        struct { float x; float y; float z; } mag;
        float gas;
        float proximity;
    } value;
} sensor_data_t;

/*===========================================================
 * 错误码定义
 *===========================================================*/

#define DRIVER_OK               0
#define DRIVER_ERR_INVALID      -1
#define DRIVER_ERR_I2C          -2
#define DRIVER_ERR_SPI          -3
#define DRIVER_ERR_TIMEOUT      -4
#define DRIVER_ERR_NOT_SUPPORTED -5
#define DRIVER_ERR_CRC          -6

/*===========================================================
 * GPIO 类型重定义 (兼容 iot_ext.h)
 *===========================================================*/

#ifndef GPIO_DIR_INPUT
    #define GPIO_DIR_INPUT       0
#endif

#ifndef GPIO_DIR_OUTPUT
    #define GPIO_DIR_OUTPUT      1
#endif

#ifndef GPIO_LEVEL_LOW
    #define GPIO_LEVEL_LOW       0
#endif

#ifndef GPIO_LEVEL_HIGH
    #define GPIO_LEVEL_HIGH      1
#endif

typedef int gpio_dir_t;
typedef int gpio_level_t;

/*===========================================================
 * I2C 句柄和配置
 *===========================================================*/

typedef struct {
    int bus;
    int fd;
    uint8_t addr;
} driver_i2c_t;

int driver_i2c_init(driver_i2c_t* i2c, int bus, int speed);
int driver_i2c_deinit(driver_i2c_t* i2c);
int driver_i2c_write_reg(driver_i2c_t* i2c, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len);
int driver_i2c_read_reg(driver_i2c_t* i2c, uint8_t addr, uint8_t reg, uint8_t* data, size_t len);
int driver_i2c_write_bytes(driver_i2c_t* i2c, uint8_t addr, const uint8_t* data, size_t len);
int driver_i2c_read_bytes(driver_i2c_t* i2c, uint8_t addr, uint8_t* data, size_t len);

/*===========================================================
 * SPI 句柄和配置
 *===========================================================*/

typedef struct {
    int bus;
    int cs;
    int fd;
    int speed;
    int mode;
} driver_spi_t;

int driver_spi_init(driver_spi_t* spi, int bus, int cs, int speed, int mode);
int driver_spi_deinit(driver_spi_t* spi);
int driver_spi_transfer(driver_spi_t* spi, uint8_t* tx, uint8_t* rx, size_t len);

/*===========================================================
 * GPIO 接口定义 (直接使用 iot_ext.h)
 *===========================================================*/

#define driver_gpio_init(pin, dir) \
    iot_gpio_init(pin, dir)

#define driver_gpio_deinit(pin) \
    iot_gpio_deinit(pin)

#define driver_gpio_set_level(pin, level) \
    iot_gpio_write(pin, level)

#define driver_gpio_get_level(pin, level_ptr) \
    (*(level_ptr) = iot_gpio_read(pin))

/*===========================================================
 * 延时接口定义
 *===========================================================*/

void driver_delay_ms(uint32_t ms);
void driver_delay_us(uint32_t us);

/*===========================================================
 * 驱动注册接口
 *===========================================================*/

int driver_register(driver_t* drv);
driver_t* driver_find(const char* name);
int driver_unregister(const char* name);

/*===========================================================
 * 内置驱动声明
 *===========================================================*/

extern driver_t drv_bme280;
extern driver_t drv_bh1750;
extern driver_t drv_dht11;
extern driver_t drv_dht22;
extern driver_t drv_ssd1306;
extern driver_t drv_mpu6050;
extern driver_t drv_pcf8574;
extern driver_t drv_ads1115;
extern driver_t drv_ds18b20;
extern driver_t drv_hmc5883l;
extern driver_t drv_lcd1602;
extern driver_t drv_relay;
extern driver_t drv_led;
extern driver_t drv_buzzer;

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_H */
