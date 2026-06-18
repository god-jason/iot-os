/*
@module  pm
@summary 电源管理
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     PM
*/

/*
PM参考示例
-- 获取上电原因
local reason = pm.get_power_on_reason()
print("power on reason:", reason)

-- 重启
pm.reboot()

-- 关机
pm.poweroff()

-- 睡眠锁定(禁止进入休眠)
pm.work_lock()

-- 睡眠解锁(允许进入休眠)
pm.work_unlock()
*/


#include "lua.h"
#include "module.h"
#include "cm_pm.h"

/**
 * @brief 关机
 * @api pm.poweroff()
 * @return nil
 * @usage
pm.poweroff()
*/
static int iot_pm_poweroff(lua_State* L) {
    cm_pm_poweroff();
    return 0;
}

/**
 * @brief 获取上电原因
 * @api pm.get_power_on_reason()
 * @return int 上电原因码:
 *   = 0 未知原因\n
 *   = 1 充电供电时关机重启\n
 *   = 2 产线工具下发导致重启\n
 *   = 3 RTC闹钟开机\n
 *   = 4 正常开机\n
 *   = 5 看门狗超时导致重启\n
 * @usage
local reason = pm.get_power_on_reason()
*/
static int iot_pm_get_power_on_reason(lua_State* L) {
    int reason = cm_pm_get_power_on_reason();
    lua_pushinteger(L, reason);
    return 1;
}

/**
 * @brief 重启
 * @api pm.reboot()
 * @return nil
 * @usage
pm.reboot()
*/
static int iot_pm_reboot(lua_State* L) {
    cm_pm_reboot();
    return 0;
}

/**
 * @brief 睡眠模式上锁
 * @api pm.work_lock()
 * @return nil
 * @details 执行后,模组将无法进入休眠状态
 * @usage
pm.work_lock()
*/
static int iot_pm_work_lock(lua_State* L) {
    cm_pm_work_lock();
    return 0;
}

/**
 * @brief 睡眠模式解锁
 * @api pm.work_unlock()
 * @return nil
 * @details 执行后模组将允许进入休眠状态
 * @usage
pm.work_unlock()
*/
static int iot_pm_work_unlock(lua_State* L) {
    cm_pm_work_unlock();
    return 0;
}

static const luaL_Reg pm_lib[] = {
    { "poweroff",          iot_pm_poweroff },
    { "get_power_on_reason", iot_pm_get_power_on_reason },
    { "reboot",           iot_pm_reboot },
    { "work_lock",        iot_pm_work_lock },
    { "work_unlock",      iot_pm_work_unlock },
    {NULL, NULL}
};

LUAMOD_API int luaopen_pm(lua_State* L) {
    luaL_newlibtable(L, pm_lib);
    luaL_setfuncs(L, pm_lib, 0);
    return 1;
}
