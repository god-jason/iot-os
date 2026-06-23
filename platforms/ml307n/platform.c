/**
 * @file platform.c
 * @brief ML307N 平台初始化实现
 */

#include "platform.h"
#include "lua.h"
#include "iot_base.h"
#include "cm_fota.h"
#include "iot_event.h"
#include "iot_rtos.h"
#include "net_api.h"

/* 网络状态事件常量 */
#define EVENT_IP_READY      "IP_READY"
#define EVENT_IP_LOSE       "IP_LOSE"

/* SIM状态事件常量 */
#define EVENT_SIM_READY         "SIM_READY"
#define EVENT_SIM_PIN_REQUIRED  "SIM_PIN_REQUIRED"
#define EVENT_SIM_PUK_REQUIRED   "SIM_PUK_REQUIRED"
#define EVENT_SIM_REMOVED       "SIM_REMOVED"
#define EVENT_SIM_INSERTED      "SIM_INSERTED"

/* FOTA事件常量 */
#define EVENT_FOTA_START       "FOTA_START"
#define EVENT_FOTA_PROGRESS    "FOTA_PROGRESS"
#define EVENT_FOTA_SUCCESS     "FOTA_SUCCESS"
#define EVENT_FOTA_FAILED      "FOTA_FAILED"

/* 上一次网络状态 */
static uint8_t g_last_net_state = NETDEVCTL_URC_STATE_DEACTIVE;

/* 上一次SIM状态 */
static uint8_t g_last_sim_state = 0;

/* ==================== 网络状态监控 ==================== */

/**
 * @brief 网络状态变化回调
 */
static void net_status_callback(uint8_t cid, uint8_t status)
{
    (void)cid;
    
    LOG("net status=%u, last=%u", status, g_last_net_state);
    
    switch (status) {
        case NETDEVCTL_URC_STATE_ACT:
            /* 网络激活成功 */
            if (g_last_net_state != NETDEVCTL_URC_STATE_ACT) {
                LOG("publish %s", EVENT_IP_READY);
                iot_rtos_publish(EVENT_IP_READY, NULL);
                g_last_net_state = NETDEVCTL_URC_STATE_ACT;
            }
            break;
            
        case NETDEVCTL_URC_STATE_DEACTIVE:
        case NETDEVCTL_URC_STATE_LINK_DIS:
        case NETDEVCTL_URC_STATE_PDP_DIS:
            /* 网络断开 */
            if (g_last_net_state == NETDEVCTL_URC_STATE_ACT) {
                LOG("publish %s", EVENT_IP_LOSE);
                iot_rtos_publish(EVENT_IP_LOSE, NULL);
                g_last_net_state = status;
            }
            break;
            
        default:
            break;
    }
}

/* ==================== SIM状态监控 ==================== */

/* 来自 app_urc_monitor.h */
extern int APP_URC_AddHandler(void (*handler)(const char*, uint32_t));

/**
 * @brief SIM状态URC处理器
 */
static void sim_urc_callback(const char *cmd, uint32_t cmdLen)
{
    (void)cmdLen;
    
    LOG("URC: %.*s", (int)cmdLen, cmd);
    
    /* 解析 +CPIN: READY / SIM PIN / SIM PUK */
    if (strncmp(cmd, "+CPIN:", 6) == 0) {
        const char *status = cmd + 6;
        
        if (strstr(status, "READY") != NULL) {
            if (g_last_sim_state != 1) {
                LOG("publish %s", EVENT_SIM_READY);
                iot_rtos_publish(EVENT_SIM_READY, NULL);
                g_last_sim_state = 1;
            }
        } else if (strstr(status, "SIM PIN") != NULL) {
            if (g_last_sim_state != 2) {
                LOG("publish %s", EVENT_SIM_PIN_REQUIRED);
                iot_rtos_publish(EVENT_SIM_PIN_REQUIRED, NULL);
                g_last_sim_state = 2;
            }
        } else if (strstr(status, "SIM PUK") != NULL) {
            if (g_last_sim_state != 3) {
                LOG("publish %s", EVENT_SIM_PUK_REQUIRED);
                iot_rtos_publish(EVENT_SIM_PUK_REQUIRED, NULL);
                g_last_sim_state = 3;
            }
        }
    }
    
    /* 解析 +CMUSLOT: <slot>,<state> 卡插入状态 */
    if (strncmp(cmd, "+CMUSLOT:", 9) == 0) {
        int slot = 0, state = 0;
        if (sscanf(cmd + 9, "%d,%d", &slot, &state) == 2) {
            if (state == 0) {
                LOG("publish %s", EVENT_SIM_REMOVED);
                iot_rtos_publish(EVENT_SIM_REMOVED, NULL);
                g_last_sim_state = 0;
            } else if (state == 1) {
                LOG("publish %s", EVENT_SIM_INSERTED);
                iot_rtos_publish(EVENT_SIM_INSERTED, NULL);
            }
        }
    }
    
    /* 解析 +CMURDY: <initresult> SIM就绪结果 */
    if (strncmp(cmd, "+CMURDY:", 9) == 0) {
        int result = 0;
        if (sscanf(cmd + 9, "%d", &result) == 1) {
            if (result == 1 && g_last_sim_state != 1) {
                LOG("publish %s", EVENT_SIM_READY);
                iot_rtos_publish(EVENT_SIM_READY, NULL);
                g_last_sim_state = 1;
            }
        }
    }
}

/* ==================== FOTA升级监听 ==================== */

/* FOTA升级状态 */
static volatile uint8_t g_fota_state = 0;  /* 0: idle, 1: downloading, 2: upgrading */

/**
 * @brief FOTA SDK结果回调
 */
static void fota_sdk_result_callback(cm_fota_error_e error)
{
    LOG("FOTA result error=%d", error);
    
    if (error == 0) {
        /* FOTA成功 */
        g_fota_state = 0;
        LOG("publish %s", EVENT_FOTA_SUCCESS);
        iot_rtos_publish(EVENT_FOTA_SUCCESS, NULL);
    } else {
        /* FOTA失败 */
        g_fota_state = 0;
        LOG("publish %s", EVENT_FOTA_FAILED);
        iot_rtos_publish(EVENT_FOTA_FAILED, NULL);
    }
}

/* ==================== 初始化接口 ==================== */

/**
 * @brief 初始化网络状态监控
 */
void iot_event_init(void)
{
    LOG("init event");
    
    /* 注册网络状态回调 */
    NET_MgrNetDevCtlStatusCbRegister(net_status_callback);
    
    /* 注册SIM状态URC处理器 */
    APP_URC_AddHandler(sim_urc_callback);
    
    /* 注册FOTA结果回调 */
    cm_fota_res_callback_register(fota_sdk_result_callback);
    
    LOG("init OK");
}

/**
 * @brief 反初始化网络状态监控
 */
void iot_event_deinit(void)
{
    /* 当前不支持注销回调，预留接口 */
}
