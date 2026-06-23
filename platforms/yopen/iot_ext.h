/**
 * @file iot_ext.h
 * @brief YOPEN 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_YOPEN_EXT_H
#define IOT_PLATFORM_YOPEN_EXT_H

#include "iot.h"
#include "yopen_gpio.h"
#include "yopen_i2c.h"
#include "yopen_spi.h"

/*===========================================================
 * GPIO 适配层
 *===========================================================*/

#define IOT_GPIO_DIR_INPUT       GPIO_INPUT
#define IOT_GPIO_DIR_OUTPUT      GPIO_OUTPUT

#define IOT_GPIO_LEVEL_LOW       LVL_LOW
#define IOT_GPIO_LEVEL_HIGH      LVL_HIGH

#define IOT_GPIO_PULL_NONE       PULL_DEFAULT
#define IOT_GPIO_PULL_UP         FORCE_PULL_UP
#define IOT_GPIO_PULL_DOWN       FORCE_PULL_DOWN

#define IOT_GPIO_MAX_PIN         32

static inline int iot_gpio_init_internal(int pin, int dir) {
    return yopen_gpio_init(pin, dir, PULL_DEFAULT, LVL_LOW);
}

static inline int iot_gpio_deinit_internal(int pin) {
    return yopen_gpio_deinit(pin);
}

static inline int iot_gpio_write_internal(int pin, int level) {
    return yopen_gpio_set_level(pin, level);
}

static inline int iot_gpio_read_internal(int pin) {
    yopen_LvlMode lvl;
    yopen_gpio_get_level(pin, &lvl);
    return lvl;
}

static inline int iot_gpio_set_direction_internal(int pin, int dir) {
    return yopen_gpio_set_direction(pin, dir);
}

static inline int iot_gpio_set_pull_internal(int pin, int pull) {
    return yopen_gpio_set_pull(pin, pull);
}

#define iot_gpio_init(pin, dir) \
    iot_gpio_init_internal(pin, dir)

#define iot_gpio_deinit(pin) \
    iot_gpio_deinit_internal(pin)

#define iot_gpio_write(pin, level) \
    iot_gpio_write_internal(pin, level)

#define iot_gpio_read(pin) \
    iot_gpio_read_internal(pin)

#define iot_gpio_set_dir(pin, dir) \
    iot_gpio_set_direction_internal(pin, dir)

#define iot_gpio_set_pull(pin, pull) \
    iot_gpio_set_pull_internal(pin, pull)

/*===========================================================
 * I2C 适配层
 *===========================================================*/

#define IOT_I2C_BUS_0            i2c_0
#define IOT_I2C_BUS_1            i2c_1

#define IOT_I2C_SPEED_STANDARD   STANDARD_MODE
#define IOT_I2C_SPEED_FAST       FAST_MODE

static inline int iot_i2c_init_internal(int bus, int speed) {
    return yopen_I2cInit(bus, speed);
}

static inline int iot_i2c_deinit_internal(int bus) {
    return yopen_I2cRelease(bus);
}

static inline int iot_i2c_write_internal(int bus, uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t len) {
    return yopen_I2cWrite(bus, addr, reg, (uint8_t*)data, len);
}

static inline int iot_i2c_read_internal(int bus, uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    return yopen_I2cRead(bus, addr, reg, data, len);
}

static inline int iot_i2c_write_noaddr_internal(int bus, uint8_t addr, const uint8_t* data, uint32_t len) {
    return yopen_I2cWrite_Noaddr(bus, addr, (uint8_t*)data, len);
}

static inline int iot_i2c_read_noaddr_internal(int bus, uint8_t addr, uint8_t* data, uint32_t len) {
    return yopen_I2cRead_Noaddr(bus, addr, data, len);
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
    iot_i2c_write_noaddr_internal(fd, addr, data, len)

#define iot_i2c_read_raw(fd, addr, data, len) \
    iot_i2c_read_noaddr_internal(fd, addr, data, len)

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#define IOT_SPI_BUS_0            YOPEN_SPI_PORT0
#define IOT_SPI_BUS_1            YOPEN_SPI_PORT1

#define IOT_SPI_MODE_0           (YOPEN_SPI_CPOL_LOW | YOPEN_SPI_CPHA_1Edge)
#define IOT_SPI_MODE_1           (YOPEN_SPI_CPOL_LOW | YOPEN_SPI_CPHA_2Edge)
#define IOT_SPI_MODE_2           (YOPEN_SPI_CPOL_HIGH | YOPEN_SPI_CPHA_1Edge)
#define IOT_SPI_MODE_3           (YOPEN_SPI_CPOL_HIGH | YOPEN_SPI_CPHA_2Edge)

#define IOT_SPI_CLK_812K         YOPEN_SPI_CLK_812_5KHZ
#define IOT_SPI_CLK_1_6M         YOPEN_SPI_CLK_1_625MHZ
#define IOT_SPI_CLK_3_2M         YOPEN_SPI_CLK_3_25MHZ
#define IOT_SPI_CLK_6_5M         YOPEN_SPI_CLK_6_5MHZ
#define IOT_SPI_CLK_13M          YOPEN_SPI_CLK_13MHZ

typedef struct {
    int port;
    int initialized;
} iot_spi_info_t;

static iot_spi_info_t g_spi_info[2];
static int g_spi_count = 0;

static inline int iot_spi_init_internal(int bus, int cs, int mode, int speed) {
    (void)cs;
    if (g_spi_count >= 2) {
        return -1;
    }
    int idx = g_spi_count++;
    g_spi_info[idx].port = bus;
    g_spi_info[idx].initialized = 1;

    yopen_spi_config_s config = {
        .port = bus,
        .framesize = 8,
        .spiclk = speed,
        .cpol = (mode & 0x02) ? YOPEN_SPI_CPOL_HIGH : YOPEN_SPI_CPOL_LOW,
        .cpha = (mode & 0x01) ? YOPEN_SPI_CPHA_2Edge : YOPEN_SPI_CPHA_1Edge,
        .transmode = YOPEN_SPI_DIRECT_POLLING
    };
    return yopen_spi_init(config);
}

static inline int iot_spi_deinit_internal(int fd) {
    if (fd < 0 || fd >= g_spi_count) {
        return -1;
    }
    g_spi_info[fd].initialized = 0;
    return yopen_spi_release(g_spi_info[fd].port);
}

static inline int iot_spi_transfer_internal(int fd, uint8_t* tx, uint8_t* rx, size_t len, int speed) {
    (void)speed;
    if (fd < 0 || fd >= g_spi_count) {
        return -1;
    }
    return yopen_spi_write_read(g_spi_info[fd].port, rx, tx, len);
}

#define iot_spi_init(bus, cs, mode, speed) \
    iot_spi_init_internal(bus, cs, mode, speed)

#define iot_spi_deinit(fd) \
    iot_spi_deinit_internal(fd)

#define iot_spi_write(fd, data, len) \
    yopen_spi_write(g_spi_info[fd].port, (unsigned char*)data, len)

#define iot_spi_read(fd, data, len) \
    yopen_spi_read(g_spi_info[fd].port, data, len)

#define iot_spi_transfer(fd, tx, rx, len) \
    iot_spi_transfer_internal(fd, tx, rx, len, 0)

#define iot_spi_transfer_raw(fd, tx, rx, len, speed) \
    iot_spi_transfer_internal(fd, tx, rx, len, speed)

#endif /* IOT_PLATFORM_YOPEN_EXT_H */
