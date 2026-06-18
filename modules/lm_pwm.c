/*
@module  pwm
@summary PWM脉冲宽度调制
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     PWM
*/

/*
PWM参考示例
-- 打开PWM0,周期1ms(1000000ns),高电平500us(500000ns)
pwm.open(0, 1000000, 500000)

-- 关闭PWM0
pwm.close(0)
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_pwm.h"

/**
 * @brief 打开PWM
 * @api pwm.open(id, period, period_h)
 * @int id PWM ID,0-1
 * @int period 周期,单位ns,最小120000
 * @int period_h 高电平时间,单位ns
 * @return bool 成功返回true,失败返回false
 * @usage
pwm.open(0, 1000000, 500000)
*/
static int iot_pwm_open(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    uint32_t period = luaL_checkinteger(L, 2);
    uint32_t period_h = luaL_checkinteger(L, 3);

    if (id < 0 || id >= CM_PWM_DEV_NUM) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_pwm_open_ns((cm_pwm_dev_e)id, period, period_h);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 关闭PWM
 * @api pwm.close(id)
 * @int id PWM ID,0-1
 * @return bool 成功返回true,失败返回false
 * @usage
pwm.close(0)
*/
static int iot_pwm_close(lua_State* L) {
    int id = luaL_checkinteger(L, 1);

    if (id < 0 || id >= CM_PWM_DEV_NUM) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_pwm_close((cm_pwm_dev_e)id);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg pwm_lib[] = {
    { "open",  iot_pwm_open },
    { "close", iot_pwm_close },
    {NULL, NULL}
};

LUAMOD_API int luaopen_pwm(lua_State* L) {
    luaL_newlibtable(L, pwm_lib);
    luaL_setfuncs(L, pwm_lib, 0);
    return 1;
}
