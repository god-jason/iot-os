/**
 * @file iot_sim.h
 * @brief SIM卡管理
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_SIM_H
#define IOT_SIM_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_sim(lua_State* L);

#endif /* IOT_SIM_H */
