/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_kpd.h
 *
 * @brief       keypad驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRIVER_KPD_H_
#define DRIVER_KPD_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup Kpd
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/
#define KPD_ROW_NUM     0x8UL
#define KPD_COL_NUM     0x9UL
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define KPD_FLAG_INITIALIZED ((uint8_t)(1U))
#define KPD_FLAG_POWERED     ((uint8_t)(1U << 1))

#define KPD_KEY_QUEUE_SIZE (1 << 5)


/****** UART Event *****/
#define KPD_EVENT_CHNAGED        (1UL << 0)  ///< pressed or released
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/**
  \brief 按键状态
*/
typedef enum
{
  KEY_PRESSED = 0,
  KEY_RELEASED,
} KEY_STATUS;

/**
 * @brief 按键值
 *
 */
typedef enum
{
  KPD_VK_00 = 1,
  KPD_VK_10,
  KPD_VK_20,
  KPD_VK_30,
  KPD_VK_40,
  KPD_VK_50,
  KPD_VK_60,
  KPD_VK_70,

  KPD_VK_01,
  KPD_VK_11,
  KPD_VK_21,
  KPD_VK_31,
  KPD_VK_41,
  KPD_VK_51,
  KPD_VK_61,
  KPD_VK_71,

  KPD_VK_02,
  KPD_VK_12,
  KPD_VK_22,
  KPD_VK_32,
  KPD_VK_42,
  KPD_VK_52,
  KPD_VK_62,
  KPD_VK_72,

  KPD_VK_03,
  KPD_VK_13,
  KPD_VK_23,
  KPD_VK_33,
  KPD_VK_43,
  KPD_VK_53,
  KPD_VK_63,
  KPD_VK_73,

  KPD_VK_04,
  KPD_VK_14,
  KPD_VK_24,
  KPD_VK_34,
  KPD_VK_44,
  KPD_VK_54,
  KPD_VK_64,
  KPD_VK_74,

  KPD_VK_05,
  KPD_VK_15,
  KPD_VK_25,
  KPD_VK_35,
  KPD_VK_45,
  KPD_VK_55,
  KPD_VK_65,
  KPD_VK_75,

  KPD_VK_06,
  KPD_VK_16,
  KPD_VK_26,
  KPD_VK_36,
  KPD_VK_46,
  KPD_VK_56,
  KPD_VK_66,
  KPD_VK_76,

  KPD_VK_07,
  KPD_VK_17,
  KPD_VK_27,
  KPD_VK_37,
  KPD_VK_47,
  KPD_VK_57,
  KPD_VK_67,
  KPD_VK_77,

  KPD_VK_08,
  KPD_VK_18,
  KPD_VK_28,
  KPD_VK_38,
  KPD_VK_48,
  KPD_VK_58,
  KPD_VK_68,
  KPD_VK_78,
  KPD_VK_MAX,
} KPD_VIRTUAL_KEY;

/**
  \brief 回调函数
  */
typedef void (*kpdHook_t)(KPD_VIRTUAL_KEY id, KEY_STATUS event, void *ctx);

/**
 * @brief 按键信息
 *
 */
typedef struct
{
  KPD_VIRTUAL_KEY key;
  uint8_t         state;
  uint32_t        times;
  kpdHook_t callback;
  void *ctx;
} kpd_key_item;

/**
 * @brief 按键队列
 *
 */
typedef struct
{
  uint8_t      head;                        /* position of the first valid data */
  uint8_t      tail;                        /* position of the last valid data */
  uint8_t      count;                       /* number of keys in queue */
  kpd_key_item items[ KPD_KEY_QUEUE_SIZE ]; /* store the key information */
} kpd_key_queue;

/**
  \brief 回调函数
  */
typedef void (*KPD_Callback)(void *KPD, uint32_t event); ///< Pointer to \ref UART_SignalEvent : Signal UART Event.


/**
  \brief 按键矩阵状态
*/
typedef struct
{
    uint8_t   report;
    uint8_t   current;
    kpdHook_t callback;
    void *ctx;
} Kpd_MatrixStatus;

typedef struct{
    KPD_Callback                          cb_event;
    Kpd_MatrixStatus    keyMatrix[ KPD_ROW_NUM][KPD_COL_NUM];
    kpd_key_queue    key_queue;
    uint8_t            flags;        // Current kpd flags
    uint8_t         row;
    uint8_t         col;
    KEY_STATUS key_status;
    KPD_VIRTUAL_KEY key_value;
    struct osSemaphore kpdSema;
    struct osMutex kpdQueueMutex;
} KPD_INFO;

/**
 \brief Access structure of UART_Handle.
*/
typedef struct
{
    const struct KEY_Res  *pRes;
    /* 高层使用定义 */
    KPD_INFO info;
    void    *userData;
} KPD_Handle;
/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           初始化KPD控制器
 *
 * @param[in]       KPD            UART控制器句柄
 *
 * @return          int32_t
 * @retval          DRV_OK          成功
 ************************************************************************************
*/
extern int32_t KPD_Initialize(KPD_Handle *KPD, KPD_Callback cb_event);

/**
 ************************************************************************************
 * @brief           设置KPD控制器的供电
 *
 * @param[in]       KPD            KPD控制器句柄
 * @param[in]       state           设置控制的状态
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR_UNSUPPORTED           不支持
 *                  DRV_ERR                       错误
 *                  DRV_OK                        成功
 ************************************************************************************
*/
extern int32_t KPD_PowerControl(KPD_Handle *KPD, DRV_POWER_STATE state);
/**
 ************************************************************************************
 * @brief           获取信息
 *
 * @param[in]       KPD            KPD控制器句柄
 * @param[in]       buffer         二维数组指针
 *@param[in]        size           长度
 * @return          void
 * @retval          void       按键按下的数量
 ************************************************************************************
*/
uint32_t KPD_KeyStatus(KPD_Handle *KPD, void *buffer, uint32_t size);

/**
 ************************************************************************************
 * @brief           注册按键回调
 *
 * @param[in]       KPD            KPD控制器句柄
 * @param[in]       hook_func      回调函数
 * @param[in]       ctx            回调入参
 * @return          void
 * @retval          void
 ************************************************************************************
*/
void KPD_Listen(KPD_Handle *KPD, KPD_VIRTUAL_KEY id, kpdHook_t hook_func, void *ctx);

#endif // DRIVER_KPD_H_

/** @} */