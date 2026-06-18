/*
@module  spi
@summary SPI总线
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SPI
*/

/*
SPI参考示例
-- 打开SPI总线
local spi = spi.open(0, {
    mode = "MODE0",
    freq = 13000000,
    cpol = 0,
    cpha = 0
})

-- 发送和接收数据
local rx = spi:transfer(string.char(0x9F))
print("response:", rx:byte())

-- 关闭SPI
spi:close()
*/

#include "module.h"
#include "yopen_spi.h"

/* SPI句柄结构 */
typedef struct {
    yopen_spi_port_e port;
    int initialized;
} spi_handle_t;

/* SPI默认配置 */
#define SPI_DEFAULT_PORT   YOPEN_SPI_PORT0
#define SPI_DEFAULT_CLK    YOPEN_SPI_CLK_6_5MHZ
#define SPI_DEFAULT_CPOL   YOPEN_SPI_CPOL_LOW
#define SPI_DEFAULT_CPHA   YOPEN_SPI_CPHA_1Edge
#define SPI_DEFAULT_TRANS   YOPEN_SPI_DIRECT_POLLING

/**
 * @brief 打开SPI总线
 * @api spi.open(port, config)
 * @int port 端口号 (0=SPI0)
 * @table config 配置参数
 * @return userdata SPI操作句柄
 */
static int luaopen_spi_open(lua_State* L) {
    int port = (int)luaL_checkinteger(L, 1);

    /* 解析配置参数 */
    yopen_spi_config_s config = {0};
    config.port = (yopen_spi_port_e)port;
    config.framesize = 8;
    config.spiclk = SPI_DEFAULT_CLK;
    config.cpol = SPI_DEFAULT_CPOL;
    config.cpha = SPI_DEFAULT_CPHA;
    config.transmode = SPI_DEFAULT_TRANS;

    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "freq");
        if (lua_isnumber(L, -1)) {
            int freq = (int)lua_tonumber(L, -1);
            /* 根据频率设置时钟 */
            if (freq >= 13000000) config.spiclk = YOPEN_SPI_CLK_13MHZ;
            else if (freq >= 6500000) config.spiclk = YOPEN_SPI_CLK_6_5MHZ;
            else if (freq >= 3250000) config.spiclk = YOPEN_SPI_CLK_3_25MHZ;
            else if (freq >= 1625000) config.spiclk = YOPEN_SPI_CLK_1_625MHZ;
            else config.spiclk = YOPEN_SPI_CLK_812_5KHZ;
        }
        lua_pop(L, 1);

        lua_getfield(L, 2, "cpol");
        if (lua_isnumber(L, -1)) config.cpol = (yopen_spi_cpol_pol_e)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "cpha");
        if (lua_isnumber(L, -1)) config.cpha = (yopen_spi_cpha_pol_e)(int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    /* 分配句柄 */
    spi_handle_t* handle = (spi_handle_t*)iot_malloc(sizeof(spi_handle_t));
    if (!handle) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }

    handle->port = config.port;
    handle->initialized = 0;

    /* 初始化SPI */
    yopen_errcode_spi_e ret = yopen_spi_init(config);
    if (ret == YOPEN_SPI_SUCCESS) {
        handle->initialized = 1;
        lua_pushlightuserdata(L, handle);
        return 1;
    }

    iot_free(handle);
    lua_pushnil(L);
    lua_pushstring(L, "spi init failed");
    return 2;
}

/**
 * @brief 发送数据
 * @api spi:write(data)
 * @string data 发送的数据
 * @return int 发送的字节数
 */
static int luaopen_spi_write(lua_State* L) {
    spi_handle_t* handle = (spi_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        lua_pushinteger(L, 0);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    yopen_errcode_spi_e ret = yopen_spi_write(handle->port, (uint8_t*)data, (uint32_t)len);

    if (ret == YOPEN_SPI_SUCCESS) {
        lua_pushinteger(L, (int)len);
    } else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

/**
 * @brief 读取数据
 * @api spi:read(len)
 * @int len 读取长度
 * @return string 读取的数据
 */
static int luaopen_spi_read(lua_State* L) {
    spi_handle_t* handle = (spi_handle_t*)lua_touserdata(L, 1);
    int len = (int)luaL_checkinteger(L, 2);

    if (!handle || !handle->initialized) {
        lua_pushstring(L, "");
        return 1;
    }

    if (len <= 0 || len > 4096) {
        lua_pushstring(L, "");
        return 1;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(len);
    if (!buf) {
        lua_pushstring(L, "");
        return 1;
    }

    yopen_errcode_spi_e ret = yopen_spi_read(handle->port, buf, (uint32_t)len);

    if (ret == YOPEN_SPI_SUCCESS) {
        lua_pushlstring(L, (char*)buf, len);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 同时发送和接收数据
 * @api spi:transfer(data)
 * @string data 发送的数据
 * @return string 接收的数据
 */
static int luaopen_spi_transfer(lua_State* L) {
    spi_handle_t* handle = (spi_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        lua_pushstring(L, "");
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    if (len <= 0 || len > 4096) {
        lua_pushstring(L, "");
        return 1;
    }

    uint8_t* rx_buf = (uint8_t*)iot_malloc(len);
    if (!rx_buf) {
        lua_pushstring(L, "");
        return 1;
    }

    yopen_errcode_spi_e ret = yopen_spi_write_read(
        handle->port,
        rx_buf,
        (uint8_t*)data,
        (uint32_t)len
    );

    if (ret == YOPEN_SPI_SUCCESS) {
        lua_pushlstring(L, (char*)rx_buf, len);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(rx_buf);
    return 1;
}

/**
 * @brief 关闭SPI总线
 * @api spi:close()
 * @return nil
 */
static int luaopen_spi_close(lua_State* L) {
    spi_handle_t* handle = (spi_handle_t*)lua_touserdata(L, 1);

    if (handle) {
        if (handle->initialized) {
            yopen_spi_release(handle->port);
            handle->initialized = 0;
        }
        iot_free(handle);
    }
    return 0;
}

static const luaL_Reg luaopen_spi_funcs[] = {
    {"open",    luaopen_spi_open},
    {NULL, NULL}
};

static const luaL_Reg luaopen_spi_methods[] = {
    {"write",    luaopen_spi_write},
    {"read",     luaopen_spi_read},
    {"transfer", luaopen_spi_transfer},
    {"close",    luaopen_spi_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_spi_consts[] = {
    {"PORT0",  YOPEN_SPI_PORT0},
    {"CS0",    YOPEN_SPI_CS0},
    {"CS1",    YOPEN_SPI_CS1},
    {"MODE0",  0},
    {"MODE1",  1},
    {"MODE2",  2},
    {"MODE3",  3},
    {NULL, NULL}
};

int luaopen_spi(lua_State* L) {
    luaL_newlib(L, luaopen_spi_funcs);
    luaL_setfuncs(L, luaopen_spi_consts, 0);

    /* 创建元表 */
    luaL_newmetatable(L, "spi_handle");
    luaL_setfuncs(L, luaopen_spi_methods, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    return 1;
}