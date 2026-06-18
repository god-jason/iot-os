/*
@module  lvgl.bar
@summary LVGL进度条控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建进度条
local bar = lvgl.bar.create(scr)
bar:set_size(200, 20)
bar:set_pos(50, 50)

-- 设置范围和值
bar:set_range(0, 100)
bar:set_value(50, 0)  -- 无动画

-- 设置模式
bar:set_mode(lvgl.BAR_MODE_NORMAL)

-- 获取值
local value = bar:get_value()
local min = bar:get_min_value()
local max = bar:get_max_value()

-- 链式调用
local bar2 = lvgl.bar.create(scr):set_size(200, 20):set_pos(50, 100):set_range(0, 100):set_value(75)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* bar组件的metatable引用 */
static int bar_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_bar_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* bar = lv_bar_create(parent);
    lua_pushlightuserdata(L, bar);
    return 1;
}

/* ==================== 进度条OO方法 ==================== */

/*
创建进度条控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的进度条实例
@usage local bar = lvgl.bar.create(scr)
*/
static int lvgl_bar_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_bar_create_internal, bar_metatable_ref);
}

/*
设置进度条值
@param self 进度条实例或指针
@param value 值
@param anim 动画使能(可选,默认0=无动画)
@return self
@usage bar:set_value(50, 0)
*/
static int lvgl_bar_set_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_bar_set_value(bar, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置进度条范围
@param self 进度条实例或指针
@param min 最小值
@param max 最大值
@return self
@usage bar:set_range(0, 100)
*/
static int lvgl_bar_set_range(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_bar_set_range(bar, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置进度条模式
@param self 进度条实例或指针
@param mode 模式: BAR_MODE_NORMAL, BAR_MODE_SYMMETRICAL, BAR_MODE_REVERSE
@return self
@usage bar:set_mode(lvgl.BAR_MODE_NORMAL)
*/
static int lvgl_bar_set_mode(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    lv_bar_mode_t mode = (lv_bar_mode_t)luaL_checkinteger(L, 2);
    lv_bar_set_mode(bar, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置进度条起始值
@param self 进度条实例或指针
@param value 起始值
@param anim 动画使能(可选,默认0=无动画)
@return self
@usage bar:set_start_value(20, 0)
*/
static int lvgl_bar_set_start_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取进度条值
@param self 进度条实例或指针
@return integer 当前值
@usage local value = bar:get_value()
*/
static int lvgl_bar_get_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
获取进度条起始值
@param self 进度条实例或指针
@return integer 起始值
@usage local value = bar:get_start_value()
*/
static int lvgl_bar_get_start_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_start_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
获取进度条最小值
@param self 进度条实例或指针
@return integer 最小值
@usage local min = bar:get_min_value()
*/
static int lvgl_bar_get_min_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_min_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
获取进度条最大值
@param self 进度条实例或指针
@return integer 最大值
@usage local max = bar:get_max_value()
*/
static int lvgl_bar_get_max_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_max_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/* 注册 bar 子模块 */
void lvgl_register_bar(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_value", lvgl_bar_set_value);
    REG_METHOD(L, "get_value", lvgl_bar_get_value);
    REG_METHOD(L, "set_range", lvgl_bar_set_range);
    REG_METHOD(L, "set_mode", lvgl_bar_set_mode);
    REG_METHOD(L, "set_start_value", lvgl_bar_set_start_value);
    REG_METHOD(L, "get_start_value", lvgl_bar_get_start_value);
    REG_METHOD(L, "get_min_value", lvgl_bar_get_min_value);
    REG_METHOD(L, "get_max_value", lvgl_bar_get_max_value);

    bar_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, bar_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.bar) */
    REG_METHOD(L, "create", lvgl_bar_create);
}
