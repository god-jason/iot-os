/**
 * @file iot_zlib.h
 * @brief 压缩解压
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_ZLIB_H
#define IOT_ZLIB_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_zlib(lua_State* L);

#endif /* IOT_ZLIB_H */
