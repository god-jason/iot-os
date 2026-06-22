/*
@module  uart
@summary UART串口模块
@version 1.0
@date    2026.06.20
@tag     UART
*/

#include "module.h"
#include "driver/uart.h"

typedef struct uart_ctx {
    uart_port_t port;
} uart_ctx_t;

static uart_ctx_t* uart_get_ctx_from_userdata(lua_State* L, int idx) {
    uart_ctx_t** ctx_ptr = (uart_ctx_t**)luaL_checkudata(L, idx, "uart");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid uart");
        return NULL;
    }
    return *ctx_ptr;
}

static int luaopen_uart_new(lua_State* L) {
    int port = luaL_checkinteger(L, 1);
    
    uart_ctx_t* ctx = (uart_ctx_t*)iot_malloc(sizeof(uart_ctx_t));
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    memset(ctx, 0, sizeof(uart_ctx_t));
    ctx->port = (uart_port_t)port;
    
    uart_ctx_t** ctx_ptr = (uart_ctx_t**)lua_newuserdata(L, sizeof(uart_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, "uart");
    lua_setmetatable(L, -2);
    
    return 1;
}

static int luaopen_uart_config(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int baud_rate = luaL_checkinteger(L, 2);
    int data_bits = luaL_optinteger(L, 3, 8);
    int parity = luaL_optinteger(L, 4, 0);
    int stop_bits = luaL_optinteger(L, 5, 1);
    
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = (uart_word_length_t)data_bits,
        .parity = (uart_parity_t)parity,
        .stop_bits = (uart_stop_bits_t)stop_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    uart_param_config(ctx->port, &uart_config);
    uart_driver_install(ctx->port, 2048, 2048, 0, NULL, 0);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_set_pin(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int tx = luaL_checkinteger(L, 2);
    int rx = luaL_checkinteger(L, 3);
    int rts = luaL_optinteger(L, 4, UART_PIN_NO_CHANGE);
    int cts = luaL_optinteger(L, 5, UART_PIN_NO_CHANGE);
    
    uart_set_pin(ctx->port, tx, rx, rts, cts);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_write(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    
    int ret = uart_write_bytes(ctx->port, data, len);
    lua_pushinteger(L, ret);
    
    return 1;
}

static int luaopen_uart_read(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int maxlen = luaL_checkinteger(L, 2);
    
    char* buf = (char*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    
    int ret = uart_read_bytes(ctx->port, buf, maxlen, pdMS_TO_TICKS(100));
    if (ret < 0) {
        iot_free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "read failed");
        return 2;
    }
    
    lua_pushlstring(L, buf, ret);
    iot_free(buf);
    
    return 1;
}

static int luaopen_uart_read_bytes(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int maxlen = luaL_checkinteger(L, 2);
    int timeout_ms = luaL_optinteger(L, 3, 100);
    
    char* buf = (char*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    
    int ret = uart_read_bytes(ctx->port, buf, maxlen, pdMS_TO_TICKS(timeout_ms));
    if (ret < 0) {
        iot_free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "read failed");
        return 2;
    }
    
    lua_pushlstring(L, buf, ret);
    iot_free(buf);
    
    return 1;
}

static int luaopen_uart_flush(lua_State* L) {
    uart_ctx_t* ctx = uart_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    uart_flush_input(ctx->port);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_close(lua_State* L) {
    uart_ctx_t** ctx_ptr = (uart_ctx_t**)luaL_checkudata(L, 1, "uart");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    uart_ctx_t* ctx = *ctx_ptr;
    uart_driver_delete(ctx->port);
    iot_free(ctx);
    *ctx_ptr = NULL;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_gc(lua_State* L) {
    uart_ctx_t** ctx_ptr = (uart_ctx_t**)luaL_checkudata(L, 1, "uart");
    if (ctx_ptr && *ctx_ptr) {
        uart_driver_delete((*ctx_ptr)->port);
        iot_free(*ctx_ptr);
        *ctx_ptr = NULL;
    }
    return 0;
}

static const luaL_Reg luaopen_uart_methods[] = {
    { "config",        luaopen_uart_config },
    { "set_pin",       luaopen_uart_set_pin },
    { "write",         luaopen_uart_write },
    { "read",          luaopen_uart_read },
    { "read_bytes",    luaopen_uart_read_bytes },
    { "flush",         luaopen_uart_flush },
    { "close",         luaopen_uart_close },
    { NULL, NULL }
};

static const luaL_Reg luaopen_uart_metatable[] = {
    { "__gc",          luaopen_uart_gc },
    { NULL, NULL }
};

static const luaL_Reg luaopen_uart_lib[] = {
    { "new",           luaopen_uart_new },
    { NULL, NULL }
};

LUAMOD_API int luaopen_uart_register(lua_State* L) {
    luaL_newmetatable(L, "uart");
    
    luaL_newlibtable(L, luaopen_uart_methods);
    luaL_setfuncs(L, luaopen_uart_methods, 0);
    lua_setfield(L, -2, "__index");
    
    luaL_setfuncs(L, luaopen_uart_metatable, 0);
    
    luaL_newlibtable(L, luaopen_uart_lib);
    luaL_setfuncs(L, luaopen_uart_lib, 0);
    
    lua_pushinteger(L, UART_NUM_0);
    lua_setfield(L, -2, "UART_NUM_0");
    lua_pushinteger(L, UART_NUM_1);
    lua_setfield(L, -2, "UART_NUM_1");
    lua_pushinteger(L, UART_NUM_2);
    lua_setfield(L, -2, "UART_NUM_2");
    
    return 1;
}
