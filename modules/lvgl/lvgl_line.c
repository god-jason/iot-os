/*
@module  lvgl.line
@summary LVGL????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??(OO??)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- ????
local line = lvgl.line.create(scr)

-- ??????????{{x1,y1},{x2,y2},...})
local points = {{0,0}, {100,50}, {50,100}}
line:set_points(points)

-- ??????
line:set_auto_size(true)

-- Y????(????)
line:set_y_invert(true)

-- ??Y??????
local inverted = line:get_y_invert()

-- ????
local line2 = lvgl.line.create(scr):set_auto_size(true):set_y_invert(false)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* line???metatable?? */
static int line_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_line_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* line = lv_line_create(parent);
    lua_pushlightuserdata(L, line);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local line = lvgl.line.create(scr)
*/
static int lvgl_line_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_line_create_internal, line_metatable_ref);
}

/*
??????
@param self ????????
@param points ????????{{x1,y1},{x2,y2},...})
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
??????
@param self ????????
@param style ????
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
??????
@param self ????????
@param en ????????
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
??Y????
@param self ????????
@param en ????
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
??Y??????
@param self ????????
@return boolean ????
@usage local inverted = line:get_y_invert()
*/
static int lvgl_line_get_y_invert(lua_State* L) {
    lv_obj_t* line = lvgl_get_obj_ptr(L, 1);
    bool inverted = lv_line_get_y_invert(line);
    lua_pushboolean(L, inverted);
    return 1;
}

/* ?? line ????*/
void lvgl_register_line(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_points", lvgl_line_set_points);
    REG_METHOD(L, "set_style", lvgl_line_set_style);
    REG_METHOD(L, "set_auto_size", lvgl_line_set_auto_size);
    REG_METHOD(L, "set_y_invert", lvgl_line_set_y_invert);
    REG_METHOD(L, "get_y_invert", lvgl_line_get_y_invert);

    /* ????metatable??(????) */
    line_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.line.set_points(line, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, line_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.line) */
    REG_METHOD(L, "create", lvgl_line_create);
}
