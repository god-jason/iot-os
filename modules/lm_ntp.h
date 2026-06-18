/**
 * @file iot_ntp.h
 * @brief NTP时间同步
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_NTP_H
#define IOT_NTP_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_ntp(lua_State* L);

#endif /* IOT_NTP_H */
