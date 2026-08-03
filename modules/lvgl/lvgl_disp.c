/**
 * @file iot_lvgl_disp.c
 * @brief LVGL显示设备接口
 *
 * 实现LVGL显示设备的Lua绑定，包括获取水平/垂直分辨率、获取物理分辨率、设置/获取显示旋转、设置背景颜色/背景图片等接口。
 * 适配 LVGL 9.5.0 API：lv_disp_* 系列函数替换为 lv_display_* 系列。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== 显示器操作 ==================== */

static int iot_lvgl_disp_get_hor_res(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t res = lv_display_get_horizontal_resolution(disp);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_ver_res(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t res = lv_display_get_vertical_resolution(disp);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_physical_hor_res(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t res = lv_display_get_physical_horizontal_resolution(disp);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_get_physical_ver_res(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t res = lv_display_get_physical_vertical_resolution(disp);
    lua_pushinteger(L, res);
    return 1;
}

static int iot_lvgl_disp_set_rotation(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_display_rotation_t rotation = (lv_display_rotation_t)luaL_checkinteger(L, 2);
    lv_display_set_rotation(disp, rotation);
    return 0;
}

static int iot_lvgl_disp_get_rotation(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    lua_pushinteger(L, rotation);
    return 1;
}

static int iot_lvgl_disp_set_bg_color(lua_State* L) {
    /* LVGL 9 移除了 lv_disp_set_bg_color，通过活动屏幕的样式设置背景色 */
    lv_color_t color = lv_color_from_u32((uint32_t)luaL_checkinteger(L, 1));
    lv_obj_t* scr = lv_screen_active();
    if (scr) {
        lv_obj_set_style_bg_color(scr, color, 0);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    }
    return 0;
}

static int iot_lvgl_disp_set_bg_image(lua_State* L) {
    /* LVGL 9 移除了 lv_disp_set_bg_image，通过活动屏幕的样式设置背景图 */
    lv_obj_t* scr = lv_screen_active();
    if (scr) {
        const char* path = luaL_optstring(L, 1, NULL);
        lv_obj_set_style_bg_img_src(scr, path, 0);
    }
    return 0;
}

static int iot_lvgl_disp_set_dpi(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t dpi = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_display_set_dpi(disp, dpi);
    return 0;
}

static int iot_lvgl_disp_get_dpi(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t dpi = lv_display_get_dpi(disp);
    lua_pushinteger(L, dpi);
    return 1;
}

static int iot_lvgl_disp_set_offset(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t x = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_coord_t y = (lv_coord_t)luaL_checkinteger(L, 3);
    lv_display_set_offset(disp, x, y);
    return 0;
}

static int iot_lvgl_disp_get_offset(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_coord_t x = lv_display_get_offset_x(disp);
    lv_coord_t y = lv_display_get_offset_y(disp);
    lua_newtable(L);
    lua_pushinteger(L, x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, y);
    lua_setfield(L, -2, "y");
    return 1;
}

static int iot_lvgl_disp_get_color_format(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    lv_color_format_t cf = lv_display_get_color_format(disp);
    lua_pushinteger(L, (int)cf);
    return 1;
}

static int iot_lvgl_disp_get_inactive_time(lua_State* L) {
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);
    if (!disp) disp = lv_display_get_default();
    uint32_t t = lv_display_get_inactive_time(disp);
    lua_pushinteger(L, t);
    return 1;
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
    REG_METHOD(L, "set_dpi", iot_lvgl_disp_set_dpi);
    REG_METHOD(L, "get_dpi", iot_lvgl_disp_get_dpi);
    REG_METHOD(L, "set_offset", iot_lvgl_disp_set_offset);
    REG_METHOD(L, "get_offset", iot_lvgl_disp_get_offset);
    REG_METHOD(L, "get_color_format", iot_lvgl_disp_get_color_format);
    REG_METHOD(L, "get_inactive_time", iot_lvgl_disp_get_inactive_time);
}
