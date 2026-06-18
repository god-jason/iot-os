#ifndef IOT_I2C_YOPEN_H
#define IOT_I2C_YOPEN_H

#include "../../iot_types.h"
#include "yopen_i2c.h"

#define iot_i2c_t               yopen_i2c_channel_e

#define iot_i2c_init(channel, config) \
    do { \
        yopen_i2c_mode_e mode = (config->freq <= 100000) ? STANDARD_MODE : FAST_MODE; \
        yopen_I2cInit((yopen_i2c_channel_e)channel, mode); \
    } while(0)

#define iot_i2c_deinit(channel) \
    yopen_I2cRelease((yopen_i2c_channel_e)channel)

#define iot_i2c_master_send(channel, addr, data, len) \
    yopen_I2cWrite((yopen_i2c_channel_e)channel, (uint8_t)addr, 0, (uint8_t*)data, len)

#define iot_i2c_master_recv(channel, addr, data, len) \
    yopen_I2cRead((yopen_i2c_channel_e)channel, (uint8_t)addr, 0, (uint8_t*)data, len)

#define iot_i2c_master_write_reg(channel, addr, reg, data, len) \
    yopen_I2cWrite((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint8_t)reg, (uint8_t*)data, len)

#define iot_i2c_master_read_reg(channel, addr, reg, data, len) \
    yopen_I2cRead((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint8_t)reg, (uint8_t*)data, len)

#define iot_i2c_master_write_reg16(channel, addr, reg, data, len) \
    yopen_I2cWrite_16bit_addr((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint16_t)reg, (uint8_t*)data, len)

#define iot_i2c_master_read_reg16(channel, addr, reg, data, len) \
    yopen_I2cRead_16bit_addr((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint16_t)reg, (uint8_t*)data, len)

#define iot_i2c_master_send_noaddr(channel, addr, data, len) \
    yopen_I2cWrite_Noaddr((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint8_t*)data, len)

#define iot_i2c_master_recv_noaddr(channel, addr, data, len) \
    yopen_I2cRead_Noaddr((yopen_i2c_channel_e)channel, (uint8_t)addr, (uint8_t*)data, len)

#endif
