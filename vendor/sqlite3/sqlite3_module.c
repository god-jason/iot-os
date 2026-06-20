/**
 * @file lua_module.c
 * @brief SQLite3 库的 Lua 模块封装
 *
 * 提供 SQLite3 数据库的 Lua 接口
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* SQLite3 头文件 */
#include "sqlite3.h"

/*===========================================================
 * 常量定义
 *===========================================================*/

/* 数据库连接元表名称 */
#define SQLITE3_DB_METATABLE "sqlite3.database"

/* 预编译语句元表名称 */
#define SQLITE3_STMT_METATABLE "sqlite3.statement"

/*===========================================================
 * 版本信息
 *===========================================================*/

/**
 * @brief 获取 SQLite3 版本号
 * @api sqlite3.version_number()
 * @return 版本号数字
 */
static int sqlite3_version_number(lua_State* L) {
    lua_pushinteger(L, SQLITE_VERSION_NUMBER);
    return 1;
}

/**
 * @brief 获取 SQLite3 版本字符串
 * @api sqlite3.version()
 * @return 版本字符串 "X.Y.Z"
 */
static int sqlite3_version(lua_State* L) {
    lua_pushstring(L, SQLITE_VERSION);
    return 1;
}

/**
 * @brief 获取 SQLite3 源码标识
 * @api sqlite3.source_id()
 * @return 源码标识字符串
 */
static int sqlite3_source_id(lua_State* L) {
    lua_pushstring(L, SQLITE_SOURCE_ID);
    return 1;
}

/**
 * @brief 获取 SQLite3 库版本字符串
 * @api sqlite3.libversion()
 * @return 库版本字符串
 */
static int sqlite3_libversion(lua_State* L) {
    lua_pushstring(L, sqlite3_libversion());
    return 1;
}

/*===========================================================
 * 数据库连接
 *===========================================================*/

/**
 * @brief 打开数据库
 * @api sqlite3.open(filename)
 * @string filename 数据库文件路径，":memory:" 表示内存数据库
 * @return 数据库连接对象，失败返回 nil 和错误信息
 */
static int sqlite3_open(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);

    sqlite3* db = NULL;
    int result = sqlite3_open(filename, &db);

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(db));
        if (db) {
            sqlite3_close(db);
        }
        return 2;
    }

    sqlite3** ptr = (sqlite3**)lua_newuserdata(L, sizeof(sqlite3*));
    *ptr = db;

    luaL_getmetatable(L, SQLITE3_DB_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 打开数据库（带选项）
 * @api sqlite3.open_v2(filename, flags)
 * @string filename 数据库文件路径
 * @int flags 打开标志（SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, SQLITE_OPEN_CREATE）
 * @return 数据库连接对象，失败返回 nil 和错误信息
 */
static int sqlite3_open_v2(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    int flags = (int)luaL_checkinteger(L, 2);

    sqlite3* db = NULL;
    int result = sqlite3_open_v2(filename, &db, flags, NULL);

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(db));
        if (db) {
            sqlite3_close(db);
        }
        return 2;
    }

    sqlite3** ptr = (sqlite3**)lua_newuserdata(L, sizeof(sqlite3*));
    *ptr = db;

    luaL_getmetatable(L, SQLITE3_DB_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 关闭数据库
 * @api db:close()
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int db_close(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);

    if (!ptr || !*ptr) {
        lua_pushboolean(L, 1);
        return 1;
    }

    int result = sqlite3_close(*ptr);
    *ptr = NULL;

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errstr(result));
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 执行 SQL 语句
 * @api db:exec(sql)
 * @string sql SQL 语句
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int db_exec(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);
    const char* sql = luaL_checkstring(L, 2);

    if (!ptr || !*ptr) {
        luaL_error(L, "database is closed");
    }

    char* errmsg = NULL;
    int result = sqlite3_exec(*ptr, sql, NULL, NULL, &errmsg);

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, errmsg);
        sqlite3_free(errmsg);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 获取最后插入的行 ID
 * @api db:last_insert_rowid()
 * @return 最后插入的行 ID
 */
static int db_last_insert_rowid(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "database is closed");
    }

    lua_Integer rowid = sqlite3_last_insert_rowid(*ptr);
    lua_pushinteger(L, rowid);
    return 1;
}

/**
 * @brief 获取受影响的行数
 * @api db:changes()
 * @return 受影响的行数
 */
static int db_changes(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "database is closed");
    }

    int changes = sqlite3_changes(*ptr);
    lua_pushinteger(L, changes);
    return 1;
}

/**
 * @brief 获取错误信息
 * @api db:errmsg()
 * @return 错误信息字符串
 */
static int db_errmsg(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);

    if (!ptr || !*ptr) {
        lua_pushstring(L, "database is closed");
        return 1;
    }

    lua_pushstring(L, sqlite3_errmsg(*ptr));
    return 1;
}

/**
 * @brief 获取错误码
 * @api db:errcode()
 * @return 错误码
 */
static int db_errcode(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);

    if (!ptr || !*ptr) {
        lua_pushinteger(L, SQLITE_ERROR);
        return 1;
    }

    lua_pushinteger(L, sqlite3_errcode(*ptr));
    return 1;
}

/*===========================================================
 * 预编译语句
 *===========================================================*/

/**
 * @brief 预编译 SQL 语句
 * @api db:prepare(sql)
 * @string sql SQL 语句
 * @return 预编译语句对象，失败返回 nil 和错误信息
 */
static int db_prepare(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);
    const char* sql = luaL_checkstring(L, 2);

    if (!ptr || !*ptr) {
        luaL_error(L, "database is closed");
    }

    sqlite3_stmt* stmt = NULL;
    const char* tail = NULL;
    int result = sqlite3_prepare_v2(*ptr, sql, -1, &stmt, &tail);

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errmsg(*ptr));
        return 2;
    }

    sqlite3_stmt** stmt_ptr = (sqlite3_stmt**)lua_newuserdata(L, sizeof(sqlite3_stmt*));
    *stmt_ptr = stmt;

    luaL_getmetatable(L, SQLITE3_STMT_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 绑定参数
 * @api stmt:bind(...)
 * @param ... 参数值
 * @return 成功返回 true
 */
static int stmt_bind(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int param_count = sqlite3_bind_parameter_count(*ptr);
    int arg_count = lua_gettop(L) - 1;

    if (arg_count != param_count) {
        luaL_error(L, "parameter count mismatch: expected %d, got %d", param_count, arg_count);
    }

    for (int i = 1; i <= arg_count; i++) {
        int param_idx = i;
        int result;

        int type = lua_type(L, i + 1);

        switch (type) {
            case LUA_TNIL:
                result = sqlite3_bind_null(*ptr, param_idx);
                break;

            case LUA_TBOOLEAN:
                result = sqlite3_bind_int(*ptr, param_idx, lua_toboolean(L, i + 1));
                break;

            case LUA_TNUMBER:
                if (lua_isinteger(L, i + 1)) {
                    result = sqlite3_bind_int64(*ptr, param_idx, lua_tointeger(L, i + 1));
                } else {
                    result = sqlite3_bind_double(*ptr, param_idx, lua_tonumber(L, i + 1));
                }
                break;

            case LUA_TSTRING:
                result = sqlite3_bind_text(*ptr, param_idx, lua_tostring(L, i + 1), -1, SQLITE_TRANSIENT);
                break;

            default:
                luaL_error(L, "unsupported parameter type at index %d", i);
                break;
        }

        if (result != SQLITE_OK) {
            luaL_error(L, "bind failed at index %d", i);
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 重置语句
 * @api stmt:reset()
 * @return 成功返回 true
 */
static int stmt_reset(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int result = sqlite3_reset(*ptr);

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errstr(result));
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 执行一步
 * @api stmt:step()
 * @return 有行返回 true，完成返回 false，失败返回 nil 和错误信息
 */
static int stmt_step(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int result = sqlite3_step(*ptr);

    if (result == SQLITE_ROW) {
        lua_pushboolean(L, 1);
        return 1;
    }

    if (result == SQLITE_DONE) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushnil(L);
    lua_pushstring(L, sqlite3_errstr(result));
    return 2;
}

/**
 * @brief 获取列数
 * @api stmt:column_count()
 * @return 列数
 */
static int stmt_column_count(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int count = sqlite3_column_count(*ptr);
    lua_pushinteger(L, count);
    return 1;
}

/**
 * @brief 获取列名
 * @api stmt:column_name(index)
 * @int index 列索引（从 0 开始）
 * @return 列名
 */
static int stmt_column_name(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);
    int index = (int)luaL_checkinteger(L, 2);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    const char* name = sqlite3_column_name(*ptr, index);
    lua_pushstring(L, name);
    return 1;
}

/**
 * @brief 获取列值
 * @api stmt:column_value(index)
 * @int index 列索引（从 0 开始）
 * @return 列值
 */
static int stmt_column_value(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);
    int index = (int)luaL_checkinteger(L, 2);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int type = sqlite3_column_type(*ptr, index);

    switch (type) {
        case SQLITE_INTEGER:
            lua_pushinteger(L, sqlite3_column_int64(*ptr, index));
            break;

        case SQLITE_FLOAT:
            lua_pushnumber(L, sqlite3_column_double(*ptr, index));
            break;

        case SQLITE_TEXT:
            lua_pushstring(L, (const char*)sqlite3_column_text(*ptr, index));
            break;

        case SQLITE_BLOB:
            lua_pushlstring(L, (const char*)sqlite3_column_blob(*ptr, index), sqlite3_column_bytes(*ptr, index));
            break;

        case SQLITE_NULL:
            lua_pushnil(L);
            break;

        default:
            lua_pushnil(L);
            break;
    }

    return 1;
}

/**
 * @brief 获取一行数据（数组形式）
 * @api stmt:get_row()
 * @return 包含所有列值的数组
 */
static int stmt_get_row(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int column_count = sqlite3_column_count(*ptr);

    lua_newtable(L);

    for (int i = 0; i < column_count; i++) {
        int type = sqlite3_column_type(*ptr, i);

        switch (type) {
            case SQLITE_INTEGER:
                lua_pushinteger(L, sqlite3_column_int64(*ptr, i));
                break;

            case SQLITE_FLOAT:
                lua_pushnumber(L, sqlite3_column_double(*ptr, i));
                break;

            case SQLITE_TEXT:
                lua_pushstring(L, (const char*)sqlite3_column_text(*ptr, i));
                break;

            case SQLITE_BLOB:
                lua_pushlstring(L, (const char*)sqlite3_column_blob(*ptr, i), sqlite3_column_bytes(*ptr, i));
                break;

            case SQLITE_NULL:
                lua_pushnil(L);
                break;

            default:
                lua_pushnil(L);
                break;
        }

        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

/**
 * @brief 获取一行数据（字典形式）
 * @api stmt:get_row_dict()
 * @return 包含所有列名和值的字典
 */
static int stmt_get_row_dict(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        luaL_error(L, "statement is finalized");
    }

    int column_count = sqlite3_column_count(*ptr);

    lua_newtable(L);

    for (int i = 0; i < column_count; i++) {
        const char* name = sqlite3_column_name(*ptr, i);
        lua_pushstring(L, name);

        int type = sqlite3_column_type(*ptr, i);

        switch (type) {
            case SQLITE_INTEGER:
                lua_pushinteger(L, sqlite3_column_int64(*ptr, i));
                break;

            case SQLITE_FLOAT:
                lua_pushnumber(L, sqlite3_column_double(*ptr, i));
                break;

            case SQLITE_TEXT:
                lua_pushstring(L, (const char*)sqlite3_column_text(*ptr, i));
                break;

            case SQLITE_BLOB:
                lua_pushlstring(L, (const char*)sqlite3_column_blob(*ptr, i), sqlite3_column_bytes(*ptr, i));
                break;

            case SQLITE_NULL:
                lua_pushnil(L);
                break;

            default:
                lua_pushnil(L);
                break;
        }

        lua_rawset(L, -3);
    }

    return 1;
}

/**
 * @brief 释放预编译语句
 * @api stmt:finalize()
 * @return 成功返回 true
 */
static int stmt_finalize(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);

    if (!ptr || !*ptr) {
        lua_pushboolean(L, 1);
        return 1;
    }

    int result = sqlite3_finalize(*ptr);
    *ptr = NULL;

    if (result != SQLITE_OK) {
        lua_pushnil(L);
        lua_pushstring(L, sqlite3_errstr(result));
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 数据库连接垃圾回收
 */
static int db_gc(lua_State* L) {
    sqlite3** ptr = (sqlite3**)luaL_checkudata(L, 1, SQLITE3_DB_METATABLE);
    if (ptr && *ptr) {
        sqlite3_close(*ptr);
        *ptr = NULL;
    }
    return 0;
}

/**
 * @brief 预编译语句垃圾回收
 */
static int stmt_gc(lua_State* L) {
    sqlite3_stmt** ptr = (sqlite3_stmt**)luaL_checkudata(L, 1, SQLITE3_STMT_METATABLE);
    if (ptr && *ptr) {
        sqlite3_finalize(*ptr);
        *ptr = NULL;
    }
    return 0;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* 数据库连接方法表 */
static const luaL_Reg db_methods[] = {
    { "close",              db_close },
    { "exec",               db_exec },
    { "prepare",            db_prepare },
    { "last_insert_rowid",  db_last_insert_rowid },
    { "changes",            db_changes },
    { "errmsg",             db_errmsg },
    { "errcode",            db_errcode },
    { "__gc",               db_gc },
    { NULL,                 NULL }
};

/* 预编译语句方法表 */
static const luaL_Reg stmt_methods[] = {
    { "bind",             stmt_bind },
    { "reset",            stmt_reset },
    { "step",             stmt_step },
    { "column_count",     stmt_column_count },
    { "column_name",      stmt_column_name },
    { "column_value",     stmt_column_value },
    { "get_row",          stmt_get_row },
    { "get_row_dict",     stmt_get_row_dict },
    { "finalize",         stmt_finalize },
    { "__gc",             stmt_gc },
    { NULL,               NULL }
};

/* sqlite3 模块方法表 */
static const luaL_Reg sqlite3_module_methods[] = {
    /* 版本信息 */
    { "version_number", sqlite3_version_number },
    { "version",        sqlite3_version },
    { "source_id",      sqlite3_source_id },
    { "libversion",     sqlite3_libversion },

    /* 数据库连接 */
    { "open",           sqlite3_open },
    { "open_v2",        sqlite3_open_v2 },

    { NULL,             NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_sqlite3(lua_State* L) {
    /* 创建模块 */
    luaL_newlib(L, sqlite3_module_methods);

    /* 创建数据库连接元表 */
    luaL_newmetatable(L, SQLITE3_DB_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, db_methods, 0);
    lua_pop(L, 1);

    /* 创建预编译语句元表 */
    luaL_newmetatable(L, SQLITE3_STMT_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, stmt_methods, 0);
    lua_pop(L, 1);

    /* 注册常量 */
    lua_pushinteger(L, SQLITE_OK);
    lua_setfield(L, -2, "OK");

    lua_pushinteger(L, SQLITE_ERROR);
    lua_setfield(L, -2, "ERROR");

    lua_pushinteger(L, SQLITE_ROW);
    lua_setfield(L, -2, "ROW");

    lua_pushinteger(L, SQLITE_DONE);
    lua_setfield(L, -2, "DONE");

    lua_pushinteger(L, SQLITE_OPEN_READONLY);
    lua_setfield(L, -2, "OPEN_READONLY");

    lua_pushinteger(L, SQLITE_OPEN_READWRITE);
    lua_setfield(L, -2, "OPEN_READWRITE");

    lua_pushinteger(L, SQLITE_OPEN_CREATE);
    lua_setfield(L, -2, "OPEN_CREATE");

    return 1;
}