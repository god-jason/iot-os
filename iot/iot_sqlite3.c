#include "lua.h"
#include "lauxlib.h"
#include "sqlite3.h"
#include "iot_sqlite3.h"

#define DB_METATABLE "sqlite3.db"
#define STMT_METATABLE "sqlite3.stmt"

static int iot_sqlite3_open(lua_State* L);
static int iot_sqlite3_close(lua_State* L);
static int iot_sqlite3_exec(lua_State* L);
static int iot_sqlite3_query(lua_State* L);
static int iot_sqlite3_prepare(lua_State* L);
static int iot_sqlite3_last_insert_id(lua_State* L);
static int iot_sqlite3_changes(lua_State* L);

static int iot_sqlite3_stmt_bind(lua_State* L);
static int iot_sqlite3_stmt_step(lua_State* L);
static int iot_sqlite3_stmt_finalize(lua_State* L);
static int iot_sqlite3_stmt_reset(lua_State* L);

static int iot_sqlite3_db_gc(lua_State* L);
static int iot_sqlite3_stmt_gc(lua_State* L);

static int iot_sqlite3_open(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    sqlite3* db = NULL;
    
    int rc = sqlite3_open(filename, &db);
    if (rc != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 2;
    }
    
    sqlite3** db_ptr = (sqlite3**)lua_newuserdata(L, sizeof(sqlite3*));
    *db_ptr = db;
    
    luaL_getmetatable(L, DB_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

static int iot_sqlite3_close(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    if (*db_ptr) {
        sqlite3_close(*db_ptr);
        *db_ptr = NULL;
    }
    return 0;
}

static int iot_sqlite3_db_gc(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)lua_touserdata(L, 1);
    if (*db_ptr) {
        sqlite3_close(*db_ptr);
        *db_ptr = NULL;
    }
    return 0;
}

static int iot_sqlite3_exec(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    const char* sql = luaL_checkstring(L, 2);
    
    char* err_msg = NULL;
    int rc = sqlite3_exec(*db_ptr, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, err_msg);
        sqlite3_free(err_msg);
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_sqlite3_query_callback(void* data, int argc, char** argv, char** azColName) {
    lua_State* L = (lua_State*)data;
    
    lua_newtable(L);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(L, azColName[i]);
        if (argv[i]) {
            lua_pushstring(L, argv[i]);
        } else {
            lua_pushnil(L);
        }
        lua_settable(L, -3);
    }
    
    lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
    return 0;
}

static int iot_sqlite3_query(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    const char* sql = luaL_checkstring(L, 2);
    
    char* err_msg = NULL;
    
    lua_newtable(L);
    int rc = sqlite3_exec(*db_ptr, sql, iot_sqlite3_query_callback, L, &err_msg);
    
    if (rc != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, err_msg);
        sqlite3_free(err_msg);
        return 2;
    }
    
    return 1;
}

static int iot_sqlite3_prepare(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    const char* sql = luaL_checkstring(L, 2);
    
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(*db_ptr, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(*db_ptr));
        return 2;
    }
    
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)lua_newuserdata(L, sizeof(sqlite3_stmt*));
    *stmt_ptr = stmt;
    
    luaL_getmetatable(L, STMT_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

static int iot_sqlite3_last_insert_id(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    lua_pushinteger(L, sqlite3_last_insert_rowid(*db_ptr));
    return 1;
}

static int iot_sqlite3_changes(lua_State* L) {
    sqlite3** db_ptr = (sqlite3**)luaL_checkudata(L, 1, DB_METATABLE);
    lua_pushinteger(L, sqlite3_changes(*db_ptr));
    return 1;
}

static int iot_sqlite3_stmt_bind(lua_State* L) {
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, STMT_METATABLE);
    int index = luaL_checkinteger(L, 2);
    
    int rc;
    if (lua_isnil(L, 3)) {
        rc = sqlite3_bind_null(*stmt_ptr, index);
    } else if (lua_isboolean(L, 3)) {
        rc = sqlite3_bind_int(*stmt_ptr, index, lua_toboolean(L, 3) ? 1 : 0);
    } else if (lua_isinteger(L, 3)) {
        rc = sqlite3_bind_int64(*stmt_ptr, index, lua_tointeger(L, 3));
    } else if (lua_isnumber(L, 3)) {
        rc = sqlite3_bind_double(*stmt_ptr, index, lua_tonumber(L, 3));
    } else if (lua_isstring(L, 3)) {
        size_t len;
        const char* str = lua_tolstring(L, 3, &len);
        rc = sqlite3_bind_text(*stmt_ptr, index, str, len, SQLITE_TRANSIENT);
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unsupported value type");
        return 2;
    }
    
    if (rc != SQLITE_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, sqlite3_errmsg(sqlite3_db_handle(*stmt_ptr)));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_sqlite3_stmt_step(lua_State* L) {
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, STMT_METATABLE);
    
    int rc = sqlite3_step(*stmt_ptr);
    
    if (rc == SQLITE_DONE) {
        lua_pushnil(L);
        return 1;
    }
    
    if (rc != SQLITE_ROW) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(sqlite3_db_handle(*stmt_ptr)));
        return 2;
    }
    
    int col_count = sqlite3_column_count(*stmt_ptr);
    lua_newtable(L);
    
    for (int i = 0; i < col_count; i++) {
        const char* col_name = sqlite3_column_name(*stmt_ptr, i);
        int col_type = sqlite3_column_type(*stmt_ptr, i);
        
        lua_pushstring(L, col_name);
        
        switch (col_type) {
            case SQLITE_INTEGER:
                lua_pushinteger(L, sqlite3_column_int64(*stmt_ptr, i));
                break;
            case SQLITE_FLOAT:
                lua_pushnumber(L, sqlite3_column_double(*stmt_ptr, i));
                break;
            case SQLITE_TEXT:
                lua_pushstring(L, (const char*)sqlite3_column_text(*stmt_ptr, i));
                break;
            case SQLITE_BLOB:
                lua_pushlstring(L, (const char*)sqlite3_column_blob(*stmt_ptr, i),
                               sqlite3_column_bytes(*stmt_ptr, i));
                break;
            case SQLITE_NULL:
            default:
                lua_pushnil(L);
                break;
        }
        
        lua_settable(L, -3);
    }
    
    return 1;
}

static int iot_sqlite3_stmt_finalize(lua_State* L) {
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, STMT_METATABLE);
    if (*stmt_ptr) {
        sqlite3_finalize(*stmt_ptr);
        *stmt_ptr = NULL;
    }
    return 0;
}

static int iot_sqlite3_stmt_reset(lua_State* L) {
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, STMT_METATABLE);
    sqlite3_reset(*stmt_ptr);
    sqlite3_clear_bindings(*stmt_ptr);
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_sqlite3_stmt_gc(lua_State* L) {
    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)lua_touserdata(L, 1);
    if (*stmt_ptr) {
        sqlite3_finalize(*stmt_ptr);
        *stmt_ptr = NULL;
    }
    return 0;
}

static const luaL_Reg db_methods[] = {
    {"close", iot_sqlite3_close},
    {"exec", iot_sqlite3_exec},
    {"query", iot_sqlite3_query},
    {"prepare", iot_sqlite3_prepare},
    {"last_insert_id", iot_sqlite3_last_insert_id},
    {"changes", iot_sqlite3_changes},
    {"__gc", iot_sqlite3_db_gc},
    {NULL, NULL}
};

static const luaL_Reg stmt_methods[] = {
    {"bind", iot_sqlite3_stmt_bind},
    {"step", iot_sqlite3_stmt_step},
    {"finalize", iot_sqlite3_stmt_finalize},
    {"reset", iot_sqlite3_stmt_reset},
    {"__gc", iot_sqlite3_stmt_gc},
    {NULL, NULL}
};

LUAMOD_API int luaopen_sqlite3(lua_State* L) {
    luaL_newmetatable(L, DB_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, db_methods, 0);
    
    luaL_newmetatable(L, STMT_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, stmt_methods, 0);
    
    lua_newtable(L);
    lua_pushcfunction(L, iot_sqlite3_open);
    lua_setfield(L, -2, "open");
    
    lua_pushstring(L, "version");
    lua_pushstring(L, "3.53.2");
    lua_settable(L, -3);
    
    return 1;
}