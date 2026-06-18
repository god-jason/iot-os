/**
 * @file iot_i2c.h
 * @brief I2C总线
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_I2C_H
#define IOT_I2C_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_i2c(lua_State* L);

#endif /* IOT_I2C_H */
