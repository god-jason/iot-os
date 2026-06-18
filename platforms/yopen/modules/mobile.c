/*
@module  mobile
@summary 移动网络
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     MOBILE
*/

#include "module.h"
#include "yopen_nw.h"

static int luaopen_mobile_connect(lua_State* L) {
    const char* apn = luaL_optstring(L, 1, "cmnet");
    lua_pushboolean(L, 0);
    return 1;
}

static int luaopen_mobile_disconnect(lua_State* L) {
    return 0;
}

static int luaopen_mobile_status(lua_State* L) {
    lua_createtable(L, 0, 4);
    lua_pushstring(L, "disconnected");
    lua_setfield(L, -2, "status");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "csq");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "rssi");
    lua_pushstring(L, "");
    lua_setfield(L, -2, "operator");
    return 1;
}

static int luaopen_mobile_signal(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static const luaL_Reg luaopen_mobile_funcs[] = {
    {"connect",    luaopen_mobile_connect},
    {"disconnect", luaopen_mobile_disconnect},
    {"status",     luaopen_mobile_status},
    {"signal",     luaopen_mobile_signal},
    {NULL, NULL}
};

int luaopen_mobile(lua_State* L) {
    luaL_newlib(L, luaopen_mobile_funcs);
    return 1;
}