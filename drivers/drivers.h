#ifndef DRIVERS_H
#define DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
 * I2C 接口定义
 *===========================================================*/

typedef struct {
    int bus;
    uint8_t addr;
    int speed;
} i2c_config_t;

int driver_i2c_init(const i2c_config_t* config);
int driver_i2c_deinit(void);
int driver_i2c_read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len);
int driver_i2c_write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len);
int driver_i2c_read_bytes(uint8_t addr, uint8_t* data, size_t len);
int driver_i2c_write_bytes(uint8_t addr, const uint8_t* data, size_t len);

/*===========================================================
 * SPI 接口定义
 *===========================================================*/

typedef struct {
    int bus;
    int cs_pin;
    int speed;
    int mode;
} spi_config_t;

int driver_spi_init(const spi_config_t* config);
int driver_spi_deinit(void);
int driver_spi_transfer(uint8_t* tx, uint8_t* rx, size_t len);

/*===========================================================
 * GPIO 接口定义
 *===========================================================*/

typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT
} gpio_dir_t;

typedef enum {
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH
} gpio_level_t;

int driver_gpio_init(int pin, gpio_dir_t dir);
int driver_gpio_deinit(int pin);
int driver_gpio_set_level(int pin, gpio_level_t level);
int driver_gpio_get_level(int pin, gpio_level_t* level);

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

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_H */
