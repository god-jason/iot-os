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
#include <stdio.h>
#include <stdlib.h>

/* 看门狗状态结构 */
typedef struct {
    bool initialized;       /**< 是否已初始化 */
    bool timeout;           /**< 是否超时 */
    uint32_t timeout_ms;    /**< 超时时间（毫秒） */
    iot_mutex_t mutex;      /**< 互斥锁 */
    iot_sem_t sem;          /**< 信号量，用于等待超时 */
    iot_task_t task;        /**< 看门狗任务 */
} iot_wdt_t;

static iot_wdt_t g_wdt = {0};

/**
 * @brief 看门狗任务函数
 * @param arg 参数（未使用）
 */
static void iot_wdt_task(void* arg) {
    (void)arg;
    
    while (1) {
        /* 等待超时 */
        iot_sem_wait(g_wdt.sem, g_wdt.timeout_ms);
        
        iot_mutex_lock(g_wdt.mutex);
        
        if (g_wdt.initialized) {
            /* 超时处理 */
            g_wdt.timeout = true;
            printf("[WDT] Watchdog timeout triggered!\n");
            
            /* 退出应用程序 */
            exit(1);
        }
        
        iot_mutex_unlock(g_wdt.mutex);
        
        /* 如果未初始化，退出任务 */
        if (!g_wdt.initialized) {
            break;
        }
    }
    
    iot_task_delete(NULL);
}

bool iot_wdt_init(uint32_t timeout_ms) {
    if (g_wdt.initialized) {
        printf("[WDT] Watchdog already initialized\n");
        return false;
    }
    
    if (timeout_ms == 0) {
        printf("[WDT] Invalid timeout value\n");
        return false;
    }
    
    /* 初始化互斥锁 */
    if (iot_mutex_init(&g_wdt.mutex) != 0) {
        printf("[WDT] Failed to initialize mutex\n");
        return false;
    }
    
    /* 初始化信号量 */
    if (iot_sem_init(&g_wdt.sem, 0) != 0) {
        printf("[WDT] Failed to initialize semaphore\n");
        iot_mutex_deinit(&g_wdt.mutex);
        return false;
    }
    
    /* 设置超时时间 */
    g_wdt.timeout_ms = timeout_ms;
    g_wdt.timeout = false;
    g_wdt.initialized = true;
    
    /* 创建看门狗任务 */
    if (iot_task_create(&g_wdt.task, "wdt", iot_wdt_task, NULL, 2048, 5) != 0) {
        printf("[WDT] Failed to create watchdog task\n");
        iot_sem_deinit(&g_wdt.sem);
        iot_mutex_deinit(&g_wdt.mutex);
        g_wdt.initialized = false;
        return false;
    }
    
    printf("[WDT] Watchdog initialized with timeout %u ms\n", timeout_ms);
    return true;
}

bool iot_wdt_feed(void) {
    if (!g_wdt.initialized) {
        printf("[WDT] Watchdog not initialized\n");
        return false;
    }
    
    iot_mutex_lock(g_wdt.mutex);
    
    /* 重置超时状态 */
    g_wdt.timeout = false;
    
    /* 发送信号量，重置计时器 */
    iot_sem_post(g_wdt.sem);
    
    iot_mutex_unlock(g_wdt.mutex);
    
    return true;
}

void iot_wdt_wait(void) {
    if (!g_wdt.initialized) {
        printf("[WDT] Watchdog not initialized, cannot wait\n");
        return;
    }
    
    printf("[WDT] Waiting for watchdog timeout...\n");
    
    /* 阻塞等待看门狗超时 */
    while (1) {
        iot_task_delay(100);
        
        iot_mutex_lock(g_wdt.mutex);
        if (g_wdt.timeout) {
            iot_mutex_unlock(g_wdt.mutex);
            break;
        }
        iot_mutex_unlock(g_wdt.mutex);
    }
    
    printf("[WDT] Watchdog wait ended\n");
}

bool iot_wdt_stop(void) {
    if (!g_wdt.initialized) {
        printf("[WDT] Watchdog not initialized\n");
        return false;
    }
    
    iot_mutex_lock(g_wdt.mutex);
    
    /* 标记为未初始化 */
    g_wdt.initialized = false;
    
    /* 发送信号量，唤醒任务 */
    iot_sem_post(g_wdt.sem);
    
    iot_mutex_unlock(g_wdt.mutex);
    
    /* 等待任务退出 */
    iot_task_delay(100);
    
    /* 清理资源 */
    iot_sem_deinit(&g_wdt.sem);
    iot_mutex_deinit(&g_wdt.mutex);
    
    printf("[WDT] Watchdog stopped\n");
    return true;
}

bool iot_wdt_is_initialized(void) {
    return g_wdt.initialized;
}

bool iot_wdt_is_timeout(void) {
    bool timeout;
    
    iot_mutex_lock(g_wdt.mutex);
    timeout = g_wdt.timeout;
    iot_mutex_unlock(g_wdt.mutex);
    
    return timeout;
}

/*===========================================================
 * Lua 接口实现
 *===========================================================*/

#ifdef IOT_ENABLE_LUA_WDT

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "iot_log.h"

/**
 * @brief Lua: wdt.init(timeout_ms)
 * @brief 初始化看门狗
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

#endif /* IOT_ENABLE_LUA_WDT */