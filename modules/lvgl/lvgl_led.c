/**
 * @file lvgl_led.c
 * @brief LVGL LED控件
 *
 * 实现LVGL LED控件的OO风格Lua绑定，包括LED创建、点亮/熄灭、设置亮度、获取当前亮度等接口，用于状态指示显示。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ledç»ä»¶çmetatableå¼ç¨ */
static int led_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_led_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* led = lv_led_create(parent);
    lua_pushlightuserdata(L, led);
    return 1;
}

/* ==================== LED OOæ¹æ³ ==================== */

/*
åå»ºLEDæ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçLEDå®ä¾
@usage local led = lvgl.led.create(scr)
*/
static int lvgl_led_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_led_create_internal, led_metatable_ref);
}

/*
ç¹äº®LED
@param self LEDå®ä¾ææé?
@return self
@usage led:on()
*/
static int lvgl_led_on(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    lv_led_on(led);
    lua_pushvalue(L, 1);
    return 1;
}

/*
çç­LED
@param self LEDå®ä¾ææé?
@return self
@usage led:off()
*/
static int lvgl_led_off(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    lv_led_off(led);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®LEDäº®åº¦
@param self LEDå®ä¾ææé?
@param bright äº®åº¦å?0-255)
@return self
@usage led:set_brightness(200)
*/
static int lvgl_led_set_brightness(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    uint8_t bright = (uint8_t)luaL_checkinteger(L, 2);
    lv_led_set_brightness(led, bright);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®LEDé¢è²
@param self LEDå®ä¾ææé?
@param color é¢è²å?
@return self
@usage led:set_color(0xFF0000)
*/
static int lvgl_led_set_color(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    lv_led_set_color(led, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åLEDäº®åº¦
@param self LEDå®ä¾ææé?
@return integer äº®åº¦å?0-255)
@usage local bright = led:get_brightness()
*/
static int lvgl_led_get_brightness(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    uint8_t bright = lv_led_get_brightness(led);
    lua_pushinteger(L, bright);
    return 1;
}

/* æ³¨å led å­æ¨¡块*/
void lvgl_register_led(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "on", lvgl_led_on);
    REG_METHOD(L, "off", lvgl_led_off);
    REG_METHOD(L, "set_brightness", lvgl_led_set_brightness);
    REG_METHOD(L, "set_color", lvgl_led_set_color);
    REG_METHOD(L, "get_brightness", lvgl_led_get_brightness);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    led_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.led.on(led, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, led_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.led) */
    REG_METHOD(L, "create", lvgl_led_create);
}
