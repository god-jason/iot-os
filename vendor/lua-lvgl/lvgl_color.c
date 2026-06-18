/*
@module  lvgl.color
@summary LVGL颜色工具
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 创建颜色
local red = lvgl.color.make(255, 0, 0)
local blue = lvgl.color.hex(0x0000FF)

-- 颜色操作
local lighter = lvgl.color.lighten(red, 50)
local darker = lvgl.color.darken(red, 50)
local mixed = lvgl.color.mix(red, blue, 128)

-- 亮度调整
local brightened = lvgl.color.change_brightness(red, 20)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== 颜色操作 ==================== */

/*
通过RGB值创建颜色
@param r 红色分量(0-255)
@param g 绿色分量(0-255)
@param b 蓝色分量(0-255)
@return integer 颜色值
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
通过十六进制创建颜色
@param hex 十六进制颜色值(如0xFF0000)
@return integer 颜色值
@usage local color = lvgl.color.hex(0xFF0000)
*/
static int lvgl_color_hex(lua_State* L) {
    uint32_t hex = (uint32_t)luaL_checkinteger(L, 1);
    lv_color_t color = lv_color_hex(hex);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
通过3位十六进制创建颜色
@param hex 3位十六进制颜色值(如0xF00)
@return integer 颜色值
@usage local color = lvgl.color.hex3(0xF00)
*/
static int lvgl_color_hex3(lua_State* L) {
    uint32_t hex = (uint32_t)luaL_checkinteger(L, 1);
    lv_color_t color = lv_color_hex3(hex);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
调整颜色亮度
@param color 颜色值
@param bright 亮度调整值(-255到255)
@return integer 调整后的颜色值
@usage local brightened = lvgl.color.change_brightness(color, 30)
*/
static int lvgl_color_change_brightness(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    int8_t bright = (int8_t)luaL_checkinteger(L, 2);
    color = lv_color_change_brightness(color, bright);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
使颜色变亮
@param color 颜色值
@param level 变亮级别(0-255)
@return integer 变亮后的颜色值
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
使颜色变暗
@param color 颜色值
@param level 变暗级别(0-255)
@return integer 变暗后的颜色值
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
将颜色转换为1位颜色
@param color 颜色值
@return integer 转换后的颜色值
@usage local mono = lvgl.color.to_1(color)
*/
static int lvgl_color_to_1(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    color = lv_color_to_1(color);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
混合两种颜色
@param c1 颜色1值
@param c2 颜色2值
@param mix 混合比例(0-255,0=全部c1,255=全部c2)
@return integer 混合后的颜色值
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

/* 注册 color 子模块 */
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
