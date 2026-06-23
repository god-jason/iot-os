/**
 * @file iot_pack.h
 * @brief 数据打包/解包
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_PACK_H
#define IOT_PACK_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_pack_register(lua_State* L);

#endif /* IOT_PACK_H */
