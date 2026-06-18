/*
@module  mobile
@summary 蜂窝网络相关
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     MOBILE
*/

/*
Mobile参考示例
-- 获取IMEI
local imei = mobile.imei()
print("IMEI:", imei)

-- 获取SN
local sn = mobile.sn()
print("SN:", sn)

-- 获取SDK版本
local ver = mobile.ver()
print("SDK Version:", ver)

-- 获取IMSI
local imsi = mobile.imsi()
print("IMSI:", imsi)

-- 获取ICCID
local iccid = mobile.iccid()
print("ICCID:", iccid)

-- 获取CPIN状态
local cpin = mobile.cpin()
print("CPIN:", cpin)

-- 获取信号强度
local rssi = mobile.rssi()
print("rssi:", rssi)

-- 获取网络状态
local status = mobile.status()
print("network status:", status)

-- 获取小区信息
local cell = mobile.cell_info()
print("cell:", cell.ci, cell.mcc, cell.mnc)
*/


#include "lua.h"
#include "module.h"
#include "cm_sim.h"
#include "cm_modem_info.h"
#include "net_api_module.h"
#include "lwip/ip4_addr.h"

#define CM_ICCID_LEN 22

typedef enum {
    CID_UNDEF_OR_UNREGIST = 0,
    CID_ONLY_IPV4_ACT,
    CID_ONLY_IPV6_ACT,
    CID_IPV4V6_ACT,
} cm_cid_status_t;

extern uint8_t cm_get_default_cidnum(void);
extern cm_cid_status_t cm_get_cidstatus(uint8_t cid);

/**
 * @brief 获取IMEI
 * @api mobile.imei()
 * @return string|nil IMEI字符串，失败返回nil
 * @usage
local imei = mobile.imei()
if imei then
    print("IMEI:", imei)
end
*/
static int iot_mobile_imei(lua_State* L) {
    char buf[CM_IMEI_LEN] = {0};
    if (cm_sys_get_imei(buf) == 0) {
        lua_pushstring(L, buf);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 获取SN
 * @api mobile.sn()
 * @return string|nil SN字符串，失败返回nil
 * @usage
local sn = mobile.sn()
if sn then
    print("SN:", sn)
end
*/
static int iot_mobile_sn(lua_State* L) {
    char buf[CM_SN_LEN] = {0};
    if (cm_sys_get_sn(buf) == 0) {
        lua_pushstring(L, buf);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 获取SDK版本
 * @api mobile.ver()
 * @return string|nil 版本字符串，失败返回nil
 * @usage
local ver = mobile.ver()
if ver then
    print("SDK Version:", ver)
end
*/
static int iot_mobile_ver(lua_State* L) {
    char buf[CM_VER_LEN] = {0};
    int ret = cm_sys_get_cm_ver(buf, CM_VER_LEN);
    if (ret >= 0) {
        lua_pushstring(L, buf);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 获取IMSI
 * @api mobile.imsi()
 * @return string|nil IMSI字符串，失败返回nil
 * @usage
local imsi = mobile.imsi()
if imsi then
    print("IMSI:", imsi)
end
*/
static int iot_mobile_imsi(lua_State* L) {
    char buf[CM_IMEI_LEN] = {0};
    if (cm_sim_get_imsi(buf) == 0) {
        lua_pushstring(L, buf);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 获取ICCID
 * @api mobile.iccid()
 * @return string|nil ICCID字符串，失败返回nil
 * @usage
local iccid = mobile.iccid()
if iccid then
    print("ICCID:", iccid)
end
*/
static int iot_mobile_iccid(lua_State* L) {
    char buf[CM_ICCID_LEN] = {0};
    if (cm_sim_get_iccid(buf) == 0) {
        lua_pushstring(L, buf);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 获取CPIN状态
 * @api mobile.cpin()
 * @return int CPIN状态，0(READY),1(PIN),2(PUK)
 * @usage
local cpin = mobile.cpin()
print("CPIN:", cpin)
*/
static int iot_mobile_cpin(lua_State* L) {
    int ret = cm_sim_get_cpin();
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 获取信号强度
 * @api mobile.rssi()
 * @return int 信号强度(dBm)
 * @usage
local rssi = mobile.rssi()
print("rssi:", rssi)
*/
static int iot_mobile_rssi(lua_State* L) {
    cm_radio_info_t radio_info;
    int ret = cm_modem_info_radio(&radio_info);
    if (ret == 0) {
        // rssi = 10 * (rssi - 111)
        int rssi = 10 * (radio_info.rssi - 111);
        lua_pushinteger(L, rssi);
    } else {
        lua_pushinteger(L, -128);
    }
    return 1;
}

/**
 * @brief 获取网络状态
 * @api mobile.status()
 * @return int 网络状态 0未注册,1注册中,2已注册,3拒绝
 * @usage
local status = mobile.status()
*/
static int iot_mobile_status(lua_State* L) {
    uint8_t cid = cm_get_default_cidnum();
    if (cid == 0) {
        lua_pushinteger(L, 0); // 未注册
        return 1;
    }
    
    cm_cid_status_t cid_status = cm_get_cidstatus(cid);
    if (cid_status == CID_UNDEF_OR_UNREGIST) {
        lua_pushinteger(L, 0); // 未注册
    } else {
        lua_pushinteger(L, 2); // 已注册（IPv4/IPv6激活）
    }
    return 1;
}

/**
 * @brief 获取小区信息
 * @api mobile.cell_info()
 * @return table 小区信息 {ci, mcc, mnc, lac, pcid}
 * @usage
local cell = mobile.cell_info()
*/
static int iot_mobile_cell_info(lua_State* L) {
    cm_cell_info_t cell_info;
    int ret = cm_modem_info_cell(&cell_info, 1);
    
    lua_createtable(L, 0, 5);
    
    if (ret > 0) {
        lua_pushinteger(L, cell_info.cid);
        lua_setfield(L, -2, "ci");
        
        int mcc = (cell_info.mcc[0] - '0') * 100 + (cell_info.mcc[1] - '0') * 10 + (cell_info.mcc[2] - '0');
        lua_pushinteger(L, mcc);
        lua_setfield(L, -2, "mcc");
        
        int mnc = (cell_info.mnc[0] - '0') * 10 + (cell_info.mnc[1] - '0');
        lua_pushinteger(L, mnc);
        lua_setfield(L, -2, "mnc");
        
        lua_pushinteger(L, cell_info.tac);
        lua_setfield(L, -2, "lac");
        
        lua_pushinteger(L, cell_info.pci);
        lua_setfield(L, -2, "pcid");
    } else {
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "ci");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "mcc");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "mnc");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "lac");
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "pcid");
    }
    
    return 1;
}

/**
 * @brief 获取本机IP地址
 * @api mobile.local_ip()
 * @return string IP地址
 * @usage
local ip = mobile.local_ip()
*/
static int iot_mobile_local_ip(lua_State* L) {
    uint8_t cid = cm_get_default_cidnum();
    if (cid == 0) {
        lua_pushstring(L, "0.0.0.0");
        return 1;
    }
    
    NET_ApiIfInfo ifinfo;
    if (NET_ApiIfInfoGetByCid(cid, &ifinfo) != ERR_OK) {
        lua_pushstring(L, "0.0.0.0");
        return 1;
    }
    
    if (!(ifinfo.ip_flg & NET_API_IF_IP_4)) {
        lua_pushstring(L, "0.0.0.0");
        return 1;
    }
    
    char ip_str[16] = {0};
    ip4_addr_t ip = ifinfo.ifIp4Addr;
    sprintf(ip_str, "%d.%d.%d.%d", 
            (ip.addr >> 0) & 0xFF,
            (ip.addr >> 8) & 0xFF,
            (ip.addr >> 16) & 0xFF,
            (ip.addr >> 24) & 0xFF);
    lua_pushstring(L, ip_str);
    return 1;
}

static const luaL_Reg mobile_lib[] = {
    { "imei",       iot_mobile_imei },
    { "sn",         iot_mobile_sn },
    { "ver",        iot_mobile_ver },
    { "imsi",       iot_mobile_imsi },
    { "iccid",      iot_mobile_iccid },
    { "cpin",       iot_mobile_cpin },
    { "rssi",       iot_mobile_rssi },
    { "status",     iot_mobile_status },
    { "cell_info",  iot_mobile_cell_info },
    { "local_ip",   iot_mobile_local_ip },
    {NULL, NULL}
};

LUAMOD_API int luaopen_mobile(lua_State* L) {
    luaL_newlibtable(L, mobile_lib);
    luaL_setfuncs(L, mobile_lib, 0);
    return 1;
}