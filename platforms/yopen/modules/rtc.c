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
    rtc_time_t t = {0};

    /* TODO: 调用 yopen_rtc_get_time 获取实际时间 */

    lua_createtable(L, 0, 6);
    lua_pushinteger(L, t.year);
    lua_setfield(L, -2, "year");
    lua_pushinteger(L, t.month);
    lua_setfield(L, -2, "month");
    lua_pushinteger(L, t.day);
    lua_setfield(L, -2, "day");
    lua_pushinteger(L, t.hour);
    lua_setfield(L, -2, "hour");
    lua_pushinteger(L, t.min);
    lua_setfield(L, -2, "min");
    lua_pushinteger(L, t.sec);
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

    rtc_time_t t = {0};

    lua_getfield(L, 1, "year");
    if (lua_isnumber(L, -1)) t.year = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "month");
    if (lua_isnumber(L, -1)) t.month = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "day");
    if (lua_isnumber(L, -1)) t.day = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "hour");
    if (lua_isnumber(L, -1)) t.hour = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "min");
    if (lua_isnumber(L, -1)) t.min = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "sec");
    if (lua_isnumber(L, -1)) t.sec = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    /* TODO: 调用 yopen_rtc_set_time 设置时间 */

    lua_pushboolean(L, 1);
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

    /* TODO: 实现闹钟功能 */

    lua_pushboolean(L, 1);
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