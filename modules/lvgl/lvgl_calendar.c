/**
 * @file iot_lvgl_calendar.c
 * @brief LVGL日历控件
 *
 * 实现LVGL日历控件的OO风格Lua绑定，包括日历创建、设置今天日期、设置显示年月、获取按下日期、获取显示年月等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* calendar组件的metatable引用 */
static int calendar_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_calendar_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* calendar = lv_calendar_create(parent);
    lua_pushlightuserdata(L, calendar);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
创建日历控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的日历实例
@usage local calendar = lvgl.calendar.create(scr)
*/
static int iot_lvgl_calendar_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_calendar_create_internal, calendar_metatable_ref);
}

/*
设置今天的日期
@param self 日历实例或指针
@param year 年份
@param month 月份
@param day 日
@return self
@usage calendar:set_today_date(2026, 6, 18)
*/
static int iot_lvgl_calendar_set_today_date(lua_State* L) {
    lv_obj_t* calendar = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t year = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t month = (uint32_t)luaL_checkinteger(L, 3);
    uint32_t day = (uint32_t)luaL_checkinteger(L, 4);
    lv_calendar_set_today_date(calendar, year, month, day);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置显示的日期
@param self 日历实例或指针
@param year 年份
@param month 月份
@return self
@usage calendar:set_showed_date(2026, 6)
*/
static int iot_lvgl_calendar_set_showed_date(lua_State* L) {
    lv_obj_t* calendar = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t year = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t month = (uint32_t)luaL_checkinteger(L, 3);
    lv_calendar_set_showed_date(calendar, year, month);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取按下的日期
@param self 日历实例或指针
@return table 日期表{year,month,day}或nil
@usage local date = calendar:get_pressed_date()
*/
static int iot_lvgl_calendar_get_pressed_date(lua_State* L) {
    lv_obj_t* calendar = iot_lvgl_get_obj_ptr(L, 1);
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
设置高亮日期(暂未实现)
@param self 日历实例或指针
@param dates 日期数组(暂未实现)
@return self
@usage calendar:set_highlighted_dates(dates)
*/
static int iot_lvgl_calendar_set_highlighted_dates(lua_State* L) {
    lv_obj_t* calendar = iot_lvgl_get_obj_ptr(L, 1);
    /* TODO: 实现日期数组处理 */
    (void)calendar;
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取当前显示的日期
@param self 日历实例或指针
@return table 日期表{year,month}
@usage local showed = calendar:get_showed_date()
*/
static int iot_lvgl_calendar_get_showed_date(lua_State* L) {
    lv_obj_t* calendar = iot_lvgl_get_obj_ptr(L, 1);
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

/* 注册 calendar 子模块 */
void iot_lvgl_register_calendar(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_today_date", iot_lvgl_calendar_set_today_date);
    REG_METHOD(L, "set_showed_date", iot_lvgl_calendar_set_showed_date);
    REG_METHOD(L, "get_pressed_date", iot_lvgl_calendar_get_pressed_date);
    REG_METHOD(L, "set_highlighted_dates", iot_lvgl_calendar_set_highlighted_dates);
    REG_METHOD(L, "get_showed_date", iot_lvgl_calendar_get_showed_date);

    /* 保存组件metatable引用(用于继承) */
    calendar_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.calendar.set_today_date(cal, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, calendar_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.calendar) */
    REG_METHOD(L, "create", iot_lvgl_calendar_create);
}
