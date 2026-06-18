/*
@module  adc
@summary ADC模数转换
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     ADC
*/

/*
ADC参考示例
-- 读取ADC0电压
local voltage = adc.read(0)
print("ADC0 voltage:", voltage)

-- 读取ADC1电压
local voltage = adc.read(1)
print("ADC1 voltage:", voltage)

-- 读取VBAT电压
local voltage = adc.read_vbat()
print("VBAT voltage:", voltage)
*/


#include "lua.h"
#include "module.h"
#include "cm_adc.h"

#define IOT_ADC_CHAN_0   0
#define IOT_ADC_CHAN_1   1

/**
 * @brief 读取ADC电压
 * @api adc.read(chan)
 * @int chan ADC通道号(0或1)
 * @return int 电压值,单位:mv
 * @usage
-- 读取ADC0
local voltage = adc.read(0)
if voltage then
    print("ADC0 voltage:", voltage, "mv")
end
 */
static int iot_adc_read(lua_State* L) {
    int chan = luaL_checkinteger(L, 1);
    int32_t voltage = 0;
    int32_t ret;

    if (chan == IOT_ADC_CHAN_0) {
        ret = cm_adc_read(CM_ADC_0, &voltage);
    } else if (chan == IOT_ADC_CHAN_1) {
        ret = cm_adc_read(CM_ADC_1, &voltage);
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "invalid channel");
        return 2;
    }

    if (ret == 0) {
        lua_pushinteger(L, voltage);
        return 1;
    } else {
        lua_pushnil(L);
        lua_pushinteger(L, ret);
        return 2;
    }
}

/**
 * @brief 读取VBAT电压
 * @api adc.read_vbat()
 * @return int 电压值，单位:mv,读取失败返回nil
 * @usage
local voltage = adc.read_vbat()
if voltage then
    print("VBAT voltage:", voltage, "mv")
end
 */
static int iot_adc_read_vbat(lua_State* L) {
    uint32_t voltage = 0;
    int32_t ret;

    ret = cm_adc_vbat_read(&voltage);
    if (ret == 0) {
        lua_pushinteger(L, voltage);
        return 1;
    } else {
        lua_pushnil(L);
        return 1;
    }
}

static const luaL_Reg adc_lib[] = {
    { "read",      iot_adc_read },
    { "read_vbat", iot_adc_read_vbat },
    {NULL, NULL}
};

LUAMOD_API int luaopen_adc(lua_State* L) {
    luaL_newlibtable(L, adc_lib);
    luaL_setfuncs(L, adc_lib, 0);
    return 1;
}
