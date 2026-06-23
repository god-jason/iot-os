/*
@module  lvgl.chart
@summary LVGL????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??(OO??)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- ????
local chart = lvgl.chart.create(scr)
chart:set_size(300, 200)
chart:set_pos(10, 20)

-- ??????
chart:set_type(lvgl.CHART_TYPE_LINE)

-- ??????
chart:set_point_count(10)

-- ????
chart:set_range(lvgl.CHART_AXIS_PRIMARY_Y, 0, 100)
chart:set_range(lvgl.CHART_AXIS_PRIMARY_X, 0, 10)

-- ??????
local series = chart:add_series(0xFF0000)  -- ??

-- ??????
chart:set_all_value(series, 50)

-- ??????
for i = 0, 9 do
    chart:set_value_by_id(series, i, math.random(0, 100))
end

-- ????
chart:refresh()

-- ????
local c = lvgl.chart.create(scr):set_size(200, 150):set_pos(50, 40):set_type(lvgl.CHART_TYPE_LINE)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* chart???metatable?? */
static int chart_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_chart_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* chart = lv_chart_create(parent);
    lua_pushlightuserdata(L, chart);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local chart = lvgl.chart.create(scr)
*/
static int lvgl_chart_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_chart_create_internal, chart_metatable_ref);
}

/*
??????
@param self ????????
@param type ????(lvgl.CHART_TYPE_LINE??
@return self
@usage chart:set_type(lvgl.CHART_TYPE_LINE)
*/
static int lvgl_chart_set_type(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_type_t type = (lv_chart_type_t)luaL_checkinteger(L, 2);
    lv_chart_set_type(chart, type);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param axis ????lvgl.CHART_AXIS_PRIMARY_Y??
@param min ????
@param max ????
@return self
@usage chart:set_range(lvgl.CHART_AXIS_PRIMARY_Y, 0, 100)
*/
static int lvgl_chart_set_range(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_checkinteger(L, 2);
    int32_t min = (int32_t)luaL_checkinteger(L, 3);
    int32_t max = (int32_t)luaL_checkinteger(L, 4);
    lv_chart_set_range(chart, axis, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param cnt ????
@return self
@usage chart:set_point_count(10)
*/
static int lvgl_chart_set_point_count(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    uint16_t cnt = (uint16_t)luaL_checkinteger(L, 2);
    lv_chart_set_point_count(chart, cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param hdiv ????????
@param vdiv ????????
@return self
@usage chart:set_div_line_count(3, 5)
*/
static int lvgl_chart_set_div_line_count(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    uint8_t hdiv = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t vdiv = (uint8_t)luaL_checkinteger(L, 3);
    lv_chart_set_div_line_count(chart, hdiv, vdiv);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param mode ????
@return self
@usage chart:set_update_mode(lvgl.CHART_UPDATE_MODE_SHIFT)
*/
static int lvgl_chart_set_update_mode(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_update_mode_t mode = (lv_chart_update_mode_t)luaL_checkinteger(L, 2);
    lv_chart_set_update_mode(chart, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param axis ????
@param major_len ??????
@param minor_len ??????
@param major_cnt ??????
@param minor_cnt ??????
@param label_en ??????
@return self
@usage chart:set_axis_tick(lvgl.CHART_AXIS_PRIMARY_Y, 10, 5, 3, 2, true)
*/
static int lvgl_chart_set_axis_tick(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_checkinteger(L, 2);
    int32_t major_len = (int32_t)luaL_checkinteger(L, 3);
    int32_t minor_len = (int32_t)luaL_checkinteger(L, 4);
    int32_t major_cnt = (int32_t)luaL_checkinteger(L, 5);
    int32_t minor_cnt = (int32_t)luaL_checkinteger(L, 6);
    bool label_en = lua_toboolean(L, 7);
    int32_t draw_size = (int32_t)luaL_optinteger(L, 8, 20);
    lv_chart_set_axis_tick(chart, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param color ????
@return userdata ????
@usage local series = chart:add_series(0xFF0000)
*/
static int lvgl_chart_add_series(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_optinteger(L, 3, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* series = lv_chart_add_series(chart, color, axis);
    lua_pushlightuserdata(L, series);
    return 1;
}

/*
??????
@param self ????????
@param series ????
@param color ????
@return self
@usage chart:set_series_color(series, 0x00FF00)
*/
static int lvgl_chart_set_series_color(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);
    lv_chart_set_series_color(chart, series, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param series ????
@param value ??
@return self
@usage chart:set_all_value(series, 50)
*/
static int lvgl_chart_set_all_value(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_chart_set_all_value(chart, series, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param series ????
@param id ????
@param value ??
@return self
@usage chart:set_value_by_id(series, 0, 50)
*/
static int lvgl_chart_set_value_by_id(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    uint16_t id = (uint16_t)luaL_checkinteger(L, 3);
    int32_t value = (int32_t)luaL_checkinteger(L, 4);
    lv_chart_set_value_by_id(chart, series, id, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@return self
@usage chart:refresh()
*/
static int lvgl_chart_refresh(lua_State* L) {
    lv_obj_t* chart = lvgl_get_obj_ptr(L, 1);
    lv_chart_refresh(chart);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? chart ????*/
void lvgl_register_chart(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_type", lvgl_chart_set_type);
    REG_METHOD(L, "set_range", lvgl_chart_set_range);
    REG_METHOD(L, "set_point_count", lvgl_chart_set_point_count);
    REG_METHOD(L, "set_div_line_count", lvgl_chart_set_div_line_count);
    REG_METHOD(L, "set_update_mode", lvgl_chart_set_update_mode);
    REG_METHOD(L, "set_axis_tick", lvgl_chart_set_axis_tick);
    REG_METHOD(L, "add_series", lvgl_chart_add_series);
    REG_METHOD(L, "set_series_color", lvgl_chart_set_series_color);
    REG_METHOD(L, "set_all_value", lvgl_chart_set_all_value);
    REG_METHOD(L, "set_value_by_id", lvgl_chart_set_value_by_id);
    REG_METHOD(L, "refresh", lvgl_chart_refresh);

    /* ????metatable??(????) */
    chart_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.chart.set_type(chart, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, chart_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.chart) */
    REG_METHOD(L, "create", lvgl_chart_create);
}
