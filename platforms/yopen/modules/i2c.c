/*
@module  i2c
@summary I2C总线
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     I2C
*/

#include "module.h"
#include "yopen_i2c.h"

static int luaopen_i2c_open(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_i2c_write(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static int luaopen_i2c_read(lua_State* L) {
    lua_pushstring(L, "");
    return 1;
}

static int luaopen_i2c_close(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_i2c_funcs[] = {
    {"open",  luaopen_i2c_open},
    {"write", luaopen_i2c_write},
    {"read",  luaopen_i2c_read},
    {"close", luaopen_i2c_close},
    {NULL, NULL}
};

int luaopen_i2c(lua_State* L) {
    luaL_newlib(L, luaopen_i2c_funcs);
    return 1;
}