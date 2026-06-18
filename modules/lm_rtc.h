/**
 * @file iot_rtc.h
 * @brief 实时时钟
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_RTC_H
#define IOT_RTC_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_rtc(lua_State* L);

#endif /* IOT_RTC_H */
