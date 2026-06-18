/*
@module  rtc
@summary RTC实时时钟
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     RTC
*/

/*
RTC参考示例
-- 获取当前时间(秒)
local time = rtc.get_time()
print("timestamp:", time)

-- 获取当前时间(年月日)
local t = rtc.get()
print("year:", t.year, "month:", t.month, "day:", t.day)

-- 设置时间
rtc.set({year=2024, month=1, day=1, hour=0, min=0, sec=0})

-- 设置时区
rtc.set_timezone(8)

-- 设置闹钟
rtc.set_alarm({hour=12, min=0, sec=0})
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_rtc.h"

/* RTC闹钟回调 */
static void rtc_alarm_cb(void) {
    /* TODO: 实现闹钟回调 */
}

/**
 * @brief 获取当前时间(秒)
 * @api rtc.get_time()
 * @return int 时间戳(秒)
 * @usage
local time = rtc.get_time()
print("timestamp:", time)
*/
static int iot_rtc_get_time(lua_State* L) {
    uint64_t t = cm_rtc_get_current_time();
    lua_pushinteger(L, (lua_Integer)t);
    return 1;
}

/**
 * @brief 设置当前时间(秒)
 * @api rtc.set_time(timestamp)
 * @int timestamp 时间戳(秒)
 * @return bool 成功返回true,失败返回false
 * @usage
rtc.set_time(1704067200)
*/
static int iot_rtc_set_time(lua_State* L) {
    uint64_t t = luaL_checkinteger(L, 1);
    int ret = cm_rtc_set_current_time(t);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取当前时间(年月日)
 * @api rtc.get()
 * @return table 时间表,{year, month, day, hour, min, sec}
 * @usage
local t = rtc.get()
print("year:", t.year, "month:", t.month, "day:", t.day)
*/
static int iot_rtc_get(lua_State* L) {
    uint64_t t = cm_rtc_get_current_time();
    time_t tt = (time_t)t;
    struct tm* tm = localtime(&tt);

    lua_createtable(L, 0, 6);
    lua_pushinteger(L, tm->tm_year + 1900);
    lua_setfield(L, -2, "year");
    lua_pushinteger(L, tm->tm_mon + 1);
    lua_setfield(L, -2, "month");
    lua_pushinteger(L, tm->tm_mday);
    lua_setfield(L, -2, "day");
    lua_pushinteger(L, tm->tm_hour);
    lua_setfield(L, -2, "hour");
    lua_pushinteger(L, tm->tm_min);
    lua_setfield(L, -2, "min");
    lua_pushinteger(L, tm->tm_sec);
    lua_setfield(L, -2, "sec");

    return 1;
}

/**
 * @brief 设置当前时间(格林威治时间)
 * @api rtc.set(t)
 * @table t 时间参数 {year, month, day, hour, min, sec}
 * @return bool 成功返回true,失败返回false
 * @usage
rtc.set({year=2024, month=1, day=1, hour=0, min=0, sec=0})
*/
static int iot_rtc_set(lua_State* L) {
    luaL_checktable(L, 1);

    lua_getfield(L, 1, "year");
    lua_getfield(L, 1, "month");
    lua_getfield(L, 1, "day");
    lua_getfield(L, 1, "hour");
    lua_getfield(L, 1, "min");
    lua_getfield(L, 1, "sec");

    struct tm tm;
    tm.tm_year = lua_tointeger(L, -6) - 1900;
    tm.tm_mon = lua_tointeger(L, -5) - 1;
    tm.tm_mday = lua_tointeger(L, -4);
    tm.tm_hour = lua_tointeger(L, -3);
    tm.tm_min = lua_tointeger(L, -2);
    tm.tm_sec = lua_tointeger(L, -1);

    time_t t = mktime(&tm);
    int ret = cm_rtc_set_current_time((uint64_t)t);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置时区
 * @api rtc.set_timezone(timezone)
 * @int timezone 时区,-12到12
 * @return bool 成功返回true,失败返回false
 * @usage
rtc.set_timezone(8)
*/
static int iot_rtc_set_timezone(lua_State* L) {
    int timezone = luaL_checkinteger(L, 1);
    int ret = cm_rtc_set_timezone(timezone);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取时区
 * @api rtc.get_timezone()
 * @return int 时区
 * @usage
local tz = rtc.get_timezone()
*/
static int iot_rtc_get_timezone(lua_State* L) {
    int tz = cm_rtc_get_timezone();
    lua_pushinteger(L, tz);
    return 1;
}

/**
 * @brief 设置闹钟
 * @api rtc.set_alarm(t)
 * @table t 闹钟时间 {hour, min, sec}
 * @return bool 成功返回true,失败返回false
 * @usage
rtc.set_alarm({hour=12, min=0, sec=0})
*/
static int iot_rtc_set_alarm(lua_State* L) {
    luaL_checktable(L, 1);

    cm_tm_t a_time;
    a_time.tm_hour = 0;
    a_time.tm_min = 0;
    a_time.tm_sec = 0;
    a_time.tm_mday = 0;
    a_time.tm_mon = 0;
    a_time.tm_year = 0;

    lua_getfield(L, 1, "hour");
    if (!lua_isnil(L, -1)) a_time.tm_hour = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "min");
    if (!lua_isnil(L, -1)) a_time.tm_min = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "sec");
    if (!lua_isnil(L, -1)) a_time.tm_sec = lua_tointeger(L, -1);
    lua_pop(L, 1);

    /* 注册闹钟回调 */
    cm_rtc_register_alarm_cb(rtc_alarm_cb);

    int ret = cm_rtc_set_alarm(&a_time);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 使能闹钟
 * @api rtc.enable_alarm(on)
 * @bool on true使能,false失能
 * @return nil
 * @usage
rtc.enable_alarm(true)
*/
static int iot_rtc_enable_alarm(lua_State* L) {
    int on = lua_toboolean(L, 1);
    cm_rtc_enable_alarm(on ? true : false);
    return 0;
}

static const luaL_Reg rtc_lib[] = {
    { "get_time",      iot_rtc_get_time },
    { "set_time",      iot_rtc_set_time },
    { "get",           iot_rtc_get },
    { "set",           iot_rtc_set },
    { "set_timezone",  iot_rtc_set_timezone },
    { "get_timezone",  iot_rtc_get_timezone },
    { "set_alarm",    iot_rtc_set_alarm },
    { "enable_alarm",  iot_rtc_enable_alarm },
    {NULL, NULL}
};

LUAMOD_API int luaopen_rtc(lua_State* L) {
    luaL_newlibtable(L, rtc_lib);
    luaL_setfuncs(L, rtc_lib, 0);
    return 1;
}
