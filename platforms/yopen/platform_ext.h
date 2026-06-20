/**
 * @file platform_ext.h
 * @brief YOPEN 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_YOPEN_EXT_H
#define IOT_PLATFORM_YOPEN_EXT_H

#include "platform.h"
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

typedef yopen_GpioNum iot_gpio_pin_t;

static inline int iot_gpio_init_internal(iot_gpio_pin_t pin, int dir, int pull, int level) {
    return yopen_gpio_init(pin, dir, pull, level);
}

static inline int iot_gpio_deinit_internal(iot_gpio_pin_t pin) {
    return yopen_gpio_deinit(pin);
}

static inline int iot_gpio_write_internal(iot_gpio_pin_t pin, int level) {
    return yopen_gpio_set_level(pin, level);
}

static inline int iot_gpio_read_internal(iot_gpio_pin_t pin, int* level) {
    yopen_LvlMode lvl;
    int ret = yopen_gpio_get_level(pin, &lvl);
    *level = lvl;
    return ret;
}

static inline int iot_gpio_set_direction_internal(iot_gpio_pin_t pin, int dir) {
    return yopen_gpio_set_direction(pin, dir);
}

static inline int iot_gpio_set_pull_internal(iot_gpio_pin_t pin, int pull) {
    return yopen_gpio_set_pull(pin, pull);
}

#define iot_gpio_init(pin, dir) \
    iot_gpio_init_internal(pin, dir, PULL_DEFAULT, LVL_LOW)

#define iot_gpio_init_ex(pin, dir, pull, level) \
    iot_gpio_init_internal(pin, dir, pull, level)

#define iot_gpio_deinit(pin) \
    iot_gpio_deinit_internal(pin)

#define iot_gpio_write(pin, level) \
    iot_gpio_write_internal(pin, level)

#define iot_gpio_read(pin, level) \
    iot_gpio_read_internal(pin, level)

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

typedef yopen_i2c_channel_e iot_i2c_bus_t;

static inline int iot_i2c_init_internal(iot_i2c_bus_t bus, int speed) {
    return yopen_I2cInit(bus, speed);
}

static inline int iot_i2c_deinit_internal(iot_i2c_bus_t bus) {
    return yopen_I2cRelease(bus);
}

static inline int iot_i2c_write_internal(iot_i2c_bus_t bus, uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t len) {
    return yopen_I2cWrite(bus, addr, reg, (uint8_t*)data, len);
}

static inline int iot_i2c_read_internal(iot_i2c_bus_t bus, uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    return yopen_I2cRead(bus, addr, reg, data, len);
}

static inline int iot_i2c_write_16addr_internal(iot_i2c_bus_t bus, uint8_t addr, uint16_t reg, const uint8_t* data, uint32_t len) {
    return yopen_I2cWrite_16bit_addr(bus, addr, reg, (uint8_t*)data, len);
}

static inline int iot_i2c_read_16addr_internal(iot_i2c_bus_t bus, uint8_t addr, uint16_t reg, uint8_t* data, uint32_t len) {
    return yopen_I2cRead_16bit_addr(bus, addr, reg, data, len);
}

static inline int iot_i2c_write_noaddr_internal(iot_i2c_bus_t bus, uint8_t addr, const uint8_t* data, uint32_t len) {
    return yopen_I2cWrite_Noaddr(bus, addr, (uint8_t*)data, len);
}

static inline int iot_i2c_read_noaddr_internal(iot_i2c_bus_t bus, uint8_t addr, uint8_t* data, uint32_t len) {
    return yopen_I2cRead_Noaddr(bus, addr, data, len);
}

#define iot_i2c_init(bus, speed) \
    iot_i2c_init_internal(bus, speed)

#define iot_i2c_deinit(bus) \
    iot_i2c_deinit_internal(bus)

#define iot_i2c_write(bus, addr, reg, data, len) \
    iot_i2c_write_internal(bus, addr, reg, data, len)

#define iot_i2c_read(bus, addr, reg, data, len) \
    iot_i2c_read_internal(bus, addr, reg, data, len)

#define iot_i2c_write_16addr(bus, addr, reg, data, len) \
    iot_i2c_write_16addr_internal(bus, addr, reg, data, len)

#define iot_i2c_read_16addr(bus, addr, reg, data, len) \
    iot_i2c_read_16addr_internal(bus, addr, reg, data, len)

#define iot_i2c_write_raw(bus, addr, data, len) \
    iot_i2c_write_noaddr_internal(bus, addr, data, len)

#define iot_i2c_read_raw(bus, addr, data, len) \
    iot_i2c_read_noaddr_internal(bus, addr, data, len)

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#define IOT_SPI_PORT_0           YOPEN_SPI_PORT0
#define IOT_SPI_PORT_1           YOPEN_SPI_PORT1

#define IOT_SPI_CS_0             YOPEN_SPI_CS0
#define IOT_SPI_CS_1             YOPEN_SPI_CS1

#define IOT_SPI_MODE_0           (YOPEN_SPI_CPOL_LOW | YOPEN_SPI_CPHA_1Edge)
#define IOT_SPI_MODE_1           (YOPEN_SPI_CPOL_LOW | YOPEN_SPI_CPHA_2Edge)
#define IOT_SPI_MODE_2           (YOPEN_SPI_CPOL_HIGH | YOPEN_SPI_CPHA_1Edge)
#define IOT_SPI_MODE_3           (YOPEN_SPI_CPOL_HIGH | YOPEN_SPI_CPHA_2Edge)

#define IOT_SPI_CLK_812K         YOPEN_SPI_CLK_812_5KHZ
#define IOT_SPI_CLK_1_6M         YOPEN_SPI_CLK_1_625MHZ
#define IOT_SPI_CLK_3_2M         YOPEN_SPI_CLK_3_25MHZ
#define IOT_SPI_CLK_6_5M         YOPEN_SPI_CLK_6_5MHZ
#define IOT_SPI_CLK_13M          YOPEN_SPI_CLK_13MHZ

typedef yopen_spi_port_e iot_spi_port_t;

static inline int iot_spi_init_internal(iot_spi_port_t port, int clk, int cpol, int cpha, int transmode) {
    yopen_spi_config_s config = {
        .port = port,
        .framesize = 8,
        .spiclk = clk,
        .cpol = cpol,
        .cpha = cpha,
        .transmode = transmode
    };
    return yopen_spi_init(config);
}

static inline int iot_spi_deinit_internal(iot_spi_port_t port) {
    return yopen_spi_release(port);
}

static inline int iot_spi_write_internal(iot_spi_port_t port, const uint8_t* data, uint32_t len) {
    return yopen_spi_write(port, (unsigned char*)data, len);
}

static inline int iot_spi_read_internal(iot_spi_port_t port, uint8_t* data, uint32_t len) {
    return yopen_spi_read(port, data, len);
}

static inline int iot_spi_transfer_internal(iot_spi_port_t port, uint8_t* tx, uint8_t* rx, uint32_t len) {
    return yopen_spi_write_read(port, rx, tx, len);
}

#define iot_spi_init(port, clk, cpol, cpha) \
    iot_spi_init_internal(port, clk, cpol, cpha, YOPEN_SPI_DIRECT_POLLING)

#define iot_spi_init_dma(port, clk, cpol, cpha) \
    iot_spi_init_internal(port, clk, cpol, cpha, YOPEN_SPI_DMA_IRQ)

#define iot_spi_deinit(port) \
    iot_spi_deinit_internal(port)

#define iot_spi_write(port, data, len) \
    iot_spi_write_internal(port, data, len)

#define iot_spi_read(port, data, len) \
    iot_spi_read_internal(port, data, len)

#define iot_spi_transfer(port, tx, rx, len) \
    iot_spi_transfer_internal(port, tx, rx, len)

#endif /* IOT_PLATFORM_YOPEN_EXT_H */