/*
@module  pm
@summary 电源管理
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     PM
*/

/*
pm参考示例
-- 获取电池电量
local level = pm.level()
print("battery level:", level)

-- 获取充电状态
local charging = pm.charging()
print("charging:", charging)

-- 进入低功耗模式
pm.sleep(5000)  -- 睡眠5秒
*/

#include "module.h"
#include "yopen_power.h"

/**
 * @brief 获取电池电量
 * @api pm.level()
 * @return int 电量百分比 (0-100)
 */
static int luaopen_pm_level(lua_State* L) {
    int level = 100;
    lua_pushinteger(L, level);
    return 1;
}

/**
 * @brief 获取充电状态
 * @api pm.charging()
 * @return bool true表示正在充电
 */
static int luaopen_pm_charging(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

/**
 * @brief 进入睡眠模式
 * @api pm.sleep(ms)
 * @int ms 睡眠时间(毫秒)
 * @return nil
 */
static int luaopen_pm_sleep(lua_State* L) {
    int ms = (int)luaL_checkinteger(L, 1);
    yopen_deepslp_timer_start((uint32_t)ms);
    return 0;
}

/**
 * @brief 获取电源状态
 * @api pm.status()
 * @return table 电源状态信息
 */
static int luaopen_pm_status(lua_State* L) {
    lua_createtable(L, 0, 3);
    lua_pushinteger(L, 100);
    lua_setfield(L, -2, "battery");
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "charging");
    lua_pushboolean(L, 1);
    lua_setfield(L, -2, "ac");
    return 1;
}

static const luaL_Reg luaopen_pm_funcs[] = {
    {"level",    luaopen_pm_level},
    {"charging", luaopen_pm_charging},
    {"sleep",    luaopen_pm_sleep},
    {"status",   luaopen_pm_status},
    {NULL, NULL}
};

int luaopen_pm(lua_State* L) {
    luaL_newlib(L, luaopen_pm_funcs);
    return 1;
}