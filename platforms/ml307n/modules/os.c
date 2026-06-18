/*
@module  os
@summary 系统相关
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     OS
*/

/*
OS参考示例
-- 获取系统启动时间(毫秒)
local ms = os.ms()
print("uptime ms:", ms)

-- 获取系统启动时间(秒)
local s = os.s()
print("uptime s:", s)

-- 获取系统信息
local info = os.info()
print("os version:", info.version)
*/


#include "lua.h"
#include "module.h"

extern uint64_t cm_get_time_ms(void);

/**
 * @brief 获取系统启动时间(毫秒)
 * @api os.ms()
 * @return int 系统启动时间(毫秒)
 * @usage
local ms = os.ms()
*/
static int iot_os_ms(lua_State* L) {
    uint64_t ms = cm_get_time_ms();
    lua_pushinteger(L, (lua_Integer)ms);
    return 1;
}

/**
 * @brief 获取系统启动时间(秒)
 * @api os.s()
 * @return int 系统启动时间(秒)
 * @usage
local s = os.s()
*/
static int iot_os_s(lua_State* L) {
    uint64_t s = cm_get_time_ms() / 1000;
    lua_pushinteger(L, (lua_Integer)s);
    return 1;
}

/**
 * @brief 获取系统信息
 * @api os.info()
 * @return table 系统信息
 * @usage
local info = os.info()
*/
static int iot_os_info(lua_State* L) {
    lua_createtable(L, 0, 4);
    lua_pushstring(L, "ML307N");
    lua_setfield(L, -2, "board");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "version");

    return 1;
}

/**
 * @brief 系统延迟(毫秒)
 * @api os.sleep(ms)
 * @int ms 延迟毫秒数
 * @return nil
 * @usage
os.sleep(1000)
*/
static int iot_os_sleep(lua_State* L) {
    int ms = luaL_checkinteger(L, 1);
    uint32_t ticks = (uint32_t)((uint64_t)ms * osKernelGetTickFreq() / 1000U);
    if (ticks == 0 && ms > 0) {
        ticks = 1;
    }
    osDelay(ticks);
    return 0;
}

static const luaL_Reg os_lib[] = {
    { "ms",      iot_os_ms },
    { "s",       iot_os_s },
    { "info",    iot_os_info },
    { "sleep",   iot_os_sleep },
    {NULL, NULL}
};

LUAMOD_API int luaopen_os(lua_State* L) {
    luaL_newlibtable(L, os_lib);
    luaL_setfuncs(L, os_lib, 0);
    return 1;
}
