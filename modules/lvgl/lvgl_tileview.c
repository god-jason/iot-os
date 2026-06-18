/*
@module  lvgl.tileview
@summary LVGL瓦片视图控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建瓦片视图
local tv = lvgl.tileview.create(scr)
tv:set_size(320, 240)
tv:set_pos(0, 0)

-- 添加瓦片并设置位置
local tile1 = lvgl.obj.create(tv)
tv:add_tile(0, 0, lvgl.DIR_RIGHT)
tile1:set_size(320, 240)
tile1:set_click(true)

local tile2 = lvgl.obj.create(tv)
tv:add_tile(1, 0, lvgl.DIR_LEFT)
tile2:set_size(320, 240)
tile2:set_click(true)

-- 设置当前瓦片
tv:set_tile(tile1, 0)

-- 获取当前瓦片
local cur = tv:get_tile_act()

-- 链式调用
local tv2 = lvgl.tileview.create(scr):set_size(240, 320):set_pos(40, 10)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* tileview组件的metatable引用 */
static int tileview_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_tileview_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tv = lv_tileview_create(parent);
    lua_pushlightuserdata(L, tv);
    return 1;
}

/* ==================== 瓦片视图OO方法 ==================== */

/*
创建瓦片视图控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的瓦片视图实例
@usage local tv = lvgl.tileview.create(scr)
*/
static int lvgl_tileview_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_tileview_create_internal, tileview_metatable_ref);
}

/*
添加瓦片
@param self 瓦片视图实例或指针
@param col 列索引
@param row 行索引
@param dir 滑动方向(可选)
@return userdata 瓦片对象
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
设置当前瓦片
@param self 瓦片视图实例或指针
@param tile 瓦片对象
@param anim 是否动画(可选)
@return self
@usage tv:set_tile(tile, 0)
*/
static int lvgl_tileview_set_tile(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_tileview_set_tile(tv, tile, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置瓦片滑动方向
@param self 瓦片视图实例或指针
@param tile 瓦片对象
@param dir 滑动方向
@return self
@usage tv:set_tile_drag_dir(tile, lvgl.DIR_LEFT)
*/
static int lvgl_tileview_set_tile_drag_dir(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 3);
    lv_tileview_set_tile_drag_dir(tv, tile, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取当前激活瓦片
@param self 瓦片视图实例或指针
@return userdata 瓦片对象
@usage local cur = tv:get_tile_act()
*/
static int lvgl_tileview_get_tile_act(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tile = lv_tileview_get_tile_act(tv);
    lua_pushlightuserdata(L, tile);
    return 1;
}

/* 注册 tileview 子模块 */
void lvgl_register_tileview(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "add_tile", lvgl_tileview_add_tile);
    REG_METHOD(L, "set_tile", lvgl_tileview_set_tile);
    REG_METHOD(L, "set_tile_drag_dir", lvgl_tileview_set_tile_drag_dir);
    REG_METHOD(L, "get_tile_act", lvgl_tileview_get_tile_act);

    /* 保存组件metatable引用(用于继承) */
    tileview_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.tileview.add_tile(tv, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, tileview_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.tileview) */
    REG_METHOD(L, "create", lvgl_tileview_create);
}
