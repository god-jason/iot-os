/*
@module  lvgl.list
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
local list = lvgl.list.create(scr)
list:set_size(200, 300)
list:set_pos(50, 50)

-- ????????
local btn1 = list:add_btn(nil, "??")
local btn2 = list:add_btn(nil, "??")
local btn3 = list:add_btn(nil, "??")

-- ??????
list:set_direction(lvgl.DIR_TOP)

-- ??????
local sel = list:get_selected_btn()
if sel then
    print("????", sel:get_text())
end

-- ????
local list2 = lvgl.list.create(scr):set_size(150, 200):set_pos(280, 50):set_direction(lvgl.DIR_LEFT)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* list???metatable?? */
static int list_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_list_find_selected_btn(lv_obj_t* list)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_cnt(list);

    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(list, i);
        if (lv_obj_check_type(child, &lv_list_btn_class) &&
            lv_obj_has_state(child, LV_STATE_CHECKED)) {
            return child;
        }
    }
    return NULL;
}

static void lvgl_list_clear_selected(lv_obj_t* list)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_cnt(list);

    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(list, i);
        if (lv_obj_check_type(child, &lv_list_btn_class)) {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
        }
    }
}

static lv_flex_flow_t lvgl_list_dir_to_flex(lv_dir_t dir)
{
    switch (dir) {
        case LV_DIR_LEFT:
            return LV_FLEX_FLOW_ROW;
        case LV_DIR_RIGHT:
            return LV_FLEX_FLOW_ROW_REVERSE;
        case LV_DIR_BOTTOM:
            return LV_FLEX_FLOW_COLUMN_REVERSE;
        case LV_DIR_TOP:
        default:
            return LV_FLEX_FLOW_COLUMN;
    }
}

/* ==================== ?????? ==================== */

static int lvgl_list_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* list = lv_list_create(parent);
    lua_pushlightuserdata(L, list);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local list = lvgl.list.create(scr)
*/
static int lvgl_list_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_list_create_internal, list_metatable_ref);
}

/*
????????
@param self ????????
@param img ??(???
@param txt ????
@return userdata ????
@usage local btn = list:add_btn(nil, "??")
*/
static int lvgl_list_add_btn(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* img = (lv_obj_t*)luaL_optlightuserdata(L, 2, NULL);
    const char* txt = luaL_checkstring(L, 3);
    lv_obj_t* btn = lv_list_add_btn(list, img, txt);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/*
??????
@param self ????????
@return userdata ????
@usage local cont = list:get_container()
*/
static int lvgl_list_get_container(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lua_pushlightuserdata(L, list);
    return 1;
}

/*
????????
@param self ????????
@return userdata ?????nil
@usage local sel = list:get_selected_btn()
*/
static int lvgl_list_get_selected_btn(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = lvgl_list_find_selected_btn(list);
    if (btn) {
        lua_pushlightuserdata(L, btn);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/*
??????
@param self ????????
@param dir ??(lvgl.DIR_TOP??
@return self
@usage list:set_direction(lvgl.DIR_TOP)
*/
static int lvgl_list_set_direction(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 2);
    lv_obj_set_flex_flow(list, lvgl_list_dir_to_flex(dir));
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param sel ????
@return self
@usage list:set_selected(btn)
*/
static int lvgl_list_set_selected(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* sel = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lvgl_list_clear_selected(list);
    lv_obj_add_state(sel, LV_STATE_CHECKED);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? list ????*/
void lvgl_register_list(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "add_btn", lvgl_list_add_btn);
    REG_METHOD(L, "get_container", lvgl_list_get_container);
    REG_METHOD(L, "get_selected_btn", lvgl_list_get_selected_btn);
    REG_METHOD(L, "set_direction", lvgl_list_set_direction);
    REG_METHOD(L, "set_selected", lvgl_list_set_selected);

    /* ????metatable??(????) */
    list_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.list.add_btn(list, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, list_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.list) */
    REG_METHOD(L, "create", lvgl_list_create);
}
