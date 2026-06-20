/**
 * @file platform_ext.h
 * @brief Windows 平台扩展适配头文件 - GPIO/I2C/SPI
 * @note Windows 平台 GPIO/I2C/SPI 需要使用第三方库或硬件接口
 */
#ifndef IOT_PLATFORM_WINDOWS_EXT_H
#define IOT_PLATFORM_WINDOWS_EXT_H

#include "platform.h"

/*===========================================================
 * GPIO 适配层 (模拟接口)
 *===========================================================*/

#define IOT_GPIO_MAX_PIN         32

typedef struct {
    int pin;
    int direction;
    int value;
    int initialized;
} iot_gpio_handle_t;

static iot_gpio_handle_t iot_gpio_table[IOT_GPIO_MAX_PIN];

static inline int iot_gpio_init_internal(int pin, int dir) {
    if (pin < 0 || pin >= IOT_GPIO_MAX_PIN) return -1;

    iot_gpio_table[pin].pin = pin;
    iot_gpio_table[pin].direction = dir;
    iot_gpio_table[pin].value = 0;
    iot_gpio_table[pin].initialized = 1;

    return 0;
}

static inline int iot_gpio_deinit_internal(int pin) {
    if (pin < 0 || pin >= IOT_GPIO_MAX_PIN) return -1;

    iot_gpio_table[pin].initialized = 0;
    return 0;
}

static inline int iot_gpio_write_internal(int pin, int value) {
    if (pin < 0 || pin >= IOT_GPIO_MAX_PIN) return -1;
    if (!iot_gpio_table[pin].initialized) return -1;

    iot_gpio_table[pin].value = value;
    return 0;
}

static inline int iot_gpio_read_internal(int pin) {
    if (pin < 0 || pin >= IOT_GPIO_MAX_PIN) return -1;
    if (!iot_gpio_table[pin].initialized) return -1;

    return iot_gpio_table[pin].value;
}

#define iot_gpio_init(pin, dir) \
    iot_gpio_init_internal(pin, dir)

#define iot_gpio_deinit(pin) \
    iot_gpio_deinit_internal(pin)

#define iot_gpio_write(pin, value) \
    iot_gpio_write_internal(pin, value)

#define iot_gpio_read(pin) \
    iot_gpio_read_internal(pin)

/*===========================================================
 * I2C 适配层 (模拟接口)
 *===========================================================*/

#define IOT_I2C_BUS_0           0
#define IOT_I2C_BUS_1           1

typedef struct {
    int bus;
    uint8_t addr;
    uint8_t reg;
    uint8_t data[256];
    size_t len;
} iot_i2c_handle_t;

static iot_i2c_handle_t iot_i2c_table[2];

static inline int iot_i2c_init_internal(int bus, int speed) {
    if (bus < 0 || bus >= 2) return -1;

    iot_i2c_table[bus].bus = bus;
    iot_i2c_table[bus].len = 0;

    return bus;
}

static inline int iot_i2c_deinit_internal(int bus) {
    if (bus < 0 || bus >= 2) return -1;

    iot_i2c_table[bus].len = 0;
    return 0;
}

static inline int iot_i2c_write_internal(int bus, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    if (bus < 0 || bus >= 2) return -1;

    iot_i2c_table[bus].addr = addr;
    iot_i2c_table[bus].reg = reg;
    memcpy(iot_i2c_table[bus].data, data, len);
    iot_i2c_table[bus].len = len;

    return (int)len;
}

static inline int iot_i2c_read_internal(int bus, uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    if (bus < 0 || bus >= 2) return -1;

    iot_i2c_table[bus].addr = addr;
    iot_i2c_table[bus].reg = reg;
    memcpy(data, iot_i2c_table[bus].data, len);

    return (int)len;
}

#define iot_i2c_init(bus, speed) \
    iot_i2c_init_internal(bus, speed)

#define iot_i2c_deinit(bus) \
    iot_i2c_deinit_internal(bus)

#define iot_i2c_write(bus, addr, reg, data, len) \
    iot_i2c_write_internal(bus, addr, reg, data, len)

#define iot_i2c_read(bus, addr, reg, data, len) \
    iot_i2c_read_internal(bus, addr, reg, data, len)

#define iot_i2c_write_raw(bus, addr, data, len) \
    iot_i2c_write_internal(bus, addr, 0, data, len)

#define iot_i2c_read_raw(bus, addr, data, len) \
    iot_i2c_read_internal(bus, addr, 0, data, len)

/*===========================================================
 * SPI 适配层 (模拟接口)
 *===========================================================*/

#define IOT_SPI_BUS_0           0
#define IOT_SPI_BUS_1           1

#define IOT_SPI_MODE_0          0
#define IOT_SPI_MODE_1          1
#define IOT_SPI_MODE_2          2
#define IOT_SPI_MODE_3          3

typedef struct {
    int bus;
    int cs;
    int mode;
    int speed;
    uint8_t tx_buf[256];
    uint8_t rx_buf[256];
    size_t len;
} iot_spi_handle_t;

static iot_spi_handle_t iot_spi_table[2];

static inline int iot_spi_init_internal(int bus, int cs, int mode, int speed) {
    if (bus < 0 || bus >= 2) return -1;

    iot_spi_table[bus].bus = bus;
    iot_spi_table[bus].cs = cs;
    iot_spi_table[bus].mode = mode;
    iot_spi_table[bus].speed = speed;
    iot_spi_table[bus].len = 0;

    return bus;
}

static inline int iot_spi_deinit_internal(int bus) {
    if (bus < 0 || bus >= 2) return -1;

    iot_spi_table[bus].len = 0;
    return 0;
}

static inline int iot_spi_transfer_internal(int bus, uint8_t* tx, uint8_t* rx, size_t len) {
    if (bus < 0 || bus >= 2) return -1;

    memcpy(iot_spi_table[bus].tx_buf, tx, len);
    memcpy(rx, iot_spi_table[bus].rx_buf, len);
    iot_spi_table[bus].len = len;

    return (int)len;
}

#define iot_spi_init(bus, cs, mode, speed) \
    iot_spi_init_internal(bus, cs, mode, speed)

#define iot_spi_deinit(bus) \
    iot_spi_deinit_internal(bus)

#define iot_spi_write(bus, data, len) \
    iot_spi_transfer_internal(bus, data, iot_spi_table[bus].rx_buf, len)

#define iot_spi_read(bus, data, len) \
    iot_spi_transfer_internal(bus, iot_spi_table[bus].tx_buf, data, len)

#define iot_spi_transfer_raw(bus, tx, rx, len, speed) \
    iot_spi_transfer_internal(bus, tx, rx, len)

#endif /* IOT_PLATFORM_WINDOWS_EXT_H */