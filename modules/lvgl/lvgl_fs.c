/*
@module  lvgl.fs
@summary LVGL文件系统操作
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 打开文件
local f = lvgl.fs.open("test.txt", lvgl.FS_MODE_RD)

-- 读取文件
local data = f:read(1024)

-- 关闭文件
f:close()

-- 列出目录
local files = lvgl.fs.dir("data/")
for i, file in ipairs(files) do
    print(file.name, file.size, file.type)
end
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== 文件操作 ==================== */

/*
打开文件
@param path 文件路径
@param mode 打开模式
@return userdata 文件指针
@usage local f = lvgl.fs.open("test.txt", lvgl.FS_MODE_RD)
*/
static int lvgl_fs_open(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lv_fs_mode_t mode = (lv_fs_mode_t)luaL_checkinteger(L, 2);
    lv_fs_file_t* file = (lv_fs_file_t*)cm_malloc(sizeof(lv_fs_file_t));
    if (!file) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_fs_res_t res = lv_fs_open(file, path, mode);
    if (res != LV_FS_RES_OK) {
        cm_free(file);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushlightuserdata(L, file);
    return 1;
}

/*
关闭文件
@param file 文件指针
@usage f:close()
*/
static int lvgl_fs_close(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    lv_fs_close(file);
    cm_free(file);
    return 0;
}

/*
读取文件
@param file 文件指针
@param size 读取大小
@return string 读取的数据
@usage local data = f:read(1024)
*/
static int lvgl_fs_read(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    size_t size = (size_t)luaL_checkinteger(L, 2);
    char* buf = (char*)cm_malloc(size + 1);
    if (!buf) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    uint32_t bytes_read;
    lv_fs_res_t res = lv_fs_read(file, buf, size, &bytes_read);
    if (res != LV_FS_RES_OK) {
        cm_free(buf);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    buf[bytes_read] = '\0';
    lua_pushlstring(L, buf, bytes_read);
    cm_free(buf);
    return 1;
}

/*
写入文件
@param file 文件指针
@param data 数据
@return integer 写入的字节数
@usage local bytes = f:write("hello")
*/
static int lvgl_fs_write(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    size_t len;
    const char* data = luaL_checklstring(L, 2, &len);
    uint32_t bytes_written;
    lv_fs_res_t res = lv_fs_write(file, data, len, &bytes_written);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushinteger(L, bytes_written);
    return 1;
}

/*
设置文件位置
@param file 文件指针
@param pos 位置
@param whence 起始位置(SEEK_SET/SEEK_CUR/SEEK_END)
@return integer 新位置或错误码
@usage f:seek(10, lvgl.FS_SEEK_SET)
*/
static int lvgl_fs_seek(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    uint32_t pos = (uint32_t)luaL_checkinteger(L, 2);
    lv_fs_whence_t whence = (lv_fs_whence_t)luaL_optinteger(L, 3, LV_FS_SEEK_SET);
    uint32_t new_pos;
    lv_fs_res_t res = lv_fs_seek(file, pos, whence, &new_pos);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushinteger(L, new_pos);
    return 1;
}

/*
获取文件大小
@param file 文件指针
@return integer 文件大小
@usage local size = f:size()
*/
static int lvgl_fs_size(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    uint32_t size;
    lv_fs_res_t res = lv_fs_size(file, &size);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushinteger(L, size);
    return 1;
}

/*
获取文件位置
@param file 文件指针
@return integer 当前位置
@usage local pos = f:tell()
*/
static int lvgl_fs_tell(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    uint32_t pos;
    lv_fs_res_t res = lv_fs_tell(file, &pos);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushinteger(L, pos);
    return 1;
}

/* ==================== 目录操作 ==================== */

/*
打开目录
@param path 目录路径
@return userdata 目录指针
@usage local dir = lvgl.fs.dir_open("data/")
*/
static int lvgl_fs_dir_open(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lv_fs_dir_t* dir = (lv_fs_dir_t*)cm_malloc(sizeof(lv_fs_dir_t));
    if (!dir) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_fs_res_t res = lv_fs_dir_open(dir, path);
    if (res != LV_FS_RES_OK) {
        cm_free(dir);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushlightuserdata(L, dir);
    return 1;
}

/*
关闭目录
@param dir 目录指针
@usage dir:close()
*/
static int lvgl_fs_dir_close(lua_State* L) {
    lv_fs_dir_t* dir = (lv_fs_dir_t*)luaL_checklightuserdata(L, 1);
    lv_fs_dir_close(dir);
    cm_free(dir);
    return 0;
}

/*
读取目录项
@param dir 目录指针
@return table 目录项信息或nil
@usage local item = dir:read()
*/
static int lvgl_fs_dir_read(lua_State* L) {
    lv_fs_dir_t* dir = (lv_fs_dir_t*)luaL_checklightuserdata(L, 1);
    lv_fs_dir_entry_t entry;
    lv_fs_res_t res = lv_fs_dir_read(dir, &entry);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_newtable(L);
    lua_pushstring(L, entry.name);
    lua_setfield(L, -2, "name");
    lua_pushinteger(L, entry.size);
    lua_setfield(L, -2, "size");
    lua_pushinteger(L, entry.type);
    lua_setfield(L, -2, "type");
    return 1;
}

/* ==================== 文件系统信息 ==================== */

/*
获取驱动器信息
@param drv 驱动器名
@return table 驱动器信息
@usage local info = lvgl.fs.get_drive_info("S")
*/
static int lvgl_fs_get_drive_info(lua_State* L) {
    const char* drv = luaL_checkstring(L, 1);
    lv_fs_drive_info_t info;
    lv_fs_res_t res = lv_fs_get_drive_info(drv, &info);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_newtable(L);
    lua_pushinteger(L, info.free_space_kb);
    lua_setfield(L, -2, "free_space_kb");
    lua_pushinteger(L, info.total_space_kb);
    lua_setfield(L, -2, "total_space_kb");
    lua_pushinteger(L, info.letter);
    lua_setfield(L, -2, "letter");
    return 1;
}

/* ==================== 便捷函数 ==================== */

/*
列出目录(便捷函数)
@param path 目录路径
@return table 文件列表
@usage local files = lvgl.fs.list("data/")
*/
static int lvgl_fs_list(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lv_fs_dir_t* dir = (lv_fs_dir_t*)cm_malloc(sizeof(lv_fs_dir_t));
    if (!dir) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_fs_res_t res = lv_fs_dir_open(dir, path);
    if (res != LV_FS_RES_OK) {
        cm_free(dir);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_newtable(L);
    int idx = 1;
    while (1) {
        lv_fs_dir_entry_t entry;
        res = lv_fs_dir_read(dir, &entry);
        if (res != LV_FS_RES_OK) break;
        lua_newtable(L);
        lua_pushstring(L, entry.name);
        lua_setfield(L, -2, "name");
        lua_pushinteger(L, entry.size);
        lua_setfield(L, -2, "size");
        lua_pushinteger(L, entry.type);
        lua_setfield(L, -2, "type");
        lua_rawseti(L, -2, idx++);
    }
    lv_fs_dir_close(dir);
    cm_free(dir);
    return 1;
}

/*
读取文件全部内容(便捷函数)
@param path 文件路径
@return string 文件内容
@usage local content = lvgl.fs.read_file("test.txt")
*/
static int lvgl_fs_read_file(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lv_fs_file_t* file = (lv_fs_file_t*)cm_malloc(sizeof(lv_fs_file_t));
    if (!file) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_fs_res_t res = lv_fs_open(file, path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        cm_free(file);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    uint32_t size;
    res = lv_fs_size(file, &size);
    if (res != LV_FS_RES_OK) {
        lv_fs_close(file);
        cm_free(file);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    char* buf = (char*)cm_malloc(size + 1);
    if (!buf) {
        lv_fs_close(file);
        cm_free(file);
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    uint32_t bytes_read;
    res = lv_fs_read(file, buf, size, &bytes_read);
    lv_fs_close(file);
    cm_free(file);
    if (res != LV_FS_RES_OK) {
        cm_free(buf);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    buf[bytes_read] = '\0';
    lua_pushlstring(L, buf, bytes_read);
    cm_free(buf);
    return 1;
}

/* 注册 fs 子模块 */
int lvgl_register_fs(lua_State* L) {
    REG_METHOD(L, "open", lvgl_fs_open);
    REG_METHOD(L, "close", lvgl_fs_close);
    REG_METHOD(L, "read", lvgl_fs_read);
    REG_METHOD(L, "write", lvgl_fs_write);
    REG_METHOD(L, "seek", lvgl_fs_seek);
    REG_METHOD(L, "size", lvgl_fs_size);
    REG_METHOD(L, "tell", lvgl_fs_tell);
    REG_METHOD(L, "dir_open", lvgl_fs_dir_open);
    REG_METHOD(L, "dir_close", lvgl_fs_dir_close);
    REG_METHOD(L, "dir_read", lvgl_fs_dir_read);
    REG_METHOD(L, "get_drive_info", lvgl_fs_get_drive_info);
    REG_METHOD(L, "list", lvgl_fs_list);
    REG_METHOD(L, "read_file", lvgl_fs_read_file);
    return 0;
}