/*
@module  lvgl.meter
@summary LVGL仪表盘控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建仪表盘
local meter = lvgl.meter.create(scr)
meter:set_size(200, 200)
meter:set_pos(60, 20)

-- 添加刻度
local scale = meter:add_scale(270, 0)  -- 270度范围

-- 设置刻度范围
meter:set_scale_range(scale, 0, 100, 270, 225)

-- 设置刻度线
meter:set_scale_ticks(scale, 11, 2, 5, 0x000000)

-- 设置主刻度线
meter:set_scale_major_ticks(scale, 4, 2, 5, 0xFF0000)

-- 添加指针指示器(线条型)
local indic = meter:add_indicator_line(scale, 3, 0x0000FF, 10)

-- 设置指针值
meter:set_indicator_value(indic, 50)

-- 添加圆弧指示器
local arc_indic = meter:add_indicator_arc(scale, 10, 0x00FF00, 5)

-- 设置圆弧值
meter:set_indicator_value(arc_indic, 75)

-- 链式调用
local m = lvgl.meter.create(scr):set_size(150, 150):set_pos(80, 30)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* meter组件的metatable引用 */
static int meter_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_meter_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* meter = lv_meter_create(parent);
    lua_pushlightuserdata(L, meter);
    return 1;
}

/* ==================== 仪表盘OO方法 ==================== */

/*
创建仪表盘控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的仪表盘实例
@usage local meter = lvgl.meter.create(scr)
*/
static int lvgl_meter_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_meter_create_internal, meter_metatable_ref);
}

/*
添加刻度
@param self 仪表盘实例或指针
@param angle_range 角度范围
@param rotation 旋转角度(可选,默认0)
@return userdata 刻度指针
@usage local scale = meter:add_scale(270, 0)
*/
static int lvgl_meter_add_scale(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    int32_t angle_range = (int32_t)luaL_checkinteger(L, 2);
    int32_t rotation = (int32_t)luaL_optinteger(L, 3, 0);
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, 0, 100, angle_range, rotation);
    lua_pushlightuserdata(L, scale);
    return 1;
}

/*
设置刻度范围
@param self 仪表盘实例或指针
@param scale 刻度指针
@param min 最小值
@param max 最大值
@param angle_range 角度范围
@param rotation 旋转角度(可选)
@return self
@usage meter:set_scale_range(scale, 0, 100, 270, 225)
*/
static int lvgl_meter_set_scale_range(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    int32_t min = (int32_t)luaL_checkinteger(L, 3);
    int32_t max = (int32_t)luaL_checkinteger(L, 4);
    int32_t angle_range = (int32_t)luaL_checkinteger(L, 5);
    int32_t rotation = (int32_t)luaL_optinteger(L, 6, 0);
    lv_meter_set_scale_range(meter, scale, min, max, angle_range, rotation);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置刻度线
@param self 仪表盘实例或指针
@param scale 刻度指针
@param cnt 刻度线数量
@param len 刻度线长度
@param width 刻度线宽度(可选,默认2)
@param color 刻度线颜色(可选,默认黑色)
@return self
@usage meter:set_scale_ticks(scale, 11, 2, 5, 0x000000)
*/
static int lvgl_meter_set_scale_ticks(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t cnt = (uint16_t)luaL_checkinteger(L, 3);
    uint16_t len = (uint16_t)luaL_checkinteger(L, 4);
    uint16_t width = (uint16_t)luaL_optinteger(L, 5, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_optinteger(L, 6, 0x000000);
    lv_meter_set_scale_ticks(meter, scale, cnt, len, width, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置主刻度线
@param self 仪表盘实例或指针
@param scale 刻度指针
@param nth 每N个刻度为主刻度
@param len 主刻度长度
@param width 主刻度宽度(可选)
@param color 主刻度颜色(可选)
@return self
@usage meter:set_scale_major_ticks(scale, 4, 2, 5, 0xFF0000)
*/
static int lvgl_meter_set_scale_major_ticks(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t nth = (uint16_t)luaL_checkinteger(L, 3);
    uint16_t len = (uint16_t)luaL_checkinteger(L, 4);
    uint16_t width = (uint16_t)luaL_optinteger(L, 5, 2);
    int32_t color = (int32_t)luaL_optinteger(L, 6, 0x000000);
    lv_meter_set_scale_major_ticks(meter, scale, nth, len, width, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加线条指示器
@param self 仪表盘实例或指针
@param scale 刻度指针
@param width 线条宽度
@param color 线条颜色
@param length 线条长度(可选)
@return userdata 指示器指针
@usage local indic = meter:add_indicator_line(scale, 3, 0x0000FF, 10)
*/
static int lvgl_meter_add_indicator_line(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 3);
    int32_t color = (int32_t)luaL_checkinteger(L, 4);
    int32_t length = (int32_t)luaL_optinteger(L, 5, 0);
    lv_meter_indicator_t* indic = lv_meter_add_indicator_line(meter, scale, width, color, length);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
添加圆弧指示器
@param self 仪表盘实例或指针
@param scale 刻度指针
@param width 圆弧宽度
@param color 圆弧颜色
@param length 圆弧长度(可选)
@return userdata 指示器指针
@usage local arc_indic = meter:add_indicator_arc(scale, 10, 0x00FF00, 5)
*/
static int lvgl_meter_add_indicator_arc(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 3);
    int32_t color = (int32_t)luaL_checkinteger(L, 4);
    int32_t length = (int32_t)luaL_optinteger(L, 5, 0);
    lv_meter_indicator_t* indic = lv_meter_add_indicator_arc(meter, scale, width, color, length);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
添加指针指示器
@param self 仪表盘实例或指针
@param scale 刻度指针
@param color 指针颜色
@param width 指针宽度(可选,默认4)
@return userdata 指示器指针
@usage local needle = meter:add_indicator_needle(scale, 0xFF0000, 4)
*/
static int lvgl_meter_add_indicator_needle(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    int32_t color = (int32_t)luaL_checkinteger(L, 3);
    int32_t width = (int32_t)luaL_optinteger(L, 4, 4);
    lv_meter_indicator_t* indic = lv_meter_add_indicator_needle(meter, scale, width, color);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
设置指示器值
@param self 仪表盘实例或指针
@param indic 指示器指针
@param value 值
@return self
@usage meter:set_indicator_value(indic, 50)
*/
static int lvgl_meter_set_indicator_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置指示器起始值
@param self 仪表盘实例或指针
@param indic 指示器指针
@param value 起始值
@return self
@usage meter:set_indicator_start_value(indic, 0)
*/
static int lvgl_meter_set_indicator_start_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_start_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置指示器结束值
@param self 仪表盘实例或指针
@param indic 指示器指针
@param value 结束值
@return self
@usage meter:set_indicator_end_value(indic, 100)
*/
static int lvgl_meter_set_indicator_end_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_end_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 meter 子模块 */
void lvgl_register_meter(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "add_scale", lvgl_meter_add_scale);
    REG_METHOD(L, "set_scale_range", lvgl_meter_set_scale_range);
    REG_METHOD(L, "set_scale_ticks", lvgl_meter_set_scale_ticks);
    REG_METHOD(L, "set_scale_major_ticks", lvgl_meter_set_scale_major_ticks);
    REG_METHOD(L, "add_indicator_line", lvgl_meter_add_indicator_line);
    REG_METHOD(L, "add_indicator_arc", lvgl_meter_add_indicator_arc);
    REG_METHOD(L, "add_indicator_needle", lvgl_meter_add_indicator_needle);
    REG_METHOD(L, "set_indicator_value", lvgl_meter_set_indicator_value);
    REG_METHOD(L, "set_indicator_start_value", lvgl_meter_set_indicator_start_value);
    REG_METHOD(L, "set_indicator_end_value", lvgl_meter_set_indicator_end_value);

    /* 保存组件metatable引用(用于继承) */
    meter_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.meter.add_scale(meter, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, meter_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.meter) */
    REG_METHOD(L, "create", lvgl_meter_create);
}
