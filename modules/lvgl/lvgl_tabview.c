/*
@module  lvgl.tabview
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
local tv = lvgl.tabview.create(scr)
tv:set_size(320, 240)
tv:set_pos(0, 0)

-- ??????
tv:set_tab_bar_position(lvgl.TABVIEW_TAB_POS_TOP)

-- ????
local tab1 = tv:add_tab("??1")
local tab2 = tv:add_tab("??2")
local tab3 = tv:add_tab("??3")

-- ??????
local page0 = tv:get_tab(0)
local page1 = tv:get_tab(1)

-- ????????
local label1 = lvgl.label.create(page0)
label1:set_text("????1????)

-- ??????
tv:set_active(0, 0)  -- ????????????

-- ????????
local active = tv:get_active()

-- ????
local tv2 = lvgl.tabview.create(scr):set_size(240, 320):set_pos(40, 10):add_tab("Tab1")
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* tabview???metatable?? */
static int tabview_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_tabview_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_dir_t tab_pos = (lv_dir_t)luaL_optinteger(L, 2, LV_DIR_TOP);
    lv_coord_t tab_size = (lv_coord_t)luaL_optinteger(L, 3, 50);
    lv_obj_t* tv = lv_tabview_create(parent, tab_pos, tab_size);
    lua_pushlightuserdata(L, tv);
    return 1;
}

/* ==================== ????OO?? ==================== */

/*
????????(OO??)
@param self ???????
@return userdata ?metatable????????
@usage local tv = lvgl.tabview.create(scr)
*/
static int lvgl_tabview_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_tabview_create_internal, tabview_metatable_ref);
}

/*
??????
@param self ??????????
@param name ????
@return userdata ??????
@usage local tab1 = tv:add_tab("??1")
*/
static int lvgl_tabview_add_tab(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    const char* name = luaL_checkstring(L, 2);
    lv_obj_t* tab = lv_tabview_add_tab(tv, name);
    lua_pushlightuserdata(L, tab);
    return 1;
}

/*
????????
@param self ??????????
@param pos ??????
@return self
@usage tv:set_tab_bar_position(lvgl.TABVIEW_TAB_POS_TOP)
*/
static int lvgl_tabview_set_tab_bar_position(lua_State* L) {
    (void)lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* tab bar position is fixed at create time in LVGL 8 */
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ??????????
@param width ????
@return self
@usage tv:set_tab_bar_width(100)
*/
static int lvgl_tabview_set_tab_bar_width(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_coord_t size = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_tabview_t* tabview = (lv_tabview_t*)tv;
    lv_obj_t* btns = lv_tabview_get_tab_btns(tv);
    if (btns) {
        if (tabview->tab_pos & LV_DIR_VER) {
            lv_obj_set_height(btns, size);
        } else {
            lv_obj_set_width(btns, size);
        }
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ??????????
@param idx ????
@return userdata ??????
@usage local page0 = tv:get_tab(0)
*/
static int lvgl_tabview_get_tab(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint32_t idx = (uint32_t)luaL_checkinteger(L, 2);
    lv_obj_t* cont = lv_tabview_get_content(tv);
    lv_obj_t* tab = cont ? lv_obj_get_child(cont, idx) : NULL;
    lua_pushlightuserdata(L, tab);
    return 1;
}

/*
?????????
@param self ??????????
@param idx ????
@param anim ????(???
@return self
@usage tv:set_active(0, 0)
*/
static int lvgl_tabview_set_active(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint32_t idx = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_tabview_set_act(tv, idx, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
???????????
@param self ??????????
@return integer ????
@usage local active = tv:get_active()
*/
static int lvgl_tabview_get_active(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint16_t active = lv_tabview_get_tab_act(tv);
    lua_pushinteger(L, active);
    return 1;
}

/*
??????
@param self ??????????
@return integer ????
@usage local count = tv:get_tab_count()
*/
static int lvgl_tabview_get_tab_count(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_tabview_t* tabview = (lv_tabview_t*)tv;
    lua_pushinteger(L, tabview->tab_cnt);
    return 1;
}

/* ?? tabview ????*/
void lvgl_register_tabview(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "add_tab", lvgl_tabview_add_tab);
    REG_METHOD(L, "set_tab_bar_position", lvgl_tabview_set_tab_bar_position);
    REG_METHOD(L, "set_tab_bar_width", lvgl_tabview_set_tab_bar_width);
    REG_METHOD(L, "get_tab", lvgl_tabview_get_tab);
    REG_METHOD(L, "set_active", lvgl_tabview_set_active);
    REG_METHOD(L, "get_active", lvgl_tabview_get_active);
    REG_METHOD(L, "get_tab_count", lvgl_tabview_get_tab_count);

    /* ????metatable??(????) */
    tabview_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.tabview.add_tab(tv, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, tabview_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.tabview) */
    REG_METHOD(L, "create", lvgl_tabview_create);
}
