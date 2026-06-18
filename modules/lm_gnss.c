/**
 * @file iot_gnss.c
 * @brief GNSS定位模块Lua封装
 * @version 1.0
 * @date 2026.06.10
 * 
 * @example
 * -- Lua使用示例
 * local gnss = require("gnss")
 * 
 * -- 方式一：使用回调获取NMEA原始数据
 * gnss.on(function(type, nmea)
 *     print("NMEA type:", type, "data:", nmea)
 * end)
 * 
 * -- 打开GNSS（默认BDS+GPS）
 * gnss.open()
 * 
 * -- 配置NMEA输出（可选）
 * gnss.config(gnss.NMEA_GGA | gnss.NMEA_RMC, 1)  -- 输出GGA和RMC，周期1秒
 * 
 * -- 设置定位模式（可选）
 * gnss.mode(0)  -- 0-连续定位, 1-单次定位
 * 
 * -- 方式二：定时获取定位信息
 * while true do
 *     local loc = gnss.location()
 *     if loc then
 *         print(string.format("位置: %.4f, %.4f", loc.latitude, loc.longitude))
 *         print(string.format("海拔: %.1fm, 速度: %.1fkm/h", loc.altitude, loc.speed))
 *         print(string.format("卫星数: %d, 定位类型: %d", loc.satellites, loc.fix))
 *     end
 *     rtos.sleep(1000)
 * end
 * 
 * -- 关闭GNSS
 * gnss.close()
 */

#include "iot_gnss.h"
#include "cm_gnss.h"
#include "lua.h"
#include "lauxlib.h"

/* NMEA协议类型常量 */
#define NMEA_GGA  (1 << 0)
#define NMEA_GSV  (1 << 1)
#define NMEA_GSA  (1 << 2)
#define NMEA_RMC  (1 << 3)
#define NMEA_VTG  (1 << 4)
#define NMEA_GLL  (1 << 5)

/* 导航系统类型 */
static const int gnss_systems[] = {
    CM_GNSS_BDS,              // "bds"
    CM_GNSS_GPS,              // "gps"
    CM_GNSS_BDS_GPS,          // "bds_gps"
    CM_GNSS_GLONASS,          // "glonass"
    CM_GNSS_BDS_GLONASS,      // "bds_glonass"
    CM_GNSS_GPS_GLONASS,      // "gps_glonass"
    CM_GNSS_BDS_GPS_GLONASS,  // "all"
};

/* Lua回调函数存储 */
static lua_State *gnss_L = NULL;
static int gnss_callback_ref = LUA_NOREF;

/**
 * @brief NMEA信息回调函数
 */
static void gnss_nmea_callback(uint32_t type, const char *nmea, uint32_t len)
{
    if (gnss_L && gnss_callback_ref != LUA_NOREF)
    {
        lua_rawgeti(gnss_L, LUA_REGISTRYINDEX, gnss_callback_ref);
        if (lua_isfunction(gnss_L, -1))
        {
            lua_pushinteger(gnss_L, type);
            lua_pushlstring(gnss_L, nmea, len);
            lua_pcall(gnss_L, 2, 0, 0);
        }
        lua_pop(gnss_L, 1);
    }
}

/**
 * @brief 打开GNSS功能
 * gnss.open([system], [agnss])
 * - system: 导航系统类型，可选，默认为BDS_GPS
 * - agnss: 是否启用辅助定位，可选，默认为false
 */
static int l_gnss_open(lua_State *L)
{
    const char *system = luaL_optstring(L, 1, "bds_gps");
    int agnss = luaL_optboolean(L, 2, 0);
    
    int gnss_type = CM_GNSS_BDS_GPS;
    if (strcmp(system, "bds") == 0)
        gnss_type = CM_GNSS_BDS;
    else if (strcmp(system, "gps") == 0)
        gnss_type = CM_GNSS_GPS;
    else if (strcmp(system, "glonass") == 0)
        gnss_type = CM_GNSS_GLONASS;
    else if (strcmp(system, "bds_glonass") == 0)
        gnss_type = CM_GNSS_BDS_GLONASS;
    else if (strcmp(system, "gps_glonass") == 0)
        gnss_type = CM_GNSS_GPS_GLONASS;
    else if (strcmp(system, "all") == 0)
        gnss_type = CM_GNSS_BDS_GPS_GLONASS;
    
    int32_t ret = cm_gnss_open(gnss_type, agnss ? CM_AGNSS_ENABLE : CM_AGNSS_DISABLE);
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 关闭GNSS功能
 * gnss.close()
 */
static int l_gnss_close(lua_State *L)
{
    cm_gnss_close();
    
    /* 清理回调 */
    if (gnss_callback_ref != LUA_NOREF)
    {
        luaL_unref(gnss_L, LUA_REGISTRYINDEX, gnss_callback_ref);
        gnss_callback_ref = LUA_NOREF;
    }
    
    return 0;
}

/**
 * @brief 设置NMEA输出配置
 * gnss.configNmea(mask, cycle)
 * - mask: NMEA输出类型掩码
 * - cycle: 输出周期（秒）
 */
static int l_gnss_config_nmea(lua_State *L)
{
    uint32_t mask = luaL_optinteger(L, 1, NMEA_GGA | NMEA_RMC);
    int cycle = luaL_optinteger(L, 2, 1);
    
    int32_t ret1 = cm_gnss_config(CM_GNSS_CONFIG_TYPE_NMEA_MASK, &mask);
    int32_t ret2 = cm_gnss_config(CM_GNSS_CONFIG_TYPE_NMEA_CYCLE, &cycle);
    
    lua_pushboolean(L, (ret1 == 0) && (ret2 == 0));
    return 1;
}

/**
 * @brief 设置定位模式
 * gnss.setMode(mode)
 * - mode: 0-连续定位, 1-单次定位
 */
static int l_gnss_set_mode(lua_State *L)
{
    int mode = luaL_checkinteger(L, 1);
    
    int32_t ret = cm_gnss_config(CM_GNSS_CONFIG_TYPE_GNSS_CTRL, &mode);
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置NMEA回调函数
 * gnss.onNmea(callback)
 * - callback(type, nmea): 回调函数
 */
static int l_gnss_on_nmea(lua_State *L)
{
    if (lua_isfunction(L, 1))
    {
        /* 保存回调函数引用 */
        if (gnss_callback_ref != LUA_NOREF)
            luaL_unref(L, LUA_REGISTRYINDEX, gnss_callback_ref);
        
        lua_pushvalue(L, 1);
        gnss_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        gnss_L = L;
        
        /* 注册NMEA回调 */
        uint32_t nmea_type = NMEA_GGA | NMEA_GSV | NMEA_GSA | NMEA_RMC | NMEA_VTG | NMEA_GLL;
        int32_t ret = cm_gnss_set_nmea_callback(gnss_nmea_callback, nmea_type);
        
        lua_pushboolean(L, ret == 0);
    }
    else
    {
        /* 取消回调 */
        if (gnss_callback_ref != LUA_NOREF)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, gnss_callback_ref);
            gnss_callback_ref = LUA_NOREF;
        }
        lua_pushboolean(L, true);
    }
    
    return 1;
}

/**
 * @brief 获取定位信息
 * gnss.getLocation()
 * 返回: table包含定位信息
 */
static int l_gnss_get_location(lua_State *L)
{
    cm_gnss_location_info_t info;
    int32_t ret = cm_gnss_get_location_info(&info);
    
    if (ret == 0)
    {
        lua_newtable(L);
        
        /* 时间 */
        lua_pushstring(L, "year");
        lua_pushinteger(L, info.date_time.tm_year + 1900);
        lua_settable(L, -3);
        
        lua_pushstring(L, "month");
        lua_pushinteger(L, info.date_time.tm_mon + 1);
        lua_settable(L, -3);
        
        lua_pushstring(L, "day");
        lua_pushinteger(L, info.date_time.tm_mday);
        lua_settable(L, -3);
        
        lua_pushstring(L, "hour");
        lua_pushinteger(L, info.date_time.tm_hour);
        lua_settable(L, -3);
        
        lua_pushstring(L, "min");
        lua_pushinteger(L, info.date_time.tm_min);
        lua_settable(L, -3);
        
        lua_pushstring(L, "sec");
        lua_pushinteger(L, info.date_time.tm_sec);
        lua_settable(L, -3);
        
        /* 位置 */
        lua_pushstring(L, "latitude");
        lua_pushnumber(L, info.latitude);
        lua_settable(L, -3);
        
        lua_pushstring(L, "longitude");
        lua_pushnumber(L, info.longitude);
        lua_settable(L, -3);
        
        lua_pushstring(L, "altitude");
        lua_pushnumber(L, info.altitude);
        lua_settable(L, -3);
        
        /* 精度和速度 */
        lua_pushstring(L, "hdop");
        lua_pushnumber(L, info.hdop);
        lua_settable(L, -3);
        
        lua_pushstring(L, "speed");
        lua_pushnumber(L, info.spkm);
        lua_settable(L, -3);
        
        lua_pushstring(L, "cog");
        lua_pushnumber(L, info.cog);
        lua_settable(L, -3);
        
        /* 定位状态 */
        lua_pushstring(L, "fix");
        lua_pushinteger(L, info.fix);
        lua_settable(L, -3);
        
        lua_pushstring(L, "satellites");
        lua_pushinteger(L, info.nsat);
        lua_settable(L, -3);
        
        lua_pushstring(L, "dtype");
        lua_pushinteger(L, info.dtype);
        lua_settable(L, -3);
    }
    else
    {
        lua_pushnil(L);
        lua_pushstring(L, "get location failed");
        return 2;
    }
    
    return 1;
}

/**
 * @brief GNSS重启
 * gnss.reset([mode])
 * - mode: 重启模式，可选，0-自动,1-冷启动,2-温启动,3-热启动
 */
static int l_gnss_reset(lua_State *L)
{
    int mode = luaL_optinteger(L, 1, CM_GNSS_RESET_AUTO);
    
    cm_gnss_reset_mode_e reset_mode;
    switch (mode)
    {
        case 1: reset_mode = CM_GNSS_RESET_COOL; break;
        case 2: reset_mode = CM_GNSS_NMEA_WARM; break;
        case 3: reset_mode = CM_GNSS_NMEA_HOT; break;
        default: reset_mode = CM_GNSS_RESET_AUTO;
    }
    
    int32_t ret = cm_gnss_reset(reset_mode);
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 请求NMEA数据
 * gnss.requestNmea()
 */
static int l_gnss_req_nmea(lua_State *L)
{
    int32_t ret = cm_gnss_req_nmea();
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 向GNSS芯片发送原始命令
 * gnss.sendRaw(data)
 */
static int l_gnss_send_raw(lua_State *L)
{
    size_t len;
    const char *data = luaL_checklstring(L, 1, &len);
    
    int32_t ret = cm_gnss_send_raw_data(data, len, NULL);
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 启用AGNSS辅助定位
 * gnss.enableAgnss([location], [time])
 */
static int l_gnss_enable_agnss(lua_State *L)
{
    /* 设置AGNSS为启用状态 */
    int agnss_enable = CM_AGNSS_ENABLE;
    
    /* 如果提供了位置信息 */
    if (lua_istable(L, 1))
    {
        cm_agnss_location_t loc;
        
        lua_getfield(L, 1, "latitude");
        loc.latitude = lua_tonumber(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 1, "longitude");
        loc.longitude = lua_tonumber(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 1, "mode");
        loc.mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        cm_gnss_config(CM_GNSS_CONFIG_TYPE_AGNSS_LOCATION, &loc);
    }
    
    /* 如果提供了时间信息 */
    if (lua_istable(L, 2))
    {
        struct tm time_info = {0};
        
        lua_getfield(L, 2, "year");
        time_info.tm_year = lua_tointeger(L, -1) - 1900;
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "month");
        time_info.tm_mon = lua_tointeger(L, -1) - 1;
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "day");
        time_info.tm_mday = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "hour");
        time_info.tm_hour = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "min");
        time_info.tm_min = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "sec");
        time_info.tm_sec = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        cm_gnss_config(CM_GNSS_CONFIG_TYPE_AGNSS_TIME, &time_info);
    }
    
    lua_pushboolean(L, true);
    return 1;
}

/**
 * @brief 查询AGNSS状态
 * gnss.getAgnssState()
 */
static int l_gnss_get_agnss_state(lua_State *L)
{
    cm_agnss_update_result_e mode;
    char update_time[13] = {0};
    uint32_t data_size = 0;
    
    int32_t ret = cm_agnss_state_query(&mode, update_time, &data_size);
    
    if (ret == 0)
    {
        lua_newtable(L);
        
        lua_pushstring(L, "state");
        lua_pushinteger(L, mode);
        lua_settable(L, -3);
        
        lua_pushstring(L, "update_time");
        lua_pushstring(L, update_time);
        lua_settable(L, -3);
        
        lua_pushstring(L, "size");
        lua_pushinteger(L, data_size);
        lua_settable(L, -3);
    }
    else
    {
        lua_pushnil(L);
        lua_pushstring(L, "query failed");
        return 2;
    }
    
    return 1;
}

/**
 * @brief 更新AGNSS数据
 * gnss.updateAgnss()
 */
static int l_gnss_update_agnss(lua_State *L)
{
    int32_t ret = cm_agnss_data_start_update(NULL);
    
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 启用DGNSS差分定位
 * gnss.enableDgnss()
 */
static int l_gnss_enable_dgnss(lua_State *L)
{
    int32_t ret = cm_dgnss_enable();
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 禁用DGNSS差分定位
 * gnss.disableDgnss()
 */
static int l_gnss_disable_dgnss(lua_State *L)
{
    int32_t ret = cm_dgnss_disable();
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/* 模块函数列表 */
static const luaL_Reg gnss_lib[] = {
    {"open", l_gnss_open},
    {"close", l_gnss_close},
    {"config", l_gnss_config_nmea},
    {"mode", l_gnss_set_mode},
    {"on", l_gnss_on_nmea},
    {"location", l_gnss_get_location},
    {"reset", l_gnss_reset},
    {"request", l_gnss_req_nmea},
    {"send", l_gnss_send_raw},
    {"enableAgnss", l_gnss_enable_agnss},
    {"getAgnssState", l_gnss_get_agnss_state},
    {"updateAgnss", l_gnss_update_agnss},
    {"enableDgnss", l_gnss_enable_dgnss},
    {"disableDgnss", l_gnss_disable_dgnss},
    {NULL, NULL}
};

/* Lua模块入口 */
LUAMOD_API int luaopen_gnss(lua_State *L)
{
    /* 初始化回调引用 */
    gnss_callback_ref = LUA_NOREF;
    
    /* 创建模块表 */
    luaL_newlibtable(L, gnss_lib);
    luaL_setfuncs(L, gnss_lib, 0);
    
    /* 添加常量 */
    lua_pushinteger(L, NMEA_GGA);
    lua_setfield(L, -2, "NMEA_GGA");
    
    lua_pushinteger(L, NMEA_GSV);
    lua_setfield(L, -2, "NMEA_GSV");
    
    lua_pushinteger(L, NMEA_GSA);
    lua_setfield(L, -2, "NMEA_GSA");
    
    lua_pushinteger(L, NMEA_RMC);
    lua_setfield(L, -2, "NMEA_RMC");
    
    lua_pushinteger(L, NMEA_VTG);
    lua_setfield(L, -2, "NMEA_VTG");
    
    lua_pushinteger(L, NMEA_GLL);
    lua_setfield(L, -2, "NMEA_GLL");
    
    return 1;
}
