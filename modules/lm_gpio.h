/**
 * @file iot_gpio.h
 * @brief 通用IO
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_GPIO_H
#define IOT_GPIO_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_gpio(lua_State* L);

#endif /* IOT_GPIO_H */
