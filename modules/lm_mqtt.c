/*
@module  mqtt
@summary MQTT客户端
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     MQTT
*/

/*
MQTT参考示例
-- 创建MQTT客户端
local mqtt_client = mqtt.create(nil)

-- 设置回调
mqtt_client:on_connect(function(client, result)
    print("connect result:", result)
    if result == 0 then
        -- 连接成功,订阅主题
        client:subscribe({"topic1", "topic2"}, {0, 1})
    end
end)

-- 设置消息回调
mqtt_client:on_message(function(client, topic, qos, payload)
    print("message:", topic, qos, payload)
end)

-- 连接服务器
mqtt_client:connect({
    host = "mqtt.example.com",
    port = 1883,
    client_id = "my_client",
    username = "user",
    password = "pass",
    keepalive = 60,
    clean_session = 1
})

-- 发布消息
mqtt_client:publish("topic1", "hello", 0, 0)

-- 断开连接
mqtt_client:disconnect()

-- 销毁客户端
mqtt_client:destroy()
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_mqtt.h"
#include "iot_callback.h"

#define MQTT_CTX_MAX 5

/* MQTT上下文 */
typedef struct {
    cm_mqtt_client_t* client;
    void* callback_ud[10];  /* 回调函数userdata */
    int inited;
} mqtt_ctx_t;

/* MQTT全局上下文 */
static mqtt_ctx_t g_mqtt_ctx[MQTT_CTX_MAX];
static int g_mqtt_inited = 0;

/* 回调索引 */
enum {
    MQTT_CB_CONNACK = 0,
    MQTT_CB_PUBLISH,
    MQTT_CB_PUBACK,
    MQTT_CB_PUBREC,
    MQTT_CB_PUBREL,
    MQTT_CB_PUBCOMP,
    MQTT_CB_SUBACK,
    MQTT_CB_UNSUBACK,
    MQTT_CB_PINGRESP,
    MQTT_CB_TIMEOUT,
    MQTT_CB_MAX
};

/* 初始化MQTT上下文 */
static void mqtt_context_init(void) {
    if (!g_mqtt_inited) {
        memset(g_mqtt_ctx, 0, sizeof(g_mqtt_ctx));
        for (int i = 0; i < MQTT_CTX_MAX; i++) {
            for (int j = 0; j < MQTT_CB_MAX; j++) {
                g_mqtt_ctx[i].callback_ud[j] = NULL;
            }
        }
        g_mqtt_inited = 1;
    }
}

/* 查找空闲的MQTT上下文 */
static int mqtt_find_free_ctx(void) {
    for (int i = 0; i < MQTT_CTX_MAX; i++) {
        if (!g_mqtt_ctx[i].inited) {
            return i;
        }
    }
    return -1;
}

/* 根据 client 查找上下文索引 */
static int mqtt_find_ctx_by_client(cm_mqtt_client_t* client) {
    for (int i = 0; i < MQTT_CTX_MAX; i++) {
        if (g_mqtt_ctx[i].client == client) {
            return i;
        }
    }
    return -1;
}

/* MQTT回调函数 */
static int mqtt_connack_cb(cm_mqtt_client_t* client, int session, cm_mqtt_conn_state_e conn_res) {
    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) return -1;

    void* cb_ud = g_mqtt_ctx[slot].callback_ud[MQTT_CB_CONNACK];
    if (!cb_ud) return 0;

    params_t* params = params_create(2);
    if (!params) return 0;

    params_push_ptr(params, client);
    params_push_int(params, conn_res);
    iot_rtos_call(cb_ud, params);

    return 0;
}

static int mqtt_publish_cb(cm_mqtt_client_t* client, unsigned short msgid, char* topic, int total_len, int payload_len, char* payload) {
    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) return -1;

    void* cb_ud = g_mqtt_ctx[slot].callback_ud[MQTT_CB_PUBLISH];
    if (!cb_ud) return 0;

    params_t* params = params_create(4);
    if (!params) return 0;

    params_push_ptr(params, client);
    params_push_string(params, topic, topic ? strlen(topic) : 0);
    params_push_int(params, msgid);
    params_push_string(params, payload, payload_len > 0 ? (size_t)payload_len : 0);
    iot_rtos_call(cb_ud, params);

    return 0;
}

static int mqtt_suback_cb(cm_mqtt_client_t* client, unsigned short msgid, int count, int qos[]) {
    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) return -1;

    void* cb_ud = g_mqtt_ctx[slot].callback_ud[MQTT_CB_SUBACK];
    if (!cb_ud) return 0;

    int qos_count = count < 0 ? 0 : (count > 10 ? 10 : count);
    params_t* params = params_create(3 + qos_count);
    if (!params) return 0;

    params_push_ptr(params, client);
    params_push_int(params, msgid);
    params_push_int(params, qos_count);
    for (int i = 0; i < qos_count; i++) {
        params_push_int(params, qos[i]);
    }
    
    iot_rtos_call(cb_ud, params);

    return 0;
}

static int mqtt_unsuback_cb(cm_mqtt_client_t* client, unsigned short msgid) {
    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) return -1;

    void* cb_ud = g_mqtt_ctx[slot].callback_ud[MQTT_CB_UNSUBACK];
    if (!cb_ud) return 0;

    params_t* params = params_create(2);
    if (!params) return 0;

    params_push_ptr(params, client);
    params_push_int(params, msgid);
    iot_rtos_call(cb_ud, params);

    return 0;
}

static int mqtt_puback_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup) {
    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) return -1;

    void* cb_ud = g_mqtt_ctx[slot].callback_ud[MQTT_CB_PUBACK];
    if (!cb_ud) return 0;

    params_t* params = params_create(2);
    if (!params) return 0;

    params_push_ptr(params, client);
    params_push_int(params, msgid);
    iot_rtos_call(cb_ud, params);

    return 0;
}

/**
 * @brief 创建MQTT客户端
 * @api mqtt.create(url)
 * @string url broker地址(可空，如nil则使用后续connect参数)
 * @return userdata MQTT客户端句柄
 * @usage
local client = mqtt.create()
*/
static int iot_mqtt_create(lua_State* L) {
    mqtt_context_init();

    int slot = mqtt_find_free_ctx();
    if (slot < 0) {
        LOG("ERR no free slot");
        lua_pushnil(L);
        return 1;
    }

    cm_mqtt_client_t* client = cm_mqtt_client_create();
    if (!client) {
        LOG("ERR create failed");
        lua_pushnil(L);
        return 1;
    }

    g_mqtt_ctx[slot].client = client;
    g_mqtt_ctx[slot].inited = 1;

    LOG("OK slot=%d", slot);
    lua_pushlightuserdata(L, client);
    /* 设置元表，使客户端对象可以调用方法 */
    luaL_getmetatable(L, "mqtt_client");
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 设置回调函数
 * @api mqtt_client:on(event, callback)
 * @string event 事件类型 "connect", "message", "suback", "unsuback", "puback"
 * @func callback 回调函数
 * @return bool 成功返回true,失败返回false
 * @usage
client:on("connect", function(c, result) end)
client:on("message", function(c, topic, qos, payload) end)
*/
static int iot_mqtt_on(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* event = luaL_checkstring(L, 2);
    if (!lua_isfunction(L, 3)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int slot = mqtt_find_ctx_by_client(client);
    if (slot < 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int cb_index = -1;
    if (strcmp(event, "connect") == 0) {
        cb_index = MQTT_CB_CONNACK;
    } else if (strcmp(event, "message") == 0) {
        cb_index = MQTT_CB_PUBLISH;
    } else if (strcmp(event, "suback") == 0) {
        cb_index = MQTT_CB_SUBACK;
    } else if (strcmp(event, "unsuback") == 0) {
        cb_index = MQTT_CB_UNSUBACK;
    } else if (strcmp(event, "puback") == 0) {
        cb_index = MQTT_CB_PUBACK;
    }

    if (cb_index >= 0 && cb_index < MQTT_CB_MAX) {
        /* 释放旧的回调 */
        if (g_mqtt_ctx[slot].callback_ud[cb_index]) {
            iot_callback_free(g_mqtt_ctx[slot].callback_ud[cb_index]);
            g_mqtt_ctx[slot].callback_ud[cb_index] = NULL;
        }
        /* 保存新的回调 */
        g_mqtt_ctx[slot].callback_ud[cb_index] = iot_callback_save(L, 3);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

/**
 * @brief 连接MQTT服务器
 * @api mqtt_client:connect(options)
 * @table options 连接参数 {host, port, client_id, username, password, keepalive, clean_session}
 * @return int 0成功,其他失败
 * @usage
client:connect({
    host = "mqtt.example.com",
    port = 1883,
    client_id = "my_client",
    username = "user",
    password = "pass",
    keepalive = 60,
    clean_session = 1
})
*/
static int iot_mqtt_connect(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    luaL_checktable(L, 2);

    cm_mqtt_connect_options_t options;
    memset(&options, 0, sizeof(options));

    lua_getfield(L, 2, "host");
    options.hostname = lua_isnil(L, -1) ? NULL : lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "port");
    options.hostport = lua_isnil(L, -1) ? 1883 : lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "client_id");
    options.clientid = lua_isnil(L, -1) ? "" : lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "username");
    options.username = lua_isnil(L, -1) ? NULL : lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "password");
    options.password = lua_isnil(L, -1) ? NULL : lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "keepalive");
    options.keepalive = lua_isnil(L, -1) ? 60 : lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 2, "clean_session");
    options.clean_session = lua_isnil(L, -1) ? 1 : lua_tointeger(L, -1);
    lua_pop(L, 1);

    LOG("connect host=%s port=%d client_id=%s", options.hostname, options.hostport, options.clientid);

    /* 设置回调 */
    cm_mqtt_client_cb_t callbacks = {0};
    callbacks.connack_cb = mqtt_connack_cb;
    callbacks.publish_cb = mqtt_publish_cb;
    callbacks.suback_cb = mqtt_suback_cb;
    callbacks.unsuback_cb = mqtt_unsuback_cb;
    callbacks.puback_cb = mqtt_puback_cb;

    cm_mqtt_client_set_opt(client, CM_MQTT_OPT_EVENT_CONNACK_CB, &callbacks.connack_cb);
    cm_mqtt_client_set_opt(client, CM_MQTT_OPT_EVENT_PUBLISH_CB, &callbacks.publish_cb);
    cm_mqtt_client_set_opt(client, CM_MQTT_OPT_EVENT_SUBACK_CB, &callbacks.suback_cb);
    cm_mqtt_client_set_opt(client, CM_MQTT_OPT_EVENT_UNSUBACK_CB, &callbacks.unsuback_cb);
    cm_mqtt_client_set_opt(client, CM_MQTT_OPT_EVENT_PUBACK_CB, &callbacks.puback_cb);

    int ret = cm_mqtt_client_connect(client, &options);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 订阅主题
 * @api mqtt_client:subscribe(topics, qos)
 * @table topics 主题列表
 * @table qos QoS级别列表
 * @return int 0成功,其他失败
 * @usage
client:subscribe({"topic1", "topic2"}, {0, 1})
*/
static int iot_mqtt_subscribe(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    luaL_checktable(L, 2);
    luaL_checktable(L, 3);

    int count = lua_rawlen(L, 2);
    if (count == 0 || count > 10) {
        lua_pushinteger(L, -1);
        return 1;
    }

    const char* topics[10];
    char qos[10];

    for (int i = 0; i < count; i++) {
        lua_rawgeti(L, 2, i + 1);
        topics[i] = lua_tostring(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, i + 1);
        qos[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    LOG("subscribe count=%d", count);

    int ret = cm_mqtt_client_subscribe(client, topics, qos, count);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 取消订阅主题
 * @api mqtt_client:unsubscribe(topics)
 * @table topics 主题列表
 * @return int 0成功,其他失败
 * @usage
client:unsubscribe({"topic1", "topic2"})
*/
static int iot_mqtt_unsubscribe(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    luaL_checktable(L, 2);

    int count = lua_rawlen(L, 2);
    if (count == 0 || count > 10) {
        lua_pushinteger(L, -1);
        return 1;
    }

    const char* topics[10];

    for (int i = 0; i < count; i++) {
        lua_rawgeti(L, 2, i + 1);
        topics[i] = lua_tostring(L, -1);
        lua_pop(L, 1);
    }

    int ret = cm_mqtt_client_unsubscribe(client, topics, count);
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 发布消息
 * @api mqtt_client:publish(topic, payload, qos, retain)
 * @string topic 主题
 * @string payload 消息内容
 * @int qos QoS级别(0,1,2)
 * @int retain retain标志(0,1)
 * @return int 0成功,其他失败
 * @usage
client:publish("topic1", "hello", 0, 0)
*/
static int iot_mqtt_publish(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    const char* topic = luaL_checkstring(L, 2);
    size_t payload_len = 0;
    const char* payload = luaL_checklstring(L, 3, &payload_len);
    int qos = luaL_optinteger(L, 4, 0);
    int retain = luaL_optinteger(L, 5, 0);

    LOG("publish topic=%s len=%u qos=%d", topic, payload_len, qos);

    char publish_flags = (qos << 1) | retain;
    int ret = cm_mqtt_client_publish(client, topic, payload, payload_len, publish_flags);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }

    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 断开连接
 * @api mqtt_client:disconnect()
 * @return int 0成功,其他失败
 * @usage
client:disconnect()
*/
static int iot_mqtt_disconnect(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    LOG("disconnect");
    int ret = cm_mqtt_client_disconnect(client);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 销毁MQTT客户端
 * @api mqtt_client:destroy()
 * @return int 0成功,其他失败
 * @usage
client:destroy()
*/
static int iot_mqtt_destroy(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    LOG("destroy");
    int slot = mqtt_find_ctx_by_client(client);
    if (slot >= 0) {
        /* 释放所有回调 */
        for (int i = 0; i < MQTT_CB_MAX; i++) {
            if (g_mqtt_ctx[slot].callback_ud[i]) {
                iot_callback_free(g_mqtt_ctx[slot].callback_ud[i]);
                g_mqtt_ctx[slot].callback_ud[i] = NULL;
            }
        }
        g_mqtt_ctx[slot].inited = 0;
    }

    int ret = cm_mqtt_client_destroy(client);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 获取客户端状态
 * @api mqtt_client:state()
 * @return int 状态码 0已断开,1已连接,2连接中,3重连中
 * @usage
local state = client:state()
*/
static int iot_mqtt_state(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushinteger(L, -1);
        return 1;
    }

    int state = cm_mqtt_client_get_state(client);
    lua_pushinteger(L, state);
    return 1;
}

/**
 * @brief 设置客户端选项
 * @api mqtt_client:setopt(option, value)
 * @string option 选项名称
 * @value value 选项值
 * @return bool 成功返回true,失败返回false
 * @usage
client:setopt("ping_cycle", 60)
client:setopt("ssl_enable", 1)
*/
static int iot_mqtt_setopt(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (!client) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* option = luaL_checkstring(L, 2);

    cm_mqtt_option_e opt = CM_MQTT_OPT_START;
    if (strcmp(option, "ssl_enable") == 0) {
        opt = CM_MQTT_OPT_SSL_ENABLE;
    } else if (strcmp(option, "ssl_id") == 0) {
        opt = CM_MQTT_OPT_SSL_ID;
    } else if (strcmp(option, "ping_cycle") == 0) {
        opt = CM_MQTT_OPT_PING_CYCLE;
    } else if (strcmp(option, "pkt_timeout") == 0) {
        opt = CM_MQTT_OPT_PKT_TIMEOUT;
    } else if (strcmp(option, "reconn_mode") == 0) {
        opt = CM_MQTT_OPT_RECONN_MODE;
    } else if (strcmp(option, "reconn_times") == 0) {
        opt = CM_MQTT_OPT_RECONN_TIMES;
    } else if (strcmp(option, "reconn_cycle") == 0) {
        opt = CM_MQTT_OPT_RECONN_CYCLE;
    }

    if (opt == CM_MQTT_OPT_START) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int value = luaL_checkinteger(L, 3);
    int ret = cm_mqtt_client_set_opt(client, opt, &value);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg mqtt_lib[] = {
    { "create",      iot_mqtt_create },
    {NULL, NULL}
};

static const luaL_Reg mqtt_client_lib[] = {
    { "on",          iot_mqtt_on },
    { "connect",     iot_mqtt_connect },
    { "subscribe",   iot_mqtt_subscribe },
    { "unsubscribe", iot_mqtt_unsubscribe },
    { "publish",     iot_mqtt_publish },
    { "disconnect",  iot_mqtt_disconnect },
    { "destroy",     iot_mqtt_destroy },
    { "state",       iot_mqtt_state },
    { "setopt",      iot_mqtt_setopt },
    {NULL, NULL}
};

static int mqtt_client_gc(lua_State* L) {
    cm_mqtt_client_t* client = (cm_mqtt_client_t*)lua_touserdata(L, 1);
    if (client) {
        cm_mqtt_client_destroy(client);
    }
    return 0;
}

LUAMOD_API int luaopen_mqtt(lua_State* L) {
    mqtt_context_init();
    luaL_newlibtable(L, mqtt_lib);
    luaL_setfuncs(L, mqtt_lib, 0);

    /* 创建客户端类型的元表 */
    luaL_newmetatable(L, "mqtt_client");
    
    /* 将方法表设置到__index */
    luaL_newlibtable(L, mqtt_client_lib);
    luaL_setfuncs(L, mqtt_client_lib, 0);
    lua_setfield(L, -2, "__index");
    
    lua_pushcfunction(L, mqtt_client_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    return 1;
}
