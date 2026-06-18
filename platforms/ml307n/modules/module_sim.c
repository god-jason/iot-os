/*
@module  sim
@summary SIM卡相关
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     SIM
*/

/*
SIM参考示例
-- 获取SIM卡状态
local status = sim.cpin()
if status == 0 then
    print("SIM卡就绪")
end

-- 获取IMSI
local imsi = sim.imsi()
print("IMSI:", imsi)

-- 获取ICCID
local iccid = sim.iccid()
print("ICCID:", iccid)

-- 获取EID
local eid = sim.eid()
print("EID:", eid)

-- 设置/获取SIM卡模式
sim.switch({sim0_enable=true, sim0_hotswap_enable=false, sim1_enable=false, sim1_hotswap_enable=false})
local cfg = sim.switch()

-- 设置默认APN
sim.set_apn({pdp_type=0, apn="cmiot"})

-- 获取默认APN
local apn = sim.get_apn()
print("APN:", apn.apn)
*/


#include "lua.h"
#include "module.h"
#include "cm_sim.h"

/* ML307N SDK 未提供默认 APN 配置接口，先本地定义桩实现 */
#ifndef CM_SYS_MAX_APN_LEN
#define CM_SYS_MAX_APN_LEN 64
#endif

typedef struct {
    int pdp_type;
    uint8_t apn[CM_SYS_MAX_APN_LEN + 1];
} cm_default_apn_t;

static int cm_modem_set_default_apn(cm_default_apn_t* apn)
{
    (void)apn;
    return -1;
}

static int cm_modem_get_default_apn(cm_default_apn_t* apn)
{
    (void)apn;
    return -1;
}

/**
 * @brief 获取SIM卡状态
 * @api sim.cpin()
 * @return int 状态码,0表示就绪
 * @usage
local status = sim.cpin()
if status == 0 then
    print("SIM卡就绪")
end
*/
static int iot_sim_cpin(lua_State* L) {
    int32_t ret = cm_sim_get_cpin();
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 获取IMSI
 * @api sim.imsi()
 * @return string IMSI,失败返回nil
 * @usage
local imsi = sim.imsi()
*/
static int iot_sim_imsi(lua_State* L) {
    char imsi[16] = {0};
    int32_t ret = cm_sim_get_imsi(imsi);
    if (ret == 0) {
        lua_pushstring(L, imsi);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取ICCID
 * @api sim.iccid()
 * @return string ICCID,失败返回nil
 * @usage
local iccid = sim.iccid()
*/
static int iot_sim_iccid(lua_State* L) {
    char iccid[21] = {0};
    int32_t ret = cm_sim_get_iccid(iccid);
    if (ret == 0) {
        lua_pushstring(L, iccid);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取EID
 * @api sim.eid()
 * @return string EID,失败返回nil
 * @usage
local eid = sim.eid()
*/
static int iot_sim_eid(lua_State* L) {
    char eid[21] = {0};
    int ret = cm_sim_get_eid(eid);
    if (ret == 0) {
        lua_pushstring(L, eid);
    } else {
        lua_pushnil(L);
    }
    return 1;
}
/**
 * @brief 设置默认APN
 * @api sim.set_apn(cfg)
 * @table cfg APN配置参数 {pdp_type=0/1/2, apn="xxx"}
 * @return bool 成功返回true,失败返回false
 * @usage
 *sim.set_apn({pdp_type=0, apn="cmiot"})
 */
static int iot_sim_set_apn(lua_State* L) {
    if (!lua_istable(L, 1)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    cm_default_apn_t apn = {0};

    lua_getfield(L, 1, "pdp_type");
    apn.pdp_type = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "apn");
    const char* apn_str = lua_isnil(L, -1) ? NULL : lua_tostring(L, -1);
    if (apn_str) {
        strncpy((char*)apn.apn, apn_str, CM_SYS_MAX_APN_LEN);
        ((char*)apn.apn)[CM_SYS_MAX_APN_LEN] = '\0';
    }
    lua_pop(L, 1);

    int ret = cm_modem_set_default_apn(&apn);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取默认APN
 * @api sim.get_apn()
 * @return table APN配置参数 {pdp_type, apn}
 * @usage
 *local apn = sim.get_apn()
 *print("pdp_type:", apn.pdp_type)
 *print("apn:", apn.apn)
 */
static int iot_sim_get_apn(lua_State* L) {
    cm_default_apn_t apn = {0};
    int ret = cm_modem_get_default_apn(&apn);

    if (ret != 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 2);
    lua_pushinteger(L, apn.pdp_type);
    lua_setfield(L, -2, "pdp_type");
    lua_pushstring(L, (const char*)apn.apn);
    lua_setfield(L, -2, "apn");

    return 1;
}

/**
 * @brief 设置/获取SIM卡模式
 * @api sim.switch(cfg)
 * @table cfg 可选设置参数 {sim0_enable, sim0_hotswap_enable, sim1_enable, sim1_hotswap_enable}
 * @return bool|table 设置成功返回true,获取时返回配置表
 * @usage
 *-- 设置
 *sim.switch({sim0_enable=true, sim0_hotswap_enable=false, sim1_enable=false, sim1_hotswap_enable=false})
 *-- 获取
 *local cfg = sim.switch()
 */
static int iot_sim_switch(lua_State* L) {
    /* 获取模式 */
    cm_sim_switch_t mode;
    memset(&mode, 0, sizeof(mode));
    int ret = cm_sim_get_switch(&mode);

    /* 如果有参数则是设置 */
    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "sim0_enable");
        mode.sim0_enable = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "sim0_hotswap_enable");
        mode.sim0_hotswap_enable = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "sim1_enable");
        mode.sim1_enable = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "sim1_hotswap_enable");
        mode.sim1_hotswap_enable = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
        lua_pop(L, 1);

        ret = cm_sim_set_switch(mode);
        lua_pushboolean(L, ret == 0);
        return 1;
    }

    /* 无参数则返回当前配置 */
    if (ret != 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 4);
    lua_pushboolean(L, mode.sim0_enable);
    lua_setfield(L, -2, "sim0_enable");
    lua_pushboolean(L, mode.sim0_hotswap_enable);
    lua_setfield(L, -2, "sim0_hotswap_enable");
    lua_pushboolean(L, mode.sim1_enable);
    lua_setfield(L, -2, "sim1_enable");
    lua_pushboolean(L, mode.sim1_hotswap_enable);
    lua_setfield(L, -2, "sim1_hotswap_enable");

    return 1;
}

static const luaL_Reg sim_lib[] = {
    { "cpin",    iot_sim_cpin },
    { "imsi",    iot_sim_imsi },
    { "iccid",   iot_sim_iccid },
    { "eid",     iot_sim_eid },
    { "switch",  iot_sim_switch },
    { "set_apn", iot_sim_set_apn },
    { "get_apn", iot_sim_get_apn },
    { NULL, NULL }
};

LUAMOD_API int luaopen_sim(lua_State* L) {
    luaL_newlibtable(L, sim_lib);
    luaL_setfuncs(L, sim_lib, 0);
    return 1;
}
