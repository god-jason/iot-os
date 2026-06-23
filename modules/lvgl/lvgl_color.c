/*
@module  lvgl.color
@summary LVGL????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??
local lvgl = require("lvgl")

-- ????
local red = lvgl.color.make(255, 0, 0)
local blue = lvgl.color.hex(0x0000FF)

-- ????
local lighter = lvgl.color.lighten(red, 50)
local darker = lvgl.color.darken(red, 50)
local mixed = lvgl.color.mix(red, blue, 128)

-- ????
local brightened = lvgl.color.change_brightness(red, 20)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== ???? ==================== */

/*
??RGB??????
@param r ????(0-255)
@param g ????(0-255)
@param b ????(0-255)
@return integer ????
@usage local color = lvgl.color.make(255, 0, 0)
*/
static int lvgl_color_make(lua_State* L) {
    uint8_t r = (uint8_t)luaL_checkinteger(L, 1);
    uint8_t g = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t b = (uint8_t)luaL_checkinteger(L, 3);
    lv_color_t color = lv_color_make(r, g, b);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????????
@param hex ??????????xFF0000)
@return integer ????
@usage local color = lvgl.color.hex(0xFF0000)
*/
static int lvgl_color_hex(lua_State* L) {
    uint32_t hex = (uint32_t)luaL_checkinteger(L, 1);
    lv_color_t color = lv_color_hex(hex);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??3??????????
@param hex 3???????????xF00)
@return integer ????
@usage local color = lvgl.color.hex3(0xF00)
*/
static int lvgl_color_hex3(lua_State* L) {
    uint32_t hex = (uint32_t)luaL_checkinteger(L, 1);
    lv_color_t color = lv_color_hex3(hex);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????
@param color ????
@param bright ??????-255??55)
@return integer ????????
@usage local brightened = lvgl.color.change_brightness(color, 30)
*/
static int lvgl_color_change_brightness(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    int bright = (int)luaL_checkinteger(L, 2);
    int lvl = 127 + bright;
    if (lvl < 0) lvl = 0;
    if (lvl > 255) lvl = 255;
    color = lv_color_change_lightness(color, (lv_opa_t)lvl);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????
@param color ????
@param level ????(0-255)
@return integer ????????
@usage local lighter = lvgl.color.lighten(color, 50)
*/
static int lvgl_color_lighten(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    uint8_t level = (uint8_t)luaL_checkinteger(L, 2);
    color = lv_color_lighten(color, level);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????
@param color ????
@param level ????(0-255)
@return integer ????????
@usage local darker = lvgl.color.darken(color, 50)
*/
static int lvgl_color_darken(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    uint8_t level = (uint8_t)luaL_checkinteger(L, 2);
    color = lv_color_darken(color, level);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????1????
@param color ????
@return integer ????????
@usage local mono = lvgl.color.to_1(color)
*/
static int lvgl_color_to_1(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    lua_pushinteger(L, lv_color_to1(color));
    return 1;
}

/*
??????
@param c1 ??1??
@param c2 ??2??
@param mix ????(0-255,0=??c1,255=??c2)
@return integer ????????
@usage local mixed = lvgl.color.mix(color1, color2, 128)
*/
static int lvgl_color_mix(lua_State* L) {
    lv_color_t c1;
    c1.full = (uint32_t)luaL_checkinteger(L, 1);
    lv_color_t c2;
    c2.full = (uint32_t)luaL_checkinteger(L, 2);
    uint8_t mix = (uint8_t)luaL_checkinteger(L, 3);
    lv_color_t result = lv_color_mix(c1, c2, mix);
    lua_pushinteger(L, result.full);
    return 1;
}

/* ?? color ????*/
void lvgl_register_color(lua_State* L) {
    REG_METHOD(L, "make", lvgl_color_make);
    REG_METHOD(L, "hex", lvgl_color_hex);
    REG_METHOD(L, "hex3", lvgl_color_hex3);
    REG_METHOD(L, "change_brightness", lvgl_color_change_brightness);
    REG_METHOD(L, "lighten", lvgl_color_lighten);
    REG_METHOD(L, "darken", lvgl_color_darken);
    REG_METHOD(L, "to_1", lvgl_color_to_1);
    REG_METHOD(L, "mix", lvgl_color_mix);
}
