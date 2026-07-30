/**
 * @file lvgl_checkbox.c
 * @brief LVGL复选框控件
 *
 * 实现LVGL复选框控件的OO风格Lua绑定，包括复选框创建、设置/获取文本、设置/获取状态（未选中/选中/不确定）、设置静态文本等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* checkboxç»ä»¶çmetatableå¼ç¨ */
static int checkbox_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_checkbox_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cb = lv_checkbox_create(parent);
    lua_pushlightuserdata(L, cb);
    return 1;
}

/* ==================== å¤éæ¡OOæ¹æ³ ==================== */

/*
åå»ºå¤éæ¡æ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçå¤éæ¡å®ä¾
@usage local cb = lvgl.checkbox.create(scr)
*/
static int lvgl_checkbox_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_checkbox_create_internal, checkbox_metatable_ref);
}

/*
è®¾ç½®å¤éæ¡ææ¬
@param self å¤éæ¡å®ä¾ææé?
@param text ææ¬åå®¹
@return self
@usage cb:set_text("åæåè®®")
*/
static int lvgl_checkbox_set_text(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_checkbox_set_text(cb, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¤éæ¡éæææ?ä¸æ·è´å­ç¬¦ä¸²)
@param self å¤éæ¡å®ä¾ææé?
@param text ææ¬åå®¹(å¿é¡»ä¿æææ)
@return self
@usage cb:set_text_static("éé¡¹")
*/
static int lvgl_checkbox_set_text_static(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_checkbox_set_text_static(cb, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¤éæ¡éä¸­ç¶æ?
@param self å¤éæ¡å®ä¾ææé?
@param checked æ¯å¦éä¸­
@return self
@usage cb:set_checked(true)
*/
static int lvgl_checkbox_set_checked(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    bool checked = lua_toboolean(L, 2);
    lv_checkbox_set_checked(cb, checked);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¤éæ¡ç¶æ?
@param self å¤éæ¡å®ä¾ææé?
@param state ç¶æå? CHECKBOX_STATE_UNCHECKED, CHECKBOX_STATE_CHECKED, CHECKBOX_STATE_TRISTATE
@return self
@usage cb:set_state(lvgl.CHECKBOX_STATE_CHECKED)
*/
static int lvgl_checkbox_set_state(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    lv_checkbox_state_t state = (lv_checkbox_state_t)luaL_checkinteger(L, 2);
    lv_checkbox_set_state(cb, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åå¤éæ¡ææ¬
@param self å¤éæ¡å®ä¾ææé?
@return string ææ¬åå®¹
@usage local text = cb:get_text()
*/
static int lvgl_checkbox_get_text(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    const char* text = lv_checkbox_get_text(cb);
    lua_pushstring(L, text ? text : "");
    return 1;
}

/*
æ£æ¥å¤éæ¡æ¯å¦éä¸­
@param self å¤éæ¡å®ä¾ææé?
@return boolean æ¯å¦éä¸­
@usage local checked = cb:is_checked()
*/
static int lvgl_checkbox_is_checked(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    bool checked = lv_checkbox_is_checked(cb);
    lua_pushboolean(L, checked);
    return 1;
}

/*
è·åå¤éæ¡ç¶æ?
@param self å¤éæ¡å®ä¾ææé?
@return integer ç¶æå?
@usage local state = cb:get_state()
*/
static int lvgl_checkbox_get_state(lua_State* L) {
    lv_obj_t* cb = lvgl_get_obj_ptr(L, 1);
    lv_checkbox_state_t state = lv_checkbox_get_state(cb);
    lua_pushinteger(L, state);
    return 1;
}

/* æ³¨å checkbox å­æ¨¡å?*/
void lvgl_register_checkbox(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_text", lvgl_checkbox_set_text);
    REG_METHOD(L, "set_text_static", lvgl_checkbox_set_text_static);
    REG_METHOD(L, "set_checked", lvgl_checkbox_set_checked);
    REG_METHOD(L, "set_state", lvgl_checkbox_set_state);
    REG_METHOD(L, "get_text", lvgl_checkbox_get_text);
    REG_METHOD(L, "is_checked", lvgl_checkbox_is_checked);
    REG_METHOD(L, "get_state", lvgl_checkbox_get_state);

    checkbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, checkbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.checkbox) */
    REG_METHOD(L, "create", lvgl_checkbox_create);
}
