/*
@module  rtc
@summary 实时时钟
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     RTC
*/

/*
RTC参考示例
-- 获取当前时间
local time = rtc.time()
print("year:", time.year, "month:", time.month, "day:", time.day)
print("hour:", time.hour, "min:", time.min, "sec:", time.sec)

-- 设置时间
rtc.set({
    year = 2024,
    month = 1,
    day = 1,
    hour = 0,
    min = 0,
    sec = 0
})

-- 设置闹钟
rtc.alarm({
    hour = 8,
    min = 0,
    sec = 0
}, function()
    print("alarm triggered!")
end)
*/

#include "module.h"
#include "yopen_rtc.h"

/* RTC时间结构 */
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
} rtc_time_t;

/**
 * @brief 获取当前RTC时间
 * @api rtc.time()
 * @return table 时间信息 {year, month, day, hour, min, sec}
 */
static int luaopen_rtc_time(lua_State* L) {
    yopen_rtc_time_t tm = {0};
    yopen_rtc_get_time(&tm);

    lua_createtable(L, 0, 6);
    lua_pushinteger(L, tm.tm_year);
    lua_setfield(L, -2, "year");
    lua_pushinteger(L, tm.tm_mon);
    lua_setfield(L, -2, "month");
    lua_pushinteger(L, tm.tm_mday);
    lua_setfield(L, -2, "day");
    lua_pushinteger(L, tm.tm_hour);
    lua_setfield(L, -2, "hour");
    lua_pushinteger(L, tm.tm_min);
    lua_setfield(L, -2, "min");
    lua_pushinteger(L, tm.tm_sec);
    lua_setfield(L, -2, "sec");

    return 1;
}

/**
 * @brief 设置RTC时间
 * @api rtc.set(time)
 * @table time 时间信息 {year, month, day, hour, min, sec}
 * @return bool true表示成功
 */
static int luaopen_rtc_set(lua_State* L) {
    if (!lua_istable(L, 1)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    yopen_rtc_time_t tm = {0};

    lua_getfield(L, 1, "year");
    if (lua_isnumber(L, -1)) tm.tm_year = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "month");
    if (lua_isnumber(L, -1)) tm.tm_mon = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "day");
    if (lua_isnumber(L, -1)) tm.tm_mday = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "hour");
    if (lua_isnumber(L, -1)) tm.tm_hour = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "min");
    if (lua_isnumber(L, -1)) tm.tm_min = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "sec");
    if (lua_isnumber(L, -1)) tm.tm_sec = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    yopen_errcode_rtc_e ret = yopen_rtc_set_time(&tm);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置RTC闹钟
 * @api rtc.alarm(time, callback)
 * @table time 闹钟时间 {hour, min, sec}
 * @func callback 闹钟触发回调
 * @return bool true表示成功
 */
static int luaopen_rtc_alarm(lua_State* L) {
    if (!lua_istable(L, 1)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    yopen_rtc_time_t tm = {0};

    lua_getfield(L, 1, "hour");
    if (lua_isnumber(L, -1)) tm.tm_hour = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "min");
    if (lua_isnumber(L, -1)) tm.tm_min = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "sec");
    if (lua_isnumber(L, -1)) tm.tm_sec = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    yopen_errcode_rtc_e ret = yopen_rtc_set_alarm(&tm);
    if (ret == 0) {
        yopen_rtc_enable_alarm(1);
    }

    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg luaopen_rtc_funcs[] = {
    {"time",  luaopen_rtc_time},
    {"set",   luaopen_rtc_set},
    {"alarm", luaopen_rtc_alarm},
    {NULL, NULL}
};

int luaopen_rtc(lua_State* L) {
    luaL_newlib(L, luaopen_rtc_funcs);
    return 1;
}