/**
 * @file iot_lvgl_disp.c
 * @brief LVGL显示设备接口
 *
 * 实现LVGL显示设备的Lua绑定，包括获取水平/垂直分辨率、获取物理分辨率、设置/获取显示旋转、设置背景颜色/背景图片等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== 显示器操作 ==================== */

static int iot_lvgl_disp_get_hor_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_hor_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_ver_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_ver_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_physical_hor_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_physical_hor_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_physical_ver_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_physical_ver_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_set_rotation(lua_State* L) {
    lv_disp_t* disp = (lv_disp_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_rot_t rotation = (lv_disp_rot_t)luaL_checkinteger(L, 2);
    lv_disp_set_rotation(disp, rotation);
    return 0;
}

static int iot_lvgl_disp_get_rotation(lua_State* L) {
    lv_disp_t* disp = (lv_disp_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_rot_t rotation = lv_disp_get_rotation(disp);
    lua_pushinteger(L, rotation);
    return 1;
}

static int iot_lvgl_disp_set_bg_color(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    lv_disp_set_bg_color(NULL, color);
    return 0;
}

static int iot_lvgl_disp_set_bg_image(lua_State* L) {
    lv_obj_t* img = (lv_obj_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_set_bg_image(NULL, img);
    return 0;
}

/* 注册 disp 子模块 */
void iot_lvgl_register_disp(lua_State* L) {
    REG_METHOD(L, "get_hor_res", iot_lvgl_disp_get_hor_res);
    REG_METHOD(L, "get_ver_res", iot_lvgl_disp_get_ver_res);
    REG_METHOD(L, "get_physical_hor_res", iot_lvgl_disp_get_physical_hor_res);
    REG_METHOD(L, "get_physical_ver_res", iot_lvgl_disp_get_physical_ver_res);
    REG_METHOD(L, "set_rotation", iot_lvgl_disp_set_rotation);
    REG_METHOD(L, "get_rotation", iot_lvgl_disp_get_rotation);
    REG_METHOD(L, "set_bg_color", iot_lvgl_disp_set_bg_color);
    REG_METHOD(L, "set_bg_image", iot_lvgl_disp_set_bg_image);
}
