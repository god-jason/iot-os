/*
@module  lvgl.fs
@summary LVGL??????
@version 1.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??
local lvgl = require("lvgl")

-- ????
local f = lvgl.fs.open("test.txt", lvgl.FS_MODE_RD)

-- ????
local data = f:read(1024)

-- ????
f:close()

-- ????
local files = lvgl.fs.dir("data/")
for i, file in ipairs(files) do
    print(file.name, file.size, file.type)
end
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== ???? ==================== */

/*
????
@param path ????
@param mode ????
@return userdata ????
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
????
@param file ????
@usage f:close()
*/
static int lvgl_fs_close(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    lv_fs_close(file);
    cm_free(file);
    return 0;
}

/*
????
@param file ????
@param size ????
@return string ??????
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
????
@param file ????
@param data ??
@return integer ??????
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
??????
@param file ????
@param pos ??
@param whence ????(SEEK_SET/SEEK_CUR/SEEK_END)
@return integer ????????
@usage f:seek(10, lvgl.FS_SEEK_SET)
*/
static int lvgl_fs_seek(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    uint32_t pos = (uint32_t)luaL_checkinteger(L, 2);
    lv_fs_whence_t whence = (lv_fs_whence_t)luaL_optinteger(L, 3, LV_FS_SEEK_SET);
    lv_fs_res_t res = lv_fs_seek(file, pos, whence);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    uint32_t new_pos;
    res = lv_fs_tell(file, &new_pos);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lua_pushinteger(L, new_pos);
    return 1;
}

/*
??????
@param file ????
@return integer ????
@usage local size = f:size()
*/
static int lvgl_fs_size(lua_State* L) {
    lv_fs_file_t* file = (lv_fs_file_t*)luaL_checklightuserdata(L, 1);
    uint32_t cur_pos;
    lv_fs_res_t res = lv_fs_tell(file, &cur_pos);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    res = lv_fs_seek(file, 0, LV_FS_SEEK_END);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    uint32_t size;
    res = lv_fs_tell(file, &size);
    if (res != LV_FS_RES_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lv_fs_seek(file, cur_pos, LV_FS_SEEK_SET);
    lua_pushinteger(L, size);
    return 1;
}

/*
??????
@param file ????
@return integer ????
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

/* ==================== ???? ==================== */

/*
????
@param path ????
@return userdata ????
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
????
@param dir ????
@usage dir:close()
*/
static int lvgl_fs_dir_close(lua_State* L) {
    lv_fs_dir_t* dir = (lv_fs_dir_t*)luaL_checklightuserdata(L, 1);
    lv_fs_dir_close(dir);
    cm_free(dir);
    return 0;
}

/*
??????
@param dir ????
@return table ??????nil
@usage local item = dir:read()
*/
static int lvgl_fs_dir_read(lua_State* L) {
    lv_fs_dir_t* dir = (lv_fs_dir_t*)luaL_checklightuserdata(L, 1);
    char fn[LV_FS_MAX_FN_LENGTH];
    lv_fs_res_t res = lv_fs_dir_read(dir, fn);
    if (res != LV_FS_RES_OK || fn[0] == '\0') {
        lua_pushnil(L);
        if (res != LV_FS_RES_OK) {
            lua_pushinteger(L, res);
            return 2;
        }
        return 1;
    }
    lua_newtable(L);
    lua_pushstring(L, fn);
    lua_setfield(L, -2, "name");
    lua_pushinteger(L, fn[0] == '/' ? 1 : 0);
    lua_setfield(L, -2, "type");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "size");
    return 1;
}

/* ==================== ?????? ==================== */

/*
????????
@param drv ????
@return table ??????
@usage local info = lvgl.fs.get_drive_info("S")
*/
static int lvgl_fs_get_drive_info(lua_State* L) {
    (void)luaL_checkstring(L, 1);
    lua_pushnil(L);
    lua_pushinteger(L, LV_FS_RES_NOT_IMP);
    return 2;
}

/* ==================== ???? ==================== */

/*
????(????)
@param path ????
@return table ????
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
        char fn[LV_FS_MAX_FN_LENGTH];
        res = lv_fs_dir_read(dir, fn);
        if (res != LV_FS_RES_OK || fn[0] == '\0') break;
        lua_newtable(L);
        lua_pushstring(L, fn);
        lua_setfield(L, -2, "name");
        lua_pushinteger(L, fn[0] == '/' ? 1 : 0);
        lua_setfield(L, -2, "type");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "size");
        lua_rawseti(L, -2, idx++);
    }
    lv_fs_dir_close(dir);
    cm_free(dir);
    return 1;
}

/*
????????(????)
@param path ????
@return string ????
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
    res = lv_fs_seek(file, 0, LV_FS_SEEK_END);
    if (res != LV_FS_RES_OK) {
        lv_fs_close(file);
        cm_free(file);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    res = lv_fs_tell(file, &size);
    if (res != LV_FS_RES_OK) {
        lv_fs_close(file);
        cm_free(file);
        lua_pushnil(L);
        lua_pushinteger(L, res);
        return 2;
    }
    lv_fs_seek(file, 0, LV_FS_SEEK_SET);
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

/* ?? fs ????*/
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