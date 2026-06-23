/*
@module  lvgl.indev
@summary LVGLčžĺĽčŽžĺ¤ćä˝
@version 2.0
@date    2026.06.18
@author  ć°çĽ & TRAE & ChatGPT
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== čžĺĽčŽžĺ¤ćä˝ ==================== */

static int lvgl_indev_get_act(lua_State* L) {
    lv_indev_t* indev = lv_indev_get_act();
    lua_pushlightuserdata(L, indev);
    return 1;
}

static int lvgl_indev_get_type(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_indev_type_t type = lv_indev_get_type(indev);
    lua_pushinteger(L, type);
    return 1;
}

static int lvgl_indev_get_point(lua_State* L) {
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

static int lvgl_indev_get_key(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    uint32_t key = lv_indev_get_key(indev);
    lua_pushinteger(L, key);
    return 1;
}

/* ćł¨ĺ indev ĺ­ć¨Ąĺ?*/
void lvgl_register_indev(lua_State* L) {
    REG_METHOD(L, "get_act", lvgl_indev_get_act);
    REG_METHOD(L, "get_type", lvgl_indev_get_type);
    REG_METHOD(L, "get_point", lvgl_indev_get_point);
    REG_METHOD(L, "get_key", lvgl_indev_get_key);
}
