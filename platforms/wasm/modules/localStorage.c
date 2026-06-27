/*
@module  localStorage
@summary 本地存储模块
@version 1.0
@date    2026.06.27
@desc    封装浏览器的 localStorage API，简单键值对存储

### Lua 示例
```lua
-- 存储值（自动转为字符串）
localStorage.set("username", "test")
localStorage.set("score", tostring(100))
localStorage.set("data", '{"key":"value"}')

-- 读取值
local name = localStorage.get("username")
local score = tonumber(localStorage.get("score"))
local data = localStorage.get("data")

-- 删除值
localStorage.remove("username")

-- 清空所有
-- localStorage.clear()

-- 检查键是否存在
local exists = localStorage.has("username")

-- 获取所有键
local keys = localStorage.keys()
for i, v in ipairs(keys) do
    print(i, v)
end
```
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lua.h"
#include "lauxlib.h"

#include "emscripten/emscripten.h"
#include "emscripten/storage.h"

static int localstorage_set(lua_State* L) {
    const char* key = luaL_checkstring(L, 1);
    const char* value = luaL_checkstring(L, 2);
    
    EM_VAL key_val = emscripten_string_to_utf8(key, NULL);
    EM_VAL value_val = emscripten_string_to_utf8(value, NULL);
    
    int success = emscripten_local_storage_set(key, value, strlen(value));
    
    emscripten_destroy_value(key_val);
    emscripten_destroy_value(value_val);
    
    lua_pushboolean(L, success);
    return 1;
}

static int localstorage_get(lua_State* L) {
    const char* key = luaL_checkstring(L, 1);
    
    char* value = NULL;
    int value_len = 0;
    
    int success = emscripten_local_storage_get(key, &value, &value_len);
    
    if (success && value && value_len > 0) {
        lua_pushlstring(L, value, value_len);
        free(value);
    } else {
        lua_pushnil(L);
    }
    
    return 1;
}

static int localstorage_remove(lua_State* L) {
    const char* key = luaL_checkstring(L, 1);
    
    int success = emscripten_local_storage_delete(key);
    lua_pushboolean(L, success);
    return 1;
}

static int localstorage_clear(lua_State* L) {
    int success = emscripten_local_storage_clear();
    lua_pushboolean(L, success);
    return 1;
}

static int localstorage_has(lua_State* L) {
    const char* key = luaL_checkstring(L, 1);
    
    char* value = NULL;
    int value_len = 0;
    
    int success = emscripten_local_storage_get(key, &value, &value_len);
    
    if (success && value) {
        free(value);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    
    return 1;
}

static int localstorage_keys(lua_State* L) {
    int length = emscripten_local_storage_length();
    
    lua_newtable(L);
    
    for (int i = 0; i < length; i++) {
        char* key = NULL;
        int key_len = 0;
        
        if (emscripten_local_storage_get_key(i, &key, &key_len) && key) {
            lua_pushinteger(L, i + 1);
            lua_pushlstring(L, key, key_len);
            free(key);
        }
    }
    
    return 1;
}

static int localstorage_length(lua_State* L) {
    int length = emscripten_local_storage_length();
    lua_pushinteger(L, length);
    return 1;
}

static const luaL_Reg localstorage_methods[] = {
    {"set", localstorage_set},
    {"get", localstorage_get},
    {"remove", localstorage_remove},
    {"clear", localstorage_clear},
    {"has", localstorage_has},
    {"keys", localstorage_keys},
    {"length", localstorage_length},
    {NULL, NULL}
};

LUAMOD_API int luaopen_localStorage_register(lua_State* L) {
    lua_newtable(L);
    luaL_setfuncs(L, localstorage_methods, 0);
    return 1;
}