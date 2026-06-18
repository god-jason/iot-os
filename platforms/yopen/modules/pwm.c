/*
@module  pwm
@summary PWM脉宽调制
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     PWM
*/

/*
PWM参考示例
-- 打开PWM通道
local pwm = pwm.open(0)  -- 0-4

-- 配置PWM (周期1ms, 占空比50%)
pwm:set(1000, 50)

-- 设置频率和占空比
pwm:setFreq(1000)
pwm:setDuty(50)

-- 关闭PWM
pwm:close()
*/

#include "module.h"
#include "yopen_pwm.h"

/* PWM通道定义 */
#define PWM_CHANNEL_0   0
#define PWM_CHANNEL_1   1
#define PWM_CHANNEL_2   2
#define PWM_CHANNEL_3   3
#define PWM_CHANNEL_4   4

/* PWM句柄结构 */
typedef struct {
    yopen_pwm_sel channel;
    int initialized;
} pwm_handle_t;

/**
 * @brief 打开PWM通道
 * @api pwm.open(channel)
 * @int channel PWM通道号 (0-4)
 * @return userdata PWM操作句柄
 */
static int luaopen_pwm_open(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);

    if (channel < 0 || channel >= PWM_MAX) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid channel");
        return 2;
    }

    /* 分配句柄 */
    pwm_handle_t* handle = (pwm_handle_t*)iot_malloc(sizeof(pwm_handle_t));
    if (!handle) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }

    handle->channel = (yopen_pwm_sel)channel;
    handle->initialized = 0;

    /* 打开PWM */
    yopen_errcode_pwm ret = yopen_pwm_open(handle->channel);
    if (ret == 0) {
        handle->initialized = 1;
        lua_pushlightuserdata(L, handle);
        return 1;
    }

    iot_free(handle);
    lua_pushnil(L);
    lua_pushstring(L, "pwm open failed");
    return 2;
}

/**
 * @brief 配置PWM参数
 * @api pwm:set(period, duty)
 * @int period 周期(微秒)
 * @int duty 占空比(0-100)
 * @return bool true表示成功
 */
static int luaopen_pwm_set(lua_State* L) {
    pwm_handle_t* handle = (pwm_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int period = (int)luaL_checkinteger(L, 2);
    int duty = (int)luaL_checkinteger(L, 3);

    if (duty < 0) duty = 0;
    if (duty > 100) duty = 100;

    /* period单位是us，需要转换为配置结构 */
    yopen_pwm_cfg_s cfg = {0};
    cfg.period = (uint32_t)period;
    cfg.duty = (uint16_t)((period * duty) / 100);

    yopen_errcode_pwm ret = yopen_pwm_enable(handle->channel, &cfg);

    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置PWM频率
 * @api pwm:setFreq(freq)
 * @int freq 频率(Hz)
 * @return bool true表示成功
 */
static int luaopen_pwm_setfreq(lua_State* L) {
    pwm_handle_t* handle = (pwm_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        return 0;
    }

    int freq = (int)luaL_checkinteger(L, 2);

    if (freq <= 0) {
        return 0;
    }

    /* 计算周期(微秒) */
    uint32_t period_us = 1000000 / freq;
    int duty = 50;  /* 默认50% */

    yopen_pwm_cfg_s cfg = {0};
    cfg.period = period_us;
    cfg.duty = (uint16_t)((period_us * duty) / 100);

    yopen_pwm_enable(handle->channel, &cfg);

    return 0;
}

/**
 * @brief 设置PWM占空比
 * @api pwm:setDuty(duty)
 * @int duty 占空比(0-100)
 * @return bool true表示成功
 */
static int luaopen_pwm_setduty(lua_State* L) {
    pwm_handle_t* handle = (pwm_handle_t*)lua_touserdata(L, 1);

    if (!handle || !handle->initialized) {
        return 0;
    }

    int duty = (int)luaL_checkinteger(L, 2);

    if (duty < 0) duty = 0;
    if (duty > 100) duty = 100;

    yopen_errcode_pwm ret = yopen_pwm_duty_set(handle->channel, (uint16_t)duty);

    return 0;
}

/**
 * @brief 关闭PWM
 * @api pwm:close()
 * @return nil
 */
static int luaopen_pwm_close(lua_State* L) {
    pwm_handle_t* handle = (pwm_handle_t*)lua_touserdata(L, 1);

    if (handle) {
        if (handle->initialized) {
            yopen_pwm_disable(handle->channel);
            yopen_pwm_close(handle->channel);
            handle->initialized = 0;
        }
        iot_free(handle);
    }
    return 0;
}

static const luaL_Reg luaopen_pwm_funcs[] = {
    {"open",    luaopen_pwm_open},
    {NULL, NULL}
};

static const luaL_Reg luaopen_pwm_methods[] = {
    {"set",     luaopen_pwm_set},
    {"setFreq", luaopen_pwm_setfreq},
    {"setDuty", luaopen_pwm_setduty},
    {"close",   luaopen_pwm_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_pwm_consts[] = {
    {"PWM0", PWM_CHANNEL_0},
    {"PWM1", PWM_CHANNEL_1},
    {"PWM2", PWM_CHANNEL_2},
    {"PWM3", PWM_CHANNEL_3},
    {"PWM4", PWM_CHANNEL_4},
    {NULL, NULL}
};

int luaopen_pwm(lua_State* L) {
    luaL_newlib(L, luaopen_pwm_funcs);
    luaL_setfuncs(L, luaopen_pwm_consts, 0);

    /* 创建元表 */
    luaL_newmetatable(L, "pwm_handle");
    luaL_setfuncs(L, luaopen_pwm_methods, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    return 1;
}