/**
 * @file iot_sms.c
 * @brief ML307N 平台短信适配器实现
 * @details 基于 cm_sms 接口实现短信功能封装，
 *          提供统一的短信操作函数。
 */

#include "iot_sms.h"

/**
 * @brief 设置短信编码格式
 * @param[in] charset 编码格式枚举值
 * @return 0 成功，-1 失败
 */
int32_t iot_sms_set_charset(iot_sms_charset_t charset)
{
    return cm_sms_set_charset((cm_sms_charset_e)charset);
}

/**
 * @brief 获取短信编码格式
 * @return 编码格式枚举值
 */
iot_sms_charset_t iot_sms_get_charset(void)
{
    return cm_sms_get_charset();
}

/**
 * @brief 设置短信中心号码
 * @param[in] sca 短信中心号码字符串（不超过20字节）
 * @return 0 成功，-1 失败
 */
int32_t iot_sms_set_sca(const char *sca)
{
    return cm_sms_set_sca((char *)sca);
}

/**
 * @brief 获取短信中心号码
 * @param[out] sca 短信中心号码缓冲区（>=21字节）
 * @return >=0 号码字符串长度，<0 失败
 */
int32_t iot_sms_get_sca(char *sca)
{
    return cm_sms_get_sca(sca);
}

/**
 * @brief 发送短信
 * @param[in] phone_num 目标手机号码
 * @param[in] text      短信内容
 * @return 0 成功，-1 失败
 */
int32_t iot_sms_send(const char *phone_num, const char *text)
{
    return cm_sms_send((char *)phone_num, (char *)text);
}

/**
 * @brief 删除指定位置的短信
 * @param[in] storage 存储位置枚举值
 * @param[in] index   短信索引
 * @return 0 成功，-1 失败
 */
int32_t iot_sms_delete(iot_sms_storage_t storage, uint32_t index)
{
    return cm_sms_delete((cm_sms_storage_e)storage, index);
}

/**
 * @brief 读取指定位置的短信
 * @param[in]  storage 存储位置枚举值
 * @param[in]  index   短信索引
 * @param[out] info    短信信息结构体指针
 * @return 0 成功，-1 失败
 */
int32_t iot_sms_read(iot_sms_storage_t storage, uint32_t index, iot_sms_info_t *info)
{
    return cm_sms_read((cm_sms_storage_e)storage, index, (cm_sms_info_t *)info);
}