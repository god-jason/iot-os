/*
@module  lvgl.meter
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
local meter = lvgl.meter.create(scr)
meter:set_size(200, 200)
meter:set_pos(60, 20)

-- ????
local scale = meter:add_scale(270, 0)  -- 270????

-- ??????
meter:set_scale_range(scale, 0, 100, 270, 225)

-- ??????
meter:set_scale_ticks(scale, 11, 2, 5, 0x000000)

-- ??????
meter:set_scale_major_ticks(scale, 4, 2, 5, 0xFF0000)

-- ????????????
local indic = meter:add_indicator_line(scale, 3, 0x0000FF, 10)

-- ??????
meter:set_indicator_value(indic, 50)

-- ????????
local arc_indic = meter:add_indicator_arc(scale, 10, 0x00FF00, 5)

-- ??????
meter:set_indicator_value(arc_indic, 75)

-- ????
local m = lvgl.meter.create(scr):set_size(150, 150):set_pos(80, 30)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* meter???metatable?? */
static int meter_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_meter_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* meter = lv_meter_create(parent);
    lua_pushlightuserdata(L, meter);
    return 1;
}

/* ==================== ???OO?? ==================== */

/*
????????OO??)
@param self ???????
@return userdata ?metatable??????
@usage local meter = lvgl.meter.create(scr)
*/
static int lvgl_meter_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_meter_create_internal, meter_metatable_ref);
}

/*
????
@param self ????????
@param angle_range ????
@param rotation ????(?????0)
@return userdata ????
@usage local scale = meter:add_scale(270, 0)
*/
static int lvgl_meter_add_scale(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    int32_t angle_range = (int32_t)luaL_checkinteger(L, 2);
    int32_t rotation = (int32_t)luaL_optinteger(L, 3, 0);
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, 0, 100, angle_range, rotation);
    lua_pushlightuserdata(L, scale);
    return 1;
}

/*
??????
@param self ????????
@param scale ????
@param min ????
@param max ????
@param angle_range ????
@param rotation ????(???
@return self
@usage meter:set_scale_range(scale, 0, 100, 270, 225)
*/
static int lvgl_meter_set_scale_range(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    int32_t min = (int32_t)luaL_checkinteger(L, 3);
    int32_t max = (int32_t)luaL_checkinteger(L, 4);
    int32_t angle_range = (int32_t)luaL_checkinteger(L, 5);
    int32_t rotation = (int32_t)luaL_optinteger(L, 6, 0);
    lv_meter_set_scale_range(meter, scale, min, max, angle_range, rotation);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param scale ????
@param cnt ??????
@param len ??????
@param width ???????????2)
@param color ?????????????)
@return self
@usage meter:set_scale_ticks(scale, 11, 2, 5, 0x000000)
*/
static int lvgl_meter_set_scale_ticks(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t cnt = (uint16_t)luaL_checkinteger(L, 3);
    uint16_t len = (uint16_t)luaL_checkinteger(L, 4);
    uint16_t width = (uint16_t)luaL_optinteger(L, 5, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_optinteger(L, 6, 0x000000);
    lv_meter_set_scale_ticks(meter, scale, cnt, width, len, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param scale ????
@param nth ?N????????
@param len ??????
@param width ?????????
@param color ?????????
@return self
@usage meter:set_scale_major_ticks(scale, 4, 2, 5, 0xFF0000)
*/
static int lvgl_meter_set_scale_major_ticks(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t nth = (uint16_t)luaL_checkinteger(L, 3);
    uint16_t len = (uint16_t)luaL_checkinteger(L, 4);
    uint16_t width = (uint16_t)luaL_optinteger(L, 5, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_optinteger(L, 6, 0x000000);
    int16_t label_gap = (int16_t)luaL_optinteger(L, 7, 0);
    lv_meter_set_scale_major_ticks(meter, scale, nth, width, len, color, label_gap);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param scale ????
@param width ????
@param color ????
@param length ????(???
@return userdata ??????
@usage local indic = meter:add_indicator_line(scale, 3, 0x0000FF, 10)
*/
static int lvgl_meter_add_indicator_line(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 3);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 4);
    int16_t r_mod = (int16_t)luaL_optinteger(L, 5, 0);
    lv_meter_indicator_t* indic = lv_meter_add_needle_line(meter, scale, width, color, r_mod);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
????????
@param self ????????
@param scale ????
@param width ????
@param color ????
@param length ????(???
@return userdata ??????
@usage local arc_indic = meter:add_indicator_arc(scale, 10, 0x00FF00, 5)
*/
static int lvgl_meter_add_indicator_arc(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 3);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 4);
    int16_t r_mod = (int16_t)luaL_optinteger(L, 5, 0);
    lv_meter_indicator_t* indic = lv_meter_add_arc(meter, scale, width, color, r_mod);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
????????
@param self ????????
@param scale ????
@param color ????
@param width ????(?????4)
@return userdata ??????
@usage local needle = meter:add_indicator_needle(scale, 0xFF0000, 4)
*/
static int lvgl_meter_add_indicator_needle(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_scale_t* scale = (lv_meter_scale_t*)luaL_checklightuserdata(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);
    uint16_t width = (uint16_t)luaL_optinteger(L, 4, 4);
    lv_meter_indicator_t* indic = lv_meter_add_needle_line(meter, scale, width, color, 0);
    lua_pushlightuserdata(L, indic);
    return 1;
}

/*
???????
@param self ????????
@param indic ??????
@param value ??
@return self
@usage meter:set_indicator_value(indic, 50)
*/
static int lvgl_meter_set_indicator_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
?????????
@param self ????????
@param indic ??????
@param value ????
@return self
@usage meter:set_indicator_start_value(indic, 0)
*/
static int lvgl_meter_set_indicator_start_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_start_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
?????????
@param self ????????
@param indic ??????
@param value ????
@return self
@usage meter:set_indicator_end_value(indic, 100)
*/
static int lvgl_meter_set_indicator_end_value(lua_State* L) {
    lv_obj_t* meter = lvgl_get_obj_ptr(L, 1);
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_meter_set_indicator_end_value(meter, indic, value);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? meter ????*/
void lvgl_register_meter(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "add_scale", lvgl_meter_add_scale);
    REG_METHOD(L, "set_scale_range", lvgl_meter_set_scale_range);
    REG_METHOD(L, "set_scale_ticks", lvgl_meter_set_scale_ticks);
    REG_METHOD(L, "set_scale_major_ticks", lvgl_meter_set_scale_major_ticks);
    REG_METHOD(L, "add_indicator_line", lvgl_meter_add_indicator_line);
    REG_METHOD(L, "add_indicator_arc", lvgl_meter_add_indicator_arc);
    REG_METHOD(L, "add_indicator_needle", lvgl_meter_add_indicator_needle);
    REG_METHOD(L, "set_indicator_value", lvgl_meter_set_indicator_value);
    REG_METHOD(L, "set_indicator_start_value", lvgl_meter_set_indicator_start_value);
    REG_METHOD(L, "set_indicator_end_value", lvgl_meter_set_indicator_end_value);

    /* ????metatable??(????) */
    meter_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.meter.add_scale(meter, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, meter_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.meter) */
    REG_METHOD(L, "create", lvgl_meter_create);
}
