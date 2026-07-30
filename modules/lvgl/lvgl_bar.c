/**
 * @file lvgl_bar.c
 * @brief LVGL进度条控件
 *
 * 实现LVGL进度条控件的OO风格Lua绑定，包括进度条创建、设置值（支持动画）、设置范围、设置模式（正常/反向/对称）、获取当前值/最小值/最大值等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* barç»ä»¶çmetatableå¼ç¨ */
static int bar_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_bar_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* bar = lv_bar_create(parent);
    lua_pushlightuserdata(L, bar);
    return 1;
}

/* ==================== è¿åº¦æ¡OOæ¹æ³ ==================== */

/*
åå»ºè¿åº¦æ¡æ§ä»?OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçè¿åº¦æ¡å®ä¾
@usage local bar = lvgl.bar.create(scr)
*/
static int lvgl_bar_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_bar_create_internal, bar_metatable_ref);
}

/*
è®¾ç½®è¿åº¦æ¡å?
@param self è¿åº¦æ¡å®ä¾ææé
@param value å?
@param anim å¨ç»ä½¿è½(å¯é?é»è®¤0=æ å¨ç?
@return self
@usage bar:set_value(50, 0)
*/
static int lvgl_bar_set_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_bar_set_value(bar, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®è¿åº¦æ¡èå?
@param self è¿åº¦æ¡å®ä¾ææé
@param min æå°å?
@param max æå¤§å?
@return self
@usage bar:set_range(0, 100)
*/
static int lvgl_bar_set_range(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_bar_set_range(bar, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®è¿åº¦æ¡æ¨¡å¼?
@param self è¿åº¦æ¡å®ä¾ææé
@param mode æ¨¡å¼: BAR_MODE_NORMAL, BAR_MODE_SYMMETRICAL, BAR_MODE_REVERSE
@return self
@usage bar:set_mode(lvgl.BAR_MODE_NORMAL)
*/
static int lvgl_bar_set_mode(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    lv_bar_mode_t mode = (lv_bar_mode_t)luaL_checkinteger(L, 2);
    lv_bar_set_mode(bar, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®è¿åº¦æ¡èµ·å§å?
@param self è¿åº¦æ¡å®ä¾ææé
@param value èµ·å§å?
@param anim å¨ç»ä½¿è½(å¯é?é»è®¤0=æ å¨ç?
@return self
@usage bar:set_start_value(20, 0)
*/
static int lvgl_bar_set_start_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åè¿åº¦æ¡å?
@param self è¿åº¦æ¡å®ä¾ææé
@return integer å½åå?
@usage local value = bar:get_value()
*/
static int lvgl_bar_get_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
è·åè¿åº¦æ¡èµ·å§å?
@param self è¿åº¦æ¡å®ä¾ææé
@return integer èµ·å§å?
@usage local value = bar:get_start_value()
*/
static int lvgl_bar_get_start_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_start_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
è·åè¿åº¦æ¡æå°å?
@param self è¿åº¦æ¡å®ä¾ææé
@return integer æå°å?
@usage local min = bar:get_min_value()
*/
static int lvgl_bar_get_min_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_min_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/*
è·åè¿åº¦æ¡æå¤§å?
@param self è¿åº¦æ¡å®ä¾ææé
@return integer æå¤§å?
@usage local max = bar:get_max_value()
*/
static int lvgl_bar_get_max_value(lua_State* L) {
    lv_obj_t* bar = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_bar_get_max_value(bar);
    lua_pushinteger(L, value);
    return 1;
}

/* æ³¨å bar å­æ¨¡块*/
void lvgl_register_bar(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_value", lvgl_bar_set_value);
    REG_METHOD(L, "get_value", lvgl_bar_get_value);
    REG_METHOD(L, "set_range", lvgl_bar_set_range);
    REG_METHOD(L, "set_mode", lvgl_bar_set_mode);
    REG_METHOD(L, "set_start_value", lvgl_bar_set_start_value);
    REG_METHOD(L, "get_start_value", lvgl_bar_get_start_value);
    REG_METHOD(L, "get_min_value", lvgl_bar_get_min_value);
    REG_METHOD(L, "get_max_value", lvgl_bar_get_max_value);

    bar_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, bar_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.bar) */
    REG_METHOD(L, "create", lvgl_bar_create);
}
