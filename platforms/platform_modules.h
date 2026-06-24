/**
 * @file platform_modules.h
 * @brief 平台相关 Lua 模块注册入口
 */

#ifndef IOT_PLATFORM_MODULES_H
#define IOT_PLATFORM_MODULES_H

#include "lua.h"

void platform_modules_register(lua_State* L);

#endif /* IOT_PLATFORM_MODULES_H */
