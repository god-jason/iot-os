/*
@module  i2c
@summary I2C总线
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     I2C
*/

/*
I2C参考示例
-- 打开I2C总线
local i2c = i2c.open(0, {mode=i2c.STANDARD})  -- 0=I2C0, 1=I2C1

-- 写入数据到从设备寄存器
i2c:write(0x68, 0x00, string.char(0x00))

-- 读取数据
local data = i2c:read(0x68, 0x00, 6)
print("data:", data)

-- 关闭I2C
i2c:close()
*/

#include "module.h"
#include "yopen_i2c.h"

/* I2C通道 */
#define I2C_CHANNEL_0   0
#define I2C_CHANNEL_1   1

/* I2C模式 */
#define I2C_MODE_STANDARD  0  /* 100K */
#define I2C_MODE_FAST      1  /* 400K */

/* I2C句柄结构 */
typedef struct {
    yopen_i2c_channel_e channel;
    int initialized;
} i2c_handle_t;

/**
 * @brief 打开I2C总线
 * @api i2c.open(channel, config)
 * @int channel 总线编号 (0或1)
 * @table config 配置参数 {mode=i2c.STANDARD/i2c.FAST}
 * @return userdata I2C操作句柄
 */
static int luaopen_i2c_open(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);

    if (channel != I2C_CHANNEL_0 && channel != I2C_CHANNEL_1) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid channel, must be 0 or 1");
        return 2;
    }

    /* 解析配置参数 */
    int mode = I2C_MODE_STANDARD;
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "mode");
        if (lua_isnumber(L, -1)) {
            mode = (int)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }

    /* 分配句柄 */
    i2c_handle_t* handle = (i2c_handle_t*)iot_malloc(sizeof(i2c_handle_t));
    if (!handle) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }

    handle->channel = (yopen_i2c_channel_e)channel;
    handle->initialized = 0;

    /* 初始化I2C */
    yopen_errcode_i2c_e ret = yopen_I2cInit(
        handle->channel,
        (yopen_i2c_mode_e)mode
    );

    if (ret == YOPEN_I2C_SUCCESS) {
        handle->initialized = 1;
        lua_pushlightuserdata(L, handle);
        return 1;
    }

    iot_free(handle);
    lua_pushnil(L);
    lua_pushstring(L, "i2c init failed");
    return 2;
}

/**
 * @brief 向I2C从设备写入数据
 * @api i2c:write(slave_addr, reg_addr, data)
 * @int slave_addr 从设备地址
 * @int reg_addr 寄存器地址
 * @string data 写入的数据
 * @return bool true表示成功
 */
static int luaopen_i2c_write(lua_State* L) {
    i2c_handle_t* handle = (i2c_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int slave_addr = (int)luaL_checkinteger(L, 2);
    int reg_addr = (int)luaL_checkinteger(L, 3);
    size_t data_len = 0;
    const uint8_t* data = (const uint8_t*)luaL_checklstring(L, 4, &data_len);

    yopen_errcode_i2c_e ret = yopen_I2cWrite(
        handle->channel,
        (uint8_t)slave_addr,
        (uint8_t)reg_addr,
        (uint8_t*)data,
        (uint32_t)data_len
    );

    lua_pushboolean(L, ret == YOPEN_I2C_SUCCESS);
    return 1;
}

/**
 * @brief 从I2C从设备读取数据
 * @api i2c:read(slave_addr, reg_addr, len)
 * @int slave_addr 从设备地址
 * @int reg_addr 寄存器地址
 * @int len 读取长度
 * @return string 读取的数据
 */
static int luaopen_i2c_read(lua_State* L) {
    i2c_handle_t* handle = (i2c_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        lua_pushstring(L, "");
        return 1;
    }

    int slave_addr = (int)luaL_checkinteger(L, 2);
    int reg_addr = (int)luaL_checkinteger(L, 3);
    int len = (int)luaL_checkinteger(L, 4);

    if (len <= 0 || len > 256) {
        lua_pushstring(L, "");
        return 1;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(len);
    if (!buf) {
        lua_pushstring(L, "");
        return 1;
    }

    yopen_errcode_i2c_e ret = yopen_I2cRead(
        handle->channel,
        (uint8_t)slave_addr,
        (uint8_t)reg_addr,
        buf,
        (uint32_t)len
    );

    if (ret == YOPEN_I2C_SUCCESS) {
        lua_pushlstring(L, (char*)buf, len);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 关闭I2C总线
 * @api i2c:close()
 * @return nil
 */
static int luaopen_i2c_close(lua_State* L) {
    i2c_handle_t* handle = (i2c_handle_t*)lua_touserdata(L, 1);

    if (handle) {
        if (handle->initialized) {
            yopen_I2cRelease(handle->channel);
            handle->initialized = 0;
        }
        iot_free(handle);
    }
    return 0;
}

static const luaL_Reg luaopen_i2c_funcs[] = {
    {"open",  luaopen_i2c_open},
    {NULL, NULL}
};

static const luaL_Reg luaopen_i2c_methods[] = {
    {"write", luaopen_i2c_write},
    {"read",  luaopen_i2c_read},
    {"close", luaopen_i2c_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_i2c_consts[] = {
    {"CH0",      I2C_CHANNEL_0},
    {"CH1",      I2C_CHANNEL_1},
    {"STANDARD", I2C_MODE_STANDARD},
    {"FAST",     I2C_MODE_FAST},
    {NULL, NULL}
};

int luaopen_i2c(lua_State* L) {
    luaL_newlib(L, luaopen_i2c_funcs);
    luaL_setfuncs(L, luaopen_i2c_consts, 0);

    /* 创建元表 */
    luaL_newmetatable(L, "i2c_handle");
    luaL_setfuncs(L, luaopen_i2c_methods, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    return 1;
}