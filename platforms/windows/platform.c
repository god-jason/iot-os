/**
 * @file platform.c
 * @brief Windows 平台初始化实现
 */

#include "platform.h"

/*===========================================================
 * 事件初始化
 *===========================================================*/

void iot_event_init(void) {
    /* Windows 平台事件初始化（空实现） */
}

void iot_event_deinit(void) {
    /* Windows 平台事件反初始化（空实现） */
}

// 测试事件初始化
int main() {

    //启动事件循环
    iot_start();

    return 0;
}
