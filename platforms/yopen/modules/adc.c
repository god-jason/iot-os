/*
@module  adc
@summary ADC模数转换
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     ADC
*/

#include "module.h"
#include "yopen_adc.h"

static int luaopen_adc_open(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_adc_read(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static int luaopen_adc_close(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_adc_funcs[] = {
    {"open",  luaopen_adc_open},
    {"read",  luaopen_adc_read},
    {"close", luaopen_adc_close},
    {NULL, NULL}
};

int luaopen_adc(lua_State* L) {
    luaL_newlib(L, luaopen_adc_funcs);
    return 1;
}