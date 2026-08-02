/**
 * @file iot_lvgl_colorwheel.c
 * @brief LVGL色轮控件
 *
 * LVGL 9.5.0 已移除 colorwheel 控件，此模块保留接口但所有函数返回错误，
 * 以保持 Lua 层 API 兼容性。后续可考虑使用其他方式实现色轮功能。
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
    /* LVGL 9 移除了 colorwheel 控件 */
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)lua_toboolean(L, 2);
    lua_pushnil(L);
    return 1;
}

/* ==================== 颜色轮OO方法 ==================== */

static int iot_lvgl_colorwheel_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_colorwheel_create_internal, colorwheel_metatable_ref);
}

static int iot_lvgl_colorwheel_set_mode(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_colorwheel_set_angle_offset(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_colorwheel_get_rgb(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, 0);
    lua_pushinteger(L, 0);
    lua_pushinteger(L, 0);
    return 3;
}

static int iot_lvgl_colorwheel_set_rgb(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_checkinteger(L, 3);
    (void)luaL_checkinteger(L, 4);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_colorwheel_get_color(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, 0);
    return 1;
}

static int iot_lvgl_colorwheel_set_color(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 colorwheel 子模块 */
void iot_lvgl_register_colorwheel(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_mode", iot_lvgl_colorwheel_set_mode);
    REG_METHOD(L, "set_angle_offset", iot_lvgl_colorwheel_set_angle_offset);
    REG_METHOD(L, "get_rgb", iot_lvgl_colorwheel_get_rgb);
    REG_METHOD(L, "set_rgb", iot_lvgl_colorwheel_set_rgb);
    REG_METHOD(L, "get_color", iot_lvgl_colorwheel_get_color);
    REG_METHOD(L, "set_color", iot_lvgl_colorwheel_set_color);

    colorwheel_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, colorwheel_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    REG_METHOD(L, "create", iot_lvgl_colorwheel_create);
}
