/*
@module  lvgl.menu
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
local menu = lvgl.menu.create(scr)
menu:set_size(320, 240)
menu:set_pos(0, 0)

-- ??????
local main_page = menu:page_create(nil)
menu:set_page(main_page)

-- ??????
local item1 = main_page:add_item(nil, "??")
local item2 = main_page:add_item(nil, "??")

-- ??????
local settings_page = menu:page_create("??")
settings_page:add_item(nil, "WiFi")
settings_page:add_item(nil, "??")

-- ??????????
item1:set_page(settings_page)

-- ??????
local cur_page = menu:get_cur_page()

-- ????
local m = lvgl.menu.create(scr):set_size(240, 320):set_pos(40, 10):set_title("????)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* menu???metatable?? */
static int menu_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_menu_find_menu(lv_obj_t* obj)
{
    lv_obj_t* cur = obj;
    while (cur) {
        if (lv_obj_check_type(cur, &lv_menu_class)) {
            return cur;
        }
        cur = lv_obj_get_parent(cur);
    }
    return NULL;
}

/* ==================== ?????? ==================== */

static int lvgl_menu_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* menu = lv_menu_create(parent);
    lua_pushlightuserdata(L, menu);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local menu = lvgl.menu.create(scr)
*/
static int lvgl_menu_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_menu_create_internal, menu_metatable_ref);
}

/*
??????
@param self ????????
@param title ????(???
@return userdata ????
@usage local page = menu:page_create("??")
*/
static int lvgl_menu_page_create(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    const char* title = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    lv_obj_t* page = lv_menu_page_create(menu, title ? (char*)title : NULL);
    lua_pushlightuserdata(L, page);
    return 1;
}

/*
??????
@param self ????????
@param icon ??(???
@param text ????
@return userdata ??????
@usage local item = page:add_item(nil, "??")
*/
static int lvgl_menu_add_item(lua_State* L) {
    lv_obj_t* page = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* icon = (lv_obj_t*)luaL_optlightuserdata(L, 2, NULL);
    const char* text = luaL_checkstring(L, 3);
    lv_obj_t* cont = lv_menu_cont_create(page);
    if (icon) {
        lv_obj_set_parent(icon, cont);
    }
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lua_pushlightuserdata(L, cont);
    return 1;
}

/*
??????
@param self ????????
@param page ????
@return self
@usage menu:set_page(main_page)
*/
static int lvgl_menu_set_page(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_menu_set_page(menu, page);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@param page ??????
@return self
@usage item:set_page(settings_page)
*/
static int lvgl_menu_set_item_page(lua_State* L) {
    lv_obj_t* item = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_obj_t* menu = lvgl_menu_find_menu(item);
    if (menu) {
        lv_menu_set_load_page_event(menu, item, page);
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@return userdata ????
@usage local cur = menu:get_cur_page()
*/
static int lvgl_menu_get_cur_page(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = lv_menu_get_cur_main_page(menu);
    lua_pushlightuserdata(L, page);
    return 1;
}

/*
??????
@param self ????????
@param width ????
@return self
@usage menu:set_width(200)
*/
static int lvgl_menu_set_width(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_coord_t width = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_obj_set_width(menu, width);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param title ????
@return self
@usage menu:set_title("????)
*/
static int lvgl_menu_set_title(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    lv_obj_t* page = lv_menu_get_cur_main_page(menu);
    if (page) {
        lv_menu_page_t* menu_page = (lv_menu_page_t*)page;
        if (menu_page->title) {
            lv_mem_free(menu_page->title);
        }
        menu_page->title = lv_mem_alloc(strlen(title) + 1);
        if (menu_page->title) {
            strcpy(menu_page->title, title);
        }
        lv_event_send(menu, LV_EVENT_VALUE_CHANGED, NULL);
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@return self
@usage menu:clear()
*/
static int lvgl_menu_clear(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_menu_set_page(menu, NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? menu ????*/
void lvgl_register_menu(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "page_create", lvgl_menu_page_create);
    REG_METHOD(L, "add_item", lvgl_menu_add_item);
    REG_METHOD(L, "set_page", lvgl_menu_set_page);
    REG_METHOD(L, "set_item_page", lvgl_menu_set_item_page);
    REG_METHOD(L, "get_cur_page", lvgl_menu_get_cur_page);
    REG_METHOD(L, "set_width", lvgl_menu_set_width);
    REG_METHOD(L, "set_title", lvgl_menu_set_title);
    REG_METHOD(L, "clear", lvgl_menu_clear);

    /* ????metatable??(????) */
    menu_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.menu.add_item(menu, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, menu_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.menu) */
    REG_METHOD(L, "create", lvgl_menu_create);
}
