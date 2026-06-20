/**
 * @file platform_ext.h
 * @brief ML307N 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_ML307N_EXT_H
#define IOT_PLATFORM_ML307N_EXT_H

#include "platform.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_spi.h"
#include "drv_spi_core.h"
#include "drv_pin.h"

/*===========================================================
 * GPIO 适配层
 *===========================================================*/

#define IOT_GPIO_DIR_INPUT       GPIO_INPUT
#define IOT_GPIO_DIR_OUTPUT      GPIO_OUTPUT

#define IOT_GPIO_LEVEL_LOW       GPIO_LOW
#define IOT_GPIO_LEVEL_HIGH      GPIO_HIGH

#define IOT_GPIO_IRQ_RISING      PIN_IRQ_MODE_RISING
#define IOT_GPIO_IRQ_FALLING     PIN_IRQ_MODE_FALLING
#define IOT_GPIO_IRQ_BOTH        PIN_IRQ_MODE_RISING_FALLING
#define IOT_GPIO_IRQ_HIGH        PIN_IRQ_MODE_HIGH_LEVEL
#define IOT_GPIO_IRQ_LOW         PIN_IRQ_MODE_LOW_LEVEL

typedef struct PIN_Res iot_gpio_pin_t;

static inline int iot_gpio_init_internal(iot_gpio_pin_t* pin, GPIO_DIR dir) {
    GPIO_SetDir(pin, dir);
    return DRV_OK;
}

static inline int iot_gpio_deinit_internal(iot_gpio_pin_t* pin) {
    (void)pin;
    return DRV_OK;
}

static inline int iot_gpio_write_internal(iot_gpio_pin_t* pin, GPIO_LEVEL level) {
    GPIO_Write(pin, level);
    return DRV_OK;
}

static inline int iot_gpio_read_internal(iot_gpio_pin_t* pin) {
    return GPIO_Read(pin);
}

static inline int iot_gpio_set_direction_internal(iot_gpio_pin_t* pin, GPIO_DIR dir) {
    GPIO_SetDir(pin, dir);
    return DRV_OK;
}

static inline int iot_gpio_get_direction_internal(iot_gpio_pin_t* pin) {
    return GPIO_GetDir(pin);
}

static inline int iot_gpio_attach_irq_internal(iot_gpio_pin_t* pin, uint32_t mode, void (*hdr)(void*), void* arg) {
    return GPIO_AttachIrq(pin, mode, hdr, arg);
}

static inline int iot_gpio_detach_irq_internal(iot_gpio_pin_t* pin) {
    GPIO_DetachIrq(pin);
    return DRV_OK;
}

static inline int iot_gpio_irq_enable_internal(iot_gpio_pin_t* pin, uint32_t enable) {
    return GPIO_IrqEnable(pin, enable);
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

#define iot_gpio_get_dir(pin) \
    iot_gpio_get_direction_internal(pin)

#define iot_gpio_attach_irq(pin, mode, handler, arg) \
    iot_gpio_attach_irq_internal(pin, mode, handler, arg)

#define iot_gpio_detach_irq(pin) \
    iot_gpio_detach_irq_internal(pin)

#define iot_gpio_irq_enable(pin, enable) \
    iot_gpio_irq_enable_internal(pin, enable)

/*===========================================================
 * I2C 适配层
 *===========================================================*/

#define IOT_I2C_BUS_0            I2C_BUS_0
#define IOT_I2C_BUS_1            I2C_BUS_1
#define IOT_I2C_BUS_2            I2C_BUS_2

#define IOT_I2C_SPEED_STANDARD   I2C_BUS_SPEED_STANDARD
#define IOT_I2C_SPEED_FAST       I2C_BUS_SPEED_FAST
#define IOT_I2C_SPEED_FAST_PLUS  I2C_BUS_SPEED_FAST_PLUS
#define IOT_I2C_SPEED_HIGH       I2C_BUS_SPEED_HIGH

typedef I2C_Handle iot_i2c_handle_t;

/* I2C 全局句柄数组，用于统一接口 */
static iot_i2c_handle_t g_i2c_handles[2];
static int g_i2c_handle_count = 0;

static inline int iot_i2c_init_internal(int bus, int speed) {
    if (g_i2c_handle_count >= 2) {
        return -1;  /* 已达到最大句柄数 */
    }
    int idx = g_i2c_handle_count++;
    g_i2c_handles[idx].Init.busNumber = bus;
    g_i2c_handles[idx].Init.speed = speed;
    g_i2c_handles[idx].Init.longAddr = 0;
    g_i2c_handles[idx].Init.memDev = false;
    g_i2c_handles[idx].busNumber = bus;
    return I2C_Initialize(&g_i2c_handles[idx], NULL) == DRV_OK ? idx : -1;
}

static inline int iot_i2c_deinit_internal(int fd) {
    if (fd < 0 || fd >= g_i2c_handle_count) {
        return -1;
    }
    return I2C_Uninitialize(&g_i2c_handles[fd]);
}

static inline int iot_i2c_master_send_internal(int fd, uint8_t addr, const uint8_t* data, uint32_t len) {
    if (fd < 0 || fd >= g_i2c_handle_count) {
        return -1;
    }
    return I2C_MasterSend(&g_i2c_handles[fd], addr, data, len);
}

static inline int iot_i2c_master_receive_internal(int fd, uint8_t addr, uint8_t* data, uint32_t len) {
    if (fd < 0 || fd >= g_i2c_handle_count) {
        return -1;
    }
    return I2C_MasterReceive(&g_i2c_handles[fd], addr, data, len);
}

static inline int iot_i2c_write_reg_internal(int fd, uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t len) {
    uint8_t buf[256];
    buf[0] = reg;
    memcpy(buf + 1, data, len);
    return iot_i2c_master_send_internal(fd, addr, buf, len + 1);
}

static inline int iot_i2c_read_reg_internal(int fd, uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    if (fd < 0 || fd >= g_i2c_handle_count) {
        return -1;
    }
    I2C_MasterSend(&g_i2c_handles[fd], addr, &reg, 1);
    return I2C_MasterReceive(&g_i2c_handles[fd], addr, data, len);
}

#define iot_i2c_init(bus, speed) \
    iot_i2c_init_internal(bus, speed)

#define iot_i2c_deinit(fd) \
    iot_i2c_deinit_internal(fd)

#define iot_i2c_write(fd, addr, reg, data, len) \
    iot_i2c_write_reg_internal(fd, addr, reg, data, len)

#define iot_i2c_read(fd, addr, reg, data, len) \
    iot_i2c_read_reg_internal(fd, addr, reg, data, len)

#define iot_i2c_write_raw(fd, addr, data, len) \
    iot_i2c_master_send_internal(fd, addr, data, len)

#define iot_i2c_read_raw(fd, addr, data, len) \
    iot_i2c_master_receive_internal(fd, addr, data, len)

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#define IOT_SPI_BUS_0            SPI_BUS_NUMBER(0)
#define IOT_SPI_BUS_1            SPI_BUS_NUMBER(1)

#define IOT_SPI_MODE_0           SPI_POLPHA_00
#define IOT_SPI_MODE_1           SPI_POLPHA_01
#define IOT_SPI_MODE_2           SPI_POLPHA_10
#define IOT_SPI_MODE_3           SPI_POLPHA_11

#define IOT_SPI_CLK_13M          SPI_CLK_SRC_13M
#define IOT_SPI_CLK_26M          SPI_CLK_SRC_26M
#define IOT_SPI_CLK_39M          SPI_CLK_SRC_39M
#define IOT_SPI_CLK_78M          SPI_CLK_SRC_78M

#define IOT_SPI_CS_AUTO          SPI_CS_MDOE_AUTO
#define IOT_SPI_CS_MANUAL        SPI_CS_MDOE_MANUAL

#define IOT_SPI_TRANS_POLL       SPI_TRANS_MODE_POLL
#define IOT_SPI_TRANS_DMA        SPI_TRANS_MODE_DAM

typedef struct {
    uint32_t config;
    void* handle;
} iot_spi_handle_t;

static inline uint32_t iot_spi_build_config(uint8_t bus, uint32_t clk_src, uint8_t clk_div, uint32_t polpha, uint32_t trans_mode, uint32_t cs_mode) {
    uint32_t config = SPI_BUS_NUMBER(bus) | clk_src | SPI_CLK_DIV(clk_div) | SPI_MS_MODE_MASTER | polpha | SPI_DATA_BITS(8) | trans_mode | cs_mode;
    return config;
}

#define iot_spi_init(bus, clk_src, clk_div, mode, trans_mode, cs_mode) ({ \
    iot_spi_handle_t handle; \
    handle.config = iot_spi_build_config(bus, clk_src, clk_div, mode, trans_mode, cs_mode); \
    handle; \
})

#define iot_spi_deinit(handle) \
    (void)(handle)

#define iot_spi_write(handle, data, len) \
    SPI_Write(handle.handle, data, len)

#define iot_spi_read(handle, data, len) \
    SPI_Read(handle.handle, data, len)

#define iot_spi_transfer(handle, tx, rx, len) \
    SPI_Transfer(handle.handle, tx, rx, len)

#endif /* IOT_PLATFORM_ML307N_EXT_H */