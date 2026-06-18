/*
@module  flash
@summary Flash存储操作
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     FLASH
*/

/*
FLASH参考示例
-- 查找分区
local part = flash.find("partition_name")

-- 读取数据
local data = flash.read(part, 0, 128)

-- 写入数据
flash.write(part, 0, "hello world")

-- 擦除数据
flash.erase(part, 0, 4096)
*/


#include "lua.h"
#include "module.h"
#include "drv_flash.h"

static PART_Handle* g_current_part = NULL;

/**
 * @brief 查找分区
 * @api flash.find(name)
 * @param string name 分区名称
 * @return table/ nil 分区句柄，失败返回nil
 * @usage
local part = flash.find("my_partition")
if part then
    print("Found partition")
end
*/
static int iot_flash_find(lua_State* L) {
    size_t len;
    const char* name = luaL_checklstring(L, 1, &len);
    
    LOG("find name=%s", name);
    PART_Handle* part = PART_Find(name);
    if (part) {
        g_current_part = part;
        LOG("OK");
        lua_pushlightuserdata(L, part);
        return 1;
    }
    LOG("ERR not found");
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 读取数据
 * @api flash.read(part, offset, size)
 * @param table part 分区句柄
 * @param int offset 偏移地址
 * @param int size 读取长度
 * @return string/nil 读取的数据，失败返回nil
 * @usage
local data = flash.read(part, 0, 100)
if data then
    print("Read:", data)
end
*/
static int iot_flash_read(lua_State* L) {
    PART_Handle* part = NULL;
    uint32_t offset = 0;
    size_t size = 0;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
        offset = (uint32_t)luaL_checkinteger(L, 2);
        size = (size_t)luaL_checkinteger(L, 3);
    } else {
        if (g_current_part == NULL) {
            lua_pushnil(L);
            return 1;
        }
        part = g_current_part;
        offset = (uint32_t)luaL_checkinteger(L, 1);
        size = (size_t)luaL_checkinteger(L, 2);
    }
    
    if (part == NULL || size == 0) {
        lua_pushnil(L);
        return 1;
    }
    
    LOG("read offset=%u size=%u", offset, size);
    uint8_t* buf = (uint8_t*)lua_newuserdata(L, size);
    if (buf == NULL) {
        LOG("ERR malloc failed");
        lua_pushnil(L);
        return 1;
    }
    
    int32_t len = PART_Read(part, offset, buf, size);
    if (len > 0) {
        lua_pushlstring(L, (const char*)buf, len);
        return 1;
    }
    
    LOG("ERR read len=%d", len);
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 写入数据
 * @api flash.write(part, offset, data)
 * @param table part 分区句柄
 * @param int offset 偏移地址
 * @param string data 要写入的数据
 * @return int 实际写入字节数，失败返回-1
 * @usage
local written = flash.write(part, 0, "hello world")
print("Written:", written)
*/
static int iot_flash_write(lua_State* L) {
    PART_Handle* part = NULL;
    uint32_t offset = 0;
    size_t len = 0;
    const char* data = NULL;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
        offset = (uint32_t)luaL_checkinteger(L, 2);
        data = luaL_checklstring(L, 3, &len);
    } else {
        if (g_current_part == NULL) {
            lua_pushinteger(L, -1);
            return 1;
        }
        part = g_current_part;
        offset = (uint32_t)luaL_checkinteger(L, 1);
        data = luaL_checklstring(L, 2, &len);
    }
    
    if (part == NULL || len == 0) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    LOG("write offset=%u len=%u", offset, len);
    int32_t written = PART_Write(part, offset, (const uint8_t*)data, len);
    if (written < 0) {
        LOG("ERR written=%d", written);
    }
    lua_pushinteger(L, written);
    return 1;
}

/**
 * @brief 擦除数据
 * @api flash.erase(part, offset, size)
 * @param table part 分区句柄
 * @param int offset 偏移地址
 * @param int size 擦除长度
 * @return int 0成功，失败返回-1
 * @usage
local ret = flash.erase(part, 0, 4096)
if ret == 0 then
    print("Erase success")
end
*/
static int iot_flash_erase(lua_State* L) {
    PART_Handle* part = NULL;
    uint32_t offset = 0;
    size_t size = 0;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
        offset = (uint32_t)luaL_checkinteger(L, 2);
        size = (size_t)luaL_checkinteger(L, 3);
    } else {
        if (g_current_part == NULL) {
            lua_pushinteger(L, -1);
            return 1;
        }
        part = g_current_part;
        offset = (uint32_t)luaL_checkinteger(L, 1);
        size = (size_t)luaL_checkinteger(L, 2);
    }
    
    if (part == NULL) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    LOG("erase offset=%u size=%u", offset, size);
    int32_t ret = PART_Erase(part, offset, size);
    if (ret != DRV_OK) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret == DRV_OK ? 0 : -1);
    return 1;
}

/**
 * @brief 擦除整个分区
 * @api flash.eraseAll(part)
 * @param table part 分区句柄
 * @return int 0成功，失败返回-1
 * @usage
local ret = flash.eraseAll(part)
if ret == 0 then
    print("Erase all success")
end
*/
static int iot_flash_eraseAll(lua_State* L) {
    PART_Handle* part = NULL;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
    } else {
        part = g_current_part;
    }
    
    if (part == NULL) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    LOG("eraseAll");
    int32_t ret = PART_EraseAll(part);
    if (ret != DRV_OK) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret == DRV_OK ? 0 : -1);
    return 1;
}

/**
 * @brief 获取分区大小
 * @api flash.getSize(part)
 * @param table part 分区句柄
 * @return int 分区大小
 * @usage
local size = flash.getSize(part)
print("Partition size:", size)
*/
static int iot_flash_getSize(lua_State* L) {
    PART_Handle* part = NULL;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
    } else {
        part = g_current_part;
    }
    
    if (part == NULL) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    lua_pushinteger(L, part->size);
    return 1;
}

/**
 * @brief 锁定分区
 * @api flash.lock(part)
 * @param table part 分区句柄
 * @return int 0成功，失败返回-1
 * @usage
local ret = flash.lock(part)
if ret == 0 then
    print("Lock success")
end
*/
static int iot_flash_lock(lua_State* L) {
    PART_Handle* part = NULL;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
    } else {
        part = g_current_part;
    }
    
    if (part == NULL) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    LOG("lock");
    int32_t ret = PART_Lock(part);
    if (ret != DRV_OK) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret == DRV_OK ? 0 : -1);
    return 1;
}

/**
 * @brief 解锁分区
 * @api flash.unlock(part)
 * @param table part 分区句柄
 * @return int 0成功，失败返回-1
 * @usage
local ret = flash.unlock(part)
if ret == 0 then
    print("Unlock success")
end
*/
static int iot_flash_unlock(lua_State* L) {
    PART_Handle* part = NULL;
    
    if (lua_islightuserdata(L, 1)) {
        part = (PART_Handle*)lua_touserdata(L, 1);
    } else {
        part = g_current_part;
    }
    
    if (part == NULL) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    LOG("unlock");
    int32_t ret = PART_UnLock(part);
    if (ret != DRV_OK) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret == DRV_OK ? 0 : -1);
    return 1;
}

static const luaL_Reg flash_lib[] = {
    { "find", iot_flash_find },
    { "read", iot_flash_read },
    { "write", iot_flash_write },
    { "erase", iot_flash_erase },
    { "eraseAll", iot_flash_eraseAll },
    { "getSize", iot_flash_getSize },
    { "lock", iot_flash_lock },
    { "unlock", iot_flash_unlock },
    {NULL, NULL}
};

LUAMOD_API int luaopen_flash(lua_State* L) {
    luaL_newlibtable(L, flash_lib);
    luaL_setfuncs(L, flash_lib, 0);
    return 1;
}
