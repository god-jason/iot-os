/*
@module  spi
@summary SPI总线通信
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     SPI
*/

/*
SPI参考示例
-- 打开SPI0,13MHz,模式0
spi.setup(0, 13000000, 0)

-- 发送并接收数据
local send_data = string.char(0x9F)
local recv_data = spi.transfer(0, send_data)
if recv_data then
    print("SPI0 recv:", recv_data:toHex())
end

-- 写数据
spi.write(0, string.char(0x06, 0x00))

-- 读数据
local data = spi.read(0, 4)

-- 关闭SPI0
spi.close(0)
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_spi.h"

/* SPI设备数量 */
#define IOT_SPI_MAX   2

/* SPI上下文 */
typedef struct {
    int inited;
    cm_spi_dev_e dev;
} iot_spi_ctx_t;

/* 全局SPI上下文 */
static iot_spi_ctx_t g_spi_ctx[IOT_SPI_MAX] = {0};

/**
 * @brief 配置并打开SPI
 * @api spi.setup(id, speed, mode)
 * @int id SPI ID,0-1
 * @int speed 速度,单位Hz,支持13000000,26000000,39000000
 * @int mode 模式,0-3,默认0
 * @return bool 成功返回true,失败返回false
 * @usage
spi.setup(0, 13000000, 0)
*/
static int iot_spi_setup(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int speed = luaL_checkinteger(L, 2);
    int mode = luaL_optinteger(L, 3, 0);

    if (id < 0 || id >= IOT_SPI_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 如果已经初始化，先关闭 */
    if (g_spi_ctx[id].inited) {
        cm_spi_close(g_spi_ctx[id].dev);
        g_spi_ctx[id].inited = 0;
    }

    /* 配置SPI */
    cm_spi_cfg_t cfg;
    cfg.mode = CM_SPI_MODE_MASTER;
    cfg.work_mode = (cm_spi_work_mode_e)mode;
    cfg.data_width = CM_SPI_DATA_WIDTH_8BIT;
    cfg.nss = CM_SPI_NSS_HARD;
    cfg.clk = speed;

    /* 打开SPI */
    int ret = cm_spi_open((cm_spi_dev_e)id, &cfg);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 保存配置 */
    g_spi_ctx[id].inited = 1;
    g_spi_ctx[id].dev = (cm_spi_dev_e)id;

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭SPI
 * @api spi.close(id)
 * @int id SPI ID,0-1
 * @return bool 成功返回true,失败返回false
 * @usage
spi.close(0)
*/
static int iot_spi_close(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_SPI_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_spi_ctx[id].inited) {
        lua_pushboolean(L, 1);
        return 1;
    }

    cm_spi_close((cm_spi_dev_e)id);
    g_spi_ctx[id].inited = 0;

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief SPI发送数据
 * @api spi.write(id, data)
 * @int id SPI ID,0-1
 * @string data 待发送数据
 * @return int 实际发送长度，失败返回nil
 * @usage
spi.write(0, string.char(0x06, 0x00))
*/
static int iot_spi_write(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    if (id < 0 || id >= IOT_SPI_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_spi_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    int ret = cm_spi_write((cm_spi_dev_e)id, data, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

/**
 * @brief SPI读取数据
 * @api spi.read(id, len)
 * @int id SPI ID,0-1
 * @int len 读取长度
 * @return string 读取到的数据,失败返回nil
 * @usage
local data = spi.read(0, 4)
*/
static int iot_spi_read(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int len = luaL_checkinteger(L, 2);

    if (id < 0 || id >= IOT_SPI_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_spi_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    if (len <= 0) {
        lua_pushnil(L);
        return 1;
    }

    char* buff = (char*)lua_newuserdata(L, len);
    int ret = cm_spi_read((cm_spi_dev_e)id, buff, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, buff, ret);
    }
    return 1;
}

/**
 * @brief SPI发送并接收数据
 * @api spi.transfer(id, data)
 * @int id SPI ID,0-1
 * @string data 待发送数据
 * @return string 接收到的数据,失败返回nil
 * @usage
local send_data = string.char(0x9F)
local recv_data = spi.transfer(0, send_data)
if recv_data then
    print("recv:", recv_data:toHex())
end
*/
static int iot_spi_transfer(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    size_t w_len = 0;
    const char* w_data = luaL_checklstring(L, 2, &w_len);
    int r_len = luaL_optinteger(L, 3, w_len);

    if (id < 0 || id >= IOT_SPI_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_spi_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    if (r_len <= 0) {
        lua_pushnil(L);
        return 1;
    }

    char* r_buff = (char*)lua_newuserdata(L, r_len);
    int ret = cm_spi_write_then_read((cm_spi_dev_e)id, w_data, w_len, r_buff, r_len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushlstring(L, r_buff, r_len);
    }
    return 1;
}

static const luaL_Reg spi_lib[] = {
    { "setup",   iot_spi_setup },
    { "close",   iot_spi_close },
    { "write",   iot_spi_write },
    { "read",    iot_spi_read },
    { "transfer", iot_spi_transfer },
    {NULL, NULL}
};

LUAMOD_API int luaopen_spi(lua_State* L) {
    luaL_newlibtable(L, spi_lib);
    luaL_setfuncs(L, spi_lib, 0);
    return 1;
}
