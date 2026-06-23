/**
 * @file iot_ext.h
 * @brief ML307N 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_ML307N_EXT_H
#define IOT_PLATFORM_ML307N_EXT_H

#include "iot.h"
#include "cm_gpio.h"
#include "cm_i2c.h"
#include "cm_spi.h"

/*===========================================================
 * GPIO 适配层
 *===========================================================*/

#define IOT_GPIO_DIR_INPUT       CM_GPIO_DIRECTION_INPUT
#define IOT_GPIO_DIR_OUTPUT      CM_GPIO_DIRECTION_OUTPUT

#define IOT_GPIO_LEVEL_LOW       CM_GPIO_LEVEL_LOW
#define IOT_GPIO_LEVEL_HIGH      CM_GPIO_LEVEL_HIGH

#define IOT_GPIO_IRQ_RISING      CM_GPIO_IT_EDGE_RISING
#define IOT_GPIO_IRQ_FALLING     CM_GPIO_IT_EDGE_FALLING
#define IOT_GPIO_IRQ_BOTH        CM_GPIO_IT_EDGE_BOTH
#define IOT_GPIO_IRQ_HIGH        CM_GPIO_IT_LEVEL_HIGH
#define IOT_GPIO_IRQ_LOW         CM_GPIO_IT_LEVEL_LOW

#define IOT_GPIO_PULL_NONE       CM_GPIO_PULL_NONE
#define IOT_GPIO_PULL_DOWN       CM_GPIO_PULL_DOWN
#define IOT_GPIO_PULL_UP         CM_GPIO_PULL_UP

#define iot_gpio_init(pin, dir) ({ \
    cm_gpio_cfg_t cfg; \
    cfg.mode = CM_GPIO_MODE_NUM; \
    cfg.direction = (dir); \
    cfg.pull = CM_GPIO_PULL_NONE; \
    cm_gpio_init((cm_gpio_num_e)(pin), &cfg); \
})

#define iot_gpio_deinit(pin) \
    cm_gpio_deinit((cm_gpio_num_e)(pin))

#define iot_gpio_set_dir(pin, dir) \
    cm_gpio_set_direction((cm_gpio_num_e)(pin), (cm_gpio_direction_e)(dir))

#define iot_gpio_write(pin, level) \
    cm_gpio_set_level((cm_gpio_num_e)(pin), (cm_gpio_level_e)(level))

#define iot_gpio_read(pin) ({ \
    cm_gpio_level_e level; \
    cm_gpio_get_level((cm_gpio_num_e)(pin), &level); \
    level; \
})

#define iot_gpio_toggle(pin) ({ \
    cm_gpio_level_e level; \
    cm_gpio_get_level((cm_gpio_num_e)(pin), &level); \
    cm_gpio_set_level((cm_gpio_num_e)(pin), level == CM_GPIO_LEVEL_HIGH ? CM_GPIO_LEVEL_LOW : CM_GPIO_LEVEL_HIGH); \
})

#define iot_gpio_set_pull(pin, pull) \
    cmo_gpio_set_pull((cm_gpio_num_e)(pin), (cm_gpio_pull_e)(pull))

#define iot_gpio_register_irq(pin, mode, cb) ({ \
    cm_gpio_interrupt_register((cm_gpio_num_e)(pin), (void*)(cb)); \
    cm_gpio_interrupt_enable((cm_gpio_num_e)(pin), (cm_gpio_interrupt_e)(mode)); \
})

#define iot_gpio_unregister_irq(pin) \
    cm_gpio_interrupt_disable((cm_gpio_num_e)(pin))

/*===========================================================
 * I2C 适配层
 *===========================================================*/

#define IOT_I2C_BUS_0            CM_I2C_DEV_0
#define IOT_I2C_BUS_1            CM_I2C_DEV_1

#define IOT_I2C_ADDR_7BIT        CM_I2C_ADDR_TYPE_7BIT
#define IOT_I2C_ADDR_10BIT       CM_I2C_ADDR_TYPE_10BIT

#define IOT_I2C_CLK_100K         I2C_CLK_100KHZ
#define IOT_I2C_CLK_400K         I2C_CLK_400KHZ
#define IOT_I2C_CLK_1M           I2C_CLK_1MHZ

static cm_i2c_cfg_t g_i2c_cfgs[CM_I2C_DEV_NUM] = {0};

#define iot_i2c_init(bus, speed) ({ \
    cm_i2c_cfg_t cfg; \
    cfg.addr_type = CM_I2C_ADDR_TYPE_7BIT; \
    cfg.mode = CM_I2C_MODE_MASTER; \
    cfg.clk = (speed); \
    int ret = cm_i2c_open_v2((cm_i2c_dev_e)(bus), &cfg); \
    if (ret == 0) { \
        g_i2c_cfgs[bus] = cfg; \
    } \
    ret; \
})

#define iot_i2c_deinit(fd) \
    cm_i2c_close((cm_i2c_dev_e)(fd))

#define iot_i2c_write(fd, addr, data, len) \
    cm_i2c_write_v2((cm_i2c_dev_e)(fd), (addr), (data), (len))

#define iot_i2c_read(fd, addr, data, len) \
    cm_i2c_read_v2((cm_i2c_dev_e)(fd), (addr), (data), (len))

#define iot_i2c_write_then_read(fd, addr, wdata, wlen, rdata, rlen) \
    cm_i2c_write_then_read((cm_i2c_dev_e)(fd), (addr), (wdata), (wlen), (rdata), (rlen))

#define iot_i2c_write_raw(fd, addr, data, len) \
    cm_i2c_write_v2((cm_i2c_dev_e)(fd), (addr), (data), (len))

#define iot_i2c_read_raw(fd, addr, data, len) \
    cm_i2c_read_v2((cm_i2c_dev_e)(fd), (addr), (data), (len))

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#define IOT_SPI_BUS_0            CM_SPI_DEV_0
#define IOT_SPI_BUS_1            CM_SPI_DEV_1

#define IOT_SPI_MODE_0           CM_SPI_WORK_MODE_0
#define IOT_SPI_MODE_1           CM_SPI_WORK_MODE_1
#define IOT_SPI_MODE_2           CM_SPI_WORK_MODE_2
#define IOT_SPI_MODE_3           CM_SPI_WORK_MODE_3

#define iot_spi_init(bus, cs, mode, speed) ({ \
    cm_spi_cfg_t cfg; \
    cfg.mode = CM_SPI_MODE_MASTER; \
    cfg.work_mode = (cm_spi_work_mode_e)(mode); \
    cfg.data_width = CM_SPI_DATA_WIDTH_8BIT; \
    cfg.nss = CM_SPI_NSS_HARD; \
    cfg.clk = (speed) / 1000000; \
    cm_spi_open((cm_spi_dev_e)(bus), &cfg); \
})

#define iot_spi_deinit(fd) \
    cm_spi_close((cm_spi_dev_e)(fd))

#define iot_spi_write(fd, data, len) \
    cm_spi_write((cm_spi_dev_e)(fd), (data), (len))

#define iot_spi_read(fd, data, len) \
    cm_spi_read((cm_spi_dev_e)(fd), (data), (len))

#define iot_spi_transfer(fd, tx, rx, len) \
    cm_spi_write_then_read((cm_spi_dev_e)(fd), (tx), (len), (rx), (len))

#define iot_spi_transfer_raw(fd, tx, rx, len, speed) \
    cm_spi_write_then_read((cm_spi_dev_e)(fd), (tx), (len), (rx), (len))

#endif /* IOT_PLATFORM_ML307N_EXT_H */
