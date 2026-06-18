/*
@module  spi
@summary SPI总线
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SPI
*/

#include "module.h"
#include "yopen_spi.h"

static int luaopen_spi_open(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_spi_write(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static int luaopen_spi_read(lua_State* L) {
    lua_pushstring(L, "");
    return 1;
}

static int luaopen_spi_close(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_spi_funcs[] = {
    {"open",  luaopen_spi_open},
    {"write", luaopen_spi_write},
    {"read",  luaopen_spi_read},
    {"close", luaopen_spi_close},
    {NULL, NULL}
};

int luaopen_spi(lua_State* L) {
    luaL_newlib(L, luaopen_spi_funcs);
    return 1;
}