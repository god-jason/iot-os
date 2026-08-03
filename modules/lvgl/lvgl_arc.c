/**
 * @file iot_lvgl_arc.c
 * @brief LVGL弧形控件
 *
 * 实现LVGL弧形控件的OO风格Lua绑定，包括弧形创建、设置起止角度范围、设置/获取当前值、设置背景角度、设置动画使能等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* arc组件的metatable引用 */
static int arc_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_arc_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_arc_create_internal, arc_metatable_ref);
}

/*
设置弧形值
@param self 弧形实例或指针
@param value 值
@param anim 动画使能(可选,默认0=无动画)
@return self
@usage arc:set_value(75)
*/
static int iot_lvgl_arc_set_value(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    (void)anim;
    lv_arc_set_value(arc, (int16_t)value);
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
static int iot_lvgl_arc_set_range(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_set_bg_angles(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_set_angles(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_get_value(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_get_angle_start(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_arc_get_angle_end(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    uint16_t angle = lv_arc_get_angle_end(arc);
    lua_pushinteger(L, angle);
    return 1;
}

/*
设置弧的旋转角度
@param self 弧实例或指针
@param rotation 旋转角度
@return self
@usage arc:set_rotation(90)
*/
static int iot_lvgl_arc_set_rotation(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    int32_t rotation = (int32_t)luaL_checkinteger(L, 2);
    lv_arc_set_rotation(arc, rotation);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧的模式
@param self 弧实例或指针
@param mode 模式: ARC_MODE_NORMAL(0), ARC_MODE_SYMMETRICAL(1), ARC_MODE_REVERSE(2)
@return self
@usage arc:set_mode(lvgl.ARC_MODE_NORMAL)
*/
static int iot_lvgl_arc_set_mode(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    lv_arc_mode_t mode = (lv_arc_mode_t)luaL_checkinteger(L, 2);
    lv_arc_set_mode(arc, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧的变化率
@param self 弧实例或指针
@param rate 变化率
@return self
@usage arc:set_change_rate(10)
*/
static int iot_lvgl_arc_set_change_rate(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t rate = (uint32_t)luaL_checkinteger(L, 2);
    lv_arc_set_change_rate(arc, rate);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧的旋钮偏移
@param self 弧实例或指针
@param offset 偏移角度
@return self
@usage arc:set_knob_offset(30)
*/
static int iot_lvgl_arc_set_knob_offset(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_arc_set_knob_offset(arc, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取背景起始角度
@param self 弧实例或指针
@return integer 背景起始角度
@usage local angle = arc:get_bg_angle_start()
*/
static int iot_lvgl_arc_get_bg_angle_start(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = lv_arc_get_bg_angle_start(arc);
    lua_pushinteger(L, (int32_t)angle);
    return 1;
}

/*
获取背景结束角度
@param self 弧实例或指针
@return integer 背景结束角度
@usage local angle = arc:get_bg_angle_end()
*/
static int iot_lvgl_arc_get_bg_angle_end(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = lv_arc_get_bg_angle_end(arc);
    lua_pushinteger(L, (int32_t)angle);
    return 1;
}

/*
获取弧的旋转角度
@param self 弧实例或指针
@return integer 旋转角度
@usage local rotation = arc:get_rotation()
*/
static int iot_lvgl_arc_get_rotation(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    int32_t rotation = lv_arc_get_rotation(arc);
    lua_pushinteger(L, rotation);
    return 1;
}

/*
获取弧的模式
@param self 弧实例或指针
@return integer 模式值
@usage local mode = arc:get_mode()
*/
static int iot_lvgl_arc_get_mode(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    lv_arc_mode_t mode = lv_arc_get_mode(arc);
    lua_pushinteger(L, mode);
    return 1;
}

/*
获取弧的旋钮偏移
@param self 弧实例或指针
@return integer 旋钮偏移角度
@usage local offset = arc:get_knob_offset()
*/
static int iot_lvgl_arc_get_knob_offset(lua_State* L) {
    lv_obj_t* arc = iot_lvgl_get_obj_ptr(L, 1);
    int32_t offset = lv_arc_get_knob_offset(arc);
    lua_pushinteger(L, offset);
    return 1;
}

/* 注册 arc 子模块 */
void iot_lvgl_register_arc(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_value", iot_lvgl_arc_set_value);
    REG_METHOD(L, "get_value", iot_lvgl_arc_get_value);
    REG_METHOD(L, "set_range", iot_lvgl_arc_set_range);
    REG_METHOD(L, "set_bg_angles", iot_lvgl_arc_set_bg_angles);
    REG_METHOD(L, "set_angles", iot_lvgl_arc_set_angles);
    REG_METHOD(L, "set_rotation", iot_lvgl_arc_set_rotation);
    REG_METHOD(L, "set_mode", iot_lvgl_arc_set_mode);
    REG_METHOD(L, "set_change_rate", iot_lvgl_arc_set_change_rate);
    REG_METHOD(L, "set_knob_offset", iot_lvgl_arc_set_knob_offset);
    REG_METHOD(L, "get_angle_start", iot_lvgl_arc_get_angle_start);
    REG_METHOD(L, "get_angle_end", iot_lvgl_arc_get_angle_end);
    REG_METHOD(L, "get_bg_angle_start", iot_lvgl_arc_get_bg_angle_start);
    REG_METHOD(L, "get_bg_angle_end", iot_lvgl_arc_get_bg_angle_end);
    REG_METHOD(L, "get_rotation", iot_lvgl_arc_get_rotation);
    REG_METHOD(L, "get_mode", iot_lvgl_arc_get_mode);
    REG_METHOD(L, "get_knob_offset", iot_lvgl_arc_get_knob_offset);

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
    REG_METHOD(L, "create", iot_lvgl_arc_create);
}
