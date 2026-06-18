/**
 * @file iot_mem.h
 * @brief 内存管理
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_MEM_H
#define IOT_MEM_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_mem(lua_State* L);

#endif /* IOT_MEM_H */
