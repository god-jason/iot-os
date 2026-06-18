/*
@module  mem
@summary 内存管理
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     MEM
*/

/*
mem参考示例
-- 获取内存信息
local total, used, free = mem.info()
print("total:", total, "used:", used, "free:", free)

-- 分配内存
local ptr = mem.alloc(1024)
if ptr then
    mem.free(ptr)
end
*/

#include "module.h"
#include "yopen_mem.h"

/**
 * @brief 获取内存信息
 * @api mem.info()
 * @return int total 总内存(字节)
 * @return int used  已用内存(字节)
 * @return int free  空闲内存(字节)
 */
static int luaopen_mem_info(lua_State* L) {
    uint32 total = 0, used = 0, free = 0;
    yopen_mem_info(&total, &used, &free);
    lua_pushinteger(L, total);
    lua_pushinteger(L, used);
    lua_pushinteger(L, free);
    return 3;
}

/**
 * @brief 分配内存
 * @api mem.alloc(size)
 * @int size 分配大小(字节)
 * @return userdata 内存指针
 */
static int luaopen_mem_alloc(lua_State* L) {
    int size = (int)luaL_checkinteger(L, 1);
    void* ptr = iot_malloc(size);
    if (ptr) {
        lua_pushlightuserdata(L, ptr);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 释放内存
 * @api mem.free(ptr)
 * @userdata ptr 内存指针
 * @return nil
 */
static int luaopen_mem_free(lua_State* L) {
    void* ptr = lua_touserdata(L, 1);
    if (ptr) {
        iot_free(ptr);
    }
    return 0;
}

/**
 * @brief 内存重分配
 * @api mem.realloc(ptr, size)
 * @userdata ptr 原内存指针
 * @int size 新大小
 * @return userdata 新内存指针
 */
static int luaopen_mem_realloc(lua_State* L) {
    void* ptr = lua_touserdata(L, 1);
    int size = (int)luaL_checkinteger(L, 2);
    void* new_ptr = iot_realloc(ptr, size);
    if (new_ptr) {
        lua_pushlightuserdata(L, new_ptr);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static const luaL_Reg luaopen_mem_funcs[] = {
    {"info",    luaopen_mem_info},
    {"alloc",   luaopen_mem_alloc},
    {"free",    luaopen_mem_free},
    {"realloc", luaopen_mem_realloc},
    {NULL, NULL}
};

int luaopen_mem(lua_State* L) {
    luaL_newlib(L, luaopen_mem_funcs);
    return 1;
}