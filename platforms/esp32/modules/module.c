/*
@module  platform_modules
@summary ESP32平台模块注册入口
@version 1.0
@date    2026.06.20
*/

extern int luaopen_log(lua_State* L);
extern int luaopen_socket(lua_State* L);
extern int luaopen_http(lua_State* L);
extern int luaopen_uart(lua_State* L);
extern int luaopen_wifi(lua_State* L);
extern int luaopen_bt(lua_State* L);

static const luaL_Reg platform_modules[] = {
    {"log",    luaopen_log},
    {"socket", luaopen_socket},
    {"http",   luaopen_http},
    {"uart",   luaopen_uart},
    {"wifi",   luaopen_wifi},
    {"bt",     luaopen_bt},
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
