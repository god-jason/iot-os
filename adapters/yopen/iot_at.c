/**
 * @file iot_at.c
 * @brief YOPEN 平台 AT指令适配器实现
 * @details 基于 yopen_ril 接口实现 AT 指令操作，
 *          支持同步/异步发送、URC 注册等功能。
 */

#include "iot_at.h"
#include <string.h>

/* ============================================================
 * AT 操作函数实现
 * ============================================================ */

/**
 * @brief AT模块初始化
 * @param[in] urc_table URC注册表
 * @param[in] table_size URC表大小
 * @return 0 成功，<0 失败
 */
int32_t iot_at_init_ex(const iot_at_urc_table_t *urc_table, uint32_t table_size)
{
    int ret;
    uint32_t i;
    
    ret = yopen_ril_init(NULL);
    if (ret != 0) {
        return ret;
    }
    
    /* 注册URC表 */
    if (urc_table != NULL && table_size > 0) {
        for (i = 0; i < table_size; i++) {
            if (urc_table[i].prefix != NULL && urc_table[i].callback != NULL) {
                yopen_ril_register_urc(urc_table[i].prefix, 
                                       (yopen_ril_urc_callback)urc_table[i].callback, 
                                       urc_table[i].user_data);
            }
        }
    }
    
    return 0;
}

/**
 * @brief AT发送同步命令并等待响应
 * @param[in] cmd     AT指令字符串
 * @param[in] cmd_len 指令长度
 * @param[out] rsp     响应缓冲区
 * @param[in] rsp_len 响应缓冲区长度
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return >=0 响应长度，<0 失败
 */
int32_t iot_at_send_sync(const char *cmd, uint32_t cmd_len, char *rsp, uint32_t rsp_len, uint32_t timeout_ms)
{
    return yopen_ril_send_atcmd_sync(cmd, cmd_len, rsp, rsp_len, timeout_ms);
}

/**
 * @brief 注册URC主动上报回调
 * @param[in] prefix URC前缀，如"+CSQ:"
 * @param[in] cb     回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_at_register_urc(const char *prefix, iot_at_urc_callback_t cb, void *user_data)
{
    return yopen_ril_register_urc(prefix, (yopen_ril_urc_callback)cb, user_data);
}

/**
 * @brief 注销URC主动上报回调
 * @param[in] prefix URC前缀
 * @return 0 成功，<0 失败
 */
int32_t iot_at_unregister_urc(const char *prefix)
{
    return yopen_ril_unregister_urc(prefix);
}