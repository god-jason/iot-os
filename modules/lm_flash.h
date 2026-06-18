/**
 * @file iot_flash.h
 * @brief Flash操作
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_FLASH_H
#define IOT_FLASH_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_flash(lua_State* L);

#endif /* IOT_FLASH_H */
