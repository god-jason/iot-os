/*
@module  lvgl.disp
@summary LVGL??????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== ??????==================== */

static int lvgl_disp_get_hor_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_hor_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int lvgl_disp_get_ver_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_ver_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int lvgl_disp_get_physical_hor_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_physical_hor_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int lvgl_disp_get_physical_ver_res(lua_State* L) {
    lv_coord_t res = lv_disp_get_physical_ver_res(NULL);
    lua_pushinteger(L, res);
    return 1;
}

static int lvgl_disp_set_rotation(lua_State* L) {
    lv_disp_t* disp = (lv_disp_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_rot_t rotation = (lv_disp_rot_t)luaL_checkinteger(L, 2);
    lv_disp_set_rotation(disp, rotation);
    return 0;
}

static int lvgl_disp_get_rotation(lua_State* L) {
    lv_disp_t* disp = (lv_disp_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_rot_t rotation = lv_disp_get_rotation(disp);
    lua_pushinteger(L, rotation);
    return 1;
}

static int lvgl_disp_set_bg_color(lua_State* L) {
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 1);
    lv_disp_set_bg_color(NULL, color);
    return 0;
}

static int lvgl_disp_set_bg_image(lua_State* L) {
    lv_obj_t* img = (lv_obj_t*)luaL_optlightuserdata(L, 1, NULL);
    lv_disp_set_bg_image(NULL, img);
    return 0;
}

/* ?? disp ????*/
void lvgl_register_disp(lua_State* L) {
    REG_METHOD(L, "get_hor_res", lvgl_disp_get_hor_res);
    REG_METHOD(L, "get_ver_res", lvgl_disp_get_ver_res);
    REG_METHOD(L, "get_physical_hor_res", lvgl_disp_get_physical_hor_res);
    REG_METHOD(L, "get_physical_ver_res", lvgl_disp_get_physical_ver_res);
    REG_METHOD(L, "set_rotation", lvgl_disp_set_rotation);
    REG_METHOD(L, "get_rotation", lvgl_disp_get_rotation);
    REG_METHOD(L, "set_bg_color", lvgl_disp_set_bg_color);
    REG_METHOD(L, "set_bg_image", lvgl_disp_set_bg_image);
}
