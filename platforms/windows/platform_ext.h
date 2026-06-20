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

#define IOT_GPIO_DIR_INPUT       0
#define IOT_GPIO_DIR_OUTPUT      1

#define IOT_GPIO_LEVEL_LOW       0
#define IOT_GPIO_LEVEL_HIGH      1

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
    (void)speed;
    if (bus < 0 || bus >= 2) return -1;

    iot_i2c_table[bus].bus = bus;
    iot_i2c_table[bus].len = 0;

    return bus;
}

static inline int iot_i2c_deinit_internal(int fd) {
    if (fd < 0 || fd >= 2) return -1;

    iot_i2c_table[fd].len = 0;
    return 0;
}

static inline int iot_i2c_write_internal(int fd, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    if (fd < 0 || fd >= 2) return -1;

    iot_i2c_table[fd].addr = addr;
    iot_i2c_table[fd].reg = reg;
    memcpy(iot_i2c_table[fd].data, data, len);
    iot_i2c_table[fd].len = len;

    return (int)len;
}

static inline int iot_i2c_read_internal(int fd, uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    if (fd < 0 || fd >= 2) return -1;

    iot_i2c_table[fd].addr = addr;
    iot_i2c_table[fd].reg = reg;
    memcpy(data, iot_i2c_table[fd].data, len);

    return (int)len;
}

#define iot_i2c_init(bus, speed) \
    iot_i2c_init_internal(bus, speed)

#define iot_i2c_deinit(fd) \
    iot_i2c_deinit_internal(fd)

#define iot_i2c_write(fd, addr, reg, data, len) \
    iot_i2c_write_internal(fd, addr, reg, data, len)

#define iot_i2c_read(fd, addr, reg, data, len) \
    iot_i2c_read_internal(fd, addr, reg, data, len)

#define iot_i2c_write_raw(fd, addr, data, len) \
    iot_i2c_write_internal(fd, addr, 0, data, len)

#define iot_i2c_read_raw(fd, addr, data, len) \
    iot_i2c_read_internal(fd, addr, 0, data, len)

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

static inline int iot_spi_deinit_internal(int fd) {
    if (fd < 0 || fd >= 2) return -1;

    iot_spi_table[fd].len = 0;
    return 0;
}

static inline int iot_spi_transfer_internal(int fd, uint8_t* tx, uint8_t* rx, size_t len, int speed) {
    (void)speed;
    if (fd < 0 || fd >= 2) return -1;

    memcpy(iot_spi_table[fd].tx_buf, tx, len);
    memcpy(rx, iot_spi_table[fd].rx_buf, len);
    iot_spi_table[fd].len = len;

    return (int)len;
}

#define iot_spi_init(bus, cs, mode, speed) \
    iot_spi_init_internal(bus, cs, mode, speed)

#define iot_spi_deinit(fd) \
    iot_spi_deinit_internal(fd)

#define iot_spi_write(fd, data, len) \
    iot_spi_transfer_internal(fd, data, iot_spi_table[fd].rx_buf, len, 0)

#define iot_spi_read(fd, data, len) \
    iot_spi_transfer_internal(fd, iot_spi_table[fd].tx_buf, data, len, 0)

#define iot_spi_transfer(fd, tx, rx, len) \
    iot_spi_transfer_internal(fd, tx, rx, len, 0)

#define iot_spi_transfer_raw(fd, tx, rx, len, speed) \
    iot_spi_transfer_internal(fd, tx, rx, len, speed)

#endif /* IOT_PLATFORM_WINDOWS_EXT_H */
