/*
@module  fs
@summary 文件系统
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     FS
*/

/*
FS参考示例
-- 打开文件
local f = fs.open("/usr/test.txt", "r+")

-- 读取文件
local data = f:read(100)

-- 写入文件
f:write("hello")

-- 关闭文件
f:close()

-- 删除文件
fs.remove("/usr/test.txt")

-- 重命名文件
fs.rename("/usr/test.txt", "/usr/test2.txt")

-- 判断文件是否存在
local exist = fs.exists("/usr/test.txt")

-- 获取文件大小
local size = fs.size("/usr/test.txt")

-- 创建目录
fs.mkdir("/app/data")

-- 删除空目录
fs.rmdir("/app/data")

-- 遍历目录
local files = fs.list("/app")
for i, v in ipairs(files) do
    print(v.name, v.is_file and "file" or "dir", v.size)
end
*/


#include "lua.h"
#include "module.h"

/* 文件打开模式映射 */
static int fs_mode_to_flag(const char* mode) {
    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0) return CM_FS_RB;
    if (strcmp(mode, "w") == 0 || strcmp(mode, "wb") == 0) return CM_FS_WB;
    if (strcmp(mode, "a") == 0 || strcmp(mode, "ab") == 0) return CM_FS_AB;
    if (strcmp(mode, "r+") == 0 || strcmp(mode, "rb+") == 0) return CM_FS_RBPLUS;
    if (strcmp(mode, "w+") == 0 || strcmp(mode, "wb+") == 0) return CM_FS_WBPLUS;
    if (strcmp(mode, "a+") == 0 || strcmp(mode, "ab+") == 0) return CM_FS_ABPLUS;
    return CM_FS_RB;
}

/**
 * @brief 打开文件
 * @api fs.open(path, mode)
 * @string path 文件路径
 * @string mode 打开模式,"r","w","a","r+","w+","a+"
 * @return file|userdata|nil 成功返回文件对象,失败返回nil
 * @usage
local f = fs.open("/usr/test.txt", "r")
if f then
    local data = f:read(100)
    f:close()
end
*/
static int iot_fs_open(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* mode = luaL_optstring(L, 2, "r");

    int flag = fs_mode_to_flag(mode);
    cm_fs_t fd = cm_fs_open(path, flag);

    if (fd == NULL) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlightuserdata(L, fd);
    return 1;
}

/**
 * @brief 关闭文件
 * @api file:close()
 * @userdata file 文件对象
 * @return bool 成功返回true,失败返回false
 * @usage
f:close()
*/
static int iot_fs_file_close(lua_State* L) {
    cm_fs_t fd = (cm_fs_t)lua_touserdata(L, 1);
    if (fd == NULL) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_fs_close(fd);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 读取文件
 * @api file:read(len)
 * @userdata file 文件对象
 * @int len 读取长度
 * @return string 读取到的数据,失败返回nil
 * @usage
local data = f:read(100)
*/
static int iot_fs_file_read(lua_State* L) {
    cm_fs_t fd = (cm_fs_t)lua_touserdata(L, 1);
    int len = luaL_checkinteger(L, 2);

    if (fd == NULL || len <= 0) {
        lua_pushnil(L);
        return 1;
    }

    char* buff = (char*)lua_newuserdata(L, len);
    int ret = cm_fs_read(fd, buff, len);
    if (ret <= 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, buff, ret);
    return 1;
}

/**
 * @brief 写入文件
 * @api file:write(data)
 * @userdata file 文件对象
 * @string data 待写入数据
 * @return int 实际写入长度,失败返回nil
 * @usage
f:write("hello")
*/
static int iot_fs_file_write(lua_State* L) {
    cm_fs_t fd = (cm_fs_t)lua_touserdata(L, 1);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    if (fd == NULL) {
        lua_pushnil(L);
        return 1;
    }

    int ret = cm_fs_write(fd, data, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

/**
 * @brief 文件指针定位
 * @api file:seek(offset, base)
 * @userdata file 文件对象
 * @int offset 偏移量
 * @string base 基准位置,"set"(开头),"cur"(当前位置),"end"(结尾)
 * @return int 新的文件位置,失败返回nil
 * @usage
f:seek(0, "set")
*/
static int iot_fs_file_seek(lua_State* L) {
    cm_fs_t fd = (cm_fs_t)lua_touserdata(L, 1);
    int offset = luaL_checkinteger(L, 2);
    const char* base = luaL_optstring(L, 3, "cur");

    if (fd == NULL) {
        lua_pushnil(L);
        return 1;
    }

    int base_val;
    if (strcmp(base, "set") == 0) base_val = CM_FS_SEEK_SET;
    else if (strcmp(base, "end") == 0) base_val = CM_FS_SEEK_END;
    else base_val = CM_FS_SEEK_CUR;

    int ret = cm_fs_seek(fd, offset, base_val);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

/**
 * @brief 删除文件
 * @api fs.remove(path)
 * @string path 文件路径
 * @return bool 成功返回true,失败返回false
 * @usage
fs.remove("/usr/test.txt")
*/
static int iot_fs_remove(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = cm_fs_delete(path);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 重命名文件
 * @api fs.rename(old_path, new_path)
 * @string old_path 原文件路径
 * @string new_path 新文件路径
 * @return bool 成功返回true,失败返回false
 * @usage
fs.rename("/usr/test.txt", "/usr/test2.txt")
*/
static int iot_fs_rename(lua_State* L) {
    const char* old_path = luaL_checkstring(L, 1);
    const char* new_path = luaL_checkstring(L, 2);
    int ret = cm_fs_move(old_path, new_path);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 判断文件是否存在
 * @api fs.exists(path)
 * @string path 文件路径
 * @return bool 存在返回true,不存在返回false
 * @usage
local exist = fs.exists("/usr/test.txt")
*/
static int iot_fs_exists(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = cm_fs_exist(path);
    lua_pushboolean(L, ret == 1);
    return 1;
}

/**
 * @brief 获取文件大小
 * @api fs.size(path)
 * @string path 文件路径
 * @return int 文件大小,失败返回0
 * @usage
local size = fs.size("/usr/test.txt")
*/
static int iot_fs_size(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = cm_fs_filesize(path);
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 获取文件系统信息
 * @api fs.info()
 * @return table 文件系统信息 {total_size, free_size}
 * @usage
local info = fs.info()
print("total:", info.total_size, "free:", info.free_size)
*/
static int iot_fs_info(lua_State* L) {
    cm_fs_system_info_t info;
    int ret = cm_fs_getinfo(&info);

    if (ret != 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 2);
    lua_pushinteger(L, info.total_size);
    lua_setfield(L, -2, "total_size");
    lua_pushinteger(L, info.free_size);
    lua_setfield(L, -2, "free_size");

    return 1;
}

/**
 * @brief 创建目录
 * @api fs.mkdir(path)
 * @string path 目录路径
 * @return bool 成功返回true,失败返回false
 * @usage
fs.mkdir("/app/data")
*/
static int iot_fs_mkdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = cm_fs_mkdir(path);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 删除目录
 * @api fs.rmdir(path)
 * @string path 目录路径
 * @return bool 成功返回true,失败返回false
 * @usage
fs.rmdir("/app/data")
*/
static int iot_fs_rmdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ret = cm_fs_rmdir(path);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 遍历目录
 * @api fs.list(path)
 * @string path 目录路径
 * @return table 目录内容列表，每项 {name, is_file, size}
 * @usage
local files = fs.list("/app")
for i, v in ipairs(files) do
    print(v.name, v.is_file and "file" or "dir", v.size)
end
*/
static int iot_fs_list(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    cm_fs_file_data_t file_data;
    uint32_t find_fd;
    int index = 1;

    /* 打开目录遍历 */
    find_fd = cm_fs_find_first(path, &file_data);
    if (find_fd == 0) {
        lua_createtable(L, 0, 0);
        return 1;
    }

    lua_createtable(L, 0, 16);

    do {
        /* 跳过 "." 和 ".." */
        if (file_data.file_name[0] == '.' &&
            (file_data.file_name[1] == '\0' ||
             (file_data.file_name[1] == '.' && file_data.file_name[2] == '\0'))) {
            continue;
        }

        lua_createtable(L, 0, 3);
        lua_pushstring(L, (const char*)file_data.file_name);
        lua_setfield(L, -2, "name");
        lua_pushboolean(L, file_data.file_attr == 1);
        lua_setfield(L, -2, "is_file");
        lua_pushinteger(L, file_data.file_size);
        lua_setfield(L, -2, "size");

        lua_rawseti(L, -2, index++);
    } while (cm_fs_find_next(find_fd, &file_data) == 0);

    /* 关闭遍历句柄 */
    cm_fs_find_close(find_fd);

    return 1;
}

static const luaL_Reg fs_lib[] = {
    { "open",   iot_fs_open },
    { "remove", iot_fs_remove },
    { "rename", iot_fs_rename },
    { "exists", iot_fs_exists },
    { "size",   iot_fs_size },
    { "info",   iot_fs_info },
    { "mkdir",  iot_fs_mkdir },
    { "rmdir",  iot_fs_rmdir },
    { "list",   iot_fs_list },
    {NULL, NULL}
};

static const luaL_Reg fs_file_lib[] = {
    { "close", iot_fs_file_close },
    { "read",  iot_fs_file_read },
    { "write", iot_fs_file_write },
    { "seek",  iot_fs_file_seek },
    {NULL, NULL}
};

static int iot_fs_file_gc(lua_State* L) {
    cm_fs_t fd = (cm_fs_t)lua_touserdata(L, 1);
    if (fd != NULL) {
        cm_fs_close(fd);
    }
    return 0;
}

static int iot_fs_file_index(lua_State* L) {
    const char* key = luaL_checkstring(L, 2);

    /* 查找方法 */
    for (int i = 0; fs_file_lib[i].name != NULL; i++) {
        if (strcmp(key, fs_file_lib[i].name) == 0) {
            lua_pushcfunction(L, fs_file_lib[i].func);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

LUAMOD_API int luaopen_fs(lua_State* L) {
    luaL_newmetatable(L, "fs.file");
    lua_pushcfunction(L, iot_fs_file_gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, iot_fs_file_index);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    luaL_newlibtable(L, fs_lib);
    luaL_setfuncs(L, fs_lib, 0);
    return 1;
}
