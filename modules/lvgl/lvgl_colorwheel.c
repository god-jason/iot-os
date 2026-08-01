/**
 * @file iot_lvgl_colorwheel.c
 * @brief LVGL色轮控件
 *
 * 实现LVGL色轮控件的OO风格Lua绑定，包括色轮创建、设置模式（色相/饱和度/亮度）、设置RGB值、获取RGB值、设置角度偏移等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* colorwheel组件的metatable引用 */
static int colorwheel_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_colorwheel_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    bool knob_recolor = lua_toboolean(L, 2);
    lv_obj_t* cw = lv_colorwheel_create(parent, knob_recolor);
    lua_pushlightuserdata(L, cw);
    return 1;
}

/* ==================== 颜色轮OO方法 ==================== */

static int iot_lvgl_colorwheel_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_colorwheel_create_internal, colorwheel_metatable_ref);
}

/*
设置颜色轮模式
@param self 颜色轮实例或指针
@param mode 颜色轮模式
@return self
@usage cw:set_mode(lvgl.COLORWHEEL_MODE_HUE)
*/
static int iot_lvgl_colorwheel_set_mode(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_colorwheel_set_angle_offset(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_colorwheel_set_angle_offset() was removed in LVGL 8 */
    (void)cw;
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取RGB颜色值
@param self 颜色轮实例或指针
@return r,g,b 红色、绿色、蓝色分量
@usage local r, g, b = cw:get_rgb()
*/
static int iot_lvgl_colorwheel_get_rgb(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_rgb(cw);
    uint32_t c32 = lv_color_to32(color);
    lua_pushinteger(L, (c32 >> 16) & 0xFF);
    lua_pushinteger(L, (c32 >> 8) & 0xFF);
    lua_pushinteger(L, c32 & 0xFF);
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
static int iot_lvgl_colorwheel_set_rgb(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
    uint8_t r = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t g = (uint8_t)luaL_checkinteger(L, 3);
    uint8_t b = (uint8_t)luaL_checkinteger(L, 4);
    lv_color_t color = lv_color_make(r, g, b);
    lv_colorwheel_set_rgb(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取颜色值(完整值)
@param self 颜色轮实例或指针
@return integer 颜色完整值
@usage local color = cw:get_color()
*/
static int iot_lvgl_colorwheel_get_color(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_rgb(cw);
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
static int iot_lvgl_colorwheel_set_color(lua_State* L) {
    lv_obj_t* cw = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t color_val = (uint32_t)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = color_val;
    lv_colorwheel_set_rgb(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 colorwheel 子模块 */
void iot_lvgl_register_colorwheel(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_mode", iot_lvgl_colorwheel_set_mode);
    REG_METHOD(L, "set_angle_offset", iot_lvgl_colorwheel_set_angle_offset);
    REG_METHOD(L, "get_rgb", iot_lvgl_colorwheel_get_rgb);
    REG_METHOD(L, "set_rgb", iot_lvgl_colorwheel_set_rgb);
    REG_METHOD(L, "get_color", iot_lvgl_colorwheel_get_color);
    REG_METHOD(L, "set_color", iot_lvgl_colorwheel_set_color);

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

    /* 注册create函数到主表lvgl.colorwheel) */
    REG_METHOD(L, "create", iot_lvgl_colorwheel_create);
}
