/**
 * @file iot_fota.h
 * @brief 固件空中升级
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_FOTA_H
#define IOT_FOTA_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_fota(lua_State* L);

#endif /* IOT_FOTA_H */
