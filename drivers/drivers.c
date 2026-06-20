#include "drivers.h"
#include "platform.h"

/*===========================================================
 * 驱动链表
 *===========================================================*/

static driver_t* driver_list = NULL;

/*===========================================================
 * I2C 接口实现
 *===========================================================*/

int driver_i2c_init(const i2c_config_t* config) {
    (void)config;
    return DRIVER_OK;
}

int driver_i2c_deinit(void) {
    return DRIVER_OK;
}

int driver_i2c_read(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return DRIVER_OK;
}

int driver_i2c_write(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    (void)addr;
    (void)reg;
    (void)data;
    (void)len;
    return DRIVER_OK;
}

int driver_i2c_read_bytes(uint8_t addr, uint8_t* data, size_t len) {
    (void)addr;
    (void)data;
    (void)len;
    return DRIVER_OK;
}

int driver_i2c_write_bytes(uint8_t addr, const uint8_t* data, size_t len) {
    (void)addr;
    (void)data;
    (void)len;
    return DRIVER_OK;
}

/*===========================================================
 * SPI 接口实现
 *===========================================================*/

int driver_spi_init(const spi_config_t* config) {
    (void)config;
    return DRIVER_OK;
}

int driver_spi_deinit(void) {
    return DRIVER_OK;
}

int driver_spi_transfer(uint8_t* tx, uint8_t* rx, size_t len) {
    (void)tx;
    (void)rx;
    (void)len;
    return DRIVER_OK;
}

/*===========================================================
 * GPIO 接口实现
 *===========================================================*/

int driver_gpio_init(int pin, gpio_dir_t dir) {
    (void)pin;
    (void)dir;
    return DRIVER_OK;
}

int driver_gpio_deinit(int pin) {
    (void)pin;
    return DRIVER_OK;
}

int driver_gpio_set_level(int pin, gpio_level_t level) {
    (void)pin;
    (void)level;
    return DRIVER_OK;
}

int driver_gpio_get_level(int pin, gpio_level_t* level) {
    (void)pin;
    (void)level;
    return DRIVER_OK;
}

/*===========================================================
 * 延时接口实现
 *===========================================================*/

void driver_delay_ms(uint32_t ms) {
    iot_os_delay_ms(ms);
}

void driver_delay_us(uint32_t us) {
    iot_os_delay_us(us);
}

/*===========================================================
 * 驱动注册接口实现
 *===========================================================*/

int driver_register(driver_t* drv) {
    if (!drv || !drv->name) {
        return DRIVER_ERR_INVALID;
    }

    drv->next = driver_list;
    driver_list = drv;
    drv->status = DRIVER_STATUS_UNINIT;

    return DRIVER_OK;
}

driver_t* driver_find(const char* name) {
    if (!name) {
        return NULL;
    }

    driver_t* drv = driver_list;
    while (drv) {
        if (strcmp(drv->name, name) == 0) {
            return drv;
        }
        drv = drv->next;
    }

    return NULL;
}

int driver_unregister(const char* name) {
    driver_t **pprev = &driver_list;
    driver_t *curr = driver_list;

    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            *pprev = curr->next;
            return DRIVER_OK;
        }
        pprev = &curr->next;
        curr = curr->next;
    }

    return DRIVER_ERR_INVALID;
}

/*===========================================================
 * 驱动全局链表定义
 *===========================================================*/

#define DRIVER_LIST_ENTRY(drv) \
    driver_t* drv_##drv##_next

static DRIVER_LIST_ENTRY(bme280);
static DRIVER_LIST_ENTRY(bh1750);
static DRIVER_LIST_ENTRY(dht11);
static DRIVER_LIST_ENTRY(dht22);
static DRIVER_LIST_ENTRY(ssd1306);
static DRIVER_LIST_ENTRY(mpu6050);
static DRIVER_LIST_ENTRY(pcf8574);
static DRIVER_LIST_ENTRY(ads1115);
