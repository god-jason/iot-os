#include "drivers.h"
#include "platform.h"

/*===========================================================
 * 驱动链表
 *===========================================================*/

static driver_t* driver_list = NULL;

/*===========================================================
 * I2C 接口实现
 *===========================================================*/

int driver_i2c_init(driver_i2c_t* i2c, int bus, int speed) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    memset(i2c, 0, sizeof(driver_i2c_t));
    i2c->bus = bus;
    i2c->fd = iot_i2c_init(bus, speed);

    return (i2c->fd >= 0) ? DRIVER_OK : DRIVER_ERR_I2C;
}

int driver_i2c_deinit(driver_i2c_t* i2c) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    iot_i2c_deinit(i2c->fd);
    return DRIVER_OK;
}

int driver_i2c_write_reg(driver_i2c_t* i2c, uint8_t addr, uint8_t reg, const uint8_t* data, size_t len) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    return iot_i2c_write(i2c->fd, addr, reg, data, len);
}

int driver_i2c_read_reg(driver_i2c_t* i2c, uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    return iot_i2c_read(i2c->fd, addr, reg, data, len);
}

int driver_i2c_write_bytes(driver_i2c_t* i2c, uint8_t addr, const uint8_t* data, size_t len) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    return iot_i2c_write_raw(i2c->fd, addr, data, len);
}

int driver_i2c_read_bytes(driver_i2c_t* i2c, uint8_t addr, uint8_t* data, size_t len) {
    if (!i2c) {
        return DRIVER_ERR_INVALID;
    }

    return iot_i2c_read_raw(i2c->fd, addr, data, len);
}

/*===========================================================
 * SPI 接口实现
 *===========================================================*/

int driver_spi_init(driver_spi_t* spi, int bus, int cs, int speed, int mode) {
    if (!spi) {
        return DRIVER_ERR_INVALID;
    }

    memset(spi, 0, sizeof(driver_spi_t));
    spi->bus = bus;
    spi->cs = cs;
    spi->speed = speed;
    spi->mode = mode;
    spi->fd = iot_spi_init(bus, cs, mode, speed);

    return (spi->fd >= 0) ? DRIVER_OK : DRIVER_ERR_SPI;
}

int driver_spi_deinit(driver_spi_t* spi) {
    if (!spi) {
        return DRIVER_ERR_INVALID;
    }

    iot_spi_deinit(spi->fd);
    return DRIVER_OK;
}

int driver_spi_transfer(driver_spi_t* spi, uint8_t* tx, uint8_t* rx, size_t len) {
    if (!spi) {
        return DRIVER_ERR_INVALID;
    }

    return iot_spi_transfer(spi->fd, tx, rx, len, spi->speed);
}

/*===========================================================
 * 延时接口实现
 *===========================================================*/

void driver_delay_ms(uint32_t ms) {
    iot_task_delay(ms);
}

void driver_delay_us(uint32_t us) {
#ifdef __linux__
    usleep(us);
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE timer;
    LARGE_INTEGER due_time;
    due_time.QuadPart = -(int64_t)us * 10;
    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (timer) {
        SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
    }
#else
    volatile int i;
    for (i = 0; i < us * 10; i++);
#endif
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
