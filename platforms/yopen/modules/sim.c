/*
@module  sim
@summary SIM卡管理
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SIM
*/

#include "module.h"
#include "yopen_sim.h"

static int luaopen_sim_info(lua_State* L) {
    lua_createtable(L, 0, 4);
    lua_pushstring(L, "");
    lua_setfield(L, -2, "iccid");
    lua_pushstring(L, "");
    lua_setfield(L, -2, "imsi");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "status");
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "ready");
    return 1;
}

static int luaopen_sim_ready(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

static const luaL_Reg luaopen_sim_funcs[] = {
    {"info",   luaopen_sim_info},
    {"ready",  luaopen_sim_ready},
    {NULL, NULL}
};

int luaopen_sim(lua_State* L) {
    luaL_newlib(L, luaopen_sim_funcs);
    return 1;
}