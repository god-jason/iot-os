/*
@module  lvgl.win
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
local win = lvgl.win.create(scr)
win:set_size(300, 200)
win:set_pos(10, 10)

-- ??????
win:set_title("??")

-- ????????
local btn = win:add_btn(nil)  -- nil??????
local btn2 = win:add_btn("OK")

-- ??????
win:set_btn_title(btn, "X")

-- ??????
local cont = win:get_content()

-- ????????
local label = lvgl.label.create(cont)
label:set_text("???????????")

-- ????
local w = lvgl.win.create(scr):set_size(280, 180):set_pos(20, 30):set_title("MyWindow")
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* win???metatable?? */
static int win_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_win_find_title(lv_obj_t* win)
{
    lv_obj_t* header = lv_win_get_header(win);
    uint32_t i;
    uint32_t cnt;

    if (!header) {
        return NULL;
    }
    cnt = lv_obj_get_child_cnt(header);
    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(header, i);
        if (lv_obj_check_type(child, &lv_label_class)) {
            return child;
        }
    }
    return NULL;
}

/* ==================== ?????? ==================== */

static int lvgl_win_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_coord_t header_height = (lv_coord_t)luaL_optinteger(L, 2, 40);
    lv_obj_t* win = lv_win_create(parent, header_height);
    lua_pushlightuserdata(L, win);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local win = lvgl.win.create(scr)
*/
static int lvgl_win_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_win_create_internal, win_metatable_ref);
}

/*
??????
@param self ????????
@param title ????
@return self
@usage win:set_title("??")
*/
static int lvgl_win_set_title(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    lv_obj_t* label = lvgl_win_find_title(win);
    if (label) {
        lv_label_set_text(label, title);
    } else {
        lv_win_add_title(win, title);
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param icon ??(???
@return userdata ????
@usage local btn = win:add_btn(nil)
*/
static int lvgl_win_add_btn(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    const void* icon = NULL;
    if (!lua_isnoneornil(L, 2)) {
        if (lua_isstring(L, 2)) {
            icon = luaL_checkstring(L, 2);
        } else {
            icon = luaL_checklightuserdata(L, 2);
        }
    }
    lv_coord_t btn_w = (lv_coord_t)luaL_optinteger(L, 3, 40);
    lv_obj_t* btn = lv_win_add_btn(win, icon, btn_w);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/*
??????
@param self ????????
@param btn ????
@param title ????
@return self
@usage win:set_btn_title(btn, "X")
*/
static int lvgl_win_set_btn_title(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    const char* title = luaL_checkstring(L, 3);
    (void)win;
    lv_btn_set_text(btn, title);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@return userdata ??????
@usage local cont = win:get_content()
*/
static int lvgl_win_get_content(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cont = lv_win_get_content(win);
    lua_pushlightuserdata(L, cont);
    return 1;
}

/*
????????
@param self ????????
@param height ????
@return self
@usage win:set_title_height(40)
*/
static int lvgl_win_set_title_height(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_coord_t height = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_obj_t* header = lv_win_get_header(win);
    if (header) {
        lv_obj_set_height(header, height);
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param flags ????
@return self
@usage win:set_flags(lvgl.WIN_FLAG_HEADER)
*/
static int lvgl_win_set_flags(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_win_set_flags() was removed in LVGL 8 */
    (void)win;
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param flags ????
@return self
@usage win:clear_flags(lvgl.WIN_FLAG_HEADER)
*/
static int lvgl_win_clear_flags(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_win_clear_flags() was removed in LVGL 8 */
    (void)win;
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? win ????*/
void lvgl_register_win(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_title", lvgl_win_set_title);
    REG_METHOD(L, "add_btn", lvgl_win_add_btn);
    REG_METHOD(L, "set_btn_title", lvgl_win_set_btn_title);
    REG_METHOD(L, "get_content", lvgl_win_get_content);
    REG_METHOD(L, "set_title_height", lvgl_win_set_title_height);
    REG_METHOD(L, "set_flags", lvgl_win_set_flags);
    REG_METHOD(L, "clear_flags", lvgl_win_clear_flags);

    /* ????metatable??(????) */
    win_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.win.set_title(win, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, win_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.win) */
    REG_METHOD(L, "create", lvgl_win_create);
}
