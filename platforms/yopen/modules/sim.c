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
#include "yopen_ril.h"

/* 信号强度映射 */
static const int rssi_map[] = {0, 1, 2, 3, 4, 5, 31, 99};

/**
 * @brief 获取SIM卡状态
 * @api sim.status()
 * @return int 状态值 (0=未知, 1=正常, 2=错误, 3=缺失)
 */
static int luaopen_sim_status(lua_State* L) {
    yopen_sim_status_e status = yopen_SimGetStatus();

    int result;
    switch (status) {
        case YOPEN_SIM_STATUS_READY:
            result = 1;
            break;
        case YOPEN_SIM_STATUS_ERROR:
            result = 2;
            break;
        case YOPEN_SIM_STATUS_NOT_INSERT:
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
    char iccid[32] = {0};

    yopen_errcode_ril_e ret = yopen_ril_request_iccid(iccid, sizeof(iccid));

    if (ret == YOPEN_RIL_SUCCESS && iccid[0] != 0) {
        lua_pushstring(L, iccid);
    } else {
        lua_pushstring(L, "");
    }
    return 1;
}

/**
 * @brief 获取SIM卡IMSI
 * @api sim.imsi()
 * @return string IMSI字符串
 */
static int luaopen_sim_imsi(lua_State* L) {
    char imsi[16] = {0};

    yopen_errcode_ril_e ret = yopen_ril_request_imsi(imsi, sizeof(imsi));

    if (ret == YOPEN_RIL_SUCCESS && imsi[0] != 0) {
        lua_pushstring(L, imsi);
    } else {
        lua_pushstring(L, "");
    }
    return 1;
}

/**
 * @brief 获取信号强度
 * @api sim.rssi()
 * @return int 信号强度 (0-31, 99=未知)
 */
static int luaopen_sim_rssi(lua_State* L) {
    int rssi = 0;

    yopen_errcode_ril_e ret = yopen_ril_request_signal_strength(&rssi);

    if (ret == YOPEN_RIL_SUCCESS) {
        /* 转换到0-31范围 */
        if (rssi >= 0 && rssi < sizeof(rssi_map)/sizeof(rssi_map[0])) {
            rssi = rssi_map[rssi];
        }
    } else {
        rssi = 99;
    }

    lua_pushinteger(L, rssi);
    return 1;
}

/**
 * @brief 获取运营商名称
 * @api sim.operator()
 * @return string 运营商名称
 */
static int luaopen_sim_operator(lua_State* L) {
    char oper[32] = {0};

    yopen_errcode_ril_e ret = yopen_ril_request_operator(oper, sizeof(oper));

    if (ret == YOPEN_RIL_SUCCESS && oper[0] != 0) {
        lua_pushstring(L, oper);
    } else {
        lua_pushstring(L, "");
    }
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