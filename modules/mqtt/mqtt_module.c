#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "iot.h"
#include "iot_params.h"
#include "iot_rtos.h"
#include "iot_callback.h"

#include "mqtt_client.h"
#include "mqtt_packets.h"

typedef struct mqtt_route_entry {
    char* topic_filter;
    void* callback_ud;
    struct mqtt_route_entry* next;
} mqtt_route_entry_t;

typedef struct {
    mqtt_client_t* client;
    void* connect_callback_ud;
    void* close_callback_ud;
    mqtt_route_entry_t* routes;
    char* client_id;
    int inited;
} mqtt_lua_ctx_t;

#define MQTT_CTX_METATABLE "mqtt.client"

static mqtt_lua_ctx_t* mqtt_get_ctx(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        return NULL;
    }
    
    mqtt_lua_ctx_t** ctx_ptr = (mqtt_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        return NULL;
    }
    return *ctx_ptr;
}

static mqtt_route_entry_t* mqtt_route_find(mqtt_lua_ctx_t* ctx, const char* topic_filter) {
    mqtt_route_entry_t* entry = ctx->routes;
    while (entry) {
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

static mqtt_route_entry_t* mqtt_route_add(mqtt_lua_ctx_t* ctx, const char* topic_filter, void* callback_ud) {
    mqtt_route_entry_t* entry = (mqtt_route_entry_t*)iot_malloc(sizeof(mqtt_route_entry_t));
    if (!entry) return NULL;
    
    entry->topic_filter = iot_strdup(topic_filter);
    if (!entry->topic_filter) {
        iot_free(entry);
        return NULL;
    }
    
    entry->callback_ud = callback_ud;
    entry->next = ctx->routes;
    ctx->routes = entry;
    
    return entry;
}

static void mqtt_route_remove(mqtt_lua_ctx_t* ctx, const char* topic_filter) {
    mqtt_route_entry_t** pp = &ctx->routes;
    while (*pp) {
        mqtt_route_entry_t* entry = *pp;
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            *pp = entry->next;
            iot_free(entry->topic_filter);
            iot_free(entry);
            return;
        }
        pp = &entry->next;
    }
}

static void mqtt_route_destroy_all(mqtt_lua_ctx_t* ctx) {
    mqtt_route_entry_t* entry = ctx->routes;
    while (entry) {
        mqtt_route_entry_t* next = entry->next;
        iot_free(entry->topic_filter);
        iot_free(entry);
        entry = next;
    }
    ctx->routes = NULL;
}

static mqtt_lua_ctx_t* mqtt_ctx_create(void) {
    mqtt_lua_ctx_t* ctx = (mqtt_lua_ctx_t*)iot_malloc(sizeof(mqtt_lua_ctx_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(mqtt_lua_ctx_t));
    ctx->client = mqtt_client_create();
    if (!ctx->client) {
        iot_free(ctx);
        return NULL;
    }
    ctx->inited = 1;
    
    return ctx;
}

static void mqtt_ctx_destroy(mqtt_lua_ctx_t* ctx) {
    if (!ctx) return;
    
    mqtt_route_destroy_all(ctx);
    
    if (ctx->client_id) {
        iot_free(ctx->client_id);
    }
    
    if (ctx->client) {
        mqtt_client_destroy(ctx->client);
    }
    
    iot_free(ctx);
}

static void mqtt_event_callback(mqtt_client_t* client, mqtt_event_type_t event, void* user_data) {
    mqtt_lua_ctx_t* ctx = (mqtt_lua_ctx_t*)user_data;
    if (!ctx) return;
    
    params_t* params = params_create(1);
    if (!params) return;
    
    if (event == MQTT_EVENT_CONNECTED) {
        params_push_int(params, 1);
        if (ctx->connect_callback_ud) {
            iot_rtos_call(ctx->connect_callback_ud, params);
            params = NULL;
        }
    } else if (event == MQTT_EVENT_DISCONNECTED || event == MQTT_EVENT_ERROR) {
        params_push_int(params, 0);
        if (ctx->close_callback_ud) {
            iot_rtos_call(ctx->close_callback_ud, params);
            params = NULL;
        }
    }

    if (params) {
        params_destroy(params);
    }
}

static void mqtt_message_callback(const char* topic, const uint8_t* payload,
                                  size_t payload_len, mqtt_qos_t qos, bool retain, void* user_data) {
    mqtt_lua_ctx_t* ctx = (mqtt_lua_ctx_t*)user_data;
    if (!ctx) return;
    
    mqtt_route_entry_t* entry = ctx->routes;
    while (entry) {
        if (mqtt_topic_match(entry->topic_filter, topic)) {
            params_t* params = params_create(4);
            if (!params) continue;
            
            params_push_string(params, topic, 0);
            
            if (payload && payload_len > 0) {
                params_push_string(params, (const char*)payload, payload_len);
            } else {
                params_push_string(params, "", 0);
            }
            
            params_push_int(params, qos);
            params_push_int(params, retain ? 1 : 0);
            
            if (entry->callback_ud) {
                iot_rtos_call(entry->callback_ud, params);
            } else {
                params_destroy(params);
            }
        }
        entry = entry->next;
    }
}

static int luaopen_mqtt_new(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create mqtt client");
        return 2;
    }
    
    mqtt_lua_ctx_t** ctx_ptr = (mqtt_lua_ctx_t**)lua_newuserdata(L, sizeof(mqtt_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, MQTT_CTX_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

static int luaopen_mqtt_connect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    luaL_checktype(L, 2, LUA_TTABLE);
    
    mqtt_connect_options_t options = {0};
    
    lua_getfield(L, 2, "host");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.host = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    if (!options.host) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "host is required");
        return 2;
    }
    
    lua_getfield(L, 2, "port");
    options.port = (uint16_t)luaL_optinteger(L, -1, 1883);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "client_id");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.client_id = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    if (!options.client_id) {
        char default_client_id[64];
        snprintf(default_client_id, sizeof(default_client_id), "mqtt_%u", (unsigned int)iot_get_tick_ms());
        ctx->client_id = iot_strdup(default_client_id);
        options.client_id = ctx->client_id;
    }
    
    lua_getfield(L, 2, "username");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.username = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "password");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.password = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "keepalive");
    options.keepalive = (int)luaL_optinteger(L, -1, 60);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "clean_session");
    options.clean_session = luaL_optboolean(L, -1, 1);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "will_topic");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.will_topic = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "will_message");
    if (lua_type(L, -1) == LUA_TSTRING) {
        options.will_message = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "will_qos");
    options.will_qos = (mqtt_qos_t)luaL_optinteger(L, -1, MQTT_QOS_0);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "will_retain");
    options.will_retain = luaL_optboolean(L, -1, 0);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "timeout_ms");
    options.timeout_ms = (int)luaL_optinteger(L, -1, 30000);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "use_ssl");
    options.use_ssl = luaL_optboolean(L, -1, 0);
    lua_pop(L, 1);
    
    mqtt_client_set_event_callback(ctx->client, mqtt_event_callback, ctx);
    
    int ret = mqtt_client_connect(ctx->client, &options);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "connect failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_disconnect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    int ret = mqtt_client_disconnect(ctx->client);
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

static int luaopen_mqtt_close(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (ctx) {
        mqtt_ctx_destroy(ctx);
    }
    return 0;
}

static int luaopen_mqtt_publish(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    const char* topic = luaL_checkstring(L, 2);
    
    size_t payload_len = 0;
    const char* payload = luaL_optlstring(L, 3, "", &payload_len);
    
    mqtt_qos_t qos = (mqtt_qos_t)luaL_optinteger(L, 4, MQTT_QOS_0);
    bool retain = luaL_optboolean(L, 5, false);
    
    int ret = mqtt_client_publish(ctx->client, topic, (const uint8_t*)payload, payload_len, qos, retain);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "publish failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_subscribe(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    const char* topic_filter = luaL_checkstring(L, 2);
    mqtt_qos_t qos = (mqtt_qos_t)luaL_optinteger(L, 3, MQTT_QOS_0);
    
    if (lua_type(L, 4) != LUA_TFUNCTION) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }
    
    void* callback_ud = iot_callback_save(L, 4);
    if (!callback_ud) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to save callback");
        return 2;
    }
    
    mqtt_route_entry_t* existing = mqtt_route_find(ctx, topic_filter);
    if (existing) {
        existing->callback_ud = callback_ud;
    } else {
        if (!mqtt_route_add(ctx, topic_filter, callback_ud)) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "failed to add route");
            return 2;
        }
    }
    
    int ret = mqtt_client_subscribe(ctx->client, topic_filter, qos, mqtt_message_callback, ctx);
    if (ret != 0) {
        mqtt_route_remove(ctx, topic_filter);
        lua_pushboolean(L, 0);
        lua_pushstring(L, "subscribe failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_unsubscribe(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    const char* topic_filter = luaL_checkstring(L, 2);
    
    int ret = mqtt_client_unsubscribe(ctx->client, topic_filter);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unsubscribe failed");
        return 2;
    }
    
    mqtt_route_remove(ctx, topic_filter);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_on(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    const char* event = luaL_checkstring(L, 2);
    
    if (lua_type(L, 3) != LUA_TFUNCTION) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }
    
    if (strcmp(event, "connect") == 0) {
        ctx->connect_callback_ud = iot_callback_save(L, 3);
        mqtt_client_set_event_callback(ctx->client, mqtt_event_callback, ctx);
    } else if (strcmp(event, "close") == 0) {
        ctx->close_callback_ud = iot_callback_save(L, 3);
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unsupported event");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_state(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushinteger(L, MQTT_STATE_DISCONNECTED);
        return 1;
    }
    
    mqtt_state_t state = mqtt_client_get_state(ctx->client);
    lua_pushinteger(L, state);
    return 1;
}

static int luaopen_mqtt_enable_auto_reconnect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    int interval_ms = (int)luaL_optinteger(L, 2, 5000);
    
    mqtt_client_enable_auto_reconnect(ctx->client, interval_ms);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_disable_auto_reconnect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    mqtt_client_disable_auto_reconnect(ctx->client);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mqtt_is_connected(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_pushboolean(L, mqtt_client_is_connected(ctx->client));
    return 1;
}

static int luaopen_mqtt_tostring(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx) {
        lua_pushstring(L, "mqtt.client (invalid)");
    } else {
        lua_pushstring(L, "mqtt.client");
    }
    return 1;
}

static const luaL_Reg mqtt_client_methods[] = {
    { "connect",               luaopen_mqtt_connect },
    { "disconnect",            luaopen_mqtt_disconnect },
    { "close",                 luaopen_mqtt_close },
    { "publish",               luaopen_mqtt_publish },
    { "subscribe",             luaopen_mqtt_subscribe },
    { "unsubscribe",           luaopen_mqtt_unsubscribe },
    { "on",                    luaopen_mqtt_on },
    { "state",                 luaopen_mqtt_state },
    { "enable_auto_reconnect", luaopen_mqtt_enable_auto_reconnect },
    { "disable_auto_reconnect",luaopen_mqtt_disable_auto_reconnect },
    { "is_connected",          luaopen_mqtt_is_connected },
    { "__gc",                  luaopen_mqtt_close },
    { "__tostring",            luaopen_mqtt_tostring },
    { NULL,                    NULL }
};

static const luaL_Reg mqtt_module_methods[] = {
    { "new",                      luaopen_mqtt_new },
    { NULL,                       NULL }
};

LUAMOD_API int luaopen_mqtt_register(lua_State* L) {
    static int s_mqtt_manager_started = 0;
    if (!s_mqtt_manager_started) {
        if (mqtt_manager_start() == 0) {
            s_mqtt_manager_started = 1;
        }
    }

    luaL_newlib(L, mqtt_module_methods);
    
    /* 注册常量 - MQTT QoS 级别 */
    lua_pushinteger(L, MQTT_QOS_0); lua_setfield(L, -2, "QOS_0");
    lua_pushinteger(L, MQTT_QOS_1); lua_setfield(L, -2, "QOS_1");
    lua_pushinteger(L, MQTT_QOS_2); lua_setfield(L, -2, "QOS_2");
    
    /* 注册常量 - MQTT 连接状态 */
    lua_pushinteger(L, MQTT_STATE_DISCONNECTED); lua_setfield(L, -2, "STATE_DISCONNECTED");
    lua_pushinteger(L, MQTT_STATE_CONNECTING);   lua_setfield(L, -2, "STATE_CONNECTING");
    lua_pushinteger(L, MQTT_STATE_CONNECTED);     lua_setfield(L, -2, "STATE_CONNECTED");
    lua_pushinteger(L, MQTT_STATE_ERROR);         lua_setfield(L, -2, "STATE_ERROR");
    
    /* 注册常量 - MQTT 错误码 */
    lua_pushinteger(L, MQTT_ERR_SUCCESS);   lua_setfield(L, -2, "ERR_SUCCESS");
    lua_pushinteger(L, MQTT_ERR_CONNECT);   lua_setfield(L, -2, "ERR_CONNECT");
    lua_pushinteger(L, MQTT_ERR_SEND);      lua_setfield(L, -2, "ERR_SEND");
    lua_pushinteger(L, MQTT_ERR_RECV);      lua_setfield(L, -2, "ERR_RECV");
    lua_pushinteger(L, MQTT_ERR_TIMEOUT);   lua_setfield(L, -2, "ERR_TIMEOUT");
    lua_pushinteger(L, MQTT_ERR_PROTOCOL);  lua_setfield(L, -2, "ERR_PROTOCOL");
    lua_pushinteger(L, MQTT_ERR_QOS);       lua_setfield(L, -2, "ERR_QOS");
    lua_pushinteger(L, MQTT_ERR_KEEPALIVE); lua_setfield(L, -2, "ERR_KEEPALIVE");
    lua_pushinteger(L, MQTT_ERR_MEMORY);    lua_setfield(L, -2, "ERR_MEMORY");
    lua_pushinteger(L, MQTT_ERR_PARAM);     lua_setfield(L, -2, "ERR_PARAM");
    
    /* 注册常量 - MQTT CONNACK 码 */
    lua_pushinteger(L, MQTT_CONNACK_ACCEPTED);            lua_setfield(L, -2, "CONNACK_ACCEPTED");
    lua_pushinteger(L, MQTT_CONNACK_UNACCEPTABLE_PROTO);  lua_setfield(L, -2, "CONNACK_UNACCEPTABLE_PROTO");
    lua_pushinteger(L, MQTT_CONNACK_IDENTIFIER_REJECTED); lua_setfield(L, -2, "CONNACK_IDENTIFIER_REJECTED");
    lua_pushinteger(L, MQTT_CONNACK_SERVER_UNAVAILABLE);  lua_setfield(L, -2, "CONNACK_SERVER_UNAVAILABLE");
    lua_pushinteger(L, MQTT_CONNACK_BAD_AUTH);            lua_setfield(L, -2, "CONNACK_BAD_AUTH");
    lua_pushinteger(L, MQTT_CONNACK_NOT_AUTHORIZED);      lua_setfield(L, -2, "CONNACK_NOT_AUTHORIZED");
    
    luaL_newmetatable(L, MQTT_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, mqtt_client_methods, 0);
    lua_pop(L, 1);

    return 1;
}