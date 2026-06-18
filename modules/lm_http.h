/**
 * @file iot_http.h
 * @brief HTTP协议
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_HTTP_H
#define IOT_HTTP_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_http(lua_State* L);

#endif /* IOT_HTTP_H */
