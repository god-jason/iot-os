/*
@module  platform_modules
@summary Windows平台模块注册入口
@version 1.0
@date    2026.06.19
*/

#include "module.h"
#include "iot_log.h"

extern int luaopen_uart_register(lua_State* L);

static const luaL_Reg platform_modules[] = {
    {"uart",   luaopen_uart_register},    /* UART 串口 */
    {NULL, NULL}
};

static void register_module(lua_State* L, const char* name, lua_CFunction func) {
    LOG_DEBUG("register module: %s", name);
    luaL_requiref(L, name, func, 1);
    lua_pop(L, 1);
}

void platform_modules_register(lua_State* L) {
    LOG_INFO("platform modules register start");
    const luaL_Reg *lib;
    int count = 0;
    for (lib = platform_modules; lib->func; lib++) {
        register_module(L, lib->name, lib->func);
        count++;
    }
    LOG_INFO("platform modules registered: count=%d", count);
}
