/*
@module  yaml
@summary YAML编解码
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include "lua.h"
#include "iot_base.h"
#include "iot_json.h"
#include "yaml.h"
#include "iot_mem.h"

/**
 * @module yaml
 * YAML编解码模块,基于cJSON和yaml库实现
 */

/**
 * encode Lua值转YAML字符串
 * @param value 要编码的Lua值(table/number/string/boolean/nil)
 * @return string YAML格式字符串
 */
static int iot_yaml_encode(lua_State *L) {
    // 先转化为JSON对象，再将JSON对象转换为YAML字符串，最后释放JSON对象
    // 效率不高，但简单易行
    cJSON *json = iot_json_lua_to_cjson(L, 1);
    if (!json) {
        return luaL_error(L, "failed to convert lua value to json");
    }

    char *yaml = yaml_encode_cjson_dup(json);
    cJSON_Delete(json);

    if (!yaml) {
        return luaL_error(L, "out of memory");
    }

    lua_pushstring(L, yaml);
    iot_free(yaml);
    return 1;
}

/**
 * decode YAML字符串转Lua值
 * @param str YAML格式字符串
 * @return value Lua值(table/string/number/boolean/nil)
 */
static int iot_yaml_decode(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    
    cJSON *json = yaml_decode_cjson(str);
    if (!json) {
        return luaL_error(L, "failed to parse yaml");
    }
    
    // 先转化为JSON对象，再将JSON对象转换为Lua值，最后释放JSON对象
    // 效率不高，但简单易行
    iot_json_cjson_to_lua(L, json);
    cJSON_Delete(json);
    return 1;
}

static const luaL_Reg yaml_lib[] = {
    {"encode",  iot_yaml_encode},
    {"decode",  iot_yaml_decode},
    {NULL, NULL}
};

LUAMOD_API int luaopen_yaml(lua_State* L) {
    luaL_newlibtable(L, yaml_lib);
    luaL_setfuncs(L, yaml_lib, 0);
    return 1;
}
