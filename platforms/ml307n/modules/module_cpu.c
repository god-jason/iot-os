/*
@module  cpu
@summary CPU相关
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     CPU
*/

/*
CPU参考示例
-- 启动CPU监控
cpu.start()

-- 获取CPU信息(频率和使用率)
local info = cpu.info()
print("CPU freq:", info.freq, "Hz")
print("CPU usage:", info.usage, "%")

-- 停止CPU监控
cpu.stop()
*/


#include "lua.h"
#include "module.h"

/* ML307N AP 主时钟 26MHz（与 drv_module.h MAIN_CLOCK 一致） */
#define IOT_CPU_MAIN_CLOCK_HZ  (26U * 1000000U)

/**
 * @brief 启动CPU监控
 * @api cpu.start()
 * @return boolean 是否成功启动
 * @usage
cpu.start()
*/
static int iot_cpu_start(lua_State* L)
{
    if (osStartCpuMonitor() == 0)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 停止CPU监控
 * @api cpu.stop()
 * @return boolean 是否成功停止
 * @usage
cpu.stop()
*/
static int iot_cpu_stop(lua_State* L)
{
    if (osStopCpuMonitor() == 0)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 获取CPU信息
 * @api cpu.info()
 * @return table CPU信息
 * @int freq CPU频率(Hz)
 * @int usage CPU使用率百分比(整数部分)
 * @usage
local info = cpu.info()
print("CPU freq:", info.freq, "Hz")
print("CPU usage:", info.usage, "%")
*/
static int iot_cpu_info(lua_State* L)
{
    lua_createtable(L, 0, 2);

    /* freq */
    lua_pushinteger(L, IOT_CPU_MAIN_CLOCK_HZ);
    lua_setfield(L, -2, "freq");

    /* usage */
    osCpuRate_t cpu_rate = {0};
    osReadCpuMonitor(&cpu_rate);
    lua_pushinteger(L, cpu_rate.integer_part);
    lua_setfield(L, -2, "usage");

    return 1;
}

static const luaL_Reg cpu_lib[] =
{
    { "start", iot_cpu_start },
    { "stop",  iot_cpu_stop },
    { "info",  iot_cpu_info },
    {NULL, NULL}
};

LUAMOD_API int luaopen_cpu(lua_State* L)
{
    luaL_newlibtable(L, cpu_lib);
    luaL_setfuncs(L, cpu_lib, 0);
    return 1;
}
