/*
@module  wifi
@summary WIFI扫描
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     WIFI
*/

/*
WIFI参考示例
-- 配置扫描参数
wifi.config("maxCount", 5)
wifi.config("timeout", 10)

-- 开始扫描
wifi.scan(function(result)
    for i, ap in ipairs(result) do
        print("AP", i, ap.bssid, ap.rssi, ap.channel)
    end
end)

-- 停止扫描
wifi.stop()

-- 查询上次扫描结果
local result = wifi.query()
if result then
    for i, ap in ipairs(result) do
        print("AP", i, ap.bssid, ap.rssi, ap.channel)
    end
end
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_wifiscan.h"
#include "iot_rtos.h"

static void* wifi_scan_callback_ud = NULL;
static int wifi_scan_in_progress = 0;

static void wifi_scan_cb(cm_wifi_scan_info_t* param, void* user_param) {
    if (!wifi_scan_callback_ud) {
        wifi_scan_in_progress = 0;
        return;
    }

    params_t* params = params_create(1);
    if (!params) {
        LOG("ERR params_create failed");
        wifi_scan_in_progress = 0;
        return;
    }

    /* 创建JSON字符串表示扫描结果 */
    char json_buf[2048] = "{\"aps\":[";
    int len = strlen(json_buf);

    for (int i = 0; i < param->bssid_number; i++) {
        if (i > 0) {
            strcat(json_buf + len, ",");
            len++;
        }
        
        char bssid[20];
        snprintf(bssid, sizeof(bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
                 param->channel_cell_list[i].bssid[0],
                 param->channel_cell_list[i].bssid[1],
                 param->channel_cell_list[i].bssid[2],
                 param->channel_cell_list[i].bssid[3],
                 param->channel_cell_list[i].bssid[4],
                 param->channel_cell_list[i].bssid[5]);
        
        len += snprintf(json_buf + len, sizeof(json_buf) - len,
            "{\"bssid\":\"%s\",\"rssi\":%d,\"channel\":%d}",
            bssid, param->channel_cell_list[i].rssi,
            param->channel_cell_list[i].channel_number);
    }
    
    strcat(json_buf + len, "]}");

    params_push_string(params, json_buf, strlen(json_buf));
    iot_rtos_call(wifi_scan_callback_ud, params);
    
    wifi_scan_callback_ud = NULL;
    wifi_scan_in_progress = 0;
}

/**
 * @brief 配置WIFI扫描参数
 * @api wifi.config(key, value)
 * @string key 参数键 (maxCount/timeout)
 * @int value 参数值
 * @return int 0成功, 其他失败
 * @usage
wifi.config("maxCount", 5)
wifi.config("timeout", 10)
*/
static int iot_wifi_config(lua_State* L) {
    const char* key = luaL_checkstring(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    
    LOG("WiFi config: %s=%d", key, value);
    if (strcmp(key, "maxCount") == 0) {
        uint8_t max_count = (uint8_t)value;
        if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max_count) == 0) {
            lua_pushinteger(L, 0);
        } else {
            lua_pushinteger(L, -1);
        }
    } else if (strcmp(key, "timeout") == 0) {
        uint8_t timeout = (uint8_t)value;
        if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &timeout) == 0) {
            lua_pushinteger(L, 0);
        } else {
            lua_pushinteger(L, -1);
        }
    } else {
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 开始WIFI扫描
 * @api wifi.scan(callback)
 * @function callback 扫描完成回调
 * @return int 0成功, 其他失败
 * @usage
wifi.scan(function(result)
    for i, ap in ipairs(result) do
        print("AP", i, ap.bssid, ap.rssi, ap.channel)
    end
end)
*/
static int iot_wifi_scan(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    
    if (wifi_scan_in_progress) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    /* 保存回调函数 */
    void* ud = lua_newuserdata(L, 1);
    lua_pushvalue(L, 1);
    lua_setuservalue(L, -2);
    wifi_scan_callback_ud = ud;

    lua_pushlightuserdata(L, ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pop(L, 1);
    
    if (cm_wifiscan_start(wifi_scan_cb, NULL) == 0) {
        wifi_scan_in_progress = 1;
        lua_pushinteger(L, 0);
    } else {
        lua_pushlightuserdata(L, wifi_scan_callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        wifi_scan_callback_ud = NULL;
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 停止WIFI扫描
 * @api wifi.stop()
 * @return int 0成功, 其他失败
 * @usage
wifi.stop()
*/
static int iot_wifi_stop(lua_State* L) {
    LOG("WiFi scan stop");
    if (cm_wifiscan_stop() == 0) {
        if (wifi_scan_callback_ud) {
            lua_pushlightuserdata(L, wifi_scan_callback_ud);
            lua_pushnil(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            wifi_scan_callback_ud = NULL;
        }
        wifi_scan_in_progress = 0;
        lua_pushinteger(L, 0);
    } else {
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 查询上次WIFI扫描结果
 * @api wifi.query()
 * @return table 扫描结果
 * @usage
local result = wifi.query()
if result then
    for i, ap in ipairs(result) do
        print("AP", i, ap.bssid, ap.rssi, ap.channel)
    end
end
*/
static int iot_wifi_query(lua_State* L) {
    cm_wifi_scan_info_t* param = NULL;
    
    if (cm_wifiscan_query(&param) == 0 && param) {
        lua_newtable(L);
        for (int i = 0; i < param->bssid_number; i++) {
            lua_newtable(L);
            
            char bssid[20];
            snprintf(bssid, sizeof(bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
                     param->channel_cell_list[i].bssid[0],
                     param->channel_cell_list[i].bssid[1],
                     param->channel_cell_list[i].bssid[2],
                     param->channel_cell_list[i].bssid[3],
                     param->channel_cell_list[i].bssid[4],
                     param->channel_cell_list[i].bssid[5]);
            lua_pushstring(L, "bssid");
            lua_pushstring(L, bssid);
            lua_settable(L, -3);
            
            lua_pushstring(L, "rssi");
            lua_pushinteger(L, param->channel_cell_list[i].rssi);
            lua_settable(L, -3);
            
            lua_pushstring(L, "channel");
            lua_pushinteger(L, param->channel_cell_list[i].channel_number);
            lua_settable(L, -3);
            
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static const luaL_Reg wifi_lib[] = {
    { "config", iot_wifi_config },
    { "scan",   iot_wifi_scan },
    { "stop",   iot_wifi_stop },
    { "query",  iot_wifi_query },
    {NULL, NULL}
};

LUAMOD_API int luaopen_wifi(lua_State* L) {
    luaL_newlibtable(L, wifi_lib);
    luaL_setfuncs(L, wifi_lib, 0);
    return 1;
}
