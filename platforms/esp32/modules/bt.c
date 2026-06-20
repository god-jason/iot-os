/*
@module  bt
@summary 蓝牙模块
@version 1.0
@date    2026.06.20
@tag     BLUETOOTH
*/

#include "module.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

static int iot_bt_init(lua_State* L) {
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    
    esp_bluedroid_init();
    esp_bluedroid_enable();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_deinit(lua_State* L) {
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_set_device_name(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    
    esp_ble_gap_set_device_name(name);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_start_advertising(lua_State* L) {
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x20,
        .adv_int_max = 0x40,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    
    esp_ble_gap_start_advertising(&adv_params);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_stop_advertising(lua_State* L) {
    esp_ble_gap_stop_advertising();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_start_scan(lua_State* L) {
    esp_ble_scan_params_t scan_params = {
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = 0x50,
        .scan_window = 0x30,
        .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    };
    
    esp_ble_gap_start_scanning(0);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_bt_stop_scan(lua_State* L) {
    esp_ble_gap_stop_scanning();
    
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg bt_lib[] = {
    { "init",               iot_bt_init },
    { "deinit",             iot_bt_deinit },
    { "set_device_name",    iot_bt_set_device_name },
    { "start_advertising",  iot_bt_start_advertising },
    { "stop_advertising",   iot_bt_stop_advertising },
    { "start_scan",         iot_bt_start_scan },
    { "stop_scan",          iot_bt_stop_scan },
    { NULL, NULL }
};

LUAMOD_API int luaopen_bt(lua_State* L) {
    luaL_newlib(L, bt_lib);
    return 1;
}
