/*
@module  platform_modules
@summary WebAssembly平台模块注册入口
@version 1.0
@date    2026.06.27
*/

extern int luaopen_websocket_register(lua_State* L);

static const luaL_Reg platform_modules[] = {
    {"websocket", luaopen_websocket_register},
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