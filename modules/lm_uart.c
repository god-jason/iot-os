/*
@module  uart
@summary UART串口通信
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     UART
*/

/*
UART参考示例
-- 打开串口1, 波特率115200
uart.setup(1, 115200, 8, 0, 0, 0)

-- 发送数据
uart.write(1, "hello world")

-- 读取数据
local data = uart.read(1, 100)
if data and #data > 0 then
    print("UART1 received:", data)
end

-- 关闭串口
uart.close(1)
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_uart.h"
#include "iot_rtos.h"

/* UART设备数量 */
#define IOT_UART_MAX   4

/* 串口配置参数 */
typedef struct {
    int inited;
    cm_uart_dev_e dev;
    cm_uart_cfg_t cfg;
    void* callback_ud;  /* 回调userdata，闭包存uservalue */
} iot_uart_ctx_t;

/* 全局UART上下文 */
static iot_uart_ctx_t g_uart_ctx[IOT_UART_MAX] = {0};

/* 串口事件回调 */
static void uart_event_cb(void *param, uint32_t evt) {
    int id = (int)(uintptr_t)param;
    if (id < 0 || id >= IOT_UART_MAX) return;
    
    LOG("evt id=%d evt=%u", id, evt);
    
    if (g_uart_ctx[id].callback_ud) {
        iot_rtos_call(g_uart_ctx[id].callback_ud, NULL);
    }
}

/**
 * @brief 配置并打开串口
 * @api uart.setup(id, baudrate, databits, parity, stopbits, flowctl)
 * @int id 串口ID,0-3
 * @int baudrate 波特率,支持2400-12000000
 * @int databits 数据位,5-8,默认8
 * @int parity 校验位,0无,1奇,2偶,默认0
 * @int stopbits 停止位,0一位,2两位,默认0
 * @int flowctl 流控,0无,1硬件,默认0
 * @return bool 成功返回true,失败返回false
 * @usage
uart.setup(1, 115200, 8, 0, 0, 0)
*/
static int iot_uart_setup(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int baudrate = luaL_checkinteger(L, 2);
    int databits = luaL_optinteger(L, 3, 8);
    int parity = luaL_optinteger(L, 4, 0);
    int stopbits = luaL_optinteger(L, 5, 0);
    int flowctl = luaL_optinteger(L, 6, 0);

    if (id < 0 || id >= IOT_UART_MAX) {
        LOG("ERR invalid id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 如果已经初始化,先关闭 */
    if (g_uart_ctx[id].inited) {
        LOG("reopen id=%d", id);
        cm_uart_close(g_uart_ctx[id].dev);
        g_uart_ctx[id].inited = 0;
    }

    /* 配置串口参数 */
    cm_uart_cfg_t cfg;
    cfg.baudrate = baudrate;
    cfg.is_lpuart = 0;
    cfg.rxrb_buf_size = 2048;

    switch (databits) {
        case 5: cfg.byte_size = CM_UART_BYTE_SIZE_5; break;
        case 6: cfg.byte_size = CM_UART_BYTE_SIZE_6; break;
        case 7: cfg.byte_size = CM_UART_BYTE_SIZE_7; break;
        default: cfg.byte_size = CM_UART_BYTE_SIZE_8; break;
    }

    switch (parity) {
        case 1: cfg.parity = CM_UART_PARITY_ODD; break;
        case 2: cfg.parity = CM_UART_PARITY_EVEN; break;
        default: cfg.parity = CM_UART_PARITY_NONE; break;
    }

    switch (stopbits) {
        case 2: cfg.stop_bit = CM_UART_STOP_BIT_TWO; break;
        default: cfg.stop_bit = CM_UART_STOP_BIT_ONE; break;
    }

    switch (flowctl) {
        case 1: cfg.flow_ctrl = CM_UART_FLOW_CTRL_HW; break;
        default: cfg.flow_ctrl = CM_UART_FLOW_CTRL_NONE; break;
    }

    /* 注册事件（须在 open 之前） */
    cm_uart_event_t event;
    event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED;
    event.event_entry = (void*)uart_event_cb;
    event.event_param = (void*)(uintptr_t)id;
    if (cm_uart_register_event((cm_uart_dev_e)id, &event) != 0) {
        LOG("ERR register event id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 打开串口 */
    LOG("open id=%d baud=%d", id, baudrate);
    int ret = cm_uart_open((cm_uart_dev_e)id, &cfg);
    if (ret != 0) {
        LOG("ERR open ret=%d", ret);
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 保存配置 */
    g_uart_ctx[id].inited = 1;
    g_uart_ctx[id].dev = (cm_uart_dev_e)id;
    g_uart_ctx[id].cfg = cfg;

    LOG("OK id=%d", id);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭串口
 * @api uart.close(id)
 * @int id 串口ID,0-3
 * @return bool 成功返回true,失败返回false
 * @usage
uart.close(1)
*/
static int iot_uart_close(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_UART_MAX) {
        LOG("ERR invalid id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        lua_pushboolean(L, 1);
        return 1;
    }

    /* 清理回调 */
    if (g_uart_ctx[id].callback_ud) {
        lua_pushlightuserdata(L, g_uart_ctx[id].callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        g_uart_ctx[id].callback_ud = NULL;
    }

    LOG("close id=%d", id);
    int ret = cm_uart_close((cm_uart_dev_e)id);
    g_uart_ctx[id].inited = 0;

    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 发送数据
 * @api uart.write(id, data)
 * @int id 串口ID,0-3
 * @string data 待发送数据
 * @return int 实际发送长度,失败返回nil
 * @usage
uart.write(1, "hello")
uart.write(1, string.char(0x55, 0xAA))
*/
static int iot_uart_write(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    if (id < 0 || id >= IOT_UART_MAX) {
        LOG("ERR invalid id=%d", id);
        lua_pushnil(L);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        LOG("ERR not open id=%d", id);
        lua_pushnil(L);
        return 1;
    }

    int ret = cm_uart_write((cm_uart_dev_e)id, data, len, 0);
    if (ret < 0) {
        LOG("ERR write ret=%d", ret);
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

/**
 * @brief 读取数据
 * @api uart.read(id, len)
 * @int id 串口ID,0-3
 * @int len 读取长度
 * @return string 读取到的数据,失败返回nil
 * @usage
local data = uart.read(1, 128)
if data then
    print("recv:", data)
end
*/
static int iot_uart_read(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int len = luaL_checkinteger(L, 2);

    if (id < 0 || id >= IOT_UART_MAX) {
        LOG("ERR invalid id=%d", id);
        lua_pushnil(L);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        LOG("ERR not open id=%d", id);
        lua_pushnil(L);
        return 1;
    }

    if (len <= 0) {
        lua_pushnil(L);
        return 1;
    }

    char* buff = (char*)lua_newuserdata(L, len);
    int ret = cm_uart_read((cm_uart_dev_e)id, buff, len, 0);
    if (ret <= 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, buff, ret);
    return 1;
}

/**
 * @brief 获取接收缓冲区数据长度
 * @api uart.get_rxrb_data_len(id)
 * @int id 串口ID,0-3
 * @return int 数据长度,失败返回0
 * @usage
local len = uart.get_rxrb_data_len(1)
print("rx len:", len)
*/
static int iot_uart_get_rxrb_data_len(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushinteger(L, 0);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        lua_pushinteger(L, 0);
        return 1;
    }

    int len = cm_uart_get_rxrb_data_len((cm_uart_dev_e)id);
    lua_pushinteger(L, len);
    return 1;
}

/**
 * @brief 清空接收缓冲区
 * @api uart.clean(id)
 * @int id 串口ID,0-3
 * @return bool 成功返回true,失败返回false
 * @usage
uart.clean(1)
*/
static int iot_uart_clean(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_uart_clean((cm_uart_dev_e)id);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 查询串口发送状态
 * @api uart.is_sending(id)
 * @int id 串口ID,0-3
 * @return bool 正在发送返回true,空闲返回false
 * @usage
local sending = uart.is_sending(1)
*/
static int iot_uart_is_sending(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_uart_is_sending((cm_uart_dev_e)id);
    lua_pushboolean(L, ret == 1);
    return 1;
}

/**
 * @brief 获取串口配置
 * @api uart.get_cfg(id)
 * @int id 串口ID,0-3
 * @return table 串口配置信息
 * @usage
local cfg = uart.get_cfg(1)
if cfg then
    print("baudrate:", cfg.baudrate)
end
*/
static int iot_uart_get_cfg(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushnil(L);
        return 1;
    }

    if (!g_uart_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }

    cm_uart_cfg_t cfg;
    int ret = cm_uart_get_cfg((cm_uart_dev_e)id, &cfg);
    if (ret != 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 5);
    lua_pushinteger(L, cfg.baudrate);
    lua_setfield(L, -2, "baudrate");
    lua_pushinteger(L, cfg.byte_size);
    lua_setfield(L, -2, "databits");
    lua_pushinteger(L, cfg.parity);
    lua_setfield(L, -2, "parity");
    lua_pushinteger(L, cfg.stop_bit);
    lua_setfield(L, -2, "stopbits");
    lua_pushinteger(L, cfg.flow_ctrl);
    lua_setfield(L, -2, "flowctl");

    return 1;
}

/**
 * @brief 注册UART事件回调
 * @api uart.on(id, callback)
 * @int id 串口ID,0-3
 * @func callback 回调函数，接收事件类型作为参数
 * @return bool 成功返回true,失败返回false
 * @usage
 * uart.on(1, function()
 *     local data = uart.read(1, 1024)
 *     if data then
 *         print("UART1 received:", data)
 *     end
 * end)
*/
static int iot_uart_on(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 释放之前的回调 */
    if (g_uart_ctx[id].callback_ud) {
        /* 从注册表中删除旧的引用 */
        lua_pushlightuserdata(L, g_uart_ctx[id].callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        g_uart_ctx[id].callback_ud = NULL;
    }

    /* 创建userdata并将闭包存储在uservalue中 */
    void* ud = lua_newuserdata(L, 1);
    lua_pushvalue(L, 2);
    lua_setuservalue(L, -2);
    g_uart_ctx[id].callback_ud = ud;

    /* 将userdata保存到注册表，防止被GC回收 */
    lua_pushlightuserdata(L, ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pop(L, 1);

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg uart_lib[] = {
    { "setup",   iot_uart_setup },
    { "close",   iot_uart_close },
    { "write",   iot_uart_write },
    { "read",    iot_uart_read },
    { "on",      iot_uart_on },
    { "get_cfg", iot_uart_get_cfg },
    { "get_rxrb_data_len", iot_uart_get_rxrb_data_len },
    { "clean",   iot_uart_clean },
    { "is_sending", iot_uart_is_sending },
    {NULL, NULL}
};

LUAMOD_API int luaopen_uart(lua_State* L) {
    luaL_newlibtable(L, uart_lib);
    luaL_setfuncs(L, uart_lib, 0);
    return 1;
}
