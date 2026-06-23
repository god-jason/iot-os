/**
 * @file iot_wdt.c
 * @brief 看门狗模块实现
 *
 * 本模块提供看门狗功能，用于监控 Lua 脚本的执行状态。
 * 当 Lua 脚本长时间未响应时，看门狗会触发超时退出。
 *
 * @author  TRAE
 * @date    2026.06.23
 */
#include "iot.h"
#include "iot_wdt.h"
#include "iot_rtos.h"
#include "iot_log.h"
#include <stdio.h>
#include <stdlib.h>

/*===========================================================
 * 数据结构
 *===========================================================*/

/**
 * @brief 看门狗状态结构
 */
typedef struct {
    bool initialized;       /**< 资源是否已初始化 */
    bool started;           /**< 定时器是否已启动 */
    bool timeout;           /**< 是否超时 */
    uint32_t timeout_ms;    /**< 超时时间（毫秒） */
    iot_mutex_t mutex;      /**< 互斥锁 */
    iot_timer_t timer;      /**< 循环定时器 */
} iot_wdt_t;

static iot_wdt_t g_wdt = {0};

/* 互斥锁超时时间（毫秒） */
#define IOT_WDT_MUTEX_TIMEOUT  10000

/* 默认超时时间（毫秒） */
#define IOT_WDT_DEFAULT_TIMEOUT  30000

/*===========================================================
 * 内部函数
 *===========================================================*/

/**
 * @brief 看门狗定时器回调函数
 * @param arg 参数（未使用）
 */
static void iot_wdt_timer_callback(void* arg) {
    (void)arg;

    iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);
    if (g_wdt.started) {
        g_wdt.timeout = true;
        LOG_INFO("[WDT] Watchdog timeout triggered!");
        exit(1);
    }
    iot_mutex_unlock(g_wdt.mutex);
}

/**
 * @brief 初始化看门狗资源（不启动定时器）
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 true，失败返回 false
 */
static bool iot_wdt_init_resources(uint32_t timeout_ms) {
    if (g_wdt.initialized) {
        return true;
    }

    /* 创建互斥锁 */
    g_wdt.mutex = iot_mutex_create();
    if (g_wdt.mutex == NULL) {
        LOG_INFO("[WDT] Failed to create mutex");
        return false;
    }

    /* 设置超时时间 */
    g_wdt.timeout_ms = (timeout_ms == 0) ? IOT_WDT_DEFAULT_TIMEOUT : timeout_ms;
    g_wdt.timeout = false;
    g_wdt.started = false;
    g_wdt.timer = NULL;
    g_wdt.initialized = true;

    LOG_INFO("[WDT] Resources initialized, timeout=%u ms", g_wdt.timeout_ms);
    return true;
}

/**
 * @brief 启动看门狗定时器
 * @return 成功返回 true，失败返回 false
 */
static bool iot_wdt_start_timer(void) {
    if (!g_wdt.initialized) {
        LOG_INFO("[WDT] Cannot start timer: not initialized");
        return false;
    }

    iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);

    /* 删除旧定时器 */
    if (g_wdt.timer != NULL) {
        iot_timer_delete(g_wdt.timer);
        g_wdt.timer = NULL;
    }

    /* 创建循环定时器 */
    g_wdt.timer = iot_timer_create(iot_wdt_timer_callback, NULL, g_wdt.timeout_ms, IOT_TIMER_PERIODIC);
    if (g_wdt.timer == NULL) {
        LOG_INFO("[WDT] Failed to create timer");
        iot_mutex_unlock(g_wdt.mutex);
        return false;
    }

    g_wdt.started = true;
    g_wdt.timeout = false;

    iot_mutex_unlock(g_wdt.mutex);

    LOG_INFO("[WDT] Timer started, period=%u ms", g_wdt.timeout_ms);
    return true;
}

/*===========================================================
 * 外部接口
 *===========================================================*/

bool iot_wdt_init(uint32_t timeout_ms) {
    if (!iot_wdt_init_resources(timeout_ms)) {
        return false;
    }
    return iot_wdt_start_timer();
}

bool iot_wdt_feed(void) {
    if (!g_wdt.initialized) {
        LOG_INFO("[WDT] Watchdog not initialized");
        return false;
    }

    iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);

    /* 重置超时状态 */
    g_wdt.timeout = false;

    /* 删除旧定时器，创建新定时器（重置计时） */
    if (g_wdt.timer != NULL) {
        iot_timer_delete(g_wdt.timer);
    }
    g_wdt.timer = iot_timer_create(iot_wdt_timer_callback, NULL, g_wdt.timeout_ms, IOT_TIMER_PERIODIC);

    iot_mutex_unlock(g_wdt.mutex);

    LOG_INFO("[WDT] Fed");
    return true;
}

void iot_wdt_wait(void) {
    /* 如果未初始化，则初始化资源但不启动定时器 */
    if (!g_wdt.initialized) {
        if (!iot_wdt_init_resources(IOT_WDT_DEFAULT_TIMEOUT)) {
            LOG_INFO("[WDT] Auto-init failed in wait");
            return;
        }
        LOG_INFO("[WDT] Auto-initialized in wait (timer not started)");
    }

    LOG_INFO("[WDT] Waiting for watchdog timeout...");

    /* 阻塞等待超时 */
    while (1) {
        iot_task_delay(100);

        iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);
        if (g_wdt.timeout) {
            iot_mutex_unlock(g_wdt.mutex);
            break;
        }
        iot_mutex_unlock(g_wdt.mutex);
    }

    LOG_INFO("[WDT] Watchdog wait ended");
}

bool iot_wdt_stop(void) {
    if (!g_wdt.initialized) {
        LOG_INFO("[WDT] Watchdog not initialized");
        return false;
    }

    iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);

    /* 停止定时器 */
    g_wdt.started = false;
    if (g_wdt.timer != NULL) {
        iot_timer_delete(g_wdt.timer);
        g_wdt.timer = NULL;
    }

    iot_mutex_unlock(g_wdt.mutex);

    /* 清理资源 */
    if (g_wdt.mutex != NULL) {
        iot_mutex_delete(g_wdt.mutex);
        g_wdt.mutex = NULL;
    }

    g_wdt.initialized = false;

    LOG_INFO("[WDT] Watchdog stopped");
    return true;
}

bool iot_wdt_is_initialized(void) {
    return g_wdt.initialized;
}

bool iot_wdt_is_timeout(void) {
    bool timeout;

    iot_mutex_lock(g_wdt.mutex, IOT_WDT_MUTEX_TIMEOUT);
    timeout = g_wdt.timeout;
    iot_mutex_unlock(g_wdt.mutex);

    return timeout;
}

/*===========================================================
 * Lua 接口实现
 *===========================================================*/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/**
 * @brief Lua: wdt.init(timeout_ms)
 * @brief 初始化看门狗并启动定时器
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int lua_wdt_init(lua_State* L) {
    int timeout_ms = luaL_checkinteger(L, 1);

    if (timeout_ms <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, "Invalid timeout value");
        return 2;
    }

    if (iot_wdt_init(timeout_ms)) {
        lua_pushboolean(L, 1);
        return 1;
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to initialize watchdog");
        return 2;
    }
}

/**
 * @brief Lua: wdt.feed()
 * @brief 喂狗（重置看门狗计时器）
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int lua_wdt_feed(lua_State* L) {
    (void)L;

    if (iot_wdt_feed()) {
        lua_pushboolean(L, 1);
        return 1;
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to feed watchdog");
        return 2;
    }
}

/**
 * @brief Lua: wdt.stop()
 * @brief 停止看门狗
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int lua_wdt_stop(lua_State* L) {
    (void)L;

    if (iot_wdt_stop()) {
        lua_pushboolean(L, 1);
        return 1;
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to stop watchdog");
        return 2;
    }
}

/**
 * @brief Lua: wdt.is_initialized()
 * @brief 检查看门狗是否已初始化
 * @return 已初始化返回 true，否则返回 false
 */
static int lua_wdt_is_initialized(lua_State* L) {
    (void)L;

    lua_pushboolean(L, iot_wdt_is_initialized());
    return 1;
}

/**
 * @brief Lua: wdt.is_timeout()
 * @brief 检查看门狗是否超时
 * @return 超时返回 true，否则返回 false
 */
static int lua_wdt_is_timeout(lua_State* L) {
    (void)L;

    lua_pushboolean(L, iot_wdt_is_timeout());
    return 1;
}

/* Lua 模块方法表 */
static const luaL_Reg wdt_lib[] = {
    {"init",            lua_wdt_init},
    {"feed",            lua_wdt_feed},
    {"stop",            lua_wdt_stop},
    {"is_initialized",  lua_wdt_is_initialized},
    {"is_timeout",      lua_wdt_is_timeout},
    {NULL, NULL}
};

/**
 * @brief Lua 模块注册函数
 * @param L Lua状态机
 * @return 模块表
 */
int luaopen_wdt_register(lua_State* L) {
    luaL_newlib(L, wdt_lib);
    return 1;
}
