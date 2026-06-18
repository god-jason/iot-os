/**
 * @file iot_mqtt.h
 * @brief MQTT协议
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_MQTT_H
#define IOT_MQTT_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_mqtt(lua_State* L);

#endif /* IOT_MQTT_H */
