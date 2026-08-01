/**
 * @file iot_lvgl_indev.c
 * @brief LVGL输入设备接口
 *
 * 实现LVGL输入设备的Lua绑定，包括获取活动输入设备、获取输入类型、获取触摸点坐标、获取按键值等接口，支持指针和按键两种输入模式。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== 输入设备操作 ==================== */

static int iot_lvgl_indev_get_act(lua_State* L) {
    lv_indev_t* indev = lv_indev_get_act();
    lua_pushlightuserdata(L, indev);
    return 1;
}

static int iot_lvgl_indev_get_type(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_indev_type_t type = lv_indev_get_type(indev);
    lua_pushinteger(L, type);
    return 1;
}

static int iot_lvgl_indev_get_point(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_point_t point;
    lv_indev_get_point(indev, &point);
    lua_newtable(L);
    lua_pushinteger(L, point.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, point.y);
    lua_setfield(L, -2, "y");
    return 1;
}

static int iot_lvgl_indev_get_key(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    uint32_t key = lv_indev_get_key(indev);
    lua_pushinteger(L, key);
    return 1;
}

/* 注册 indev 子模块 */
void iot_lvgl_register_indev(lua_State* L) {
    REG_METHOD(L, "get_act", iot_lvgl_indev_get_act);
    REG_METHOD(L, "get_type", iot_lvgl_indev_get_type);
    REG_METHOD(L, "get_point", iot_lvgl_indev_get_point);
    REG_METHOD(L, "get_key", iot_lvgl_indev_get_key);
}
