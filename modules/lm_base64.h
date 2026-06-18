/**
 * @file iot_base64.h
 * @brief Base64编解码
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_BASE64_H
#define IOT_BASE64_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_base64(lua_State* L);

#endif /* IOT_BASE64_H */
