/*
@module  lvgl.arc
@summary LVGLå¼§å½¢æ§ä»¶
@version 2.0
@date    2026.06.18
@author  æ°ç¥ & TRAE & ChatGPT
@tag     Graphics
@usage
-- Luaç¤ºä¾(OOé£æ ¼)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- åå»ºå¼§å½¢æ§ä»¶
local arc = lvgl.arc.create(scr)
arc:set_size(150, 150)
arc:set_pos(75, 75)

-- è®¾ç½®è§åº¦èå´
arc:set_bg_angles(0, 270)
arc:set_angles(0, 135)

-- è®¾ç½®å?
arc:set_value(75)

-- è·åå?
local value = arc:get_value()

-- é¾å¼è°ç¨
local arc2 = lvgl.arc.create(scr):set_size(100, 100):set_pos(200, 100):set_bg_angles(0, 180):set_value(50)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* arcç»ä»¶çmetatableå¼ç¨ */
static int arc_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_arc_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* arc = lv_arc_create(parent);
    lua_pushlightuserdata(L, arc);
    return 1;
}

/* ==================== å¼§å½¢OOæ¹æ³ ==================== */

/*
åå»ºå¼§å½¢æ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçå¼§å½¢å®ä¾?
@usage local arc = lvgl.arc.create(scr)
*/
static int lvgl_arc_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_arc_create_internal, arc_metatable_ref);
}

/*
è®¾ç½®å¼§å½¢å?
@param self å¼§å½¢å®ä¾ææé?
@param value å?
@param anim å¨ç»ä½¿è½(å¯é?é»è®¤0=æ å¨ç?
@return self
@usage arc:set_value(75)
*/
static int lvgl_arc_set_value(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    (void)anim;
    lv_arc_set_value(arc, (int16_t)value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¼§å½¢èå´
@param self å¼§å½¢å®ä¾ææé?
@param min æå°å?
@param max æå¤§å?
@return self
@usage arc:set_range(0, 100)
*/
static int lvgl_arc_set_range(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_arc_set_range(arc, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®èæ¯è§åº¦èå´
@param self å¼§å½¢å®ä¾ææé?
@param start èµ·å§è§åº¦(0-360)
@param end_angle ç»æè§åº¦(0-360)
@return self
@usage arc:set_bg_angles(0, 270)
*/
static int lvgl_arc_set_bg_angles(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t end_angle = (uint16_t)luaL_checkinteger(L, 3);
    lv_arc_set_bg_angles(arc, start, end_angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®åæ¯è§åº¦èå´
@param self å¼§å½¢å®ä¾ææé?
@param start èµ·å§è§åº¦(0-360)
@param end_angle ç»æè§åº¦(0-360)
@return self
@usage arc:set_angles(0, 135)
*/
static int lvgl_arc_set_angles(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t end_angle = (uint16_t)luaL_checkinteger(L, 3);
    lv_arc_set_angles(arc, start, end_angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åå¼§å½¢å?
@param self å¼§å½¢å®ä¾ææé?
@return integer å½åå?
@usage local value = arc:get_value()
*/
static int lvgl_arc_get_value(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_arc_get_value(arc);
    lua_pushinteger(L, value);
    return 1;
}

/*
è·åèµ·å§è§åº¦
@param self å¼§å½¢å®ä¾ææé?
@return integer èµ·å§è§åº¦
@usage local angle = arc:get_angle_start()
*/
static int lvgl_arc_get_angle_start(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = lv_arc_get_angle_start(arc);
    lua_pushinteger(L, angle);
    return 1;
}

/*
è·åç»æè§åº¦
@param self å¼§å½¢å®ä¾ææé?
@return integer ç»æè§åº¦
@usage local angle = arc:get_angle_end()
*/
static int lvgl_arc_get_angle_end(lua_State* L) {
    lv_obj_t* arc = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = lv_arc_get_angle_end(arc);
    lua_pushinteger(L, angle);
    return 1;
}

/* æ³¨å arc å­æ¨¡å?*/
void lvgl_register_arc(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_value", lvgl_arc_set_value);
    REG_METHOD(L, "get_value", lvgl_arc_get_value);
    REG_METHOD(L, "set_range", lvgl_arc_set_range);
    REG_METHOD(L, "set_bg_angles", lvgl_arc_set_bg_angles);
    REG_METHOD(L, "set_angles", lvgl_arc_set_angles);
    REG_METHOD(L, "get_angle_start", lvgl_arc_get_angle_start);
    REG_METHOD(L, "get_angle_end", lvgl_arc_get_angle_end);

    arc_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, arc_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.arc) */
    REG_METHOD(L, "create", lvgl_arc_create);
}
  