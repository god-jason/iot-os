/**
 * @file iot_wdt.h
 * @brief 看门狗
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_WDT_H
#define IOT_WDT_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_wdt(lua_State* L);

#endif /* IOT_WDT_H */
