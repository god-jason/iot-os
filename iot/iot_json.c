/*
@module  json
@summary JSON编码解码
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     JSON
*/

/*
JSON参考示例
-- 将Lua table编码为JSON字符串
local t = {name = "test", value = 123, flag = true, list = {1,2,3}}
local json_str = json.encode(t)
print(json_str)

-- 将JSON字符串解码为Lua table
local decoded = json.decode(json_str)
print(decoded.name, decoded.value)
*/


#include "lua.h"
#include "iot_base.h"
#include "cJSON.h"

/**
 * @brief 将Lua值转换为cJSON对象
 * @param L Lua状态机
 * @param index Lua值在栈上的索引
 * @return cJSON对象
 */
cJSON* iot_json_lua_to_cjson(lua_State* L, int index) {
    cJSON* result = NULL;
    int type = lua_type(L, index);

    switch (type) {
        case LUA_TNIL:
            result = cJSON_CreateNull();
            break;
        case LUA_TBOOLEAN:
            if (lua_toboolean(L, index)) {
                result = cJSON_CreateTrue();
            } else {
                result = cJSON_CreateFalse();
            }
            break;
        case LUA_TNUMBER:
            if (lua_isinteger(L, index)) {
                result = cJSON_CreateNumber((double)lua_tointeger(L, index));
            } else {
                result = cJSON_CreateNumber(lua_tonumber(L, index));
            }
            break;
        case LUA_TSTRING:
            result = cJSON_CreateString(lua_tostring(L, index));
            break;
        case LUA_TTABLE: {
            int is_array = 1;
            int max_index = 0;
            
            /* 检查是否为数组 */
            lua_pushnil(L);
            while (lua_next(L, index - 1)) {
                int key_type = lua_type(L, -2);
                if (key_type == LUA_TNUMBER) {
                    int idx = (int)lua_tointeger(L, -2);
                    if (idx >= 1) {
                        if (idx > max_index) {
                            max_index = idx;
                        }
                    } else {
                        is_array = 0;
                    }
                } else {
                    is_array = 0;
                }
                lua_pop(L, 1);
            }

            /* 验证数组索引是否连续 */
            if (is_array && max_index > 0) {
                for (int i = 1; i <= max_index; i++) {
                    lua_rawgeti(L, index, i);
                    if (lua_isnil(L, -1)) {
                        is_array = 0;
                        lua_pop(L, 1);
                        break;
                    }
                    lua_pop(L, 1);
                }
            }

            if (is_array && max_index > 0) {
                result = cJSON_CreateArray();
                for (int i = 1; i <= max_index; i++) {
                    lua_rawgeti(L, index, i);
                    cJSON* item = iot_json_lua_to_cjson(L, -1);
                    lua_pop(L, 1);
                    if (item) {
                        cJSON_AddItemToArray(result, item);
                    }
                }
            } else {
                result = cJSON_CreateObject();
                lua_pushnil(L);
                while (lua_next(L, index - 1)) {
                    const char* key = NULL;
                    int key_type = lua_type(L, -2);
                    if (key_type == LUA_TSTRING) {
                        key = lua_tostring(L, -2);
                    } else if (key_type == LUA_TNUMBER) {
                        /* 将数字键转换为字符串 */
                        double num = lua_tonumber(L, -2);
                        char buf[32];
                        if (num == (double)(int)num) {
                            snprintf(buf, sizeof(buf), "%d", (int)num);
                        } else {
                            snprintf(buf, sizeof(buf), "%g", num);
                        }
                        key = buf;
                    }

                    if (key) {
                        cJSON* item = iot_json_lua_to_cjson(L, -1);
                        if (item) {
                            cJSON_AddItemToObject(result, key, item);
                        }
                    }
                    lua_pop(L, 1);
                }
            }
            break;
        }
        default:
            result = cJSON_CreateNull();
            break;
    }

    return result;
}

/**
 * @brief 将cJSON对象转换为Lua值
 * @param L Lua状态机
 * @param json cJSON对象
 */
void iot_json_cjson_to_lua(lua_State* L, cJSON* json) {
    if (json == NULL) {
        lua_pushnil(L);
        return;
    }

    if (cJSON_IsNull(json)) {
        lua_pushnil(L);
    } else if (cJSON_IsBool(json)) {
        lua_pushboolean(L, cJSON_IsTrue(json) ? 1 : 0);
    } else if (cJSON_IsNumber(json)) {
        double num = cJSON_GetNumberValue(json);
        if (num == (double)(lua_Integer)num) {
            lua_pushinteger(L, (lua_Integer)num);
        } else {
            lua_pushnumber(L, num);
        }
    } else if (cJSON_IsString(json)) {
        lua_pushstring(L, cJSON_GetStringValue(json));
    } else if (cJSON_IsArray(json)) {
        lua_newtable(L);
        int index = 1;
        cJSON* item = json->child;
        while (item != NULL) {
            iot_json_cjson_to_lua(L, item);
            lua_rawseti(L, -2, index++);
            item = item->next;
        }
    } else if (cJSON_IsObject(json)) {
        lua_newtable(L);
        cJSON* item = json->child;
        while (item != NULL) {
            lua_pushstring(L, item->string);
            iot_json_cjson_to_lua(L, item);
            lua_rawset(L, -3);
            item = item->next;
        }
    } else {
        lua_pushnil(L);
    }
}

/**
 * @brief 将Lua table编码为JSON字符串
 * @api json.encode(value)
 * @param value Lua值 (table, string, number, boolean, nil)
 * @return string JSON字符串,失败返回nil
 * @usage
local t = {name = "test", value = 123}
local json_str = json.encode(t)
*/
static int iot_json_encode(lua_State* L) {
    cJSON* json = iot_json_lua_to_cjson(L, 1);
    if (json == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "encode failed");
        return 2;
    }

    char* str = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    if (str == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "print failed");
        return 2;
    }

    lua_pushstring(L, str);
    cJSON_free(str);
    return 1;
}

/**
 * @brief 将JSON字符串解码为Lua值
 * @api json.decode(json_str)
 * @string json_str JSON字符串
 * @return Lua值(table, string, number, boolean, nil),失败返回nil
 * @usage
local t = json.decode('{"name":"test"}')
*/
static int iot_json_decode(lua_State* L) {
    const char* str = luaL_checkstring(L, 1);
    
    cJSON* json = cJSON_Parse(str);
    if (json == NULL) {
        const char* error = cJSON_GetErrorPtr();
        lua_pushnil(L);
        if (error) {
            lua_pushstring(L, error);
        } else {
            lua_pushstring(L, "parse failed");
        }
        return 2;
    }

    iot_json_cjson_to_lua(L, json);
    cJSON_Delete(json);
    return 1;
}

static const luaL_Reg json_lib[] = {
    { "encode",  iot_json_encode },
    { "decode",  iot_json_decode },
    {NULL, NULL}
};

LUAMOD_API int luaopen_json(lua_State* L) {
    luaL_newlibtable(L, json_lib);
    luaL_setfuncs(L, json_lib, 0);
    return 1;
}
