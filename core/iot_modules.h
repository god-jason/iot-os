/**
 * @file iot_modules.h
 * @brief Lua模块注册入口
 *
 * 本模块提供所有Lua模块的注册接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef MODULES_HEADER
#define MODULES_HEADER

#include "lua.h"

/**
 * @brief 注册所有Lua模块
 * @param L Lua状态机
 * @return void
 */
void modules_register(lua_State* L);

#endif /* MODULES_HEADER */
