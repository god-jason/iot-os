/*
@module  lvgl.arc
@summary LVGL弧形控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建弧形控件
local arc = lvgl.arc.create(scr)
arc:set_size(150, 150)
arc:set_pos(75, 75)

-- 设置角度范围
arc:set_bg_angles(0, 270)
arc:set_angles(0, 135)

-- 设置值
arc:set_value(75)

-- 获取值
local value = arc:get_value()

-- 链式调用
local arc2 = lvgl.arc.create(scr):set_size(100, 100):set_pos(200, 100):set_bg_angles(0, 180):set_value(50)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* arc组件的metatable引用 */
static int arc_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_arc_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* arc = lv_arc_create(parent);
    lua_pushlightuserdata(L, arc);
    return 1;
}

/* ==================== 弧形OO方法 ==================== */

/*
创建弧形控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的弧形实例
@usage local arc = lvgl.arc.create(scr)
*/
static int lvgl_arc_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_arc_create_internal, arc_metatable_ref);
}

/*
设置弧形值
@param self 弧形实例或指针
@param value 值
@param anim 动画使能(可选,默认0=无动画)
@return self
@usage arc:set_value(75)
*/
static int lvgl_arc_set_value(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_arc_set_value(arc, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形范围
@param self 弧形实例或指针
@param min 最小值
@param max 最大值
@return self
@usage arc:set_range(0, 100)
*/
static int lvgl_arc_set_range(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_arc_set_range(arc, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置背景角度范围
@param self 弧形实例或指针
@param start 起始角度(0-360)
@param end_angle 结束角度(0-360)
@return self
@usage arc:set_bg_angles(0, 270)
*/
static int lvgl_arc_set_bg_angles(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t end_angle = (uint16_t)luaL_checkinteger(L, 3);
    lv_arc_set_bg_angles(arc, start, end_angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置前景角度范围
@param self 弧形实例或指针
@param start 起始角度(0-360)
@param end_angle 结束角度(0-360)
@return self
@usage arc:set_angles(0, 135)
*/
static int lvgl_arc_set_angles(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t end_angle = (uint16_t)luaL_checkinteger(L, 3);
    lv_arc_set_angles(arc, start, end_angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形值
@param self 弧形实例或指针
@return integer 当前值
@usage local value = arc:get_value()
*/
static int lvgl_arc_get_value(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_arc_get_value(arc);
    lua_pushinteger(L, value);
    return 1;
}

/*
获取起始角度
@param self 弧形实例或指针
@return integer 起始角度
@usage local angle = arc:get_angle_start()
*/
static int lvgl_arc_get_angle_start(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = lv_arc_get_angle_start(arc);
    lua_pushinteger(L, angle);
    return 1;
}

/*
获取结束角度
@param self 弧形实例或指针
@return integer 结束角度
@usage local angle = arc:get_angle_end()
*/
static int lvgl_arc_get_angle_end(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = lv_arc_get_angle_end(arc);
    lua_pushinteger(L, angle);
    return 1;
}

/* 注册 arc 子模块 */
void lvgl_register_arc(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_value", lvgl_arc_set_value);
    REG_METHOD(L, "get_value", lvgl_arc_get_value);
    REG_METHOD(L, "set_range", lvgl_arc_set_range);
    REG_METHOD(L, "set_bg_angles", lvgl_arc_set_bg_angles);
    REG_METHOD(L, "set_angles", lvgl_arc_set_angles);
    REG_METHOD(L, "get_angle_start", lvgl_arc_get_angle_start);
    REG_METHOD(L, "get_angle_end", lvgl_arc_get_angle_end);

    arc_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, arc_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.arc) */
    REG_METHOD(L, "create", lvgl_arc_create);
}
  