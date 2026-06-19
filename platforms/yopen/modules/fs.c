/*
@module  fs
@summary 文件系统
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     FS
*/

/*
fs参考示例
-- 打开文件
local fd = fs.open("/test.txt", "r+")
if fd then
    -- 读取内容
    local data = fd:read(1024)
    print("content:", data)
    -- 写入内容
    fd:write("hello")
    -- 关闭文件
    fd:close()
end

-- 检查文件是否存在
local exists = fs.exists("/test.txt")
print("exists:", exists)

-- 创建目录
fs.mkdir("/dir")

-- 删除文件
fs.remove("/test.txt")
*/

#include "module.h"
#include "yopen_fs.h"

/**
 * @brief 打开文件
 * @api fs.open(path, mode)
 * @string path 文件路径
 * @string mode 打开模式 "r"/"w"/"a"/"r+"/"w+"/"a+"
 * @return userdata 文件句柄，nil表示失败
 */
static int luaopen_fs_open(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* mode = luaL_checkstring(L, 2);

    QFILE fp = yopen_fopen(path, mode);
    if (fp >= 0) {
        lua_pushlightuserdata(L, (void*)(intptr_t)fp);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 关闭文件
 * @api fd:close()
 * @return bool true表示成功
 */
static int luaopen_fs_file_close(lua_State* L) {
    QFILE fp = (QFILE)(intptr_t)lua_touserdata(L, 1);
    if (fp) {
        yopen_fclose(fp);
    }
    return 0;
}

/**
 * @brief 读取文件
 * @api fd:read(maxlen)
 * @int maxlen 最大读取字节数
 * @return string 读取的数据
 */
static int luaopen_fs_file_read(lua_State* L) {
    QFILE fp = (QFILE)(intptr_t)lua_touserdata(L, 1);
    int maxlen = (int)luaL_checkinteger(L, 2);

    char* buf = (char*)iot_malloc(maxlen + 1);
    if (!buf) {
        lua_pushnil(L);
        return 1;
    }

    int len = yopen_fread(fp, buf, maxlen);
    if (len > 0) {
        buf[len] = '\0';
        lua_pushstring(L, buf);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 写入文件
 * @api fd:write(data)
 * @string data 要写入的数据
 * @return int 写入的字节数
 */
static int luaopen_fs_file_write(lua_State* L) {
    QFILE fp = (QFILE)(intptr_t)lua_touserdata(L, 1);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int written = yopen_fwrite(fp, (void*)data, len);
    lua_pushinteger(L, written);
    return 1;
}

/**
 * @brief 文件指针定位
 * @api fd:seek(offset, whence)
 * @int offset 偏移量
 * @string whence 基准位置 "set"/"cur"/"end"
 * @return int 新的文件位置
 */
static int luaopen_fs_file_seek(lua_State* L) {
    QFILE fp = (QFILE)(intptr_t)lua_touserdata(L, 1);
    int offset = (int)luaL_checkinteger(L, 2);
    const char* whence = luaL_optstring(L, 3, "cur");

    int whence_val = YOPEN_SEEK_CUR;
    if (strcmp(whence, "set") == 0) whence_val = YOPEN_SEEK_SET;
    else if (strcmp(whence, "end") == 0) whence_val = YOPEN_SEEK_END;

    int pos = yopen_fseek(fp, offset, whence_val);
    lua_pushinteger(L, pos);
    return 1;
}

/**
 * @brief 检查文件是否存在
 * @api fs.exists(path)
 * @string path 文件路径
 * @return bool true表示存在
 */
static int luaopen_fs_exists(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    /* 使用 yopen_fopen 检查文件是否存在 */
    QFILE fp = yopen_fopen(path, "r");
    if (fp >= 0) {
        yopen_fclose(fp);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 创建目录
 * @api fs.mkdir(path)
 * @string path 目录路径
 * @return bool true表示成功
 */
static int luaopen_fs_mkdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = yopen_mkdir(path);
    lua_pushboolean(L, ret == YOPEN_FS_OK);
    return 1;
}

/**
 * @brief 删除文件
 * @api fs.remove(path)
 * @string path 文件路径
 * @return bool true表示成功
 */
static int luaopen_fs_remove(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = yopen_remove(path);
    lua_pushboolean(L, ret == YOPEN_FS_OK);
    return 1;
}

/**
 * @brief 重命名文件
 * @api fs.rename(oldpath, newpath)
 * @string oldpath 原路径
 * @string newpath 新路径
 * @return bool true表示成功
 */
static int luaopen_fs_rename(lua_State* L) {
    const char* oldpath = luaL_checkstring(L, 1);
    const char* newpath = luaL_checkstring(L, 2);
    int ret = yopen_rename(oldpath, newpath);
    lua_pushboolean(L, ret == YOPEN_FS_OK);
    return 1;
}

/**
 * @brief 获取文件大小
 * @api fs.size(path)
 * @string path 文件路径
 * @return int 文件大小，-1表示失败
 */
static int luaopen_fs_size(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int size = -1;

    QFILE fp = yopen_fopen(path, "r");
    if (fp >= 0) {
        size = yopen_fsize(fp);
        yopen_fclose(fp);
    }
    lua_pushinteger(L, size);
    return 1;
}

static const luaL_Reg luaopen_fs_funcs[] = {
    {"open",   luaopen_fs_open},
    {"exists", luaopen_fs_exists},
    {"mkdir",  luaopen_fs_mkdir},
    {"remove", luaopen_fs_remove},
    {"rename", luaopen_fs_rename},
    {"size",   luaopen_fs_size},
    {NULL, NULL}
};

static const luaL_Reg luaopen_fs_file_funcs[] = {
    {"close", luaopen_fs_file_close},
    {"read",  luaopen_fs_file_read},
    {"write", luaopen_fs_file_write},
    {"seek",  luaopen_fs_file_seek},
    {NULL, NULL}
};

static int luaopen_fs_gc(lua_State* L) {
    QFILE fp = (QFILE)(intptr_t)lua_touserdata(L, 1);
    if (fp) yopen_fclose(fp);
    return 0;
}

int luaopen_fs(lua_State* L) {
    luaL_newlib(L, luaopen_fs_funcs);

    /* 注册文件方法的元表 */
    luaL_newmetatable(L, "fs_file");
    luaL_setfuncs(L, luaopen_fs_file_funcs, 0);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, luaopen_fs_gc);
    lua_settable(L, -3);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    return 1;
}