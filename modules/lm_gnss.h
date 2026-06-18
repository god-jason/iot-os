/**
 * @file iot_gnss.h
 * @brief GNSS定位模块
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_GNSS_H
#define IOT_GNSS_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_gnss(lua_State* L);

#endif /* IOT_GNSS_H */
