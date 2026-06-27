/*
@module  storage
@summary IndexedDB 持久化存储模块
@version 1.0
@date    2026.06.27
@desc    用于在浏览器中持久化存储数据，类似 localStorage 但支持更大容量和结构化数据

### Lua 示例
```lua
-- 打开/创建数据库
local db = storage.open("mydb", "1.0", "My Database", 1024*1024)

-- 存储数据（自动序列化）
storage.set(db, "user", {name = "test", age = 25})
storage.set(db, "score", 100)

-- 读取数据（自动反序列化）
local user = storage.get(db, "user")
print(user.name, user.age)  -- test, 25

local score = storage.get(db, "score")
print(score)  -- 100

-- 删除数据
storage.remove(db, "user")

-- 关闭数据库
storage.close(db)
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

#define MAX_DB_INSTANCES 8

typedef struct {
    EM_VAL db;
    int lua_ref;
    char name[64];
} db_instance_t;

static db_instance_t db_instances[MAX_DB_INSTANCES];

static int db_find_free_slot(void) {
    for (int i = 0; i < MAX_DB_INSTANCES; i++) {
        if (db_instances[i].db == 0) {
            return i;
        }
    }
    return -1;
}

static int storage_open(lua_State* L) {
    const char* db_name = luaL_checkstring(L, 1);
    const char* db_version = luaL_optstring(L, 2, "1.0");
    const char* db_description = luaL_optstring(L, 3, "");
    int estimated_size = luaL_optinteger(L, 4, 1024 * 1024);
    
    int idx = db_find_free_slot();
    if (idx < 0) {
        return luaL_error(L, "storage: max database instances exceeded");
    }
    
    EM_VAL open_request = emscripten_idb_delete(db_name, NULL);
    if (open_request) {
        emscripten_destroy_value(open_request);
    }
    
    open_request = emscripten_idb_open(db_name, estimated_size, NULL, NULL);
    if (!open_request) {
        return luaL_error(L, "storage: failed to open database '%s'", db_name);
    }
    
    db_instances[idx].db = open_request;
    strncpy(db_instances[idx].name, db_name, sizeof(db_instances[idx].name) - 1);
    db_instances[idx].name[sizeof(db_instances[idx].name) - 1] = '\0';
    
    lua_newtable(L);
    lua_pushinteger(L, idx);
    lua_setfield(L, -2, "__index");
    lua_pushstring(L, db_name);
    lua_setfield(L, -2, "name");
    
    return 1;
}

static int storage_set(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    const char* key = luaL_checkstring(L, 2);
    
    if (idx < 0 || idx >= MAX_DB_INSTANCES || db_instances[idx].db == 0) {
        return luaL_error(L, "storage: invalid database instance");
    }
    
    size_t value_len;
    const char* value = luaL_checklstring(L, 3, &value_len);
    
    EM_VAL open_request = db_instances[idx].db;
    if (!open_request) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    EM_VAL key_val = emscripten_string_to_utf8(key, NULL);
    EM_VAL value_val = emscripten_string_to_utf8(value, NULL);
    
    int success = emscripten_idb_store(db_instances[idx].db, key, value, value_len, NULL, NULL);
    
    emscripten_destroy_value(key_val);
    emscripten_destroy_value(value_val);
    
    lua_pushboolean(L, success);
    return 1;
}

static int storage_get(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    const char* key = luaL_checkstring(L, 2);
    
    if (idx < 0 || idx >= MAX_DB_INSTANCES || db_instances[idx].db == 0) {
        return luaL_error(L, "storage: invalid database instance");
    }
    
    char* value = NULL;
    int value_len = 0;
    
    int success = emscripten_idb_load(db_instances[idx].db, key, &value, &value_len, NULL, NULL);
    
    if (success && value && value_len > 0) {
        lua_pushlstring(L, value, value_len);
        free(value);
    } else {
        lua_pushnil(L);
    }
    
    return 1;
}

static int storage_remove(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    const char* key = luaL_checkstring(L, 2);
    
    if (idx < 0 || idx >= MAX_DB_INSTANCES || db_instances[idx].db == 0) {
        return luaL_error(L, "storage: invalid database instance");
    }
    
    int success = emscripten_idb_delete(db_instances[idx].db, key, NULL, NULL);
    lua_pushboolean(L, success);
    return 1;
}

static int storage_close(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_DB_INSTANCES) {
        return 0;
    }
    
    if (db_instances[idx].db != 0) {
        emscripten_destroy_value(db_instances[idx].db);
        db_instances[idx].db = 0;
    }
    
    return 0;
}

static int storage_exists(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    const char* key = luaL_checkstring(L, 2);
    
    if (idx < 0 || idx >= MAX_DB_INSTANCES || db_instances[idx].db == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    char* value = NULL;
    int value_len = 0;
    int success = emscripten_idb_load(db_instances[idx].db, key, &value, &value_len, NULL, NULL);
    
    if (success && value) {
        free(value);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    
    return 1;
}

static const luaL_Reg storage_methods[] = {
    {"open", storage_open},
    {"set", storage_set},
    {"get", storage_get},
    {"remove", storage_remove},
    {"close", storage_close},
    {"exists", storage_exists},
    {NULL, NULL}
};

LUAMOD_API int luaopen_storage_register(lua_State* L) {
    lua_newtable(L);
    luaL_setfuncs(L, storage_methods, 0);
    return 1;
}