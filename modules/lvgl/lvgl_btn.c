/**
 * @file lvgl_btn.c
 * @brief LVGL按钮控件
 *
 * 实现LVGL按钮控件的OO风格Lua绑定，包括按钮创建、设置/获取文本、设置可选中/切换、设置布局、设置点击事件、切换状态等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* btnç»ä»¶çmetatableå¼ç¨ */
static int btn_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

/* å®éåå»ºæé®çå½æ?è¢«OOåè£å¨è°ç? */
static int lvgl_btn_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = lv_btn_create(parent);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/* ==================== æé®OOæ¹æ³ ==================== */

/*
åå»ºæé®(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçæé®å®ä¾?
@usage local btn = lvgl.btn.create(scr)
*/
static int lvgl_btn_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_btn_create_internal, btn_metatable_ref);
}

/*
è®¾ç½®æé®ææ¬
@param self æé®å®ä¾ææé?
@param text ææ¬åå®¹
@return self
@usage btn:set_text("OK")
*/
static int lvgl_btn_set_text(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_btn_set_text(btn, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åæé®ææ¬
@param self æé®å®ä¾ææé?
@return string ææ¬åå®¹
@usage local text = btn:get_text()
*/
static int lvgl_btn_get_text(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    const char* text = lv_btn_get_text(btn);
    lua_pushstring(L, text);
    return 1;
}

/*
è®¾ç½®æé®ç¶æ?
@param self æé®å®ä¾ææé?
@param state ç¶æå?
@return self
@usage btn:set_state(lvgl.BTN_STATE_PRESSED)
*/
static int lvgl_btn_set_state(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_state_t state = (lv_btn_state_t)luaL_checkinteger(L, 2);
    lv_btn_set_state(btn, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åæé®ç¶æ?
@param self æé®å®ä¾ææé?
@return integer ç¶æå?
@usage local state = btn:get_state()
*/
static int lvgl_btn_get_state(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_state_t state = lv_btn_get_state(btn);
    lua_pushinteger(L, state);
    return 1;
}

/*
åæ¢æé®ç¶æ?
@param self æé®å®ä¾ææé?
@return self
@usage btn:toggle()
*/
static int lvgl_btn_toggle(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_toggle(btn);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æ¯å¦å¯åæ?
@param self æé®å®ä¾ææé?
@param en æ¯å¦å¯åæ?
@return self
@usage btn:set_checkable(true)
*/
static int lvgl_btn_set_checkable(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_btn_set_checkable(btn, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æé®å¸å±
@param self æé®å®ä¾ææé?
@param layout å¸å±ç±»å
@return self
@usage btn:set_layout(lvgl.LAYOUT_CENTER)
*/
static int lvgl_btn_set_layout(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    uint32_t layout = (uint32_t)luaL_checkinteger(L, 2);
    lv_btn_set_layout(btn, layout);
    lua_pushvalue(L, 1);
    return 1;
}

/*
æ¸é¤æé®å¸å±
@param self æé®å®ä¾ææé?
@return self
@usage btn:clear_layout()
*/
static int lvgl_btn_clear_layout(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_set_layout(btn, LV_LAYOUT_NONE);
    lua_pushvalue(L, 1);
    return 1;
}

/* æ³¨å btn å­æ¨¡å?*/
void lvgl_register_btn(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);
    
    /* æ³¨åOOé£æ ¼æ¹æ³(å¯ä»¥instance:method()è°ç¨) */
    REG_METHOD(L, "set_text", lvgl_btn_set_text);
    REG_METHOD(L, "get_text", lvgl_btn_get_text);
    REG_METHOD(L, "set_state", lvgl_btn_set_state);
    REG_METHOD(L, "get_state", lvgl_btn_get_state);
    REG_METHOD(L, "toggle", lvgl_btn_toggle);
    REG_METHOD(L, "set_checkable", lvgl_btn_set_checkable);
    REG_METHOD(L, "set_layout", lvgl_btn_set_layout);
    REG_METHOD(L, "clear_layout", lvgl_btn_clear_layout);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    btn_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.btn.set_text(btn, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, btn_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.btn) */
    REG_METHOD(L, "create", lvgl_btn_create);
}
