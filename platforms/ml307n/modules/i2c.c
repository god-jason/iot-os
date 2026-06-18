/*
@module  i2c
@summary I2C总线通信
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     I2C
*/

/*
I2C参考示例
-- 打开I2C0,100KHz
i2c.setup(0, 100000)

-- 写数据到从机0x18
local data = string.char(0xFD, 0x00)
i2c.write(0, 0x18, data)

-- 读取数据
local data = i2c.read(0, 0x18, 2)
if data then
    print("I2C0 read:", data:toHex())
end

-- 关闭I2C0
i2c.close(0)
*/


#include "lua.h"
#include "module.h"
#include "cm_i2c.h"

/* I2C设备数量 */
#define IOT_I2C_MAX   3

/* I2C上下文 */
typedef struct {
    int inited;
    cm_i2c_dev_e dev;
} iot_i2c_ctx_t;

/* 全局I2C上下文 */
static iot_i2c_ctx_t g_i2c_ctx[IOT_I2C_MAX] = {0};

/**
 * @brief 配置并打开I2C
 * @api i2c.setup(id, speed)
 * @int id I2C ID,0-2
 * @int speed 速度,100000~400000
 * @return bool 成功返回true,失败返回false
 * @usage
i2c.setup(0, 100000)
*/
static int iot_i2c_setup(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int speed = luaL_checkinteger(L, 2);

    if (id < 0 || id >= IOT_I2C_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 如果已经初始化，先关闭 */
    if (g_i2c_ctx[id].inited) {
        cm_i2c_close(g_i2c_ctx[id].dev);
        g_i2c_ctx[id].inited = 0;
    }

    /* 配置I2C */
    cm_i2c_cfg_t cfg;
    cfg.addr_type = CM_I2C_ADDR_TYPE_7BIT;
    cfg.mode = CM_I2C_MODE_MASTER;
    cfg.clk = speed;

    /* 打开I2C */
    int ret = cm_i2c_open((cm_i2c_dev_e)id, &cfg);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 保存配置 */
    g_i2c_ctx[id].inited = 1;
    g_i2c_ctx[id].dev = (cm_i2c_dev_e)id;

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭I2C
 * @api i2c.close(id)
 * @int id I2C ID,0-2
 * @return bool 成功返回true,失败返回false
 * @usage
i2c.close(0)
*/
static int iot_i2c_close(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_I2C_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_i2c_ctx[id].inited) {
        lua_pushboolean(L, 1);
        return 1;
    }

    int ret = cm_i2c_close((cm_i2c_dev_e)id);
    g_i2c_ctx[id].inited = 0;

    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief I2C写数据
 * @api i2c.write(id, addr, data)
 * @int id I2C ID,0-2
 * @int addr 从机地址
 * @string data 待写数据
 * @return int 实际写入长度,失败返回nil
 * @usage
i2c.write(0, 0x18, string.char(0xFD, 0x00))
*/
static int iot_i2c_write(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int addr = luaL_checkinteger(L, 2);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 3, &len);

    if (id < 0 || id >= IOT_I2C_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_i2c_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    int ret = cm_i2c_write((cm_i2c_dev_e)id, (uint16_t)addr, (uint8_t*)data, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

/**
 * @brief I2C读数据
 * @api i2c.read(id, addr, len)
 * @int id I2C ID,0-2
 * @int addr 从机地址
 * @int len 读取长度
 * @return string 读取到的数据,失败返回nil
 * @usage
local data = i2c.read(0, 0x18, 2)
if data then
    print("I2C0 read:", data:toHex())
end
*/
static int iot_i2c_read(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int addr = luaL_checkinteger(L, 2);
    int len = luaL_checkinteger(L, 3);

    if (id < 0 || id >= IOT_I2C_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_i2c_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    if (len <= 0) {
        lua_pushnil(L);
        return 1;
    }

    char* buff = (char*)lua_newuserdata(L, len);
    int ret = cm_i2c_read((cm_i2c_dev_e)id, (uint16_t)addr, (uint8_t*)buff, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, buff, ret);
    }
    return 1;
}

static const luaL_Reg i2c_lib[] = {
    { "setup", iot_i2c_setup },
    { "close", iot_i2c_close },
    { "write", iot_i2c_write },
    { "read",  iot_i2c_read },
    {NULL, NULL}
};

LUAMOD_API int luaopen_i2c(lua_State* L) {
    luaL_newlibtable(L, i2c_lib);
    luaL_setfuncs(L, i2c_lib, 0);
    return 1;
}
