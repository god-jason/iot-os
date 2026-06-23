/**
 * @file iot_wdt.h
 * @brief 看门狗模块头文件
 *
 * 本模块提供看门狗功能，用于监控 Lua 脚本的执行状态。
 * 当 Lua 脚本长时间未响应时，看门狗会触发超时退出。
 *
 * @author  TRAE
 * @date    2026.06.23
 */

#ifndef IOT_WDT_HEADER
#define IOT_WDT_HEADER

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 初始化看门狗
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 true，失败返回 false
 */
bool iot_wdt_init(uint32_t timeout_ms);

/**
 * @brief 喂狗（重置看门狗计时器）
 * @return 成功返回 true，失败返回 false
 */
bool iot_wdt_feed(void);

/**
 * @brief 等待看门狗超时（阻塞主进程）
 * @note 此函数会阻塞当前线程，直到看门狗超时或被显式停止
 */
void iot_wdt_wait(void);

/**
 * @brief 停止看门狗
 * @return 成功返回 true，失败返回 false
 */
bool iot_wdt_stop(void);

/**
 * @brief 检查看门狗是否已初始化
 * @return 已初始化返回 true，否则返回 false
 */
bool iot_wdt_is_initialized(void);

/**
 * @brief 检查看门狗是否超时
 * @return 超时返回 true，否则返回 false
 */
bool iot_wdt_is_timeout(void);

#endif /* IOT_WDT_HEADER */