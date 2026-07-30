/**
 * @file lvgl_dropdown.c
 * @brief LVGL下拉菜单控件
 *
 * 实现LVGL下拉菜单控件的OO风格Lua绑定，包括下拉菜单创建、设置选项（用
分隔）、设置/获取选中项、设置方向、打开/关闭下拉列表等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* dropdownç»ä»¶çmetatableå¼ç¨ */
static int dropdown_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_dropdown_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* dd = lv_dropdown_create(parent);
    lua_pushlightuserdata(L, dd);
    return 1;
}

/* ==================== ä¸æèåOOæ¹æ³ ==================== */

/*
åå»ºä¸æèåæ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçä¸æèåå®ä¾?
@usage local dd = lvgl.dropdown.create(scr)
*/
static int lvgl_dropdown_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_dropdown_create_internal, dropdown_metatable_ref);
}

/*
è®¾ç½®ä¸æèåéé¡¹
@param self ä¸æèåå®ä¾ææé?
@param options éé¡¹å­ç¬¦ä¸?ç¨\nåé)
@return self
@usage dd:set_options("Apple\nBanana\nOrange")
*/
static int lvgl_dropdown_set_options(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_dropdown_set_options(dd, options);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®éæéé¡¹(ä¸å¤å¶å­ç¬¦ä¸²)
@param self ä¸æèåå®ä¾ææé?
@param options éé¡¹å­ç¬¦ä¸?ç¨\nåé)
@return self
@usage dd:set_options_static("A\nB\nC")
*/
static int lvgl_dropdown_set_options_static(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_dropdown_set_options_static(dd, options);
    lua_pushvalue(L, 1);
    return 1;
}

/*
æ·»å éé¡¹
@param self ä¸æèåå®ä¾ææé?
@param option éé¡¹ææ¬
@param pos ä½ç½®(å¯é?é»è®¤æå?
@return self
@usage dd:add_option("Mango", 3)
*/
static int lvgl_dropdown_add_option(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    const char* option = luaL_checkstring(L, 2);
    int32_t pos = (int32_t)luaL_optinteger(L, 3, LV_DROPDOWN_POS_LAST);
    lv_dropdown_add_option(dd, option, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
æ¸é¤ææéé¡¹
@param self ä¸æèåå®ä¾ææé?
@return self
@usage dd:clear_options()
*/
static int lvgl_dropdown_clear_options(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    lv_dropdown_clear_options(dd);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®éä¸­é¡?
@param self ä¸æèåå®ä¾ææé?
@param sel_opt éä¸­é¡¹ç´¢å¼?
@return self
@usage dd:set_selected(1)
*/
static int lvgl_dropdown_set_selected(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    uint16_t sel_opt = (uint16_t)luaL_checkinteger(L, 2);
    lv_dropdown_set_selected(dd, sel_opt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®éä¸­é¡¹é«äº?
@param self ä¸æèåå®ä¾ææé?
@param en æ¯å¦é«äº®
@return self
@usage dd:set_selected_highlight(true)
*/
static int lvgl_dropdown_set_selected_highlight(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_dropdown_set_selected_highlight(dd, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æ¾ç¤ºææ¬
@param self ä¸æèåå®ä¾ææé?
@param txt ææ¬åå®¹
@return self
@usage dd:set_text("è¯·éæ©")
*/
static int lvgl_dropdown_set_text(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_dropdown_set_text(dd, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®ä¸ææ¹å
@param self ä¸æèåå®ä¾ææé?
@param dir æ¹å(lvgl.DIR_BOTTOMç­?
@return self
@usage dd:set_direction(lvgl.DIR_BOTTOM)
*/
static int lvgl_dropdown_set_direction(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 2);
    lv_dropdown_set_dir(dd, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åéä¸­é¡¹ç´¢å¼?
@param self ä¸æèåå®ä¾ææé?
@return integer éä¸­é¡¹ç´¢å¼?
@usage local sel = dd:get_selected()
*/
static int lvgl_dropdown_get_selected(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    uint16_t sel = lv_dropdown_get_selected(dd);
    lua_pushinteger(L, sel);
    return 1;
}

/*
è·åéä¸­é¡¹ææ?
@param self ä¸æèåå®ä¾ææé?
@return string éä¸­é¡¹ææ?
@usage local sel_str = dd:get_selected_str()
*/
static int lvgl_dropdown_get_selected_str(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    char buf[256];
    lv_dropdown_get_selected_str(dd, buf, sizeof(buf));
    lua_pushstring(L, buf);
    return 1;
}

/*
è·åæ¾ç¤ºææ¬
@param self ä¸æèåå®ä¾ææé?
@return string æ¾ç¤ºææ¬
@usage local txt = dd:get_text()
*/
static int lvgl_dropdown_get_text(lua_State* L) {
    lv_obj_t* dd = lvgl_get_obj_ptr(L, 1);
    const char* txt = lv_dropdown_get_text(dd);
    lua_pushstring(L, txt);
    return 1;
}

/* æ³¨å dropdown å­æ¨¡å?*/
void lvgl_register_dropdown(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_options", lvgl_dropdown_set_options);
    REG_METHOD(L, "set_options_static", lvgl_dropdown_set_options_static);
    REG_METHOD(L, "add_option", lvgl_dropdown_add_option);
    REG_METHOD(L, "clear_options", lvgl_dropdown_clear_options);
    REG_METHOD(L, "set_selected", lvgl_dropdown_set_selected);
    REG_METHOD(L, "set_selected_highlight", lvgl_dropdown_set_selected_highlight);
    REG_METHOD(L, "set_text", lvgl_dropdown_set_text);
    REG_METHOD(L, "set_direction", lvgl_dropdown_set_direction);
    REG_METHOD(L, "get_selected", lvgl_dropdown_get_selected);
    REG_METHOD(L, "get_selected_str", lvgl_dropdown_get_selected_str);
    REG_METHOD(L, "get_text", lvgl_dropdown_get_text);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    dropdown_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.dropdown.set_options(dd, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, dropdown_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.dropdown) */
    REG_METHOD(L, "create", lvgl_dropdown_create);
}
