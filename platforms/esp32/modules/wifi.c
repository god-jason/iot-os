/*
@module  wifi
@summary WiFi模块
@version 1.0
@date    2026.06.20
@tag     WIFI
*/

#include "module.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

static void* g_wifi_event_cb = NULL;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (!g_wifi_event_cb) return;
    
    const char* event = NULL;
    int data = 0;
    
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            event = "start";
            break;
        case WIFI_EVENT_STA_STOP:
            event = "stop";
            break;
        case WIFI_EVENT_STA_CONNECTED:
            event = "connected";
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            event = "disconnected";
            data = ((wifi_event_sta_disconnected_t*)event_data)->reason;
            break;
        case WIFI_EVENT_SCAN_DONE:
            event = "scan_done";
            break;
        default:
            return;
    }
}

static int iot_wifi_init(lua_State* L) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, NULL, NULL);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_wifi_deinit(lua_State* L) {
    esp_wifi_stop();
    esp_wifi_deinit();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_wifi_connect(lua_State* L) {
    const char* ssid = luaL_checkstring(L, 1);
    const char* password = luaL_checkstring(L, 2);
    
    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_wifi_disconnect(lua_State* L) {
    esp_wifi_disconnect();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_wifi_get_status(lua_State* L) {
    wifi_ap_record_t ap_info;
    esp_wifi_sta_get_ap_info(&ap_info);
    
    lua_newtable(L);
    lua_pushstring(L, (const char*)ap_info.ssid);
    lua_setfield(L, -2, "ssid");
    lua_pushinteger(L, ap_info.rssi);
    lua_setfield(L, -2, "rssi");
    lua_pushinteger(L, ap_info.authmode);
    lua_setfield(L, -2, "authmode");
    
    return 1;
}

static int iot_wifi_scan(lua_State* L) {
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
    };
    
    esp_wifi_scan_start(&scan_config, true);
    
    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    
    wifi_ap_record_t* ap_list = (wifi_ap_record_t*)iot_malloc(ap_count * sizeof(wifi_ap_record_t));
    if (!ap_list) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    
    esp_wifi_scan_get_ap_records(&ap_count, ap_list);
    
    lua_newtable(L);
    for (int i = 0; i < ap_count; i++) {
        lua_newtable(L);
        lua_pushstring(L, (const char*)ap_list[i].ssid);
        lua_setfield(L, -2, "ssid");
        lua_pushinteger(L, ap_list[i].rssi);
        lua_setfield(L, -2, "rssi");
        lua_pushinteger(L, ap_list[i].channel);
        lua_setfield(L, -2, "channel");
        lua_pushinteger(L, ap_list[i].authmode);
        lua_setfield(L, -2, "authmode");
        lua_rawseti(L, -2, i + 1);
    }
    
    iot_free(ap_list);
    
    return 1;
}

static int iot_wifi_get_ip(lua_State* L) {
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
    
    char ip_str[16];
    snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));
    
    lua_pushstring(L, ip_str);
    
    return 1;
}

static const luaL_Reg wifi_lib[] = {
    { "init",          iot_wifi_init },
    { "deinit",        iot_wifi_deinit },
    { "connect",       iot_wifi_connect },
    { "disconnect",    iot_wifi_disconnect },
    { "get_status",    iot_wifi_get_status },
    { "scan",          iot_wifi_scan },
    { "get_ip",        iot_wifi_get_ip },
    { NULL, NULL }
};

LUAMOD_API int luaopen_wifi(lua_State* L) {
    luaL_newlib(L, wifi_lib);
    return 1;
}
