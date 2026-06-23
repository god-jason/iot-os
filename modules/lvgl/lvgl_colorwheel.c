/*
@module  lvgl.colorwheel
@summary LVGL??????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??(OO??)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- ??????
local cw = lvgl.colorwheel.create(scr)
cw:set_size(200, 200)
cw:set_pos(60, 20)

-- ????????
cw:set_mode(lvgl.COLORWHEEL_MODE_SATURATION)

-- ??????
local r, g, b = cw:get_rgb()
print(string.format("????: RGB(%d, %d, %d)", r, g, b))

-- ????
cw:set_rgb(255, 0, 0)  -- ??????

-- ????
local c = lvgl.colorwheel.create(scr):set_size(150, 150):set_pos(85, 45):set_mode(lvgl.COLORWHEEL_MODE_HUE)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* colorwheel???metatable?? */
static int colorwheel_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_colorwheel_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    bool knob_recolor = lua_toboolean(L, 2);
    lv_obj_t* cw = lv_colorwheel_create(parent, knob_recolor);
    lua_pushlightuserdata(L, cw);
    return 1;
}

/* ==================== ???OO?? ==================== */

static int lvgl_colorwheel_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_colorwheel_create_internal, colorwheel_metatable_ref);
}

/*
????????
@param self ????????
@param mode ??????
@return self
@usage cw:set_mode(lvgl.COLORWHEEL_MODE_HUE)
*/
static int lvgl_colorwheel_set_mode(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_colorwheel_mode_t mode = (lv_colorwheel_mode_t)luaL_checkinteger(L, 2);
    lv_colorwheel_set_mode(cw, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param angle ??????
@return self
@usage cw:set_angle_offset(45)
*/
static int lvgl_colorwheel_set_angle_offset(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_colorwheel_set_angle_offset() was removed in LVGL 8 */
    (void)cw;
    lua_pushvalue(L, 1);
    return 1;
}

/*
??RGB????
@param self ????????
@return r,g,b ???????????
@usage local r, g, b = cw:get_rgb()
*/
static int lvgl_colorwheel_get_rgb(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_rgb(cw);
    uint32_t c32 = lv_color_to32(color);
    lua_pushinteger(L, (c32 >> 16) & 0xFF);
    lua_pushinteger(L, (c32 >> 8) & 0xFF);
    lua_pushinteger(L, c32 & 0xFF);
    return 3;
}

/*
??RGB????
@param self ????????
@param r ????(0-255)
@param g ????(0-255)
@param b ????(0-255)
@return self
@usage cw:set_rgb(255, 0, 0)
*/
static int lvgl_colorwheel_set_rgb(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    uint8_t r = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t g = (uint8_t)luaL_checkinteger(L, 3);
    uint8_t b = (uint8_t)luaL_checkinteger(L, 4);
    lv_color_t color = lv_color_make(r, g, b);
    lv_colorwheel_set_rgb(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@return integer ??????
@usage local color = cw:get_color()
*/
static int lvgl_colorwheel_get_color(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_colorwheel_get_rgb(cw);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
??????????
@param self ????????
@param color_val ??????
@return self
@usage cw:set_color(0xFF0000)
*/
static int lvgl_colorwheel_set_color(lua_State* L) {
    lv_obj_t* cw = lvgl_get_obj_ptr(L, 1);
    uint32_t color_val = (uint32_t)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = color_val;
    lv_colorwheel_set_rgb(cw, color);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? colorwheel ????*/
void lvgl_register_colorwheel(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_mode", lvgl_colorwheel_set_mode);
    REG_METHOD(L, "set_angle_offset", lvgl_colorwheel_set_angle_offset);
    REG_METHOD(L, "get_rgb", lvgl_colorwheel_get_rgb);
    REG_METHOD(L, "set_rgb", lvgl_colorwheel_set_rgb);
    REG_METHOD(L, "get_color", lvgl_colorwheel_get_color);
    REG_METHOD(L, "set_color", lvgl_colorwheel_set_color);

    /* ????metatable??(????) */
    colorwheel_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.colorwheel.set_mode(cw, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, colorwheel_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.colorwheel) */
    REG_METHOD(L, "create", lvgl_colorwheel_create);
}
