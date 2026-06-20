/**
 * @file platform_ext.h
 * @brief Linux 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_LINUX_EXT_H
#define IOT_PLATFORM_LINUX_EXT_H

#include "platform.h"

/*===========================================================
 * GPIO 适配层
 *===========================================================*/

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define IOT_GPIO_DIR_INPUT       0
#define IOT_GPIO_DIR_OUTPUT      1

#define IOT_GPIO_LEVEL_LOW       0
#define IOT_GPIO_LEVEL_HIGH      1

#define IOT_GPIO_MAX_PIN         256

static inline int iot_gpio_export(int pin) {
    char path[64];
    char buf[16];
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) return -1;

    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf));
    close(fd);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    for (int i = 0; i < 10; i++) {
        if (access(path, F_OK) == 0) break;
        usleep(100000);
    }

    return 0;
}

static inline int iot_gpio_unexport(int pin) {
    char buf[16];
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) return -1;

    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf));
    close(fd);

    return 0;
}

static inline int iot_gpio_set_direction(int pin, int dir) {
    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;

    if (dir == 0) {
        write(fd, "in", 2);
    } else {
        write(fd, "out", 3);
    }
    close(fd);

    return 0;
}

static inline int iot_gpio_set_value(int pin, int value) {
    char path[64];
    char buf[2];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) return -1;

    buf[0] = value ? '1' : '0';
    buf[1] = '\0';
    write(fd, buf, 1);
    close(fd);

    return 0;
}

static inline int iot_gpio_get_value(int pin) {
    char path[64];
    char buf[2];
    int fd;
    int value = 0;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (fd < 0) return -1;

    read(fd, buf, 1);
    value = (buf[0] == '1') ? 1 : 0;
    close(fd);

    return value;
}

#define iot_gpio_init(pin, dir) ({ \
    iot_gpio_export(pin); \
    iot_gpio_set_direction(pin, dir); \
    0; \
})

#define iot_gpio_deinit(pin) \
    iot_gpio_unexport(pin)

#define iot_gpio_write(pin, value) \
    iot_gpio_set_value(pin, value)

#define iot_gpio_read(pin) \
    iot_gpio_get_value(pin)

#define iot_gpio_set_dir(pin, dir) \
    iot_gpio_set_direction(pin, dir)

/*===========================================================
 * I2C 适配层
 *===========================================================*/

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define IOT_I2C_BUS_0           0
#define IOT_I2C_BUS_1           1
#define IOT_I2C_BUS_2           2

static inline int iot_i2c_open_bus(int bus) {
    char path[32];
    int fd;

    snprintf(path, sizeof(path), "/dev/i2c-%d", bus);
    fd = open(path, O_RDWR);
    return fd;
}

static inline int iot_i2c_set_addr(int fd, uint8_t addr) {
    return ioctl(fd, I2C_SLAVE, addr);
}

static inline int iot_i2c_write_reg(int fd, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    uint8_t buf[256];
    buf[0] = reg;
    memcpy(buf + 1, data, len);
    iot_i2c_set_addr(fd, addr);
    return write(fd, buf, len + 1);
}

static inline int iot_i2c_read_reg(int fd, uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    iot_i2c_set_addr(fd, addr);
    write(fd, &reg, 1);
    return read(fd, data, len);
}

static inline int iot_i2c_write_bytes(int fd, uint8_t addr, const uint8_t* data, size_t len) {
    iot_i2c_set_addr(fd, addr);
    return write(fd, data, len);
}

static inline int iot_i2c_read_bytes(int fd, uint8_t addr, uint8_t* data, size_t len) {
    iot_i2c_set_addr(fd, addr);
    return read(fd, data, len);
}

#define iot_i2c_init(bus, speed) ({ \
    (void)speed; \
    iot_i2c_open_bus(bus); \
})

#define iot_i2c_deinit(fd) \
    close(fd)

#define iot_i2c_write(fd, addr, reg, data, len) \
    iot_i2c_write_reg(fd, addr, reg, data, len)

#define iot_i2c_read(fd, addr, reg, data, len) \
    iot_i2c_read_reg(fd, addr, reg, data, len)

#define iot_i2c_write_raw(fd, addr, data, len) \
    iot_i2c_write_bytes(fd, addr, data, len)

#define iot_i2c_read_raw(fd, addr, data, len) \
    iot_i2c_read_bytes(fd, addr, data, len)

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#include <linux/spi/spidev.h>

#define IOT_SPI_BUS_0           0
#define IOT_SPI_BUS_1           1
#define IOT_SPI_BUS_2           2

#define IOT_SPI_MODE_0          0
#define IOT_SPI_MODE_1          1
#define IOT_SPI_MODE_2          2
#define IOT_SPI_MODE_3          3

static inline int iot_spi_open(int bus, int cs) {
    char path[32];
    int fd;

    snprintf(path, sizeof(path), "/dev/spidev%d.%d", bus, cs);
    fd = open(path, O_RDWR);
    return fd;
}

static inline int iot_spi_set_mode(int fd, int mode) {
    return ioctl(fd, SPI_IOC_WR_MODE, &mode);
}

static inline int iot_spi_set_speed(int fd, int speed) {
    return ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
}

static inline int iot_spi_set_bits(int fd, int bits) {
    return ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
}

static inline int iot_spi_transfer_internal(int fd, uint8_t* tx, uint8_t* rx, size_t len, int speed) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = speed,
        .bits_per_word = 8,
        .delay_usecs = 0
    };

    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

#define iot_spi_init(bus, cs, mode, speed) ({ \
    int fd = iot_spi_open(bus, cs); \
    if (fd >= 0) { \
        iot_spi_set_mode(fd, mode); \
        iot_spi_set_speed(fd, speed); \
        iot_spi_set_bits(fd, 8); \
    } \
    fd; \
})

#define iot_spi_deinit(fd) \
    close(fd)

#define iot_spi_write(fd, data, len) \
    iot_spi_transfer_internal(fd, data, NULL, len, 1000000)

#define iot_spi_read(fd, data, len) \
    iot_spi_transfer_internal(fd, NULL, data, len, 1000000)

#define iot_spi_transfer(fd, tx, rx, len) \
    iot_spi_transfer_internal(fd, tx, rx, len, 1000000)

#define iot_spi_transfer_raw(fd, tx, rx, len, speed) \
    iot_spi_transfer_internal(fd, tx, rx, len, speed)

#endif /* IOT_PLATFORM_LINUX_EXT_H */
