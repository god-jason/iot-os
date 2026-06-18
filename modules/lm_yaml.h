/**
 * @file iot_yaml.h
 * @brief YAML解析
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_YAML_H
#define IOT_YAML_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_yaml(lua_State* L);

#endif /* IOT_YAML_H */
