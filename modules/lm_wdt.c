/*
@module  wdt
@summary 看门狗
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     WDT
*/

/*
WDT参考示例
-- 启动看门狗
-- 参数1: 超时时间(毫秒), 必须为100的整数倍
-- 参数2: 喂狗周期(毫秒), 必须为100的整数倍 建议是超时时间的一半左右
local ok = wdt.start(5000, 2000)
print("wdt start:", ok)

-- 喂狗
wdt.feed()

-- 停止看门狗
wdt.stop()

-- 获取超时标志
local flag = wdt.get_timeout_flag()
print("timeout flag:", flag)
*/


#include "lua.h"
#include "iot_base.h"
#include "drv_wdt.h"

/**
 * @brief 启动看门狗
 * @api wdt.start(timeout_ms, feed_period_ms)
 * @int timeout_ms 超时时间(毫秒), 必须为100的整数倍
 * @int feed_period_ms 喂狗周期(毫秒), 必须为100的整数倍 建议是超时时间的一半左右
 * @return int 0成功, 其他失败
 * @usage
local ok = wdt.start(5000, 2000)
*/
static int iot_wdt_start(lua_State* L) {
    uint32_t timeout_ms = (uint32_t)luaL_checkinteger(L, 1);
    uint32_t feed_period_ms = (uint32_t)luaL_optinteger(L, 2, 2000);
    
    /* Ensure parameters are multiples of 100 */
    timeout_ms = (timeout_ms / 100) * 100;
    feed_period_ms = (feed_period_ms / 100) * 100;
    
    LOG("WDT start: timeout=%ums, feed_period=%ums", timeout_ms, feed_period_ms);
    uint8_t ret = HardWDT_Initlize(timeout_ms, feed_period_ms);
    lua_pushinteger(L, (lua_Integer)ret);
    return 1;
}

/**
 * @brief 停止看门狗
 * @api wdt.stop()
 * @return nil
 * @usage
wdt.stop()
*/
static int iot_wdt_stop(lua_State* L) {
    HardWDT_UnInitlize();
    return 0;
}

/**
 * @brief 喂狗
 * @api wdt.feed()
 * @return nil
 * @usage
wdt.feed()
*/
static int iot_wdt_feed(lua_State* L) {
    LOG("WDT feed");
    HardWDT_FeedDog();
    return 0;
}

/**
 * @brief 获取超时标志
 * @api wdt.get_timeout_flag()
 * @return int 超时标志
 * @usage
local flag = wdt.get_timeout_flag()
*/
static int iot_wdt_get_timeout_flag(lua_State* L) {
    uint8_t flag = HardWDT_GetTimeoutFlag();
    lua_pushinteger(L, (lua_Integer)flag);
    return 1;
}

/**
 * @brief 设置超时标志
 * @api wdt.set_timeout_flag(flag)
 * @int flag 超时标志
 * @return nil
 * @usage
wdt.set_timeout_flag(0xAB)
*/
static int iot_wdt_set_timeout_flag(lua_State* L) {
    uint8_t flag = (uint8_t)luaL_checkinteger(L, 1);
    HardWDT_SetTimeoutFlag(flag);
    return 0;
}

/**
 * @brief 获取异常复位标志
 * @api wdt.get_exception_reset_flag()
 * @return int 异常复位标志
 * @usage
local flag = wdt.get_exception_reset_flag()
*/
static int iot_wdt_get_exception_reset_flag(lua_State* L) {
    uint8_t flag = HardWDT_GetExceptionResetFlag();
    lua_pushinteger(L, (lua_Integer)flag);
    return 1;
}

/**
 * @brief 设置异常复位标志
 * @api wdt.set_exception_reset_flag(flag)
 * @int flag 异常复位标志
 * @return nil
 * @usage
wdt.set_exception_reset_flag(0x6D)
*/
static int iot_wdt_set_exception_reset_flag(lua_State* L) {
    uint8_t flag = (uint8_t)luaL_checkinteger(L, 1);
    HardWDT_SetExceptionResetFlag(flag);
    return 0;
}

static const luaL_Reg wdt_lib[] = {
    { "start",              iot_wdt_start },
    { "stop",               iot_wdt_stop },
    { "feed",               iot_wdt_feed },
    { "get_timeout_flag",   iot_wdt_get_timeout_flag },
    { "set_timeout_flag",   iot_wdt_set_timeout_flag },
    { "get_exception_reset_flag", iot_wdt_get_exception_reset_flag },
    { "set_exception_reset_flag", iot_wdt_set_exception_reset_flag },
    {NULL, NULL}
};

LUAMOD_API int luaopen_wdt(lua_State* L) {
    luaL_newlibtable(L, wdt_lib);
    luaL_setfuncs(L, wdt_lib, 0);
    return 1;
}
