/**
 * @file lvgl_slider.c
 * @brief LVGL滑块控件
 *
 * 实现LVGL滑块控件的OO风格Lua绑定，包括滑块创建、设置值（支持动画）、设置范围、设置模式、获取当前值、检查是否正在拖动等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* sliderç»ä»¶çmetatableå¼ç¨ */
static int slider_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_slider_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* slider = lv_slider_create(parent);
    lua_pushlightuserdata(L, slider);
    return 1;
}

/* ==================== æ»åOOæ¹æ³ ==================== */

/*
åå»ºæ»åæ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçæ»åå®ä¾?
@usage local slider = lvgl.slider.create(scr)
*/
static int lvgl_slider_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_slider_create_internal, slider_metatable_ref);
}

/*
è®¾ç½®æ»åå?
@param self æ»åå®ä¾ææé?
@param value å?
@param anim å¨ç»ä½¿è½(å¯é?é»è®¤0=æ å¨ç?
@return self
@usage slider:set_value(50, 0)
*/
static int lvgl_slider_set_value(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_slider_set_value(slider, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æ»åèå´
@param self æ»åå®ä¾ææé?
@param min æå°å?
@param max æå¤§å?
@return self
@usage slider:set_range(0, 100)
*/
static int lvgl_slider_set_range(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_slider_set_range(slider, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æ»åæ¨¡å¼
@param self æ»åå®ä¾ææé?
@param mode æ¨¡å¼: SLIDER_MODE_NORMAL(0), SLIDER_MODE_SYMMETRICAL(1), SLIDER_MODE_REVERSE(2)
@return self
@usage slider:set_mode(lvgl.SLIDER_MODE_NORMAL)
*/
static int lvgl_slider_set_mode(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    lv_slider_mode_t mode = (lv_slider_mode_t)luaL_checkinteger(L, 2);
    lv_slider_set_mode(slider, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åæ»åå?
@param self æ»åå®ä¾ææé?
@return integer å½åå?
@usage local value = slider:get_value()
*/
static int lvgl_slider_get_value(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_slider_get_value(slider);
    lua_pushinteger(L, value);
    return 1;
}

/*
æ£æ¥æ»åæ¯å¦æ­£å¨æå?
@param self æ»åå®ä¾ææé?
@return boolean æ¯å¦æå¨ä¸?
@usage local dragging = slider:is_dragged()
*/
static int lvgl_slider_is_dragged(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    bool dragged = lv_slider_is_dragged(slider);
    lua_pushboolean(L, dragged);
    return 1;
}

/* æ³¨å slider å­æ¨¡块*/
void lvgl_register_slider(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_value", lvgl_slider_set_value);
    REG_METHOD(L, "get_value", lvgl_slider_get_value);
    REG_METHOD(L, "set_range", lvgl_slider_set_range);
    REG_METHOD(L, "set_mode", lvgl_slider_set_mode);
    REG_METHOD(L, "is_dragged", lvgl_slider_is_dragged);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    slider_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.slider.set_value(slider, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, slider_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.slider) */
    REG_METHOD(L, "create", lvgl_slider_create);
}
