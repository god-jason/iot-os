/**
 * @file iot_event.h
 * @brief 系统事件模块
 *
 * 本模块提供系统事件的注册和监听功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_EVENT_HEADER
#define IOT_EVENT_HEADER

#include <stdint.h>

/**
 * @brief 初始化网络状态监听
 */
void iot_event_init(void);

/**
 * @brief 反初始化网络状态监听
 */
void iot_event_deinit(void);

#endif /* IOT_EVENT_HEADER */
