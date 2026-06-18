/**
 * @file iot_cpu.h
 * @brief CPU信息
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_CPU_H
#define IOT_CPU_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_cpu(lua_State* L);

#endif /* IOT_CPU_H */
