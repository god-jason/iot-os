/*
@module  lvgl.spinbox
@summary LVGL数值框控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建数值框
local spinbox = lvgl.spinbox.create(scr)
spinbox:set_size(100, 40)
spinbox:set_pos(110, 50)

-- 设置范围
spinbox:set_range(0, 100)

-- 设置当前值
spinbox:set_value(50)

-- 设置步进值
spinbox:set_step(1)

-- 设置小数位数
spinbox:set_digit_format(2, 0)  -- 2位小数,0位指数

-- 获取当前值
local value = spinbox:get_value()

-- 增量/减量
spinbox:increment()
spinbox:decrement()

-- 链式调用
local sb = lvgl.spinbox.create(scr):set_size(120, 50):set_pos(50, 100):set_range(0, 255):set_value(128)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* spinbox组件的metatable引用 */
static int spinbox_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_spinbox_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* spinbox = lv_spinbox_create(parent);
    lua_pushlightuserdata(L, spinbox);
    return 1;
}

/* ==================== 数值框OO方法 ==================== */

static int lvgl_spinbox_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_spinbox_create_internal, spinbox_metatable_ref);
}

static int lvgl_spinbox_set_value(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_spinbox_set_value(spinbox, value);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_get_value(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_spinbox_get_value(spinbox);
    lua_pushinteger(L, value);
    return 1;
}

static int lvgl_spinbox_set_range(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_spinbox_set_range(spinbox, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_step(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    uint32_t step = (uint32_t)luaL_checkinteger(L, 2);
    lv_spinbox_set_step(spinbox, step);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_digit_format(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    uint8_t digit_count = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t separator_position = (uint8_t)luaL_checkinteger(L, 3);
    lv_spinbox_set_digit_format(spinbox, digit_count, separator_position);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_scroll_speed(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    uint32_t scroll_count = (uint32_t)luaL_checkinteger(L, 2);
    lv_spinbox_set_scroll_speed(spinbox, scroll_count);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_increment(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    lv_spinbox_increment(spinbox);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_decrement(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    lv_spinbox_decrement(spinbox);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 spinbox 子模块 */
void lvgl_register_spinbox(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_value", lvgl_spinbox_set_value);
    REG_METHOD(L, "get_value", lvgl_spinbox_get_value);
    REG_METHOD(L, "set_range", lvgl_spinbox_set_range);
    REG_METHOD(L, "set_step", lvgl_spinbox_set_step);
    REG_METHOD(L, "set_digit_format", lvgl_spinbox_set_digit_format);
    REG_METHOD(L, "set_scroll_speed", lvgl_spinbox_set_scroll_speed);
    REG_METHOD(L, "increment", lvgl_spinbox_increment);
    REG_METHOD(L, "decrement", lvgl_spinbox_decrement);

    /* 保存组件metatable引用(用于继承) */
    spinbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.spinbox.set_value(sb, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, spinbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.spinbox) */
    REG_METHOD(L, "create", lvgl_spinbox_create);
}
