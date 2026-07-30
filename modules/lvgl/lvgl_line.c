/**
 * @file lvgl_line.c
 * @brief LVGL线条控件
 *
 * 实现LVGL线条控件的OO风格Lua绑定，包括线条创建、设置坐标点数组、设置自动尺寸、设置Y轴翻转等接口，用于绘制折线图。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* line组件的metatable引用 */
static int line_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_line_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* line = lv_line_create(parent);
    lua_pushlightuserdata(L, line);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
创建线条控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的线条实例
@usage local line = lvgl.line.create(scr)
*/
static int lvgl_line_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_line_create_internal, line_metatable_ref);
}

/*
设置线条点
@param self 线条实例或指针
@param points 点数组(表格式:{{x1,y1},{x2,y2},...})
@return self
@usage line:set_points({{0,0},{100,50}})
*/
static int lvgl_line_set_points(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);
    uint32_t point_num = (uint32_t)luaL_len(L, 2);

    lv_point_t* points = (lv_point_t*)cm_malloc(sizeof(lv_point_t) * point_num);
    if (!points) {
        luaL_error(L, "memory allocation failed");
        return 1;
    }

    for (uint32_t i = 0; i < point_num; i++) {
        lua_geti(L, 2, i + 1);
        if (lua_istable(L, -1)) {
            lua_geti(L, -1, 1);
            points[i].x = (int32_t)luaL_checkinteger(L, -1);
            lua_geti(L, -2, 2);
            points[i].y = (int32_t)luaL_checkinteger(L, -1);
            lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }

    lv_line_set_points(line, points, point_num);
    cm_free(points);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置线条样式
@param self 线条实例或指针
@param style 样式对象
@return self
@usage line:set_style(style)
*/
static int lvgl_line_set_style(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 2);
    lv_obj_add_style(line, style, LV_PART_MAIN);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置自动尺寸
@param self 线条实例或指针
@param en 是否启用自动尺寸
@return self
@usage line:set_auto_size(true)
*/
static int lvgl_line_set_auto_size(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    if (en) {
        lv_obj_set_width(line, LV_SIZE_CONTENT);
        lv_obj_set_height(line, LV_SIZE_CONTENT);
        lv_obj_refresh_self_size(line);
    } else {
        lv_obj_set_size(line, lv_obj_get_width(line), lv_obj_get_height(line));
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置Y坐标反转
@param self 线条实例或指针
@param en 是否反转
@return self
@usage line:set_y_invert(true)
*/
static int lvgl_line_set_y_invert(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_line_set_y_invert(line, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取Y坐标是否反转
@param self 线条实例或指针
@return boolean 是否反转
@usage local inverted = line:get_y_invert()
*/
static int lvgl_line_get_y_invert(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);
    bool inverted = lv_line_get_y_invert(line);
    lua_pushboolean(L, inverted);
    return 1;
}

/* 注册 line 子模块 */
void lvgl_register_line(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_points", lvgl_line_set_points);
    REG_METHOD(L, "set_style", lvgl_line_set_style);
    REG_METHOD(L, "set_auto_size", lvgl_line_set_auto_size);
    REG_METHOD(L, "set_y_invert", lvgl_line_set_y_invert);
    REG_METHOD(L, "get_y_invert", lvgl_line_get_y_invert);

    /* 保存组件metatable引用(用于继承) */
    line_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.line.set_points(line, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, line_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.line) */
    REG_METHOD(L, "create", lvgl_line_create);
}
