/**
 * @file iot_log.h
 * @brief 日志系统
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_LOG_H
#define IOT_LOG_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_log(lua_State* L);

#endif /* IOT_LOG_H */
