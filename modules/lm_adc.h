/**
 * @file iot_adc.h
 * @brief 模数转换
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_ADC_H
#define IOT_ADC_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_adc(lua_State* L);

#endif /* IOT_ADC_H */
