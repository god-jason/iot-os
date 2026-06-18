/*
@module  rtc
@summary 实时时钟
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     RTC
*/

#include "module.h"
#include "yopen_rtc.h"

static int luaopen_rtc_time(lua_State* L) {
    lua_createtable(L, 0, 6);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "year");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "month");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "day");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "hour");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "minute");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "second");
    return 1;
}

static int luaopen_rtc_set(lua_State* L) {
    return 0;
}

static int luaopen_rtc_alarm(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_rtc_funcs[] = {
    {"time",  luaopen_rtc_time},
    {"set",   luaopen_rtc_set},
    {"alarm", luaopen_rtc_alarm},
    {NULL, NULL}
};

int luaopen_rtc(lua_State* L) {
    luaL_newlib(L, luaopen_rtc_funcs);
    return 1;
}