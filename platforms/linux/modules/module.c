/*
@module  platform_modules
@summary Linux平台模块注册入口
@version 1.0
@date    2026.06.19
*/

extern int luaopen_socket_register(lua_State* L);
extern int luaopen_http_register(lua_State* L);
extern int luaopen_uart_register(lua_State* L);

static const luaL_Reg platform_modules[] = {
    {"socket", luaopen_socket_register},
    {"http",   luaopen_http_register},
    {"uart",   luaopen_uart_register},
    {NULL, NULL}
};

static void register_module(lua_State* L, const char* name, lua_CFunction func) {
    luaL_requiref(L, name, func, 1);
    lua_pop(L, 1);
}

void platform_modules_register(lua_State* L) {
    const luaL_Reg *lib;
    for (lib = platform_modules; lib->func; lib++) {
        register_module(L, lib->name, lib->func);
    }
}
