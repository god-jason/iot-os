/*
@module  adc
@summary ADC模数转换
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     ADC
*/

/*
ADC参考示例
-- 打开ADC通道
local adc = adc.open(0)  -- 0=ADC0, 1=ADC1, 2=TEM, 3=VBAT

-- 读取电压值(mV)
local value = adc.read()
print("voltage:", value, "mV")

-- 关闭ADC
adc.close()
*/

#include "module.h"
#include "yopen_adc.h"

/* ADC通道定义 */
#define ADC_CHANNEL_0   0
#define ADC_CHANNEL_1   1
#define ADC_CHANNEL_TEM 2
#define ADC_CHANNEL_VBAT 3

/**
 * @brief 打开ADC通道
 * @api adc.open(channel)
 * @int channel 通道号 (0=ADC0, 1=ADC1, 2=温度, 3=电池电压)
 * @return userdata ADC操作句柄
 */
static int luaopen_adc_open(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);

    if (channel < 0 || channel >= YOPEN_ADC_CHANNEL_MAX) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid channel");
        return 2;
    }

    /* 保存通道号到userdata */
    int* adc_handle = (int*)iot_malloc(sizeof(int));
    if (!adc_handle) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }
    *adc_handle = channel;

    lua_pushlightuserdata(L, adc_handle);
    return 1;
}

/**
 * @brief 读取ADC电压值
 * @api adc.read()
 * @return int 电压值(mV)
 */
static int luaopen_adc_read(lua_State* L) {
    int* adc_handle = (int*)lua_touserdata(L, 1);

    if (!adc_handle) {
        /* 使用默认通道0 */
        int value = 0;
        yopen_adc_get_volt(YOPEN_ADC0_CHANNEL, &value);
        lua_pushinteger(L, value);
        return 1;
    }

    int channel = *adc_handle;
    int value = 0;
    yopen_errcode_adc_e ret;

    switch (channel) {
        case ADC_CHANNEL_0:
            ret = yopen_adc_get_volt(YOPEN_ADC0_CHANNEL, &value);
            break;
        case ADC_CHANNEL_1:
            ret = yopen_adc_get_volt(YOPEN_ADC1_CHANNEL, &value);
            break;
        case ADC_CHANNEL_TEM:
            ret = yopen_adc_get_volt(YOPEN_ADC_TEM_CHANNEL, &value);
            break;
        case ADC_CHANNEL_VBAT:
            ret = yopen_adc_get_volt(YOPEN_ADC_VBAT_CHANNEL, &value);
            break;
        default:
            ret = YOPEN_ADC_INVALID_PARAM_ERR;
            break;
    }

    if (ret == YOPEN_ADC_SUCCESS) {
        lua_pushinteger(L, value);
    } else {
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 关闭ADC
 * @api adc.close()
 * @return nil
 */
static int luaopen_adc_close(lua_State* L) {
    int* adc_handle = (int*)lua_touserdata(L, 1);
    if (adc_handle) {
        iot_free(adc_handle);
    }
    return 0;
}

static const luaL_Reg luaopen_adc_funcs[] = {
    {"open",  luaopen_adc_open},
    {"read",  luaopen_adc_read},
    {"close", luaopen_adc_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_adc_consts[] = {
    {"CH0",   ADC_CHANNEL_0},
    {"CH1",   ADC_CHANNEL_1},
    {"CH_TEM", ADC_CHANNEL_TEM},
    {"CH_VBAT", ADC_CHANNEL_VBAT},
    {NULL, NULL}
};

int luaopen_adc(lua_State* L) {
    luaL_newlib(L, luaopen_adc_funcs);
    luaL_setfuncs(L, luaopen_adc_consts, 0);
    return 1;
}