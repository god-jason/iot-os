/*
@module  lvgl.led
@summary LVGL LED控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建LED
local led = lvgl.led.create(scr)
led:set_size(30, 30)
led:set_pos(50, 50)

-- 设置LED亮度(on/off中间的值)
led:set_brightness(255)  -- 最亮
led:set_brightness(0)    -- 熄灭

-- 点亮/熄灭
led:on()
led:off()

-- 获取LED当前亮度
local bright = led:get_brightness()

-- 链式调用
local led2 = lvgl.led.create(scr):set_size(20, 20):set_pos(100, 50):set_brightness(200):on()
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* led组件的metatable引用 */
static int led_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_led_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* led = lv_led_create(parent);
    lua_pushlightuserdata(L, led);
    return 1;
}

/* ==================== LED OO方法 ==================== */

/*
创建LED控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的LED实例
@usage local led = lvgl.led.create(scr)
*/
static int lvgl_led_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_led_create_internal, led_metatable_ref);
}

/*
点亮LED
@param self LED实例或指针
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
熄灭LED
@param self LED实例或指针
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
设置LED亮度
@param self LED实例或指针
@param bright 亮度值(0-255)
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
设置LED颜色
@param self LED实例或指针
@param color 颜色值
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
获取LED亮度
@param self LED实例或指针
@return integer 亮度值(0-255)
@usage local bright = led:get_brightness()
*/
static int lvgl_led_get_brightness(lua_State* L) {
    lv_obj_t* led = lvgl_get_obj_ptr(L, 1);
    uint8_t bright = lv_led_get_brightness(led);
    lua_pushinteger(L, bright);
    return 1;
}

/* 注册 led 子模块 */
void lvgl_register_led(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "on", lvgl_led_on);
    REG_METHOD(L, "off", lvgl_led_off);
    REG_METHOD(L, "set_brightness", lvgl_led_set_brightness);
    REG_METHOD(L, "set_color", lvgl_led_set_color);
    REG_METHOD(L, "get_brightness", lvgl_led_get_brightness);

    /* 保存组件metatable引用(用于继承) */
    led_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.led.on(led, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, led_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.led) */
    REG_METHOD(L, "create", lvgl_led_create);
}
