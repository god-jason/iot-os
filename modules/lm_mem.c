/*
@module  mem
@summary 内存相关
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     MEM
*/

/*
MEM参考示例
-- 获取内存信息
local info = mem.info()
print("total:", info.total)
print("used:", info.used)
print("free:", info.free)
print("max_cont_free:", info.max_cont_free)
print("allocated_peak:", info.allocated_peak)
*/


#include "lua.h"
#include "iot_base.h"

/**
 * @brief 获取内存信息
 * @api mem.info()
 * @return table 内存信息
 * @usage
local info = mem.info()
print("total:", info.total)
print("used:", info.used)
print("free:", info.free)
print("max_cont_free:", info.max_cont_free)
print("allocated_peak:", info.allocated_peak)
*/
static int iot_mem_info(lua_State* L) {
    cm_heap_stats_t stats = {0};
    if (cm_mem_get_heap_stats(&stats) == 0) {
        lua_createtable(L, 0, 5);
        lua_pushinteger(L, stats.total_size);
        lua_setfield(L, -2, "total");
        lua_pushinteger(L, stats.allocated);
        lua_setfield(L, -2, "used");
        lua_pushinteger(L, stats.free);
        lua_setfield(L, -2, "free");
        lua_pushinteger(L, stats.max_continuity_free);
        lua_setfield(L, -2, "max_cont_free");
        lua_pushinteger(L, stats.allocated_peak);
        lua_setfield(L, -2, "allocated_peak");
        return 1;
    }
    lua_createtable(L, 0, 3);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "total");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "used");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "free");
    return 1;
}

static const luaL_Reg mem_lib[] =
{
    { "info", iot_mem_info },
    {NULL, NULL}
};

LUAMOD_API int luaopen_mem(lua_State* L)
{
    luaL_newlibtable(L, mem_lib);
    luaL_setfuncs(L, mem_lib, 0);
    return 1;
}
