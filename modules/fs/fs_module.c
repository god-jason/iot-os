/**
 * @file fs_module.c
 * @brief 基于 platform.h fs 适配层的 Lua 文件系统封装
 *
 * 提供面向对象的文件系统接口
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* IoT 核心头文件 */
#include "platform.h"
#include "iot_callback.h"
#include "fs_path.h"
#include "fs_dir.h"

/*===========================================================
 * 类型定义
 *===========================================================*/

/* 文件 Lua 上下文 */
typedef struct file_lua_ctx {
    iot_fs_file_t fp;           /* 文件句柄 */
    int closed;                  /* 是否已关闭 */
} file_lua_ctx_t;

/*===========================================================
 * 全局变量
 *===========================================================*/

static int g_lua_module_inited = 0;

/*===========================================================
 * 内部函数
 *===========================================================*/

/* 获取文件上下文 */
static file_lua_ctx_t* file_get_ctx_from_userdata(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        LOG("ERR: not userdata");
        return NULL;
    }

    file_lua_ctx_t** ctx_ptr = (file_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        LOG("ERR: null context");
        return NULL;
    }
    return *ctx_ptr;
}

/* 创建文件上下文 */
static file_lua_ctx_t* file_ctx_create(void) {
    file_lua_ctx_t* ctx = (file_lua_ctx_t*)iot_malloc(sizeof(file_lua_ctx_t));
    if (!ctx) {
        LOG("ERR: malloc file ctx failed");
        return NULL;
    }
    memset(ctx, 0, sizeof(file_lua_ctx_t));
    ctx->fp = NULL;
    ctx->closed = 0;
    return ctx;
}

/* 销毁文件上下文 */
static void file_ctx_destroy(file_lua_ctx_t* ctx) {
    if (!ctx) return;

    /* 关闭文件 */
    if (ctx->fp && !ctx->closed) {
        iot_fs_close(ctx->fp);
    }

    iot_free(ctx);
}

/*===========================================================
 * Lua 方法实现 - 文件操作
 *===========================================================*/

/**
 * @brief 打开文件
 * @api fs.open(path, mode)
 * @string path 文件路径
 * @string mode 打开模式，默认 "r"
 * @return file 文件对象，失败返回 nil
 * @usage
 * local f = fs.open("/test.txt", "w")
 * if f then
 *     f:write("hello world")
 *     f:close()
 * end
 */
static int iot_fs_open(lua_State* L) {

    const char* path = luaL_checkstring(L, 1);
    const char* mode = luaL_optstring(L, 2, "r");

    /* 创建上下文 */
    file_lua_ctx_t* ctx = file_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create file context");
        return 2;
    }

    /* 打开文件 */
    ctx->fp = iot_fs_open(path, mode);
    if (!ctx->fp) {
        iot_free(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "failed to open file");
        return 2;
    }

    /* 创建 userdata 对象 */
    file_lua_ctx_t** ctx_ptr = (file_lua_ctx_t**)lua_newuserdata(L, sizeof(file_lua_ctx_t*));
    *ctx_ptr = ctx;

    /* 设置元表 */
    luaL_getmetatable(L, "fs.file");
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 写入数据
 * @api file:write(data)
 * @string data 要写入的数据
 * @return int 写入的字节数
 * @usage
 * local n = file:write("hello")
 */
static int iot_file_write(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushinteger(L, -1);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int written = iot_fs_write(ctx->fp, data, len);
    lua_pushinteger(L, written);
    return 1;
}

/**
 * @brief 读取数据
 * @api file:read(size)
 * @int size 要读取的字节数
 * @return string 读取到的数据，失败返回 nil
 * @usage
 * local data = file:read(1024)
 */
static int iot_file_read(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushnil(L);
        return 1;
    }

    int size = (int)luaL_optinteger(L, 2, 1024);

    char* buf = (char*)iot_malloc(size);
    if (!buf) {
        lua_pushnil(L);
        return 1;
    }

    int len = iot_fs_read(ctx->fp, buf, size);
    if (len > 0) {
        lua_pushlstring(L, buf, len);
    } else {
        lua_pushnil(L);
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 读取一行
 * @api file:readline()
 * @return string 读取到的一行数据，失败返回 nil
 * @usage
 * local line = file:readline()
 */
static int iot_file_readline(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushnil(L);
        return 1;
    }

    char buf[1024];
    int i = 0;
    while (i < sizeof(buf) - 1) {
        char c;
        int ret = iot_fs_read(ctx->fp, &c, 1);
        if (ret != 1) break;
        buf[i++] = c;
        if (c == '\n') break;
    }
    buf[i] = '\0';

    if (i > 0 || (i == 0 && buf[0] != '\0')) {
        lua_pushstring(L, buf);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

/**
 * @brief 写入一行
 * @api file:writeline(data)
 * @string data 要写入的数据
 * @return int 写入的字节数
 * @usage
 * file:writeline("hello world")
 */
static int iot_file_writeline(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushinteger(L, -1);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int written = iot_fs_write(ctx->fp, data, len);
    if (written >= 0) {
        written += iot_fs_write(ctx->fp, "\n", 1);
    }
    lua_pushinteger(L, written);
    return 1;
}

/**
 * @brief 移动文件指针
 * @api file:seek(offset, whence)
 * @int offset 偏移量
 * @int whence 起始位置：0=文件头，1=当前位置，2=文件尾
 * @return int 当前位置，失败返回 -1
 * @usage
 * file:seek(0, 0)  -- 移动到文件头
 * file:seek(0, 2)  -- 移动到文件尾
 */
static int iot_file_seek(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushinteger(L, -1);
        return 1;
    }

    int offset = (int)luaL_optinteger(L, 2, 0);
    int whence = (int)luaL_optinteger(L, 3, 0);

    int ret = iot_fs_seek(ctx->fp, offset, whence);
    if (ret == 0) {
        lua_pushinteger(L, iot_fs_tell(ctx->fp));
    } else {
        lua_pushinteger(L, -1);
    }

    return 1;
}

/**
 * @brief 刷新文件缓冲区
 * @api file:flush()
 * @return bool 成功返回 true
 * @usage
 * file:flush()
 */
static int iot_file_flush(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = iot_fs_sync(ctx->fp);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 关闭文件
 * @api file:close()
 * @usage
 * file:close()
 */
static int iot_file_close(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        return 0;
    }

    /* 销毁上下文 */
    file_ctx_destroy(ctx);

    return 0;
}

/**
 * @brief 获取文件大小
 * @api file:size()
 * @return int 文件大小
 * @usage
 * local size = file:size()
 */
static int iot_file_size(lua_State* L) {
    file_lua_ctx_t* ctx = file_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->fp || ctx->closed) {
        lua_pushinteger(L, -1);
        return 1;
    }

    long current = iot_fs_tell(ctx->fp);
    iot_fs_seek(ctx->fp, 0, IOT_FS_SEEK_END);
    long size = iot_fs_tell(ctx->fp);
    iot_fs_seek(ctx->fp, current, IOT_FS_SEEK_SET);

    lua_pushinteger(L, size);
    return 1;
}

/*===========================================================
 * Lua 方法实现 - 文件系统操作
 *===========================================================*/

/**
 * @brief 创建目录
 * @api fs.mkdir(path, mode)
 * @string path 目录路径
 * @int mode 权限模式，默认 0755
 * @return bool 成功返回 true
 * @usage
 * fs.mkdir("/tmp/test", 0755)
 */
static int iot_fs_mkdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int mode = (int)luaL_optinteger(L, 2, 0755);

    int ret = iot_fs_mkdir(path, mode);
    lua_pushboolean(L, ret);
    return 1;
}

/**
 * @brief 删除文件或目录
 * @api fs.remove(path)
 * @string path 文件或目录路径
 * @return bool 成功返回 true
 * @usage
 * fs.remove("/tmp/test.txt")
 */
static int iot_fs_remove(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);

    int ret = iot_fs_remove(path);
    lua_pushboolean(L, ret);
    return 1;
}

/**
 * @brief 重命名文件或目录
 * @api fs.rename(oldpath, newpath)
 * @string oldpath 原路径
 * @string newpath 新路径
 * @return bool 成功返回 true
 * @usage
 * fs.rename("/tmp/old.txt", "/tmp/new.txt")
 */
static int iot_fs_rename(lua_State* L) {
    const char* oldpath = luaL_checkstring(L, 1);
    const char* newpath = luaL_checkstring(L, 2);

    int ret = iot_fs_rename(oldpath, newpath);
    lua_pushboolean(L, ret);
    return 1;
}

/**
 * @brief 检查文件是否存在
 * @api fs.exists(path)
 * @string path 文件路径
 * @return bool 存在返回 true
 * @usage
 * if fs.exists("/tmp/test.txt") then
 *     print("file exists")
 * end
 */
static int iot_fs_exists(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);

    int ret = iot_fs_access(path, 0);
    lua_pushboolean(L, ret);
    return 1;
}

/**
 * @brief 检查文件权限
 * @api fs.access(path, mode)
 * @string path 文件路径
 * @int mode 权限模式：0=存在，1=可执行，2=可写，4=可读
 * @return bool 有权限返回 true
 * @usage
 * if fs.access("/tmp/test.txt", 4) then
 *     print("file readable")
 * end
 */
static int iot_fs_access(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int mode = (int)luaL_optinteger(L, 2, 0);

    int ret = iot_fs_access(path, mode);
    lua_pushboolean(L, ret);
    return 1;
}

/**
 * @brief 获取文件信息
 * @api fs.stat(path)
 * @string path 文件路径
 * @return table 文件信息表，失败返回 nil
 * @usage
 * local info = fs.stat("/tmp/test.txt")
 * if info then
 *     print("size:", info.size)
 *     print("mode:", info.mode)
 * end
 */
static int iot_fs_stat(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);

    if (!iot_fs_access(path, 0)) {
        lua_pushnil(L);
        return 1;
    }

    lua_newtable(L);

    long filesize = iot_fs_filesize(path);
    lua_pushstring(L, "size");
    lua_pushinteger(L, filesize);
    lua_settable(L, -3);

    lua_pushstring(L, "mode");
    lua_pushinteger(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "mtime");
    lua_pushinteger(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "atime");
    lua_pushinteger(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "ctime");
    lua_pushinteger(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "is_dir");
    lua_pushboolean(L, 0);
    lua_settable(L, -3);

    lua_pushstring(L, "is_file");
    lua_pushboolean(L, 1);
    lua_settable(L, -3);

    return 1;
}

/*===========================================================
 * Lua 方法实现 - 路径处理
 *===========================================================*/

static int iot_fs_basename(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* base = fs_path_basename(path);
    if (base) {
        lua_pushstring(L, base);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_dirname(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    char buf[512];
    if (fs_path_dirname(path, buf, sizeof(buf))) {
        lua_pushstring(L, buf);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_extname(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* ext = fs_path_extname(path);
    if (ext) {
        lua_pushstring(L, ext);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_join(lua_State* L) {
    const char* path1 = luaL_checkstring(L, 1);
    const char* path2 = luaL_checkstring(L, 2);
    char buf[1024];
    if (fs_path_join(buf, sizeof(buf), path1, path2)) {
        lua_pushstring(L, buf);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_is_absolute(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lua_pushboolean(L, fs_path_is_absolute(path));
    return 1;
}

static int iot_fs_normalize(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    char buf[1024];
    if (fs_path_normalize(buf, sizeof(buf), path)) {
        lua_pushstring(L, buf);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_stem(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    char buf[512];
    if (fs_path_stem(buf, sizeof(buf), path)) {
        lua_pushstring(L, buf);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_fs_split(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    char dir_buf[512];
    char base_buf[512];
    
    if (fs_path_split(path, dir_buf, sizeof(dir_buf), base_buf, sizeof(base_buf)) == 0) {
        lua_pushstring(L, dir_buf);
        lua_pushstring(L, base_buf);
        return 2;
    } else {
        lua_pushnil(L);
        return 1;
    }
}

static int iot_fs_lsdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    
    fs_dir_entry_t* entries = NULL;
    int count = 0;
    
    if (fs_dir_list(path, &entries, &count) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to list directory");
        return 2;
    }
    
    lua_newtable(L);
    
    for (int i = 0; i < count; i++) {
        lua_newtable(L);
        
        lua_pushstring(L, entries[i].name);
        lua_setfield(L, -2, "name");
        
        lua_pushboolean(L, entries[i].is_dir);
        lua_setfield(L, -2, "is_dir");
        
        lua_pushinteger(L, entries[i].size);
        lua_setfield(L, -2, "size");
        
        lua_pushinteger(L, entries[i].mtime);
        lua_setfield(L, -2, "mtime");
        
        lua_rawseti(L, -2, i + 1);
    }
    
    fs_dir_free_list(entries);
    
    return 1;
}

/*===========================================================
 * Lua 模块定义
 *===========================================================*/

/* file 方法表 */
static const luaL_Reg file_methods[] = {
    { "write",    iot_file_write },    /* 写入 */
    { "read",     iot_file_read },     /* 读取 */
    { "readline", iot_file_readline }, /* 读取一行 */
    { "writeline",iot_file_writeline },/* 写入一行 */
    { "seek",     iot_file_seek },     /* 移动指针 */
    { "flush",    iot_file_flush },    /* 刷新缓冲 */
    { "close",    iot_file_close },    /* 关闭 */
    { "size",     iot_file_size },     /* 获取大小 */
    { "__gc",     iot_file_close },    /* 垃圾回收 */
    { "__tostring", iot_file_close },  /* 字符串转换 */
    { NULL,       NULL }
};

/* fs 模块方法表 */
static const luaL_Reg fs_methods[] = {
    { "open",          iot_fs_open },          /* 打开文件 */
    { "mkdir",         iot_fs_mkdir },         /* 创建目录 */
    { "remove",        iot_fs_remove },        /* 删除文件 */
    { "rename",        iot_fs_rename },        /* 重命名 */
    { "exists",        iot_fs_exists },        /* 检查存在 */
    { "access",        iot_fs_access },        /* 检查权限 */
    { "stat",          iot_fs_stat },          /* 获取文件信息 */
    { "basename",      iot_fs_basename },      /* 获取文件名 */
    { "dirname",       iot_fs_dirname },       /* 获取目录名 */
    { "extname",       iot_fs_extname },       /* 获取扩展名 */
    { "join",          iot_fs_join },          /* 拼接路径 */
    { "isAbsolute",    iot_fs_is_absolute },   /* 判断绝对路径 */
    { "normalize",     iot_fs_normalize },     /* 规范化路径 */
    { "stem",          iot_fs_stem },          /* 获取不含扩展名的文件名 */
    { "split",         iot_fs_split },         /* 分割路径 */
    { "lsdir",         iot_fs_lsdir },         /* 遍历目录 */
    { NULL,            NULL }
};

/* 常量定义 */
static const luaL_Const fs_constants[] = {
    /* 文件打开模式 */
    { "R_OK",  4 },  /* 可读 */
    { "W_OK",  2 },  /* 可写 */
    { "X_OK",  1 },  /* 可执行 */
    { "F_OK",  0 },  /* 存在 */

    /* 文件指针位置 */
    { "SEEK_SET", 0 }, /* 文件头 */
    { "SEEK_CUR", 1 }, /* 当前位置 */
    { "SEEK_END", 2 }, /* 文件尾 */

    { NULL, 0 }
};

/**
 * @brief 注册 fs 模块
 */
int luaopen_fs(lua_State* L) {
    /* 创建 file 元表 */
    luaL_newmetatable(L, "fs.file");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, file_methods, 0);
    lua_pop(L, 1);

    /* 创建 fs 模块 */
    luaL_newlib(L, fs_methods);

    /* 注册常量 */
    for (const luaL_Const* c = fs_constants; c->name; c++) {
        lua_pushinteger(L, c->value);
        lua_setfield(L, -2, c->name);
    }

    return 1;
}