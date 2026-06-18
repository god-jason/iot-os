/*
@module  lcd
@summary LCD显示
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     LCD
*/

#include "module.h"
#include "yopen_lcd.h"

static int luaopen_lcd_open(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_lcd_clear(lua_State* L) {
    return 0;
}

static int luaopen_lcd_draw(lua_State* L) {
    return 0;
}

static int luaopen_lcd_close(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_lcd_funcs[] = {
    {"open",  luaopen_lcd_open},
    {"clear", luaopen_lcd_clear},
    {"draw",  luaopen_lcd_draw},
    {"close", luaopen_lcd_close},
    {NULL, NULL}
};

int luaopen_lcd(lua_State* L) {
    luaL_newlib(L, luaopen_lcd_funcs);
    return 1;
}