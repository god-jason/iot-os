/**
 * @file iot_json.h
 * @brief JSON与Lua值相互转换工具函数
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_JSON_H
#define IOT_JSON_H

#include "iot_base.h"
#include "cJSON.h"
#include "lua.h"

/**
 * @brief Lua值转换为cJSON对象
 * @param L Lua状态机
 * @param index Lua栈索引
 * @return 分配的cJSON对象，需要cJSON_Delete释放;失败返回NULL
 */
cJSON* iot_json_lua_to_cjson(lua_State* L, int index);

/**
 * @brief cJSON对象转换为Lua值
 * @param L Lua状态机
 * @param json cJSON对象
 */
void iot_json_cjson_to_lua(lua_State* L, cJSON* json);

/* Lua模块入口 */
LUAMOD_API int luaopen_json(lua_State* L);

#endif /* IOT_JSON_H */
