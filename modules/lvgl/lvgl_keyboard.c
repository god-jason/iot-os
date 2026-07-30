/**
 * @file lvgl_keyboard.c
 * @brief LVGL键盘控件
 *
 * 实现LVGL键盘控件的OO风格Lua绑定，包括键盘创建、设置关联文本区、设置键盘模式（文本/数字/用户）、获取关联文本区等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* keyboardç»ä»¶çmetatableå¼ç¨ */
static int keyboard_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_keyboard_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* kb = lv_keyboard_create(parent);
    lua_pushlightuserdata(L, kb);
    return 1;
}

/* ==================== é®çOOæ¹æ³ ==================== */

/*
åå»ºé®çæ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçé®çå®ä¾?
@usage local kb = lvgl.keyboard.create(scr)
*/
static int lvgl_keyboard_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_keyboard_create_internal, keyboard_metatable_ref);
}

/*
è®¾ç½®å³èçææ¬åºå?
@param self é®çå®ä¾ææé?
@param ta ææ¬åºåå¯¹è±¡
@return self
@usage kb:set_textarea(ta)
*/
static int lvgl_keyboard_set_textarea(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_keyboard_set_textarea(kb, ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®é®çæ¨¡å¼
@param self é®çå®ä¾ææé?
@param mode é®çæ¨¡å¼(lvgl.KEYBOARD_MODE_TEXTç­?
@return self
@usage kb:set_mode(lvgl.KEYBOARD_MODE_TEXT)
*/
static int lvgl_keyboard_set_mode(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = (lv_keyboard_mode_t)luaL_checkinteger(L, 2);
    lv_keyboard_set_mode(kb, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®é®çæ å°(ææªå®ç°)
@param self é®çå®ä¾ææé?
@return self
@usage kb:set_map(map)
*/
static int lvgl_keyboard_set_map(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åå³èçææ¬åºå?
@param self é®çå®ä¾ææé?
@return userdata ææ¬åºåå¯¹è±¡
@usage local ta = kb:get_textarea()
*/
static int lvgl_keyboard_get_textarea(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = lv_keyboard_get_textarea(kb);
    lua_pushlightuserdata(L, ta);
    return 1;
}

/*
è·åé®çæ¨¡å¼
@param self é®çå®ä¾ææé?
@return integer é®çæ¨¡å¼
@usage local mode = kb:get_mode()
*/
static int lvgl_keyboard_get_mode(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = lv_keyboard_get_mode(kb);
    lua_pushinteger(L, mode);
    return 1;
}

/* æ³¨å keyboard å­æ¨¡å?*/
void lvgl_register_keyboard(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_textarea", lvgl_keyboard_set_textarea);
    REG_METHOD(L, "set_mode", lvgl_keyboard_set_mode);
    REG_METHOD(L, "set_map", lvgl_keyboard_set_map);
    REG_METHOD(L, "get_textarea", lvgl_keyboard_get_textarea);
    REG_METHOD(L, "get_mode", lvgl_keyboard_get_mode);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    keyboard_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.keyboard.set_textarea(kb, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.keyboard) */
    REG_METHOD(L, "create", lvgl_keyboard_create);
}
