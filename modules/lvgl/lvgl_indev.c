/**
 * @file iot_lvgl_indev.c
 * @brief LVGL输入设备接口
 *
 * 实现LVGL输入设备的Lua绑定，包括获取活动输入设备、获取输入类型、获取触摸点坐标、获取按键值、
 * 设置/获取组关联、设置/获取显示器、获取滚动向量、获取手势方向、设置/获取光标、启用/禁用等接口。
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

/*
获取输入设备关联的组
@param indev 输入设备指针
@return userdata 组指针
@usage local group = lvgl.indev.get_group(indev)
*/
static int iot_lvgl_indev_get_group(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_group_t* group = lv_indev_get_group(indev);
    lua_pushlightuserdata(L, group);
    return 1;
}

/*
设置输入设备关联的组
@param indev 输入设备指针
@param group 组指针
@usage lvgl.indev.set_group(indev, group)
*/
static int iot_lvgl_indev_set_group(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 2);
    lv_indev_set_group(indev, group);
    return 0;
}

/*
获取输入设备关联的显示器
@param indev 输入设备指针
@return userdata 显示器指针
@usage local disp = lvgl.indev.get_display(indev)
*/
static int iot_lvgl_indev_get_display(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lua_pushlightuserdata(L, lv_indev_get_display(indev));
    return 1;
}

/*
设置输入设备关联的显示器
@param indev 输入设备指针
@param disp 显示器指针
@usage lvgl.indev.set_display(indev, disp)
*/
static int iot_lvgl_indev_set_display(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 2, NULL);
    lv_indev_set_display(indev, disp);
    return 0;
}

/*
获取输入设备的滚动向量
@param indev 输入设备指针
@return table {x, y} 滚动向量
@usage local vect = lvgl.indev.get_vect(indev)
*/
static int iot_lvgl_indev_get_vect(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    lua_newtable(L);
    lua_pushinteger(L, vect.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, vect.y);
    lua_setfield(L, -2, "y");
    return 1;
}

/*
获取手势方向
@param indev 输入设备指针
@return integer 手势方向常量
@usage local dir = lvgl.indev.get_gesture_dir(indev)
*/
static int iot_lvgl_indev_get_gesture_dir(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    lua_pushinteger(L, dir);
    return 1;
}

/*
设置输入设备的光标对象
@param indev 输入设备指针
@param cursor_obj 光标对象
@usage lvgl.indev.set_cursor(indev, cursor_obj)
*/
static int iot_lvgl_indev_set_cursor(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_obj_t* cursor_obj = NULL;
    if (!lua_isnoneornil(L, 2)) {
        cursor_obj = iot_lvgl_get_obj_ptr(L, 2);
    }
    lv_indev_set_cursor(indev, cursor_obj);
    return 0;
}

/*
获取输入设备的光标对象
@param indev 输入设备指针
@return userdata 光标对象指针
@usage local cursor = lvgl.indev.get_cursor(indev)
*/
static int iot_lvgl_indev_get_cursor(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lua_pushlightuserdata(L, lv_indev_get_cursor(indev));
    return 1;
}

/*
重置输入设备
@param indev 输入设备指针
@usage lvgl.indev.reset(indev)
*/
static int iot_lvgl_indev_reset(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    lv_indev_reset(indev, NULL);
    return 0;
}

/*
搜索指定坐标处的对象
@param indev 输入设备指针
@param point table {x, y} 坐标点
@return userdata 找到的对象指针
@usage local obj = lvgl.indev.search_obj(indev, {x=100, y=200})
*/
static int iot_lvgl_indev_search_obj(lua_State* L) {
    lv_indev_t* indev = (lv_indev_t*)luaL_checklightuserdata(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    lv_point_t point;
    point.x = (lv_coord_t)lua_tointeger(L, -2);
    point.y = (lv_coord_t)lua_tointeger(L, -1);
    lua_pop(L, 2);
    lv_obj_t* obj = lv_indev_search_obj(indev, &point);
    lua_pushlightuserdata(L, obj);
    return 1;
}

/* 注册 indev 子模块 */
void iot_lvgl_register_indev(lua_State* L) {
    REG_METHOD(L, "get_act", iot_lvgl_indev_get_act);
    REG_METHOD(L, "get_type", iot_lvgl_indev_get_type);
    REG_METHOD(L, "get_point", iot_lvgl_indev_get_point);
    REG_METHOD(L, "get_key", iot_lvgl_indev_get_key);
    REG_METHOD(L, "get_group", iot_lvgl_indev_get_group);
    REG_METHOD(L, "set_group", iot_lvgl_indev_set_group);
    REG_METHOD(L, "get_display", iot_lvgl_indev_get_display);
    REG_METHOD(L, "set_display", iot_lvgl_indev_set_display);
    REG_METHOD(L, "get_vect", iot_lvgl_indev_get_vect);
    REG_METHOD(L, "get_gesture_dir", iot_lvgl_indev_get_gesture_dir);
    REG_METHOD(L, "set_cursor", iot_lvgl_indev_set_cursor);
    REG_METHOD(L, "get_cursor", iot_lvgl_indev_get_cursor);
    REG_METHOD(L, "reset", iot_lvgl_indev_reset);
    REG_METHOD(L, "search_obj", iot_lvgl_indev_search_obj);
}
