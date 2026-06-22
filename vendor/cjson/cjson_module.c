/**
 * @file lua_module.c
 * @brief cJSON 库的 Lua 模块封装
 *
 * 提供 JSON 解析和生成的 Lua 接口
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* cJSON 头文件 */
#include "cJSON.h"

/*===========================================================
 * 内部辅助函数
 *===========================================================*/

/* cJSON 对象元表名称 */
#define CJSON_METATABLE "cjson.object"

/* 将 cJSON 对象转换为 Lua 值 */
static void luaopen_cjson_to_lua(lua_State* L, cJSON* item);

/* 将 Lua 值转换为 cJSON 对象 */
static cJSON* lua_to_cjson(lua_State* L, int idx);

/*===========================================================
 * cJSON 转 Lua
 *===========================================================*/

static void luaopen_cjson_to_lua(lua_State* L, cJSON* item) {
    if (!item) {
        lua_pushnil(L);
        return;
    }

    switch (item->type) {
        case cJSON_False:
            lua_pushboolean(L, 0);
            break;

        case cJSON_True:
            lua_pushboolean(L, 1);
            break;

        case cJSON_NULL:
            lua_pushnil(L);
            break;

        case cJSON_Number:
            if (item->valueint == (int)item->valuedouble) {
                lua_pushinteger(L, item->valueint);
            } else {
                lua_pushnumber(L, item->valuedouble);
            }
            break;

        case cJSON_String:
            lua_pushstring(L, item->valuestring);
            break;

        case cJSON_Array:
            lua_newtable(L);
            int arr_idx = 1;
            cJSON* arr_item = item->child;
            while (arr_item) {
                luaopen_cjson_to_lua(L, arr_item);
                lua_rawseti(L, -2, arr_idx);
                arr_idx++;
                arr_item = arr_item->next;
            }
            break;

        case cJSON_Object:
            lua_newtable(L);
            cJSON* obj_item = item->child;
            while (obj_item) {
                lua_pushstring(L, obj_item->string);
                luaopen_cjson_to_lua(L, obj_item);
                lua_rawset(L, -3);
                obj_item = obj_item->next;
            }
            break;

        default:
            lua_pushnil(L);
            break;
    }
}

/*===========================================================
 * Lua 转 cJSON
 *===========================================================*/

static cJSON* lua_to_cjson(lua_State* L, int idx) {
    int type = lua_type(L, idx);

    switch (type) {
        case LUA_TNIL:
            return cJSON_CreateNull();

        case LUA_TBOOLEAN:
            return cJSON_CreateBool(lua_toboolean(L, idx));

        case LUA_TNUMBER:
            if (lua_isinteger(L, idx)) {
                return cJSON_CreateNumber((double)lua_tointeger(L, idx));
            } else {
                return cJSON_CreateNumber(lua_tonumber(L, idx));
            }

        case LUA_TSTRING:
            return cJSON_CreateString(lua_tostring(L, idx));

        case LUA_TTABLE:
            /* 检查是数组还是对象 */
            lua_pushnil(L);
            if (lua_next(L, idx) == 0) {
                /* 空表，创建空数组 */
                return cJSON_CreateArray();
            }

            /* 检查第一个键 */
            int key_type = lua_type(L, -2);
            lua_pop(L, 2);  /* 弹出键和值 */

            if (key_type == LUA_TNUMBER) {
                /* 数组 */
                cJSON* arr = cJSON_CreateArray();
                int len = (int)lua_rawlen(L, idx);
                for (int i = 1; i <= len; i++) {
                    lua_rawgeti(L, idx, i);
                    cJSON* elem = lua_to_cjson(L, -1);
                    cJSON_AddItemToArray(arr, elem);
                    lua_pop(L, 1);
                }
                return arr;
            } else {
                /* 对象 */
                cJSON* obj = cJSON_CreateObject();
                lua_pushnil(L);
                while (lua_next(L, idx)) {
                    const char* key = lua_tostring(L, -2);
                    cJSON* val = lua_to_cjson(L, -1);
                    cJSON_AddItemToObject(obj, key, val);
                    lua_pop(L, 1);
                }
                return obj;
            }

        default:
            return cJSON_CreateNull();
    }
}

/*===========================================================
 * Lua API 函数
 *===========================================================*/

/**
 * @brief 解析 JSON 字符串
 * @api cjson.parse(str)
 * @string str JSON 字符串
 * @return Lua 表，失败返回 nil 和错误信息
 */
static int luaopen_cjson_parse(lua_State* L) {
    const char* str = luaL_checkstring(L, 1);

    cJSON* root = cJSON_Parse(str);
    if (!root) {
        const char* err = cJSON_GetErrorPtr();
        lua_pushnil(L);
        lua_pushstring(L, err ? err : "parse error");
        return 2;
    }

    luaopen_cjson_to_lua(L, root);
    cJSON_Delete(root);

    return 1;
}

/**
 * @brief 将 Lua 值编码为 JSON 字符串
 * @api cjson.encode(value, formatted)
 * @param value Lua 值
 * @bool formatted 是否格式化输出，默认 false
 * @return JSON 字符串
 */
static int luaopen_cjson_encode(lua_State* L) {
    cJSON* root = lua_to_cjson(L, 1);
    if (!root) {
        lua_pushnil(L);
        lua_pushstring(L, "encode error");
        return 2;
    }

    int formatted = lua_isnoneornil(L, 2) ? 0 : lua_toboolean(L, 2);

    char* json_str;
    if (formatted) {
        json_str = cJSON_Print(root);
    } else {
        json_str = cJSON_PrintUnformatted(root);
    }

    cJSON_Delete(root);

    if (!json_str) {
        lua_pushnil(L);
        lua_pushstring(L, "print error");
        return 2;
    }

    lua_pushstring(L, json_str);
    free(json_str);

    return 1;
}

/**
 * @brief 创建空对象
 * @api cjson.create_object()
 * @return cJSON 对象
 */
static int luaopen_cjson_create_object(lua_State* L) {
    cJSON* obj = cJSON_CreateObject();
    if (!obj) {
        lua_pushnil(L);
        return 1;
    }

    cJSON** ptr = (cJSON**)lua_newuserdata(L, sizeof(cJSON*));
    *ptr = obj;

    luaL_getmetatable(L, CJSON_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 创建空数组
 * @api cjson.create_array()
 * @return cJSON 数组
 */
static int luaopen_cjson_create_array(lua_State* L) {
    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        lua_pushnil(L);
        return 1;
    }

    cJSON** ptr = (cJSON**)lua_newuserdata(L, sizeof(cJSON*));
    *ptr = arr;

    luaL_getmetatable(L, CJSON_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 获取对象字段
 * @api obj:get(key)
 * @string key 字段名
 * @return 字段值
 */
static int luaopen_cjson_object_get(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);
    const char* key = luaL_checkstring(L, 2);

    if (!ptr || !*ptr) {
        lua_pushnil(L);
        return 1;
    }

    cJSON* item = cJSON_GetObjectItem(*ptr, key);
    luaopen_cjson_to_lua(L, item);

    return 1;
}

/**
 * @brief 设置对象字段
 * @api obj:set(key, value)
 * @string key 字段名
 * @param value 字段值
 */
static int luaopen_cjson_object_set(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);
    const char* key = luaL_checkstring(L, 2);

    if (!ptr || !*ptr) {
        return 0;
    }

    cJSON* val = lua_to_cjson(L, 3);
    cJSON_AddItemToObject(*ptr, key, val);

    return 0;
}

/**
 * @brief 获取数组元素
 * @api arr:get(index)
 * @int index 索引 (从 0 开始)
 * @return 元素值
 */
static int luaopen_cjson_array_get(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);
    int index = (int)luaL_checkinteger(L, 2);

    if (!ptr || !*ptr) {
        lua_pushnil(L);
        return 1;
    }

    cJSON* item = cJSON_GetArrayItem(*ptr, index);
    luaopen_cjson_to_lua(L, item);

    return 1;
}

/**
 * @brief 添加数组元素
 * @api arr:add(value)
 * @param value 元素值
 */
static int luaopen_cjson_array_add(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);

    if (!ptr || !*ptr) {
        return 0;
    }

    cJSON* val = lua_to_cjson(L, 2);
    cJSON_AddItemToArray(*ptr, val);

    return 0;
}

/**
 * @brief 获取数组长度
 * @api arr:size()
 * @return 数组长度
 */
static int luaopen_cjson_array_size(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);

    if (!ptr || !*ptr) {
        lua_pushinteger(L, 0);
        return 1;
    }

    int size = cJSON_GetArraySize(*ptr);
    lua_pushinteger(L, size);

    return 1;
}

/**
 * @brief 转为 JSON 字符串
 * @api obj:tostring(formatted)
 * @bool formatted 是否格式化
 * @return JSON 字符串
 */
static int luaopen_cjson_tostring(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);
    int formatted = lua_isnoneornil(L, 2) ? 0 : lua_toboolean(L, 2);

    if (!ptr || !*ptr) {
        lua_pushstring(L, "");
        return 1;
    }

    char* json_str;
    if (formatted) {
        json_str = cJSON_Print(*ptr);
    } else {
        json_str = cJSON_PrintUnformatted(*ptr);
    }

    if (!json_str) {
        lua_pushstring(L, "");
        return 1;
    }

    lua_pushstring(L, json_str);
    free(json_str);

    return 1;
}

/**
 * @brief 垃圾回收
 */
static int luaopen_cjson_gc(lua_State* L) {
    cJSON** ptr = (cJSON**)luaL_checkudata(L, 1, CJSON_METATABLE);
    if (ptr && *ptr) {
        cJSON_Delete(*ptr);
        *ptr = NULL;
    }
    return 0;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* cJSON 对象方法表 */
static const luaL_Reg luaopen_cjson_object_methods[] = {
    { "get",      luaopen_cjson_object_get },
    { "set",      luaopen_cjson_object_set },
    { "add",      luaopen_cjson_array_add },
    { "size",     luaopen_cjson_array_size },
    { "tostring", luaopen_cjson_tostring },
    { "__gc",     luaopen_cjson_gc },
    { "__tostring", luaopen_cjson_tostring },
    { NULL,       NULL }
};

/* cjson 模块方法表 */
static const luaL_Reg luaopen_cjson_module_methods[] = {
    { "parse",          luaopen_cjson_parse },
    { "encode",         luaopen_cjson_encode },
    { "create_object",  luaopen_cjson_create_object },
    { "create_array",   luaopen_cjson_create_array },
    { NULL,             NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_cjson_register(lua_State* L) {
    /* 创建模块 */
    luaL_newlib(L, luaopen_cjson_module_methods);

    /* 创建对象元表 */
    luaL_newmetatable(L, CJSON_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, luaopen_cjson_object_methods, 0);
    lua_pop(L, 1);

    /* 注册版本信息 */
    lua_pushstring(L, cJSON_Version());
    lua_setfield(L, -2, "version");

    return 1;
}