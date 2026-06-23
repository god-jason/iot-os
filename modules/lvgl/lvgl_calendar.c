/*
@module  lvgl.calendar
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
local calendar = lvgl.calendar.create(scr)
calendar:set_size(300, 350)
calendar:set_pos(10, 10)

-- ??????
calendar:set_today_date(2026, 6, 18)
calendar:set_showed_date(2026, 6)

-- ??????????
local date = calendar:get_pressed_date()
if date then
    print("???? " .. date.year .. "-" .. date.month .. "-" .. date.day)
end

-- ??????????
local showed = calendar:get_showed_date()
print("??: " .. showed.year .. "-" .. showed.month)

-- ????
local cal = lvgl.calendar.create(scr):set_size(240, 320):set_pos(40, 10):set_today_date(2026, 6, 18)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* calendar???metatable?? */
static int calendar_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_calendar_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* calendar = lv_calendar_create(parent);
    lua_pushlightuserdata(L, calendar);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local calendar = lvgl.calendar.create(scr)
*/
static int lvgl_calendar_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_calendar_create_internal, calendar_metatable_ref);
}

/*
????????
@param self ????????
@param year ??
@param month ??
@param day ??
@return self
@usage calendar:set_today_date(2026, 6, 18)
*/
static int lvgl_calendar_set_today_date(lua_State* L) {
    lv_obj_t* calendar = lvgl_get_obj_ptr(L, 1);
    uint32_t year = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t month = (uint32_t)luaL_checkinteger(L, 3);
    uint32_t day = (uint32_t)luaL_checkinteger(L, 4);
    lv_calendar_set_today_date(calendar, year, month, day);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param year ??
@param month ??
@return self
@usage calendar:set_showed_date(2026, 6)
*/
static int lvgl_calendar_set_showed_date(lua_State* L) {
    lv_obj_t* calendar = lvgl_get_obj_ptr(L, 1);
    uint32_t year = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t month = (uint32_t)luaL_checkinteger(L, 3);
    lv_calendar_set_showed_date(calendar, year, month);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@return table ???{year,month,day}?nil
@usage local date = calendar:get_pressed_date()
*/
static int lvgl_calendar_get_pressed_date(lua_State* L) {
    lv_obj_t* calendar = lvgl_get_obj_ptr(L, 1);
    lv_calendar_date_t date;
    if (lv_calendar_get_pressed_date(calendar, &date) == LV_RES_OK) {
        lua_createtable(L, 3, 0);
        lua_pushinteger(L, date.year);
        lua_setfield(L, -2, "year");
        lua_pushinteger(L, date.month);
        lua_setfield(L, -2, "month");
        lua_pushinteger(L, date.day);
        lua_setfield(L, -2, "day");
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/*
??????(????)
@param self ????????
@param dates ????(????)
@return self
@usage calendar:set_highlighted_dates(dates)
*/
static int lvgl_calendar_set_highlighted_dates(lua_State* L) {
    lv_obj_t* calendar = lvgl_get_obj_ptr(L, 1);
    /* TODO: ???????? */
    (void)calendar;
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@return table ???{year,month}
@usage local showed = calendar:get_showed_date()
*/
static int lvgl_calendar_get_showed_date(lua_State* L) {
    lv_obj_t* calendar = lvgl_get_obj_ptr(L, 1);
    const lv_calendar_date_t* date = lv_calendar_get_showed_date(calendar);
    if (!date) {
        lua_pushnil(L);
        return 1;
    }
    lua_createtable(L, 2, 0);
    lua_pushinteger(L, date->year);
    lua_setfield(L, -2, "year");
    lua_pushinteger(L, date->month);
    lua_setfield(L, -2, "month");
    return 1;
}

/* ?? calendar ????*/
void lvgl_register_calendar(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_today_date", lvgl_calendar_set_today_date);
    REG_METHOD(L, "set_showed_date", lvgl_calendar_set_showed_date);
    REG_METHOD(L, "get_pressed_date", lvgl_calendar_get_pressed_date);
    REG_METHOD(L, "set_highlighted_dates", lvgl_calendar_set_highlighted_dates);
    REG_METHOD(L, "get_showed_date", lvgl_calendar_get_showed_date);

    /* ????metatable??(????) */
    calendar_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.calendar.set_today_date(cal, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, calendar_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.calendar) */
    REG_METHOD(L, "create", lvgl_calendar_create);
}
