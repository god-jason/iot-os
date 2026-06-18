/*
@module  rtos
@summary RTOS消息队列
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include "lua.h"
#include "iot_base.h"
#include "iot_rtos.h"
#include "iot_params.h"

/* 消息队列大小 */
#define RTOS_MSG_QUEUE_SIZE 32

/* 定时器最大数量 */
#define MAX_TIMER_COUNT 8

/* 定时器上下文 */
typedef struct {
    osTimerId_t timer_id;
    uint32_t timeout_ticks;
    uint32_t flags;
    uint32_t timer_id_lua;  /* 给Lua的ID (slot+1) */
    uint32_t periodic;
} timer_ctx_t;

/* 全局消息队列 */
static osMessageQueueId_t g_rtos_msg_queue = NULL;

/* Lua状态机指针 */
static lua_State* g_lua_L = NULL;

/* 定时器上下文 */
static timer_ctx_t g_timer_ctx[MAX_TIMER_COUNT];
static int g_timer_inited = 0;

/**
 * @brief 设置Lua状态机（由lua_task调用） */
void iot_set_lua_state(lua_State* L)
{
    g_lua_L = L;
}

/**
 * @brief 获取Lua状态机
 */
lua_State* iot_get_lua_state(void)
{
    return g_lua_L;
}

/**
 * @brief 初始化定时器上下文 */
static void timer_context_init(void)
{
    if (!g_timer_inited) {
        memset(g_timer_ctx, 0, sizeof(g_timer_ctx));
        for (int i = 0; i < MAX_TIMER_COUNT; i++) {
            g_timer_ctx[i].timer_id = NULL;
            g_timer_ctx[i].timer_id_lua = 0;
        }
        g_timer_inited = 1;
    }
}

/**
 * @brief 查找空闲定时器插槽 */
static int timer_find_free_slot(void)
{
    for (int i = 0; i < MAX_TIMER_COUNT; i++) {
        if (g_timer_ctx[i].timer_id == NULL && g_timer_ctx[i].timer_id_lua == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 定时器回调
 */
static void timer_callback_wrapper(void* param)
{
    int index = (int)(uintptr_t)param;
    if (index < 0 || index >= MAX_TIMER_COUNT) {
        return;
    }

    timer_ctx_t* ctx = &g_timer_ctx[index];
    if (ctx->timer_id_lua == 0) {
        return;
    }

    /* 发送MSG_TIMEOUT */
    iot_rtos_timeout(ctx->timer_id_lua);
}

/**
 * @brief 初始化RTOS消息队列
 */
bool iot_rtos_msg_init(void)
{
    timer_context_init();

    g_rtos_msg_queue = osMessageQueueNew(RTOS_MSG_QUEUE_SIZE, sizeof(rtos_msg_t*), NULL);
    if (g_rtos_msg_queue == NULL) {
        LOG("ERR queue create");
        return false;
    }
    LOG("OK queue size=%d", RTOS_MSG_QUEUE_SIZE);
    return true;
}

/**
 * @brief 发送消息到队列
 */
static void iot_rtos_send_msg_internal(rtos_msg_t* msg)
{
    if (g_rtos_msg_queue == NULL || msg == NULL) {
        LOG("ERR queue or msg is NULL");
        iot_rtos_msg_destroy(msg);
        return;
    }
    osMessageQueuePut(g_rtos_msg_queue, &msg, 0, 0);
}

/**
 * @brief 发送PUBLISH消息
 */
void iot_rtos_publish(const char* str, params_t* params)
{
    rtos_msg_t* msg = (rtos_msg_t*)malloc(sizeof(rtos_msg_t));
    if (!msg) {
        LOG("ERR malloc PUBLISH");
        if (params) {
            params_destroy(params);
        }
        return;
    }

    memset(msg, 0, sizeof(rtos_msg_t));
    msg->type = MSG_TYPE_PUBLISH;
    msg->data = (void*)str; /* 仅支持常量字符串，不做内存复制 */
    msg->params = params;

    iot_rtos_send_msg_internal(msg);
}

/**
 * @brief 发送CALL消息
 */
void iot_rtos_call(void* userdata, params_t* params)
{
    rtos_msg_t* msg = (rtos_msg_t*)malloc(sizeof(rtos_msg_t));
    if (!msg) {
        LOG("ERR malloc CALL");
        if (params) {
            params_destroy(params);
        }
        return;
    }

    memset(msg, 0, sizeof(rtos_msg_t));
    msg->type = MSG_TYPE_CALL;
    msg->data = userdata;
    msg->params = params;

    iot_rtos_send_msg_internal(msg);
}

/**
 * @brief 发送定时器超时消息
 */
void iot_rtos_timeout(uint32_t timer_id)
{
    rtos_msg_t* msg = (rtos_msg_t*)malloc(sizeof(rtos_msg_t));
    if (!msg) {
        LOG("ERR malloc TIMEOUT id=%u", timer_id);
        return;
    }

    memset(msg, 0, sizeof(rtos_msg_t));
    msg->type = MSG_TYPE_TIMEOUT;
    msg->id = timer_id;

    iot_rtos_send_msg_internal(msg);
}

/**
 * @brief 销毁消息
 */
void iot_rtos_msg_destroy(rtos_msg_t* msg)
{
    if (!msg) {
        return;
    }

    if (msg->params) {
        params_destroy(msg->params);
    }

    free(msg);
}

/**
 * @brief Lua接口: 创建定时器
 * @api rtos.timer_create(timeout_ms, periodic)
 */
static int iot_rtos_timer_create(lua_State* L)
{
    uint32_t timeout_ms = luaL_checkinteger(L, 1);
    uint32_t periodic = luaL_optinteger(L, 2, 0);

    int slot = timer_find_free_slot();
    if (slot < 0) {
        LOG("ERR no free slot");
        lua_pushnil(L);
        lua_pushstring(L, "no free timer slot");
        return 2;
    }

    timer_ctx_t* ctx = &g_timer_ctx[slot];
    ctx->timeout_ticks = (timeout_ms * osKernelGetTickFreq()) / 1000;
    ctx->flags = periodic ? 0x01 : 0x00;
    ctx->periodic = periodic;
    ctx->timer_id_lua = slot + 1;

    osTimerType_t timer_type = periodic ? osTimerPeriodic : osTimerOnce;
    ctx->timer_id = osTimerNew(timer_callback_wrapper, timer_type, (void*)(uintptr_t)slot, NULL);

    if (ctx->timer_id == NULL) {
        LOG("ERR osTimerNew");
        ctx->timer_id_lua = 0;
        lua_pushnil(L);
        lua_pushstring(L, "failed to create timer");
        return 2;
    }

    osStatus_t status = osTimerStart(ctx->timer_id, ctx->timeout_ticks);
    if (status != osOK) {
        LOG("ERR osTimerStart status=%d", status);
        osTimerDelete(ctx->timer_id);
        ctx->timer_id = NULL;
        ctx->timer_id_lua = 0;
        lua_pushnil(L);
        lua_pushstring(L, "failed to start timer");
        return 2;
    }

    LOG("OK timer id=%u, ms=%u, periodic=%u", ctx->timer_id_lua, timeout_ms, periodic);
    lua_pushinteger(L, ctx->timer_id_lua);
    return 1;
}

/**
 * @brief Lua接口: 停止定时器
 * @api rtos.timer_stop(timer_id)
 */
static int iot_rtos_timer_stop(lua_State* L)
{
    int id = luaL_checkinteger(L, 1);
    int slot = id - 1;

    if (slot < 0 || slot >= MAX_TIMER_COUNT) {
        LOG("ERR invalid id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    timer_ctx_t* ctx = &g_timer_ctx[slot];
    if (ctx->timer_id == NULL) {
        LOG("ERR timer NULL id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    osStatus_t status = osTimerStop(ctx->timer_id);
    LOG("stop id=%d, status=%d", id, status);
    lua_pushboolean(L, (status == osOK) ? 1 : 0);
    return 1;
}

/**
 * @brief Lua接口: 检查定时器是否运行
 * @api rtos.timer_is_running(timer_id)
 */
static int iot_rtos_timer_is_running(lua_State* L)
{
    int id = luaL_checkinteger(L, 1);
    int slot = id - 1;

    if (slot < 0 || slot >= MAX_TIMER_COUNT) {
        LOG("ERR invalid id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    timer_ctx_t* ctx = &g_timer_ctx[slot];
    if (ctx->timer_id == NULL) {
        LOG("ERR timer NULL id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    uint32_t running = osTimerIsRunning(ctx->timer_id);
    lua_pushboolean(L, running ? 1 : 0);
    return 1;
}

/**
 * @brief Lua接口: 删除定时器
 * @api rtos.timer_delete(timer_id)
 */
static int iot_rtos_timer_delete(lua_State* L)
{
    int id = luaL_checkinteger(L, 1);
    int slot = id - 1;

    if (slot < 0 || slot >= MAX_TIMER_COUNT) {
        LOG("ERR invalid id=%d", id);
        lua_pushboolean(L, 0);
        return 1;
    }

    timer_ctx_t* ctx = &g_timer_ctx[slot];

    if (ctx->timer_id != NULL) {
        osTimerStop(ctx->timer_id);
        osTimerDelete(ctx->timer_id);
        ctx->timer_id = NULL;
    }

    ctx->timer_id_lua = 0;
    ctx->timeout_ticks = 0;
    ctx->flags = 0;
    ctx->periodic = 0;

    LOG("delete id=%d OK", id);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief Lua接口: 接收系统消息
 * @api rtos.recv(timeout)
 */
static int iot_rtos_recv(lua_State* L)
{
    uint32_t timeout = (uint32_t)luaL_optinteger(L, 1, 0);
    rtos_msg_t* msg = NULL;
    uint8_t prio = 0;

    osStatus_t status = osMessageQueueGet(g_rtos_msg_queue, &msg, &prio, timeout);
    if (status != osOK || msg == NULL) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 4);
    lua_pushinteger(L, msg->type);
    lua_setfield(L, -2, "type");
    lua_pushinteger(L, msg->id);
    lua_setfield(L, -2, "id");

    if (msg->type == MSG_TYPE_PUBLISH) {
        /* 处理PUBLISH消息 */
        if (msg->data) {
            lua_pushstring(L, (const char*)msg->data);
            lua_setfield(L, -2, "data");
        } else {
            lua_pushnil(L);
            lua_setfield(L, -2, "data");
        }

        /* 追加params字段 */
        if (msg->params && msg->params->count > 0) {
            params_to_table(L, msg->params);
            lua_setfield(L, -2, "params");
        }
    } else if (msg->type == MSG_TYPE_CALL) {
        /* 处理CALL消息 */
        if (msg->data) {
            lua_pushlightuserdata(L, msg->data);
            lua_setfield(L, -2, "userdata");
        }

        /* 追加params字段 */
        if (msg->params && msg->params->count > 0) {
            params_to_table(L, msg->params);
            lua_setfield(L, -2, "params");
        }
    }

    /* 销毁消息 */
    iot_rtos_msg_destroy(msg);

    return 1;
}

/* Lua模块注册 */
static const luaL_Reg rtos_lib[] = {
    { "recv",          iot_rtos_recv },
    { "timer_create",      iot_rtos_timer_create },
    { "timer_stop",        iot_rtos_timer_stop },
    { "timer_is_running",  iot_rtos_timer_is_running },
    { "timer_delete",      iot_rtos_timer_delete },
    { NULL, NULL }
};

/**
 * @brief 注册rtos模块到Lua
 */
LUAMOD_API int luaopen_rtos(lua_State* L)
{
    luaL_newlibtable(L, rtos_lib);
    luaL_setfuncs(L, rtos_lib, 0);

    /* 推送消息类型常量 */
    lua_pushinteger(L, MSG_TYPE_TIMEOUT);
    lua_setfield(L, -2, "MSG_TIMEOUT");
    lua_pushinteger(L, MSG_TYPE_PUBLISH);
    lua_setfield(L, -2, "MSG_PUBLISH");
    lua_pushinteger(L, MSG_TYPE_CALL);
    lua_setfield(L, -2, "MSG_CALL");

    return 1;
}
