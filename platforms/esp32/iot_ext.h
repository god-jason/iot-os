/**
 * @file iot_ext.h
 * @brief ESP32 平台扩展适配头文件 - GPIO/I2C/SPI
 */
#ifndef IOT_PLATFORM_ESP32_EXT_H
#define IOT_PLATFORM_ESP32_EXT_H

#include "iot.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"

/*===========================================================
 * GPIO 适配层
 *===========================================================*/

#define IOT_GPIO_DIR_INPUT       GPIO_MODE_INPUT
#define IOT_GPIO_DIR_OUTPUT      GPIO_MODE_OUTPUT

#define IOT_GPIO_LEVEL_LOW       0
#define IOT_GPIO_LEVEL_HIGH      1

#define IOT_GPIO_IRQ_RISING      GPIO_INTR_POSEDGE
#define IOT_GPIO_IRQ_FALLING     GPIO_INTR_NEGEDGE
#define IOT_GPIO_IRQ_BOTH        GPIO_INTR_ANYEDGE
#define IOT_GPIO_IRQ_HIGH        GPIO_INTR_HIGH_LEVEL
#define IOT_GPIO_IRQ_LOW         GPIO_INTR_LOW_LEVEL

#define IOT_GPIO_PULL_NONE       GPIO_FLOATING
#define IOT_GPIO_PULL_DOWN       GPIO_PULLDOWN_ONLY
#define IOT_GPIO_PULL_UP         GPIO_PULLUP_ONLY

#define iot_gpio_init(pin, dir) ({ \
    gpio_config_t cfg = { \
        .pin_bit_mask = (1ULL << (pin)), \
        .mode = (gpio_mode_t)(dir), \
        .pull_up_en = GPIO_PULLUP_DISABLE, \
        .pull_down_en = GPIO_PULLDOWN_DISABLE, \
        .intr_type = GPIO_INTR_DISABLE \
    }; \
    gpio_config(&cfg); \
    0; \
})

#define iot_gpio_deinit(pin) \
    gpio_reset_pin((gpio_num_t)(pin))

#define iot_gpio_set_dir(pin, dir) \
    gpio_set_direction((gpio_num_t)(pin), (gpio_mode_t)(dir))

#define iot_gpio_write(pin, level) \
    gpio_set_level((gpio_num_t)(pin), (level))

#define iot_gpio_read(pin) \
    gpio_get_level((gpio_num_t)(pin))

#define iot_gpio_toggle(pin) ({ \
    int level = gpio_get_level((gpio_num_t)(pin)); \
    gpio_set_level((gpio_num_t)(pin), level == 1 ? 0 : 1); \
})

#define iot_gpio_set_pull(pin, pull) ({ \
    gpio_set_pull_mode((gpio_num_t)(pin), (gpio_pull_mode_t)(pull)); \
})

#define iot_gpio_register_irq(pin, mode, cb) ({ \
    gpio_set_intr_type((gpio_num_t)(pin), (gpio_int_type_t)(mode)); \
    gpio_install_isr_service(0); \
    gpio_isr_handler_add((gpio_num_t)(pin), (void(*)(void*))(cb), NULL); \
    gpio_intr_enable((gpio_num_t)(pin)); \
})

#define iot_gpio_unregister_irq(pin) ({ \
    gpio_intr_disable((gpio_num_t)(pin)); \
    gpio_isr_handler_remove((gpio_num_t)(pin)); \
})

/*===========================================================
 * I2C 适配层
 *===========================================================*/

#define IOT_I2C_BUS_0            I2C_NUM_0
#define IOT_I2C_BUS_1            I2C_NUM_1

#define IOT_I2C_ADDR_7BIT        I2C_ADDR_BIT_7
#define IOT_I2C_ADDR_10BIT       I2C_ADDR_BIT_10

#define IOT_I2C_CLK_100K         100000
#define IOT_I2C_CLK_400K         400000
#define IOT_I2C_CLK_1M           1000000

#define iot_i2c_init(bus, speed) ({ \
    i2c_config_t cfg = { \
        .mode = I2C_MODE_MASTER, \
        .sda_io_num = (bus == I2C_NUM_0) ? GPIO_NUM_21 : GPIO_NUM_18, \
        .scl_io_num = (bus == I2C_NUM_0) ? GPIO_NUM_22 : GPIO_NUM_19, \
        .sda_pullup_en = GPIO_PULLUP_ENABLE, \
        .scl_pullup_en = GPIO_PULLUP_ENABLE, \
        .master.clk_speed = (speed) \
    }; \
    i2c_param_config((i2c_port_t)(bus), &cfg); \
    i2c_driver_install((i2c_port_t)(bus), I2C_MODE_MASTER, 0, 0, 0); \
    (bus); \
})

#define iot_i2c_deinit(fd) \
    i2c_driver_delete((i2c_port_t)(fd))

#define iot_i2c_write(fd, addr, data, len) ({ \
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); \
    i2c_master_start(cmd); \
    i2c_master_write_byte(cmd, ((addr) << 1) | I2C_MASTER_WRITE, true); \
    i2c_master_write(cmd, (data), (len), true); \
    i2c_master_stop(cmd); \
    esp_err_t ret = i2c_master_cmd_begin((i2c_port_t)(fd), cmd, 1000 / portTICK_PERIOD_MS); \
    i2c_cmd_link_delete(cmd); \
    (ret == ESP_OK) ? (len) : -1; \
})

#define iot_i2c_read(fd, addr, data, len) ({ \
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); \
    i2c_master_start(cmd); \
    i2c_master_write_byte(cmd, ((addr) << 1) | I2C_MASTER_READ, true); \
    i2c_master_read(cmd, (data), (len), I2C_MASTER_LAST_NACK); \
    i2c_master_stop(cmd); \
    esp_err_t ret = i2c_master_cmd_begin((i2c_port_t)(fd), cmd, 1000 / portTICK_PERIOD_MS); \
    i2c_cmd_link_delete(cmd); \
    (ret == ESP_OK) ? (len) : -1; \
})

#define iot_i2c_write_then_read(fd, addr, wdata, wlen, rdata, rlen) ({ \
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); \
    i2c_master_start(cmd); \
    i2c_master_write_byte(cmd, ((addr) << 1) | I2C_MASTER_WRITE, true); \
    i2c_master_write(cmd, (wdata), (wlen), true); \
    i2c_master_start(cmd); \
    i2c_master_write_byte(cmd, ((addr) << 1) | I2C_MASTER_READ, true); \
    i2c_master_read(cmd, (rdata), (rlen), I2C_MASTER_LAST_NACK); \
    i2c_master_stop(cmd); \
    esp_err_t ret = i2c_master_cmd_begin((i2c_port_t)(fd), cmd, 1000 / portTICK_PERIOD_MS); \
    i2c_cmd_link_delete(cmd); \
    (ret == ESP_OK) ? (rlen) : -1; \
})

#define iot_i2c_write_raw(fd, addr, data, len) \
    iot_i2c_write(fd, addr, data, len)

#define iot_i2c_read_raw(fd, addr, data, len) \
    iot_i2c_read(fd, addr, data, len)

/*===========================================================
 * SPI 适配层
 *===========================================================*/

#define IOT_SPI_BUS_0            SPI2_HOST
#define IOT_SPI_BUS_1            SPI3_HOST

#define IOT_SPI_MODE_0           0
#define IOT_SPI_MODE_1           1
#define IOT_SPI_MODE_2           2
#define IOT_SPI_MODE_3           3

typedef struct {
    spi_device_handle_t handle;
    spi_host_device_t host;
} iot_spi_ctx_t;

static iot_spi_ctx_t g_spi_ctx[2] = {0};

#define iot_spi_init(bus, cs, mode, speed) ({ \
    spi_bus_config_t buscfg = { \
        .mosi_io_num = (bus == SPI2_HOST) ? GPIO_NUM_13 : GPIO_NUM_23, \
        .miso_io_num = (bus == SPI2_HOST) ? GPIO_NUM_12 : GPIO_NUM_19, \
        .sclk_io_num = (bus == SPI2_HOST) ? GPIO_NUM_14 : GPIO_NUM_18, \
        .quadwp_io_num = -1, \
        .quadhd_io_num = -1, \
        .max_transfer_sz = 4096 \
    }; \
    spi_bus_initialize((spi_host_device_t)(bus), &buscfg, SPI_DMA_CH_AUTO); \
    \
    spi_device_interface_config_t devcfg = { \
        .clock_speed_hz = (speed), \
        .mode = (mode), \
        .spics_io_num = (cs), \
        .queue_size = 7, \
        .pre_cb = NULL \
    }; \
    spi_bus_add_device((spi_host_device_t)(bus), &devcfg, &g_spi_ctx[bus].handle); \
    g_spi_ctx[bus].host = (spi_host_device_t)(bus); \
    (bus); \
})

#define iot_spi_deinit(fd) ({ \
    spi_bus_remove_device(g_spi_ctx[fd].handle); \
    spi_bus_free(g_spi_ctx[fd].host); \
})

#define iot_spi_write(fd, data, len) ({ \
    spi_transaction_t t = { \
        .length = (len) * 8, \
        .tx_buffer = (data), \
        .rx_buffer = NULL \
    }; \
    esp_err_t ret = spi_device_transmit(g_spi_ctx[fd].handle, &t); \
    (ret == ESP_OK) ? (len) : -1; \
})

#define iot_spi_read(fd, data, len) ({ \
    spi_transaction_t t = { \
        .length = (len) * 8, \
        .tx_buffer = NULL, \
        .rx_buffer = (data) \
    }; \
    esp_err_t ret = spi_device_transmit(g_spi_ctx[fd].handle, &t); \
    (ret == ESP_OK) ? (len) : -1; \
})

#define iot_spi_transfer(fd, tx, rx, len) ({ \
    spi_transaction_t t = { \
        .length = (len) * 8, \
        .tx_buffer = (tx), \
        .rx_buffer = (rx) \
    }; \
    esp_err_t ret = spi_device_transmit(g_spi_ctx[fd].handle, &t); \
    (ret == ESP_OK) ? (len) : -1; \
})

#define iot_spi_transfer_raw(fd, tx, rx, len, speed) \
    iot_spi_transfer(fd, tx, rx, len)

#endif /* IOT_PLATFORM_ESP32_EXT_H */
