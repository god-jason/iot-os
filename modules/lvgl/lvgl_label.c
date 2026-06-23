/*
@module  lvgl.label
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
local label = lvgl.label.create(scr)

-- ??OO??
label:set_pos(10, 10)
label:set_text("Hello World")
label:set_align(lvgl.TEXT_ALIGN_CENTER)
label:set_size(200, 30)

-- ??????
label:set_long_mode(lvgl.LABEL_LONG_SCROLL_CIRCULAR)
label:set_text_sel_start(0)
label:set_text_sel_end(5)
label:set_recolor(true)

-- ????
local label2 = lvgl.label.create(scr):set_pos(10, 50):set_text("Another Label"):set_align(lvgl.TEXT_ALIGN_LEFT)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* label???metatable?? */
static int label_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_label_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* label = lv_label_create(parent);
    lua_pushlightuserdata(L, label);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local label = lvgl.label.create(scr)
*/
static int lvgl_label_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_label_create_internal, label_metatable_ref);
}

/*
??????
@param self ????????
@param text ????
@return self
@usage label:set_text("Hello")
*/
static int lvgl_label_set_text(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_label_set_text(label, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@return string ????
@usage local text = label:get_text()
*/
static int lvgl_label_get_text(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    const char* text = lv_label_get_text(label);
    lua_pushstring(L, text ? text : "");
    return 1;
}

/*
????????
@param self ????????
@param align ????
@return self
@usage label:set_align(lvgl.TEXT_ALIGN_CENTER)
*/
static int lvgl_label_set_align(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    lv_text_align_t align = (lv_text_align_t)luaL_checkinteger(L, 2);
    lv_obj_set_style_text_align(label, align, LV_PART_MAIN);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param mode ??: LABEL_LONG_WRAP, LABEL_LONG_SCROLL, LABEL_LONG_DOT, LABEL_LONG_SCROLL_CIRCULAR, LABEL_LONG_CROP
@return self
@usage label:set_long_mode(lvgl.LABEL_LONG_SCROLL_CIRCULAR)
*/
static int lvgl_label_set_long_mode(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    uint32_t mode = (uint32_t)luaL_checkinteger(L, 2);
    lv_label_set_long_mode(label, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
?????????
@param self ????????
@param en ????
@return self
@usage label:set_recolor(true)
*/
static int lvgl_label_set_recolor(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_label_set_recolor(label, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
???????????
@param self ????????
@param pos ??
@return self
@usage label:set_text_sel_start(0)
*/
static int lvgl_label_set_text_sel_start(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    int32_t pos = (int32_t)luaL_checkinteger(L, 2);
    lv_label_set_text_sel_start(label, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@param pos ??
@return self
@usage label:set_text_sel_end(5)
*/
static int lvgl_label_set_text_sel_end(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    int32_t pos = (int32_t)luaL_checkinteger(L, 2);
    lv_label_set_text_sel_end(label, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@return boolean
@usage local under = label:is_char_under_cursor()
*/
static int lvgl_label_is_char_under_cursor(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    lv_indev_t* indev = lv_indev_get_act();
    lv_point_t rel_pos = {0, 0};
    bool under = false;

    if (indev) {
        lv_point_t point;
        lv_area_t coords;
        lv_indev_get_point(indev, &point);
        lv_obj_get_coords(label, &coords);
        rel_pos.x = point.x - coords.x1;
        rel_pos.y = point.y - coords.y1;
        under = lv_label_is_char_under_pos(label, &rel_pos);
    }

    lua_pushboolean(L, under);
    return 1;
}

/* ?? label ????*/
void lvgl_register_label(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_text", lvgl_label_set_text);
    REG_METHOD(L, "get_text", lvgl_label_get_text);
    REG_METHOD(L, "set_align", lvgl_label_set_align);
    REG_METHOD(L, "set_long_mode", lvgl_label_set_long_mode);
    REG_METHOD(L, "set_recolor", lvgl_label_set_recolor);
    REG_METHOD(L, "set_text_sel_start", lvgl_label_set_text_sel_start);
    REG_METHOD(L, "set_text_sel_end", lvgl_label_set_text_sel_end);
    REG_METHOD(L, "is_char_under_cursor", lvgl_label_is_char_under_cursor);

    /* ????metatable??(????) */
    label_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.label.set_text(label, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, label_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.label) */
    REG_METHOD(L, "create", lvgl_label_create);
}
