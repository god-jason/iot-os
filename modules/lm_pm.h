/**
 * @file iot_pm.h
 * @brief 电源管理
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_PM_H
#define IOT_PM_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_pm(lua_State* L);

#endif /* IOT_PM_H */
