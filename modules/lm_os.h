/**
 * @file iot_os.h
 * @brief 操作系统接口
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_OS_H
#define IOT_OS_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_os(lua_State* L);

#endif /* IOT_OS_H */
