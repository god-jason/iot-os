/**
 * @file mqtt_module.c
 * @brief MQTT 客户端 Lua 模块封装
 *
 * 提供面向对象的 MQTT 客户端接口，支持发布、订阅、自动重连等完整功能
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "platform.h"
#include "iot_callback.h"
#include "iot_params.h"

#include "mqtt_client.h"
#include "mqtt_packets.h"

/*===========================================================
 * 类型定义
 *===========================================================*/

/**
 * @brief MQTT 路由条目
 * 
 * 每个路由条目存储一个主题过滤器和对应的 Lua 回调函数，
 * 通过链表方式管理多个订阅路由。
 */
typedef struct mqtt_route_entry {
    char* topic_filter;           /* 主题过滤器（如 "sensor/#"） */
    void* callback_ud;            /* Lua 回调函数的用户数据 */
    struct mqtt_route_entry* next; /* 下一个路由条目 */
} mqtt_route_entry_t;

/**
 * @brief MQTT Lua 上下文
 * 
 * 每个 MQTT 客户端实例的 Lua 层上下文，包含底层客户端句柄、
 * 路由表和各类回调函数。
 */
typedef struct {
    mqtt_client_t* client;        /* 底层 MQTT 客户端句柄 */
    void* connect_callback_ud;    /* 连接成功回调 */
    void* close_callback_ud;      /* 连接关闭回调 */
    mqtt_route_entry_t* routes;   /* 订阅路由表（链表） */
    char* client_id;              /* 客户端标识符（动态分配） */
    int inited;                   /* 初始化标志 */
} mqtt_lua_ctx_t;

#define MQTT_CTX_METATABLE "mqtt.client"  /* Lua 用户数据元表名称 */

/*===========================================================
 * 内部函数
 *===========================================================*/

/**
 * @brief 从 Lua 栈获取 MQTT 上下文
 * @param L Lua 状态机
 * @param idx 栈索引
 * @return MQTT 上下文指针，失败返回 NULL
 */
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

/**
 * @brief 在路由表中查找指定主题过滤器的路由条目
 * @param ctx MQTT 上下文
 * @param topic_filter 主题过滤器
 * @return 匹配的路由条目，未找到返回 NULL
 */
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

/**
 * @brief 向路由表添加新的订阅路由
 * @param ctx MQTT 上下文
 * @param topic_filter 主题过滤器
 * @param callback_ud Lua 回调函数用户数据
 * @return 新创建的路由条目，内存不足返回 NULL
 */
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

/**
 * @brief 从路由表移除指定主题过滤器的路由
 * @param ctx MQTT 上下文
 * @param topic_filter 主题过滤器
 */
static void mqtt_route_remove(mqtt_lua_ctx_t* ctx, const char* topic_filter) {
    mqtt_route_entry_t** pp = &ctx->routes;
    while (*pp) {
        mqtt_route_entry_t* entry = *pp;
        if (strcmp(entry->topic_filter, topic_filter) == 0) {
            *pp = entry->next;
            iot_callback_free(entry->callback_ud);
            iot_free(entry->topic_filter);
            iot_free(entry);
            return;
        }
        pp = &entry->next;
    }
}

/**
 * @brief 销毁路由表中所有路由条目
 * @param ctx MQTT 上下文
 */
static void mqtt_route_destroy_all(mqtt_lua_ctx_t* ctx) {
    mqtt_route_entry_t* entry = ctx->routes;
    while (entry) {
        mqtt_route_entry_t* next = entry->next;
        iot_callback_free(entry->callback_ud);
        iot_free(entry->topic_filter);
        iot_free(entry);
        entry = next;
    }
    ctx->routes = NULL;
}

/**
 * @brief 创建 MQTT Lua 上下文
 * @return 新创建的上下文，失败返回 NULL
 */
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

/**
 * @brief 销毁 MQTT Lua 上下文
 * @param ctx MQTT 上下文
 */
static void mqtt_ctx_destroy(mqtt_lua_ctx_t* ctx) {
    if (!ctx) return;
    
    mqtt_route_destroy_all(ctx);
    
    if (ctx->connect_callback_ud) {
        iot_callback_free(ctx->connect_callback_ud);
    }
    
    if (ctx->close_callback_ud) {
        iot_callback_free(ctx->close_callback_ud);
    }
    
    if (ctx->client_id) {
        iot_free(ctx->client_id);
    }
    
    if (ctx->client) {
        mqtt_client_destroy(ctx->client);
    }
    
    iot_free(ctx);
}

/*===========================================================
 * MQTT 事件回调
 *===========================================================*/

/**
 * @brief MQTT 状态事件回调
 * 
 * 当 MQTT 客户端状态发生变化时被调用，触发相应的 Lua 回调函数。
 * 
 * @param user_data 用户数据（mqtt_lua_ctx_t*）
 * @param event 事件类型
 */
static void mqtt_event_callback(void* user_data, mqtt_event_type_t event) {
    mqtt_lua_ctx_t* ctx = (mqtt_lua_ctx_t*)user_data;
    if (!ctx) return;
    
    params_t* params = params_create(1);
    if (!params) return;
    
    if (event == MQTT_EVENT_CONNECTED) {
        params_push_int(params, 1);
        if (ctx->connect_callback_ud) {
            iot_callback_call(ctx->connect_callback_ud, params);
        }
    } else if (event == MQTT_EVENT_DISCONNECTED || event == MQTT_EVENT_ERROR) {
        params_push_int(params, 0);
        if (ctx->close_callback_ud) {
            iot_callback_call(ctx->close_callback_ud, params);
        }
    }
    
    params_destroy(params);
}

/**
 * @brief MQTT 消息接收回调
 * 
 * 当收到 PUBLISH 消息时被调用，遍历路由表找到匹配的主题过滤器，
 * 并执行对应的 Lua 回调函数。
 * 
 * @param user_data 用户数据（mqtt_lua_ctx_t*）
 * @param topic 消息主题
 * @param payload 消息负载
 * @param payload_len 负载长度
 * @param qos 服务质量等级
 * @param retain 是否为保留消息
 */
static void mqtt_message_callback(void* user_data, const char* topic,
                                  const uint8_t* payload, size_t payload_len,
                                  mqtt_qos_t qos, bool retain) {
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
            
            iot_callback_call(entry->callback_ud, params);
            params_destroy(params);
        }
        entry = entry->next;
    }
}

/*===========================================================
 * Lua 方法实现
 *===========================================================*/

/**
 * @brief 创建 MQTT 客户端实例
 * 
 * Lua 调用方式：client = mqtt.new()
 * 
 * @param L Lua 状态机
 * @return 1 返回 MQTT 客户端 userdata，失败返回 nil 和错误信息
 */
static int iot_mqtt_new(lua_State* L) {
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

/**
 * @brief 连接到 MQTT 服务器
 * 
 * Lua 调用方式：client:connect({host="127.0.0.1", port=1883, client_id="test"})
 * 
 * 支持的连接参数：
 * - host: 服务器地址（必填）
 * - port: 端口号（默认 1883）
 * - client_id: 客户端标识（默认自动生成）
 * - username: 用户名（可选）
 * - password: 密码（可选）
 * - keepalive: 心跳间隔（秒，默认 60）
 * - clean_session: 清除会话（默认 true）
 * - will_topic: 遗嘱主题（可选）
 * - will_message: 遗嘱消息（可选）
 * - will_qos: 遗嘱 QoS（默认 0）
 * - will_retain: 遗嘱保留（默认 false）
 * - timeout_ms: 超时时间（毫秒，默认 30000）
 * 
 * @param L Lua 状态机
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_connect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid mqtt client");
        return 2;
    }
    
    luaL_checktype(L, 2, LUA_TTABLE);
    
    mqtt_connect_options_t options = {0};
    
    /* 解析连接参数 */
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
        /* 生成默认 client_id */
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
    
    /* 设置事件回调 */
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

/**
 * @brief 断开与 MQTT 服务器的连接
 * 
 * Lua 调用方式：client:disconnect()
 * 
 * @param L Lua 状态机
 * @return 1 返回是否成功断开
 */
static int iot_mqtt_disconnect(lua_State* L) {
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

/**
 * @brief 关闭并销毁 MQTT 客户端实例
 * 
 * Lua 调用方式：client:close()
 * 
 * @param L Lua 状态机
 * @return 0 无返回值
 */
static int iot_mqtt_close(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (ctx) {
        mqtt_ctx_destroy(ctx);
    }
    return 0;
}

/**
 * @brief 发布 MQTT 消息
 * 
 * Lua 调用方式：client:publish(topic, payload, qos, retain)
 * 
 * @param L Lua 状态机
 * @param topic 消息主题（必填）
 * @param payload 消息负载（可选，默认空字符串）
 * @param qos 服务质量等级（可选，默认 0）
 * @param retain 是否保留消息（可选，默认 false）
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_publish(lua_State* L) {
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

/**
 * @brief 订阅 MQTT 主题
 * 
 * Lua 调用方式：client:subscribe(topic_filter, qos, callback)
 * 
 * 收到匹配主题的消息时，会执行指定的 Lua 回调函数，回调参数：
 * - topic: 消息主题
 * - payload: 消息负载
 * - qos: 服务质量等级
 * - retain: 是否为保留消息
 * 
 * @param L Lua 状态机
 * @param topic_filter 主题过滤器（支持 + 和 # 通配符）
 * @param qos 服务质量等级（可选，默认 0）
 * @param callback 消息回调函数（必填）
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_subscribe(lua_State* L) {
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
    
    /* 检查是否已存在该路由 */
    mqtt_route_entry_t* existing = mqtt_route_find(ctx, topic_filter);
    if (existing) {
        /* 更新已存在的路由回调 */
        iot_callback_free(existing->callback_ud);
        existing->callback_ud = iot_callback_save(L, 4);
    } else {
        /* 添加新路由 */
        void* callback_ud = iot_callback_save(L, 4);
        if (!callback_ud) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "failed to save callback");
            return 2;
        }
        
        if (!mqtt_route_add(ctx, topic_filter, callback_ud)) {
            iot_callback_free(callback_ud);
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

/**
 * @brief 取消订阅 MQTT 主题
 * 
 * Lua 调用方式：client:unsubscribe(topic_filter)
 * 
 * @param L Lua 状态机
 * @param topic_filter 主题过滤器
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_unsubscribe(lua_State* L) {
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
    
    /* 从路由表中移除该路由 */
    mqtt_route_remove(ctx, topic_filter);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 注册事件回调函数
 * 
 * Lua 调用方式：client:on(event, callback)
 * 
 * 支持的事件类型：
 * - connect: 连接成功时触发
 * - close: 连接关闭时触发
 * 
 * @param L Lua 状态机
 * @param event 事件名称
 * @param callback 回调函数
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_on(lua_State* L) {
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
        /* 注册连接成功回调 */
        if (ctx->connect_callback_ud) {
            iot_callback_free(ctx->connect_callback_ud);
        }
        ctx->connect_callback_ud = iot_callback_save(L, 3);
        mqtt_client_set_event_callback(ctx->client, mqtt_event_callback, ctx);
    } else if (strcmp(event, "close") == 0) {
        /* 注册连接关闭回调 */
        if (ctx->close_callback_ud) {
            iot_callback_free(ctx->close_callback_ud);
        }
        ctx->close_callback_ud = iot_callback_save(L, 3);
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unsupported event");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 获取客户端状态
 * 
 * Lua 调用方式：state = client:state()
 * 
 * 返回值对应常量：
 * - mqtt.STATE_DISCONNECTED: 断开连接
 * - mqtt.STATE_CONNECTING: 正在连接
 * - mqtt.STATE_CONNECTED: 已连接
 * - mqtt.STATE_ERROR: 错误状态
 * 
 * @param L Lua 状态机
 * @return 1 返回状态值
 */
static int iot_mqtt_state(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushinteger(L, MQTT_STATE_DISCONNECTED);
        return 1;
    }
    
    mqtt_state_t state = mqtt_client_get_state(ctx->client);
    lua_pushinteger(L, state);
    return 1;
}

/**
 * @brief 轮询处理 MQTT 事件
 * 
 * Lua 调用方式：client:poll(timeout_ms)
 * 
 * 处理接收数据、发送消息、心跳等事件。
 * 
 * @param L Lua 状态机
 * @param timeout_ms 超时时间（毫秒，默认 10）
 * @return 0 无返回值
 */
static int iot_mqtt_poll(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        return 0;
    }
    
    int timeout_ms = (int)luaL_optinteger(L, 2, 10);
    
    mqtt_client_poll(ctx->client, timeout_ms);
    
    return 0;
}

/**
 * @brief 阻塞等待连接完成
 * 
 * Lua 调用方式：client:wait_for_connect(timeout_ms)
 * 
 * @param L Lua 状态机
 * @param timeout_ms 超时时间（毫秒，默认 30000）
 * @return 1 返回 true，超时或失败返回 false 和错误信息
 */
static int iot_mqtt_wait_for_connect(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        return 0;
    }
    
    int timeout_ms = (int)luaL_optinteger(L, 2, 30000);
    uint32_t start = iot_get_tick_ms();
    
    while (mqtt_client_get_state(ctx->client) == MQTT_STATE_CONNECTING) {
        mqtt_client_poll(ctx->client, 10);
        
        if (iot_get_tick_ms() - start > (uint32_t)timeout_ms) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "connect timeout");
            return 2;
        }
    }
    
    if (mqtt_client_get_state(ctx->client) == MQTT_STATE_CONNECTED) {
        lua_pushboolean(L, 1);
        return 1;
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "connect failed");
        return 2;
    }
}

/**
 * @brief 启用自动重连功能
 * 
 * Lua 调用方式：client:enable_auto_reconnect(interval_ms)
 * 
 * 当连接断开时，客户端会自动尝试重新连接。
 * 
 * @param L Lua 状态机
 * @param interval_ms 重连间隔（毫秒，默认 5000）
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_enable_auto_reconnect(lua_State* L) {
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

/**
 * @brief 禁用自动重连功能
 * 
 * Lua 调用方式：client:disable_auto_reconnect()
 * 
 * @param L Lua 状态机
 * @return 1 返回 true，失败返回 false 和错误信息
 */
static int iot_mqtt_disable_auto_reconnect(lua_State* L) {
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

/**
 * @brief 检查客户端是否已连接
 * 
 * Lua 调用方式：connected = client:is_connected()
 * 
 * @param L Lua 状态机
 * @return 1 返回连接状态（true/false）
 */
static int iot_mqtt_is_connected(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_pushboolean(L, mqtt_client_is_connected(ctx->client));
    return 1;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/**
 * @brief MQTT 客户端方法列表
 * 
 * 这些方法会注册到客户端 userdata 的元表中，可通过 client:method() 调用。
 */
static int iot_mqtt_tostring(lua_State* L) {
    mqtt_lua_ctx_t* ctx = mqtt_get_ctx(L, 1);
    if (!ctx) {
        lua_pushstring(L, "mqtt.client (invalid)");
    } else {
        lua_pushstring(L, "mqtt.client");
    }
    return 1;
}

static const luaL_Reg mqtt_client_methods[] = {
    { "connect",               iot_mqtt_connect },
    { "disconnect",            iot_mqtt_disconnect },
    { "close",                 iot_mqtt_close },
    { "publish",               iot_mqtt_publish },
    { "subscribe",             iot_mqtt_subscribe },
    { "unsubscribe",           iot_mqtt_unsubscribe },
    { "on",                    iot_mqtt_on },
    { "state",                 iot_mqtt_state },
    { "poll",                  iot_mqtt_poll },
    { "wait_for_connect",      iot_mqtt_wait_for_connect },
    { "enable_auto_reconnect", iot_mqtt_enable_auto_reconnect },
    { "disable_auto_reconnect",iot_mqtt_disable_auto_reconnect },
    { "is_connected",          iot_mqtt_is_connected },
    { "__gc",                  iot_mqtt_close },
    { "__tostring",            iot_mqtt_tostring },
    { NULL,                    NULL }
};

/**
 * @brief MQTT 模块方法列表
 * 
 * 这些方法注册到 mqtt 模块命名空间，可通过 mqtt.method() 调用。
 */
static const luaL_Reg mqtt_module_methods[] = {
    { "new",                      iot_mqtt_new },
    { NULL,                       NULL }
};

/**
 * @brief MQTT 模块常量定义
 * 
 * 包含 QoS 等级、连接状态、错误码、CONNACK 返回码等常量。
 */
static const luaL_Const mqtt_constants[] = {
    /* QoS 服务质量等级 */
    { "QOS_0",                 MQTT_QOS_0 },
    { "QOS_1",                 MQTT_QOS_1 },
    { "QOS_2",                 MQTT_QOS_2 },
    
    /* 客户端连接状态 */
    { "STATE_DISCONNECTED",    MQTT_STATE_DISCONNECTED },
    { "STATE_CONNECTING",      MQTT_STATE_CONNECTING },
    { "STATE_CONNECTED",       MQTT_STATE_CONNECTED },
    { "STATE_ERROR",           MQTT_STATE_ERROR },
    
    /* 错误码 */
    { "ERR_SUCCESS",           MQTT_ERR_SUCCESS },
    { "ERR_CONNECT",           MQTT_ERR_CONNECT },
    { "ERR_SEND",              MQTT_ERR_SEND },
    { "ERR_RECV",              MQTT_ERR_RECV },
    { "ERR_TIMEOUT",           MQTT_ERR_TIMEOUT },
    { "ERR_PROTOCOL",          MQTT_ERR_PROTOCOL },
    { "ERR_QOS",               MQTT_ERR_QOS },
    { "ERR_KEEPALIVE",         MQTT_ERR_KEEPALIVE },
    { "ERR_MEMORY",            MQTT_ERR_MEMORY },
    { "ERR_PARAM",             MQTT_ERR_PARAM },
    
    /* CONNACK 返回码 */
    { "CONNACK_ACCEPTED",           MQTT_CONNACK_ACCEPTED },
    { "CONNACK_UNACCEPTABLE_PROTO",MQTT_CONNACK_UNACCEPTABLE_PROTO },
    { "CONNACK_IDENTIFIER_REJECTED",MQTT_CONNACK_IDENTIFIER_REJECTED },
    { "CONNACK_SERVER_UNAVAILABLE", MQTT_CONNACK_SERVER_UNAVAILABLE },
    { "CONNACK_BAD_AUTH",           MQTT_CONNACK_BAD_AUTH },
    { "CONNACK_NOT_AUTHORIZED",     MQTT_CONNACK_NOT_AUTHORIZED },
    
    { NULL, 0 }
};

/**
 * @brief Lua 模块入口函数
 * 
 * 当 require("mqtt") 时调用此函数，注册模块方法、常量和客户端元表。
 * 
 * @param L Lua 状态机
 * @return 1 返回 mqtt 模块表
 */
LUAMOD_API int luaopen_mqtt(lua_State* L) {
    /* 创建模块表 */
    luaL_newlib(L, mqtt_module_methods);
    
    /* 向模块表添加常量 */
    const luaL_Const* constant = mqtt_constants;
    for (; constant->name; constant++) {
        lua_pushinteger(L, constant->value);
        lua_setfield(L, -2, constant->name);
    }
    
    /* 创建客户端元表 */
    luaL_newmetatable(L, MQTT_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, mqtt_client_methods, 0);
    
    return 1;
}
