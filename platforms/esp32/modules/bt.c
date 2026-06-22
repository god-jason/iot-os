/*
@module  bt
@summary 蓝牙BLE模块
@version 1.0
@date    2026.06.20
@tag     BLUETOOTH
*/

#include "module.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_log.h"

#define BT_SCAN_RESULT_MAX 32
#define BT_GATT_CLIENT_MAX 4

typedef struct {
    uint8_t addr[6];
    char name[32];
    int rssi;
    int connectable;
} bt_scan_result_t;

typedef struct {
    esp_gattc_cb_t cb;
    void* arg;
} bt_gattc_ctx_t;

static bt_scan_result_t g_scan_results[BT_SCAN_RESULT_MAX];
static int g_scan_result_count = 0;
static void* g_scan_cb = NULL;
static void* g_gap_cb = NULL;

static void bt_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT: {
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                if (g_scan_result_count < BT_SCAN_RESULT_MAX) {
                    bt_scan_result_t* result = &g_scan_results[g_scan_result_count++];
                    memcpy(result->addr, param->scan_rst.bda, 6);
                    
                    if (param->scan_rst.dev_name_len > 0) {
                        strncpy(result->name, (char*)param->scan_rst.dev_name, 
                                param->scan_rst.dev_name_len);
                        result->name[param->scan_rst.dev_name_len] = '\0';
                    } else {
                        result->name[0] = '\0';
                    }
                    result->rssi = param->scan_rst.rssi;
                    result->connectable = (param->scan_rst.ble_evt_type == 0);
                }
            }
            break;
        }
        case ESP_GAP_BLE_SCAN_DONE_EVT: {
            break;
        }
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: {
            break;
        }
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
            break;
        }
        default:
            break;
    }
}

static int luaopen_bt_init(lua_State* L) {
    esp_err_t ret;
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "controller init failed");
        return 2;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "controller enable failed");
        return 2;
    }
    
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "bluedroid init failed");
        return 2;
    }
    
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "bluedroid enable failed");
        return 2;
    }
    
    esp_ble_gap_register_callback(bt_gap_event_handler);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_deinit(lua_State* L) {
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_set_device_name(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    
    esp_err_t ret = esp_ble_gap_set_device_name(name);
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "set name failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_get_device_name(lua_State* L) {
    char name[32];
    esp_ble_gap_get_device_name(name, sizeof(name));
    lua_pushstring(L, name);
    return 1;
}

static int luaopen_bt_get_mac(lua_State* L) {
    uint8_t* mac = esp_bt_dev_get_address();
    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    lua_pushstring(L, mac_str);
    return 1;
}

static int luaopen_bt_start_advertising(lua_State* L) {
    const char* name = luaL_optstring(L, 1, NULL);
    int interval_min = luaL_optinteger(L, 2, 0x20);
    int interval_max = luaL_optinteger(L, 3, 0x40);
    
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = interval_min,
        .adv_int_max = interval_max,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    
    if (name) {
        uint8_t adv_data[31] = {0};
        adv_data[0] = strlen(name) + 1;
        adv_data[1] = 0x09;
        strcpy((char*)adv_data + 2, name);
        esp_ble_gap_config_adv_data(adv_data, sizeof(adv_data));
    }
    
    esp_err_t ret = esp_ble_gap_start_advertising(&adv_params);
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "start advertising failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_stop_advertising(lua_State* L) {
    esp_err_t ret = esp_ble_gap_stop_advertising();
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "stop advertising failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_start_scan(lua_State* L) {
    int duration = luaL_optinteger(L, 1, 10);
    int interval = luaL_optinteger(L, 2, 0x50);
    int window = luaL_optinteger(L, 3, 0x30);
    
    g_scan_result_count = 0;
    
    esp_ble_scan_params_t scan_params = {
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = interval,
        .scan_window = window,
        .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    };
    
    esp_err_t ret = esp_ble_gap_set_scan_params(&scan_params);
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "set scan params failed");
        return 2;
    }
    
    ret = esp_ble_gap_start_scanning(duration);
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "start scanning failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_stop_scan(lua_State* L) {
    esp_err_t ret = esp_ble_gap_stop_scanning();
    if (ret != ESP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "stop scanning failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_bt_get_scan_results(lua_State* L) {
    lua_newtable(L);
    
    for (int i = 0; i < g_scan_result_count; i++) {
        bt_scan_result_t* result = &g_scan_results[i];
        
        lua_newtable(L);
        
        char mac_str[18];
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                 result->addr[0], result->addr[1], result->addr[2],
                 result->addr[3], result->addr[4], result->addr[5]);
        lua_pushstring(L, mac_str);
        lua_setfield(L, -2, "mac");
        
        lua_pushstring(L, result->name);
        lua_setfield(L, -2, "name");
        
        lua_pushinteger(L, result->rssi);
        lua_setfield(L, -2, "rssi");
        
        lua_pushboolean(L, result->connectable);
        lua_setfield(L, -2, "connectable");
        
        lua_rawseti(L, -2, i + 1);
    }
    
    return 1;
}

static int luaopen_bt_set_scan_callback(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    return 0;
}

static int luaopen_bt_get_adapter_state(lua_State* L) {
    bt_state_t state = esp_bluedroid_get_status();
    
    const char* state_str = "unknown";
    switch (state) {
        case BT_OFF: state_str = "off"; break;
        case BT_ON: state_str = "on"; break;
        case BT_TURNING_OFF: state_str = "turning_off"; break;
        case BT_TURNING_ON: state_str = "turning_on"; break;
    }
    
    lua_pushstring(L, state_str);
    return 1;
}

static const luaL_Reg luaopen_bt_lib[] = {
    { "init",               luaopen_bt_init },
    { "deinit",             luaopen_bt_deinit },
    { "set_device_name",    luaopen_bt_set_device_name },
    { "get_device_name",    luaopen_bt_get_device_name },
    { "get_mac",            luaopen_bt_get_mac },
    { "start_advertising",  luaopen_bt_start_advertising },
    { "stop_advertising",   luaopen_bt_stop_advertising },
    { "start_scan",         luaopen_bt_start_scan },
    { "stop_scan",          luaopen_bt_stop_scan },
    { "get_scan_results",   luaopen_bt_get_scan_results },
    { "get_adapter_state",  luaopen_bt_get_adapter_state },
    { NULL, NULL }
};

LUAMOD_API int luaopen_bt_register(lua_State* L) {
    luaL_newlib(L, luaopen_bt_lib);
    
    lua_pushinteger(L, ADV_TYPE_IND);
    lua_setfield(L, -2, "ADV_TYPE_IND");
    lua_pushinteger(L, ADV_TYPE_DIRECT_IND_HIGH);
    lua_setfield(L, -2, "ADV_TYPE_DIRECT_IND");
    lua_pushinteger(L, ADV_TYPE_SCAN_IND);
    lua_setfield(L, -2, "ADV_TYPE_SCAN_IND");
    lua_pushinteger(L, ADV_TYPE_NONCONN_IND);
    lua_setfield(L, -2, "ADV_TYPE_NONCONN_IND");
    
    lua_pushinteger(L, BLE_ADDR_TYPE_PUBLIC);
    lua_setfield(L, -2, "ADDR_PUBLIC");
    lua_pushinteger(L, BLE_ADDR_TYPE_RANDOM);
    lua_setfield(L, -2, "ADDR_RANDOM");
    
    return 1;
}
