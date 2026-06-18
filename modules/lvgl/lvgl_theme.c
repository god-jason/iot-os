/*
@module  lvgl.theme
@summary LVGL主题系统
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 创建主题
local theme = lvgl.theme.create()

-- 设置主题颜色
theme:set_color(lvgl.THEME_COLOR_PRIMARY, 0x3366FF)
theme:set_color(lvgl.THEME_COLOR_SECONDARY, 0x6633FF)

-- 应用主题到对象
btn:set_theme(theme)

-- 使用内置主题
local theme_default = lvgl.theme.default()
lvgl.theme.set(theme_default)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== 主题操作 ==================== */

/*
创建主题
@return userdata 主题指针
@usage local theme = lvgl.theme.create()
*/
static int lvgl_theme_create(lua_State* L) {
    lv_theme_t* theme = lv_theme_create();
    lua_pushlightuserdata(L, theme);
    return 1;
}

/*
设置主题
@param theme 主题指针
@usage lvgl.theme.set(theme)
*/
static int lvgl_theme_set(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_set(theme);
    return 0;
}

/*
获取当前主题
@return userdata 当前主题指针
@usage local theme = lvgl.theme.get()
*/
static int lvgl_theme_get(lua_State* L) {
    lua_pushlightuserdata(L, lv_theme_get());
    return 1;
}

/*
获取默认主题
@return userdata 默认主题指针
@usage local theme = lvgl.theme.default()
*/
static int lvgl_theme_default(lua_State* L) {
    lv_theme_t* theme = lv_theme_default_init(lv_disp_get_default());
    lua_pushlightuserdata(L, theme);
    return 1;
}

/*
设置主题颜色
@param theme 主题指针
@param color_id 颜色ID
@param color 颜色值
@usage theme:set_color(lvgl.THEME_COLOR_PRIMARY, 0x3366FF)
*/
static int lvgl_theme_set_color(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_color_id_t color_id = (lv_theme_color_id_t)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);
    lv_theme_set_color(theme, color_id, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题颜色
@param theme 主题指针
@param color_id 颜色ID
@return integer 颜色值
@usage local color = theme:get_color(lvgl.THEME_COLOR_PRIMARY)
*/
static int lvgl_theme_get_color(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_color_id_t color_id = (lv_theme_color_id_t)luaL_checkinteger(L, 2);
    lv_color_t color = lv_theme_get_color(theme, color_id);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
设置主题字体
@param theme 主题指针
@param font_id 字体ID
@param font 字体指针
@usage theme:set_font(lvgl.THEME_FONT_SMALL, font)
*/
static int lvgl_theme_set_font(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_font_id_t font_id = (lv_theme_font_id_t)luaL_checkinteger(L, 2);
    lv_font_t* font = (lv_font_t*)luaL_checklightuserdata(L, 3);
    lv_theme_set_font(theme, font_id, font);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题字体
@param theme 主题指针
@param font_id 字体ID
@return userdata 字体指针
@usage local font = theme:get_font(lvgl.THEME_FONT_SMALL)
*/
static int lvgl_theme_get_font(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_font_id_t font_id = (lv_theme_font_id_t)luaL_checkinteger(L, 2);
    lua_pushlightuserdata(L, lv_theme_get_font(theme, font_id));
    return 1;
}

/*
设置主题尺寸
@param theme 主题指针
@param size_id 尺寸ID
@param size 尺寸值
@usage theme:set_size(lvgl.THEME_SIZE_BUTTON_HEIGHT, 40)
*/
static int lvgl_theme_set_size(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_size_id_t size_id = (lv_theme_size_id_t)luaL_checkinteger(L, 2);
    lv_coord_t size = (lv_coord_t)luaL_checkinteger(L, 3);
    lv_theme_set_size(theme, size_id, size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题尺寸
@param theme 主题指针
@param size_id 尺寸ID
@return integer 尺寸值
@usage local size = theme:get_size(lvgl.THEME_SIZE_BUTTON_HEIGHT)
*/
static int lvgl_theme_get_size(lua_State* L) {
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_theme_size_id_t size_id = (lv_theme_size_id_t)luaL_checkinteger(L, 2);
    lua_pushinteger(L, lv_theme_get_size(theme, size_id));
    return 1;
}

/* 注册 theme 子模块 */
int lvgl_register_theme(lua_State* L) {
    REG_METHOD(L, "create", lvgl_theme_create);
    REG_METHOD(L, "set", lvgl_theme_set);
    REG_METHOD(L, "get", lvgl_theme_get);
    REG_METHOD(L, "default", lvgl_theme_default);
    REG_METHOD(L, "set_color", lvgl_theme_set_color);
    REG_METHOD(L, "get_color", lvgl_theme_get_color);
    REG_METHOD(L, "set_font", lvgl_theme_set_font);
    REG_METHOD(L, "get_font", lvgl_theme_get_font);
    REG_METHOD(L, "set_size", lvgl_theme_set_size);
    REG_METHOD(L, "get_size", lvgl_theme_get_size);
    return 0;
}