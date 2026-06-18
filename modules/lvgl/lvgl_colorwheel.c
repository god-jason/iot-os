/*
@module  lvgl.colorwheel
@summary LVGL颜色轮控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建颜色轮
local cw = lvgl.colorwheel.create(scr)
cw:set_size(200, 200)
cw:set_pos(60, 20)

-- 设置为圆形颜色轮
cw:set_mode(lvgl.COLORWHEEL_MODE_SATURATION)

-- 获取当前颜色
local r, g, b = cw:get_rgb()
print(string.format("当前颜色: RGB(%d, %d, %d)", r, g, b))

-- 设置颜色
cw:set_rgb(255, 0, 0)  -- 设置为红色

-- 链式调用
local c = lvgl.colorwheel.create(scr):set_size(150, 150):set_pos(85, 45):set_mode(lvgl.COLORWHEEL_MODE_HUE)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* colorwheel组件的metatable引用 */
static int colorwheel_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_colorwheel_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cw = lv_colorwheel_create(parent);
    lua_pushlightuserdata(L, cw);
    return 1;
}

/* ==================== 颜色轮OO方法 ==================== */

static int lvgl_colorwheel_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_colorwheel_create_internal, colorwheel_metatable_ref);
}

/*
设置颜色轮模式
@param self 颜色轮实例或指针
@param mode 颜色轮模式
@return self
@usage cw:set_mode(lvgl.COLORWHEEL_MODE_HUE)
*/
static int lvgl_colorwheel_set_mode(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_colorwheel_mode_t mode = (lv_colorwheel_mode_t)luaL_checkinteger(L, 2);
    lv_colorwheel_set_mode(cw, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置角度偏移
@param self 颜色轮实例或指针
@param angle 角度偏移值
@return self
@usage cw:set_angle_offset(45)
*/
static int lvgl_colorwheel_set_angle_offset(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = (uint16_t)luaL_checkinteger(L, 2);
    lv_colorwheel_set_angle_offset(cw, angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取RGB颜色值
@param self 颜色轮实例或指针
@return r,g,b 红色、绿色、蓝色分量
@usage local r, g, b = cw:get_rgb()
*/
static int lvgl_colorwheel_get_rgb(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_color(cw);
    lua_pushinteger(L, color.ch.red);
    lua_pushinteger(L, color.ch.green);
    lua_pushinteger(L, color.ch.blue);
    return 3;
}

/*
设置RGB颜色值
@param self 颜色轮实例或指针
@param r 红色分量(0-255)
@param g 绿色分量(0-255)
@param b 蓝色分量(0-255)
@return self
@usage cw:set_rgb(255, 0, 0)
*/
static int lvgl_colorwheel_set_rgb(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    uint8_t r = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t g = (uint8_t)luaL_checkinteger(L, 3);
    uint8_t b = (uint8_t)luaL_checkinteger(L, 4);
    lv_color_t color = lv_color_make(r, g, b);
    lv_colorwheel_set_color(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取颜色值(完整值)
@param self 颜色轮实例或指针
@return integer 颜色完整值
@usage local color = cw:get_color()
*/
static int lvgl_colorwheel_get_color(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_color(cw);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
设置颜色值(完整值)
@param self 颜色轮实例或指针
@param color_val 颜色完整值
@return self
@usage cw:set_color(0xFF0000)
*/
static int lvgl_colorwheel_set_color(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    uint32_t color_val = (uint32_t)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = color_val;
    lv_colorwheel_set_color(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 colorwheel 子模块 */
void lvgl_register_colorwheel(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_mode", lvgl_colorwheel_set_mode);
    REG_METHOD(L, "set_angle_offset", lvgl_colorwheel_set_angle_offset);
    REG_METHOD(L, "get_rgb", lvgl_colorwheel_get_rgb);
    REG_METHOD(L, "set_rgb", lvgl_colorwheel_set_rgb);
    REG_METHOD(L, "get_color", lvgl_colorwheel_get_color);
    REG_METHOD(L, "set_color", lvgl_colorwheel_set_color);

    /* 保存组件metatable引用(用于继承) */
    colorwheel_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.colorwheel.set_mode(cw, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, colorwheel_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.colorwheel) */
    REG_METHOD(L, "create", lvgl_colorwheel_create);
}
