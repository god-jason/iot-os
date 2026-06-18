/**
 * @file iot_pwm.h
 * @brief PWM输出
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_PWM_H
#define IOT_PWM_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_pwm(lua_State* L);

#endif /* IOT_PWM_H */
