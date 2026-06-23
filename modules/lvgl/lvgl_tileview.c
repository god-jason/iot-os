/*
@module  lvgl.tileview
@summary LVGL??????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??(OO??)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- ??????
local tv = lvgl.tileview.create(scr)
tv:set_size(320, 240)
tv:set_pos(0, 0)

-- ??????????
local tile1 = lvgl.obj.create(tv)
tv:add_tile(0, 0, lvgl.DIR_RIGHT)
tile1:set_size(320, 240)
tile1:set_click(true)

local tile2 = lvgl.obj.create(tv)
tv:add_tile(1, 0, lvgl.DIR_LEFT)
tile2:set_size(320, 240)
tile2:set_click(true)

-- ??????
tv:set_tile(tile1, 0)

-- ??????
local cur = tv:get_tile_act()

-- ????
local tv2 = lvgl.tileview.create(scr):set_size(240, 320):set_pos(40, 10)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* tileview???metatable?? */
static int tileview_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_tileview_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tv = lv_tileview_create(parent);
    lua_pushlightuserdata(L, tv);
    return 1;
}

/* ==================== ????OO?? ==================== */

/*
????????(OO??)
@param self ???????
@return userdata ?metatable????????
@usage local tv = lvgl.tileview.create(scr)
*/
static int lvgl_tileview_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_tileview_create_internal, tileview_metatable_ref);
}

/*
????
@param self ??????????
@param col ????
@param row ????
@param dir ????(???
@return userdata ????
@usage local tile = tv:add_tile(0, 0, lvgl.DIR_RIGHT)
*/
static int lvgl_tileview_add_tile(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint8_t col = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t row = (uint8_t)luaL_checkinteger(L, 3);
    lv_dir_t dir = (lv_dir_t)luaL_optinteger(L, 4, LV_DIR_ALL);
    lv_obj_t* tile = lv_tileview_add_tile(tv, col, row, dir);
    lua_pushlightuserdata(L, tile);
    return 1;
}

/*
??????
@param self ??????????
@param tile ????
@param anim ????(???
@return self
@usage tv:set_tile(tile, 0)
*/
static int lvgl_tileview_set_tile(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_obj_set_tile(tv, tile, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ??????????
@param tile ????
@param dir ????
@return self
@usage tv:set_tile_drag_dir(tile, lvgl.DIR_LEFT)
*/
static int lvgl_tileview_set_tile_drag_dir(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 3);
    if (lv_obj_check_type(tile, &lv_tileview_tile_class)) {
        ((lv_tileview_tile_t*)tile)->dir = dir;
        if (lv_tileview_get_tile_act(tv) == tile) {
            lv_obj_set_scroll_dir(tv, dir);
        }
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
?????????
@param self ??????????
@return userdata ????
@usage local cur = tv:get_tile_act()
*/
static int lvgl_tileview_get_tile_act(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = lv_tileview_get_tile_act(tv);
    lua_pushlightuserdata(L, tile);
    return 1;
}

/* ?? tileview ????*/
void lvgl_register_tileview(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "add_tile", lvgl_tileview_add_tile);
    REG_METHOD(L, "set_tile", lvgl_tileview_set_tile);
    REG_METHOD(L, "set_tile_drag_dir", lvgl_tileview_set_tile_drag_dir);
    REG_METHOD(L, "get_tile_act", lvgl_tileview_get_tile_act);

    /* ????metatable??(????) */
    tileview_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.tileview.add_tile(tv, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, tileview_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.tileview) */
    REG_METHOD(L, "create", lvgl_tileview_create);
}
