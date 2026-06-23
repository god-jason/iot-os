/*
@module  lvgl.spinbox
@summary LVGLæ°å¼æ¡æ§ä»¶
@version 2.0
@date    2026.06.18
@author  æ°ç¥ & TRAE & ChatGPT
@tag     Graphics
@usage
-- Luaç¤ºä¾(OOé£æ ¼)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- åå»ºæ°å¼æ¡
local spinbox = lvgl.spinbox.create(scr)
spinbox:set_size(100, 40)
spinbox:set_pos(110, 50)

-- è®¾ç½®èå´
spinbox:set_range(0, 100)

-- è®¾ç½®å½åå?
spinbox:set_value(50)

-- è®¾ç½®æ­¥è¿å?
spinbox:set_step(1)

-- è®¾ç½®å°æ°ä½æ°
spinbox:set_digit_format(2, 0)  -- 2ä½å°æ?0ä½ææ?

-- è·åå½åå?
local value = spinbox:get_value()

-- å¢é/åé
spinbox:increment()
spinbox:decrement()

-- é¾å¼è°ç¨
local sb = lvgl.spinbox.create(scr):set_size(120, 50):set_pos(50, 100):set_range(0, 255):set_value(128)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* spinboxç»ä»¶çmetatableå¼ç¨ */
static int spinbox_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_spinbox_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* spinbox = lv_spinbox_create(parent);
    lua_pushlightuserdata(L, spinbox);
    return 1;
}

/* ==================== æ°å¼æ¡OOæ¹æ³ ==================== */

static int lvgl_spinbox_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_spinbox_create_internal, spinbox_metatable_ref);
}

static int lvgl_spinbox_set_value(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_spinbox_set_value(spinbox, value);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_get_value(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_spinbox_get_value(spinbox);
    lua_pushinteger(L, value);
    return 1;
}

static int lvgl_spinbox_set_range(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_spinbox_set_range(spinbox, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_step(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    uint32_t step = (uint32_t)luaL_checkinteger(L, 2);
    lv_spinbox_set_step(spinbox, step);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_digit_format(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    uint8_t digit_count = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t separator_position = (uint8_t)luaL_checkinteger(L, 3);
    lv_spinbox_set_digit_format(spinbox, digit_count, separator_position);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_set_scroll_speed(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_spinbox_set_scroll_speed() was removed in LVGL 8 */
    (void)spinbox;
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_increment(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    lv_spinbox_increment(spinbox);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinbox_decrement(lua_State* L) {
    lv_obj_t* spinbox = lvgl_get_obj_ptr(L, 1);
    lv_spinbox_decrement(spinbox);
    lua_pushvalue(L, 1);
    return 1;
}

/* æ³¨å spinbox å­æ¨¡å?*/
void lvgl_register_spinbox(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_value", lvgl_spinbox_set_value);
    REG_METHOD(L, "get_value", lvgl_spinbox_get_value);
    REG_METHOD(L, "set_range", lvgl_spinbox_set_range);
    REG_METHOD(L, "set_step", lvgl_spinbox_set_step);
    REG_METHOD(L, "set_digit_format", lvgl_spinbox_set_digit_format);
    REG_METHOD(L, "set_scroll_speed", lvgl_spinbox_set_scroll_speed);
    REG_METHOD(L, "increment", lvgl_spinbox_increment);
    REG_METHOD(L, "decrement", lvgl_spinbox_decrement);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    spinbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.spinbox.set_value(sb, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, spinbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.spinbox) */
    REG_METHOD(L, "create", lvgl_spinbox_create);
}
