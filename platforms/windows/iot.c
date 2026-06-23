/**
 * @file platform.c
 * @brief Windows 平台初始化实现
 */

#include "iot.h"
#include "iot_task.h"
#include "iot_wdt.h"
#include <malloc.h>

int net_port_set_nonblocking(int fd)
{
    u_long mode = 1;
    return ioctlsocket((iot_socket_t)fd, FIONBIO, &mode);
}

/*===========================================================
 * Timer 封装实现
 *===========================================================*/

/**
 * @brief 定时器回调proxy，标记执行状态后调用用户回调
 * @param param 定时器上下文
 * @param flags Windows定时器标志（未使用）
 */
static void __stdcall timer_proxy(void* param, unsigned char flags)
{
    iot_timer_ctx_t* ctx = (iot_timer_ctx_t*)param;
    InterlockedExchange(&ctx->executed, 1);
    (void)flags;
    if (ctx->user_cb) {
        ctx->user_cb(ctx->user_arg);
    }
}

/**
 * @brief 创建定时器
 */
iot_timer_t iot_timer_create(iot_timer_callback_t cb, void* arg, int period_ms, int type)
{
    iot_timer_t ctx = (iot_timer_t)malloc(sizeof(struct iot_timer_ctx));
    if (!ctx) {
        return NULL;
    }

    ctx->executed = 0;
    ctx->timer = NULL;
    ctx->user_cb = cb;
    ctx->user_arg = arg;

    HANDLE timer = NULL;
    DWORD period = (type == IOT_TIMER_PERIODIC) ? (DWORD)period_ms : 0;
    BOOL ret = CreateTimerQueueTimer(&timer, NULL, timer_proxy, ctx,
                                     (DWORD)period_ms, period, WT_EXECUTEDEFAULT);
    if (!ret) {
        free(ctx);
        return NULL;
    }

    ctx->timer = timer;
    return ctx;
}

/**
 * @brief 启动定时器（Windows定时器创建即启动，此函数仅作兼容）
 */
int iot_timer_start(iot_timer_t timer, int period_ms)
{
    (void)timer;
    (void)period_ms;
    return 0;
}

/**
 * @brief 停止定时器（如果回调已执行则直接返回）
 */
int iot_timer_stop(iot_timer_t timer)
{
    if (!timer || !timer->timer) {
        return -1;
    }
    if (timer->executed) {
        return 0;  /* 回调已执行，跳过 */
    }
    DeleteTimerQueueTimer(NULL, timer->timer, NULL);
    timer->timer = NULL;
    return 0;
}

/**
 * @brief 删除定时器（如果回调已执行则直接返回）
 */
void iot_timer_delete(iot_timer_t timer)
{
    if (!timer) {
        return;
    }
    if (timer->executed) {
        /* 回调已执行，定时器已自动删除，只需释放上下文 */
        free(timer);
        return;
    }
    if (timer->timer) {
        DeleteTimerQueueTimer(NULL, timer->timer, NULL);
        timer->timer = NULL;
    }
    free(timer);
}

/**
 * @brief 检查定时器是否运行
 */
int iot_timer_is_running(iot_timer_t timer)
{
    if (!timer || !timer->timer) {
        return 0;
    }
    return timer->executed == 0 ? 1 : 0;
}

/*===========================================================
 * Lua 输出函数适配
 *===========================================================*/

void osPuts(const char* s) {
    fputs(s, stdout);
}

/*===========================================================
 * 事件初始化
 *===========================================================*/

void iot_event_init(void) {
    /* Windows 平台事件初始化（空实现） */
}

void iot_event_deinit(void) {
    /* Windows 平台事件反初始化（空实现） */
}

/*===========================================================
 * 主入口函数
 *===========================================================*/

int main(void) {
    /* 启动 IoT 系统 */
    iot_start();
    
    /* 等待看门狗超时（阻塞主进程） */
    iot_wdt_wait();
    
    return 0;
}
