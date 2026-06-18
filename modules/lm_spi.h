/**
 * @file iot_spi.h
 * @brief SPI总线
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_SPI_H
#define IOT_SPI_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_spi(lua_State* L);

#endif /* IOT_SPI_H */
