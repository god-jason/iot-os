/**
 * @file iot_sms.h
 * @brief YOPEN 平台 SMS 短信适配器头文件
 * @details 基于 yopen_sms 接口实现短信发送与读取功能。
 */

#ifndef IOT_SMS_YOPEN_H
#define IOT_SMS_YOPEN_H

#include "../../iot_types.h"

/* 引入 yopen 平台 SMS 头文件 */
#include "yopen_sms.h"

/* ============================================================
 * SMS 宏定义
 * ============================================================ */

/**
 * @brief 初始化SMS模块
 * @return 0 成功，<0 失败
 */
#define iot_sms_init() \
    yopen_sms_init()

/**
 * @brief 去初始化SMS模块
 * @return 0 成功，<0 失败
 */
#define iot_sms_deinit() \
    yopen_sms_deinit()

/**
 * @brief 发送短信
 * @param[in] number 电话号码
 * @param[in] content 短信内容
 * @param[in] charset 编码格式
 * @return 0 成功，<0 失败
 */
#define iot_sms_send(number, content, charset) \
    yopen_sms_send_message((const char *)(number), (const char *)(content), (yopen_sms_chset_e)(charset))

/**
 * @brief 读取短信
 * @param[in] index 短信索引
 * @param[out] info 短信信息结构体
 * @return 0 成功，<0 失败
 */
#define iot_sms_read(index, info) \
    yopen_sms_read_message((uint32_t)(index), (yopen_sms_entry_t *)(info))

/**
 * @brief 删除短信
 * @param[in] index 短信索引
 * @param[in] storage 存储位置
 * @return 0 成功，<0 失败
 */
#define iot_sms_delete(index, storage) \
    yopen_sms_delete_message((uint32_t)(index), (yopen_sms_storage_e)(storage))

/**
 * @brief 获取短信数量
 * @param[out] total 总短信数
 * @param[out] unread 未读短信数
 * @return 0 成功，<0 失败
 */
int32_t iot_sms_get_count(uint32_t *total, uint32_t *unread);

#endif /* IOT_SMS_YOPEN_H */
