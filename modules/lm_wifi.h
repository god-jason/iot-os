/**
 * @file iot_wifi.h
 * @brief WiFi功能
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_WIFI_H
#define IOT_WIFI_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_wifi(lua_State* L);

#endif /* IOT_WIFI_H */
