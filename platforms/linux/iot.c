/**
 * @file platform.c
 * @brief Linux 平台初始化实现
 */

#include "iot.h"
#include "iot_wdt.h"

int net_port_set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*===========================================================
 * 事件初始化
 *===========================================================*/

void iot_event_init(void) {
    /* Linux 平台事件初始化 */
    /* TODO: 添加具体实现 */
}

void iot_event_deinit(void) {
    /* Linux 平台事件反初始化 */
    /* TODO: 添加具体实现 */
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
