/**
 * @file iot_ext.h
 * @brief WebAssembly 平台扩展适配头文件
 */
#ifndef IOT_PLATFORM_WASM_EXT_H
#define IOT_PLATFORM_WASM_EXT_H

#include "iot.h"

#define IOT_GPIO_DIR_INPUT       0
#define IOT_GPIO_DIR_OUTPUT      1

#define IOT_GPIO_LEVEL_LOW       0
#define IOT_GPIO_LEVEL_HIGH      1

#define IOT_GPIO_MAX_PIN         256

static inline int iot_gpio_export(int pin) {
    return 0;
}

static inline int iot_gpio_unexport(int pin) {
    return 0;
}

static inline int iot_gpio_set_direction(int pin, int dir) {
    return 0;
}

static inline int iot_gpio_set_value(int pin, int value) {
    return 0;
}

static inline int iot_gpio_get_value(int pin) {
    return 0;
}

#define iot_gpio_init(pin, dir) \
    (0)

#define iot_gpio_deinit(pin) \
    do {} while(0)

#define iot_gpio_write(pin, value) \
    (0)

#define iot_gpio_read(pin) \
    (0)

#define iot_gpio_set_dir(pin, dir) \
    (0)

#define IOT_I2C_BUS_0           0
#define IOT_I2C_BUS_1           1
#define IOT_I2C_BUS_2           2

static inline int iot_i2c_open_bus(int bus) {
    return -1;
}

static inline int iot_i2c_set_addr(int fd, uint8_t addr) {
    return -1;
}

static inline int iot_i2c_write_reg(int fd, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    return -1;
}

static inline int iot_i2c_read_reg(int fd, uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    return -1;
}

static inline int iot_i2c_close(int fd) {
    return 0;
}

#define IOT_SPI_BUS_0           0
#define IOT_SPI_BUS_1           1

static inline int iot_spi_open_bus(int bus) {
    return -1;
}

static inline int iot_spi_set_mode(int fd, int mode) {
    return -1;
}

static inline int iot_spi_set_speed(int fd, uint32_t speed) {
    return -1;
}

static inline int iot_spi_transfer(int fd, const uint8_t* tx_buf, uint8_t* rx_buf, size_t len) {
    return -1;
}

static inline int iot_spi_close(int fd) {
    return 0;
}

#endif /* IOT_PLATFORM_WASM_EXT_H */