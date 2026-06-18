/*
@module  sms
@summary 短信功能
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SMS
*/

#include "module.h"
#include "yopen_sms.h"

static int luaopen_sms_send(lua_State* L) {
    const char* number = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lua_pushboolean(L, 0);
    return 1;
}

static int luaopen_sms_recv(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static const luaL_Reg luaopen_sms_funcs[] = {
    {"send",  luaopen_sms_send},
    {"recv",  luaopen_sms_recv},
    {NULL, NULL}
};

int luaopen_sms(lua_State* L) {
    luaL_newlib(L, luaopen_sms_funcs);
    return 1;
}