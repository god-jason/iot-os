/*
@module  sim
@summary SIM卡操作
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SIM
*/

/*
SIM参考示例
-- 获取SIM卡状态
local status = sim.status()
print("sim status:", status)

-- 获取ICCID
local iccid = sim.iccid()
print("iccid:", iccid)

-- 获取IMSI
local imsi = sim.imsi()
print("imsi:", imsi)

-- 获取信号强度
local rssi = sim.rssi()
print("rssi:", rssi)
*/

#include "module.h"
#include "yopen_sim.h"
#include "yopen_nw.h"

/**
 * @brief 获取SIM卡状态
 * @api sim.status()
 * @return int 状态值 (0=未知, 1=正常, 2=错误, 3=缺失)
 */
static int luaopen_sim_status(lua_State* L) {
    yopen_sim_status_e status = YOPEN_SIM_STATUS_UNKNOW;
    yopen_sim_get_card_status(0, &status);

    int result;
    switch (status) {
        case YOPEN_SIM_STATUS_READY:
        case YOPEN_SIM_STATUS_SIMPIN:
        case YOPEN_SIM_STATUS_PIN1_READY:
            result = 1;
            break;
        case YOPEN_SIM_STATUS_NOSIM:
            result = 3;
            break;
        default:
            result = 0;
            break;
    }

    lua_pushinteger(L, result);
    return 1;
}

/**
 * @brief 获取SIM卡ICCID
 * @api sim.iccid()
 * @return string ICCID字符串
 */
static int luaopen_sim_iccid(lua_State* L) {
    char iccid[YOPEN_SIM_ICCID_LEN + 1] = {0};
    yopen_sim_errcode_e ret = yopen_sim_get_iccid(0, iccid, sizeof(iccid));

    lua_pushstring(L, ret == YOPEN_SIM_SUCCESS ? iccid : "");
    return 1;
}

/**
 * @brief 获取SIM卡IMSI
 * @api sim.imsi()
 * @return string IMSI字符串
 */
static int luaopen_sim_imsi(lua_State* L) {
    char imsi[YOPEN_SIM_IMSI_LEN_MAX + 1] = {0};
    yopen_sim_errcode_e ret = yopen_sim_get_imsi(0, imsi, sizeof(imsi));

    lua_pushstring(L, ret == YOPEN_SIM_SUCCESS ? imsi : "");
    return 1;
}

/**
 * @brief 获取信号强度
 * @api sim.rssi()
 * @return int 信号强度 (0-31, 99=未知)
 */
static int luaopen_sim_rssi(lua_State* L) {
    unsigned char csq = 99;
    yopen_nw_get_csq(0, &csq);
    lua_pushinteger(L, csq);
    return 1;
}

/**
 * @brief 获取运营商名称
 * @api sim.operator()
 * @return string 运营商名称
 */
static int luaopen_sim_operator(lua_State* L) {
    yopen_nw_operator_info_s oper_info = {0};
    yopen_nw_get_operator_name(0, &oper_info);
    lua_pushstring(L, oper_info.oper_name[0] ? oper_info.oper_name : "");
    return 1;
}

static const luaL_Reg luaopen_sim_funcs[] = {
    {"status",   luaopen_sim_status},
    {"iccid",    luaopen_sim_iccid},
    {"imsi",     luaopen_sim_imsi},
    {"rssi",     luaopen_sim_rssi},
    {"operator", luaopen_sim_operator},
    {NULL, NULL}
};

int luaopen_sim(lua_State* L) {
    luaL_newlib(L, luaopen_sim_funcs);

    /* 状态常量 */
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "UNKNOWN");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "READY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "ERROR");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "NOT_INSERT");

    return 1;
}