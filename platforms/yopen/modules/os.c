/*
@module  os
@summary 系统相关
@version 1.0
@date    2026.06.19
@author  TRAE
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

#include "module.h"
#include "yopen_os.h"
#include "yopen_rtc.h"

/**
 * @brief 获取系统启动时间(毫秒)
 * @api os.ms()
 * @return int 系统启动时间(毫秒)
 */
static int luaopen_os_ms(lua_State* L) {
    time_t s = yopen_rtc_get_time_s();
    lua_pushinteger(L, (lua_Integer)(s * 1000));
    return 1;
}

/**
 * @brief 获取系统启动时间(秒)
 * @api os.s()
 * @return int 系统启动时间(秒)
 */
static int luaopen_os_s(lua_State* L) {
    time_t s = yopen_rtc_get_time_s();
    lua_pushinteger(L, (lua_Integer)s);
    return 1;
}

/**
 * @brief 获取系统信息
 * @api os.info()
 * @return table 系统信息
 */
static int luaopen_os_info(lua_State* L) {
    lua_createtable(L, 0, 4);
    lua_pushstring(L, "YOPEN");
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
 */
static int luaopen_os_sleep(lua_State* L) {
    int ms = (int)luaL_checkinteger(L, 1);
    yopen_rtos_task_sleep_ms(ms);
    return 0;
}

/**
 * @brief 获取系统重启原因
 * @api os.rebootReason()
 * @return string 重启原因
 */
static int luaopen_os_reboot_reason(lua_State* L) {
    lua_pushstring(L, "unknown");
    return 1;
}

static const luaL_Reg luaopen_os_funcs[] = {
    {"ms",          luaopen_os_ms},
    {"s",           luaopen_os_s},
    {"info",        luaopen_os_info},
    {"sleep",       luaopen_os_sleep},
    {"rebootReason", luaopen_os_reboot_reason},
    {NULL, NULL}
};

int luaopen_os(lua_State* L) {
    luaL_newlib(L, luaopen_os_funcs);
    return 1;
}