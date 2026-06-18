/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file at_api.h
 *
 * @brief AT框架对外的API
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __AT_API_H__
#define __AT_API_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/

#include "os.h"
#include "ap_ps_interface.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AtApi
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
/*
 * 这里定义了AT命令的段，在编译器链接时，会将这些段连接到一起，便于命令分块定义，集中查找
 * 在查找命令时，依次在用户段，MODEM段，middleware段，driver段中查找
 * 如果在多个地方定义了相同的段，最终链接的顺序不确定，所以建议每个段只使用一次
 */
//   用户AT命令段
#define AT_CMD_TABLE_SECTION_USER  __attribute__((section(".at_cmd_table_user")))
//   MODEM AT命令段
#define AT_CMD_TABLE_SECTION_MODEM  __attribute__((section(".at_cmd_table_modem")))
//   middleware AT命令段
#define AT_CMD_TABLE_SECTION_MIDDLEWARE  __attribute__((section(".at_cmd_table_middleware")))
//   driver AT命令段
#define AT_CMD_TABLE_SECTION_DRIVER  __attribute__((section(".at_cmd_table_driver")))
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/************************************************************************************
 *                                 AT命令表项信息
 ************************************************************************************/
typedef struct
{
    const char *name;    // command name, example : "+DEMO"
    /* command api function */
    void (*setFunc) (uint8_t channelId, uint8_t *cmd, uint16_t cmdLen); //  type of command to set value, example : AT+DEMO=1 or  ATE0
    void (*readFunc)(uint8_t channelId); // example : AT+DEMO?
    void (*testFunc)(uint8_t channelId); // example : AT+DEMO=?
    void (*execFunc)(uint8_t channelId); // example : AT+DEMO or ATI  command without parameters
} AT_CommandItem;


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/************************************************************************************
 *@brief                           AT Server端API
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           发送命令响应
 *
 * @param[in]       channelId         通道ID
 * @param[in]       rsp              命令内容指针
 * @param[in]       rspLen           命令内容长度
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendResponse(uint8_t channelId, char *rsp, uint32_t rspLen);
/**
 ************************************************************************************
 * @brief           发送"\r\nOK\r\n"响应
 *
 * @param[in]       channelId         通道ID
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendResponseOK(uint8_t channelId);
/**
 ************************************************************************************
 * @brief           发送"ERROR"响应
 *
 * @param[in]       channelId         通道ID
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendResponseError(uint8_t channelId);
/**
 ************************************************************************************
 * @brief           发送"CME ERROR: XXXX"响应
 *
 * @param[in]       channelId         通道ID
 * @param[in]       errCode           错误码 最大支持到9999
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendResponseCMEError(uint8_t channelId, uint16_t errCode);
/**
 ************************************************************************************
 * @brief           发送"CMS ERROR: XXXX"响应
 *
 * @param[in]       channelId         通道ID
 * @param[in]       errCode           错误码 最大支持到9999
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendResponseCMSError(uint8_t channelId, uint16_t errCode);

/**
 ************************************************************************************
 * @brief           发送命令回显
 *
 * @param[in]       channelId         通道ID
 * @param[in]       echo              回显内容指针
 * @param[in]       echoLen           回显内容长度
 *
 * @return
 ************************************************************************************
*/
int32_t AT_SendEcho(uint8_t channelId, char *echo, uint32_t echoLen);
/**
 ************************************************************************************
 * @brief           发送主动上报命令
 *
 * @param[in]       channelId         通道ID
 * @param[in]       urc              主动上报内容指针
 * @param[in]       urcLen           主动上报内容长度
 *
 * @return
 * @note      channelId为0时表示主动上报广播,非0表示只发送到指定的通道
 ************************************************************************************
*/
int32_t AT_SendUnsolicited(uint8_t channelId, char *urc, uint32_t urcLen);

/************************************************************************************
 *                                 通道管理API
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           通道回调函数
 *
 * @param[in]       channelId    通道ID
 * @param[in]       buf          数据指针
 * @param[in]       bufLen       数据长度
 *
 * @return
 ************************************************************************************
*/
typedef int32_t (*channel_cb)(uint8_t channelId, char *buf, uint32_t bufLen);

/**
 ************************************************************************************
 * @brief           收AT命令时, 通道命令钩子函数
 *
 * @param[in]       channelId    通道ID
 * @param[in]       cmd          数据指针
 * @param[in]       cmdLen       数据长度
 *
 * @return 0表示命令没有被处理, AT框架会继续处理
 ************************************************************************************
*/
typedef int32_t (*rx_cmd_hook_cb)(uint8_t channelId, char *cmd, uint32_t cmdLen);

/**
 ************************************************************************************
 * @brief           发送AT命令响应后的回调函数
 *
 * @param[in]       channelId        通道ID
 * @param[in]       is_result        0：发送命令中间响应, 1:发送命令最终响应
 * @param[in]       user_data       用户注册回调函数时的附加数据
 *
 * @return none
 ************************************************************************************
*/
typedef void (*post_tx_cb)(uint8_t channelId, uint8_t is_result, void *user_data);

/**
 ************************************************************************************
 *@brief                            通道属性定义
 ************************************************************************************
*/
typedef enum{
    AT_CHANNEL_ATTRS_TIMER   = (1<<0),      ///<   通道定时超时机制, 暂未实现
    AT_CHANNEL_ATTRS_NORBUF  = (1<<1),      ///<   通道不使用命令接收缓存,使用者需一次发送一个完整命令
    AT_CHANNEL_ATTRS_NOECHO  = (1<<2),      ///<   通道不支持命令回显, 配置该属性后通道回显关闭, 不受ATE命令控制
    AT_CHANNEL_ATTRS_MASK    = 0xFFFF       ///<   通道属性对外最多支持后16位，前16位内部使用
} AT_ChannelAttribute;

/**
 ************************************************************************************
 *@brief                            通道数据节点定义
 ************************************************************************************
*/
typedef struct
{
    osSlist_t node;     ///<   链表节点
    uint32_t  magicB;    ///<   标记位
    char*   cmd;        ///<   数据指针
    uint32_t cmdLen;    ///<   数据长度
    uint8_t channelId;  ///<   通道ID
    uint8_t dataType;   ///<   数据类型,主动上报,命令中间过程,命令最终响应
    uint8_t urc_status; ///<   主动上报状态
    uint32_t  magicE;    ///<   标记位
}AT_ChannelData;        //   通道链表中保存的数据节点,PS过来的数据会缓存的ChannelData中

/**
 ************************************************************************************
 *@brief                         通道号定义 0和255不可用
 ************************************************************************************
*/
typedef enum{
    AT_CHANNEL_ID_INVALID  = 0,  ///<   无效通道,不可用
    AT_CHANNEL_ID_UART     = 1,  ///<   UART通道
    AT_CHANNEL_ID_SPI      = 2,  ///<   SPI通道
    AT_CHANNEL_ID_APP      = 3,  ///<   应用和OpenCPU通道
    AT_CHANNEL_ID_NET      = 4,  ///<   IP网络通道,接收IP地址与激活结果上报
    AT_CHANNEL_ID_MONITOR  = 5,  ///<   蜂窝网络状态监控,接收主动上报识别网络状态
    AT_CHANNEL_ID_MUX      = 6,  ///<   MUX通道
    AT_CHANNEL_ID_VPORT    = 7,  ///<   虚拟物理通道
    AT_CHANNEL_ID_OC_BEGIN = 20, ///<   OpenCPU发送命令的通道开始
    AT_CHANNEL_ID_OC_END   = 29, ///<   OpenCPU发送命令的通道结束
    AT_CHANNEL_ID_CUSTOM1  = 101,  ///<   客户通道1
    AT_CHANNEL_ID_CUSTOM2  = 102,  ///<   客户通道2
    AT_CHANNEL_ID_CUSTOM3  = 103,  ///<   客户通道3
    AT_CHANNEL_ID_CUSTOM4  = 104,  ///<   客户通道4
    AT_CHANNEL_ID_CUSTOM5  = 105,  ///<   客户通道5
    AT_CHANNEL_ID_MAX      = 255 ///<   uint8_t能表示的最大值,不可用
} AT_ChannelId;

/**
 ************************************************************************************
 *@brief                           通道错误码
 ************************************************************************************
*/
typedef enum{
      AT_CHANNEL_ERR_OK       = 0,   ///<   成功
      AT_CHANNEL_ERR_CLOSE    = -1,  ///<   通道被关闭,可以用Open接口打开通道
      AT_CHANNEL_ERR_USED     = -2,  ///<   通道已经使用,已经被分配
      AT_CHANNEL_ERR_NOTSUP   = -3,  ///<   通道不支持的操作
      AT_CHANNEL_ERR_INVALID  = -4,  ///<   无效通道,通道号不正确
      AT_CHANNEL_ERR_CALLBACK = -5,  ///<   回调函数异常
      AT_CHANNEL_ERR_CMD      = -6,  ///<   命令不正确
      AT_CHANNEL_ERR_BUSY     = -7,  ///<   通道忙，正在处理命令
      AT_CHANNEL_ERR_OVERFLOW = -8,  ///<   通道Buffer溢出
      AT_CHANNEL_ERR_UNDERRUN = -9,  ///<   通道命令没有前缀
      AT_CHANNEL_ERR_ABORTCMD = -10, ///<   通道取消正在处理的命令
} AT_ChannelErrorNo;

/**
 ************************************************************************************
 * @brief           申请一个通道 为通道分配资源
 *
 * @param[in]       channelId                 通道ID
 * @param[in]       at_ReadCmdCB              从通道中读命令回调,AT框架会调用这个API读命令
 * @param[in]       at_WriteResponseCB        通道的命令响应接收回调,AT框架会调用这个API发送命令响应
 * @param[in]       at_WriteUnsolicitedCB     通道的主动上报接收回调,AT框架会调用这个API发送主动上报
 * @param[in]       attrs                     通道属性 参考AT_ChannelAttribute
 *
 * @return
 * @note      如果at_ReadCmdCB为NULL,表示通道的命令通过AT_CmdReceived主动发送给AT框架
 ************************************************************************************
*/
int32_t AT_AllocChannel(uint8_t channelId, channel_cb at_ReadCmdCB, channel_cb at_WriteResponseCB, channel_cb at_WriteUnsolicitedCB, uint32_t attrs);
/**
 ************************************************************************************
 * @brief           释放一个通道
 *
 * @param[in]       channelId                 通道ID
 *
 * @return
 * @note      通道释放时 通道内缓存的命令会一并丢弃
 ************************************************************************************
*/
int32_t AT_FreeChannel(uint8_t channelId);
/**
 ************************************************************************************
 * @brief           打开通道 通道状态变成IDLE 通道可用
 *
 * @param[in]       channelId                 通道ID
 *
 * @return
 * @note      申请通道成功后 在使用通道前需要调用这个API打开通道
 ************************************************************************************
*/
int32_t AT_OpenChannel(uint8_t channelId);
/**
 ************************************************************************************
 * @brief           关闭通道 通道状态变成CLOSE 通道不可用
 *
 * @param[in]       channelId                 通道ID
 *
 * @return
 * @note      通道内缓存的命令会一并丢弃
 ************************************************************************************
*/
int32_t AT_CloseChannel(uint8_t channelId);

/************************************************************************************
 *@brief                          供设备端调用的API
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           设备收到命令后发送给AT框架
 *
 * @param[in]       channelId         通道ID
 * @param[in]       data              命令内容指针
 * @param[in]       dataLen           命令内容长度
 *
 * @return
 * @note 当channel中不注册at_ReceiveCmdCB回调时,命令放在应用的buffer中,此时data指向应用的buffer,需要保证是全局可访问内存,直到AT_CmdReceived接口返回
 ************************************************************************************
*/
int32_t AT_DeviceReceived(uint8_t channelId, char *data, uint32_t dataLen);
/**
 ************************************************************************************
 * @brief           设备发送完命令后通知AT框架
 *
 * @param[in]       channelId         通道ID
 *
 * @return
 * @note 通道AT框架发送给设备的数据,设备通知AT框架发送完成数据收到,便于AT框架释放内存
 ************************************************************************************
*/
int32_t AT_DeviceSendDone(uint8_t channelId);

/**
 ************************************************************************************
 * @brief           配置收AT命令时的钩子函数给AT框架
 *
 * @param[in]       hook         钩子函数指针
 *
 * @return
 * @note
 * 1 在AT框架收到完整的命令后会先调用hook函数, 当hook返回0时表示命令由框架继续处理
 * 2 整个框架只保存一个这样的钩子函数
 ************************************************************************************
*/
void AT_RxCmdHookSet(rx_cmd_hook_cb hook);

/**
 ************************************************************************************
 * @brief           配置AT通道为短信模式
 *
 * @param[in]       channelId         通道ID
 * @param[in]       dataReceiveCB     短信模式下接收数据的回调函数
 *
 * @return
 * @note 短信模式下，回复了命令的最终响应后会自动退出短信模式
 ************************************************************************************
*/
int32_t AT_ChannelSetSMSMode(uint8_t channelId, channel_cb SMSReceiveCB);

/**
 ************************************************************************************
 * @brief           配置发送AT命令响应后的回调函数给AT框架
 *
 * @param[in]       channelId    通道ID
 * @param[in]       callback     回调函数指针
 * @param[in]       user_data    用户附加数据

 * @return
 * @note
 * 1 AT框架在发送完命令响应后回调callback函数, 并传入user_data
 * 2 通道在发送完最终响应后, 会自动清理回调函数及参数
 * 3 该接口只能在AT框架任务中调用
 ************************************************************************************
*/
int32_t AT_ChannelPostTxRspCBSet(uint8_t channelId, post_tx_cb callback, void *user_data);
/**
 ************************************************************************************
 * @brief           数据透传模式下通道事件定义
 ************************************************************************************
*/
typedef enum{
    AT_CHANNEL_DATAMODE_EVENT_NONE = 0,
    AT_CHANNEL_DATAMODE_EVENT_RX,            ///<   透传模式下收到数据
    AT_CHANNEL_DATAMODE_EVENT_TXDONE,        ///<   透传模式下数据发送完成
} AT_ChannelDataEvent;

typedef enum{
    AT_CHANNEL_DATAMODE_RX_ERROR = -1,       ///<   透传模式错误, 不会再从设备读数据
    AT_CHANNEL_DATAMODE_RX_STOP = 0,         ///<   透传模式停止收数据, 不会再从设备读数据
    AT_CHANNEL_DATAMODE_RX_GOON,             ///<   透传模式继续收数据, 会继续从设备读数据
} AT_ChannelDataRX;

/**
 ************************************************************************************
 * @brief           数据透传模式下的通道回调函数
 *
 * @param[in]       channelId    通道ID
 * @param[in]       event        事件ID, 参考AT_ChannelDataEvent的定义
 * @param[in]       buf          数据指针
 * @param[in]       bufLen       数据长度
 *
 * @return  参考 AT_ChannelDataRX 定义
 ************************************************************************************
*/
typedef int32_t (*channel_datamode_cb)(uint8_t channelId, uint8_t eventId, char* buf, uint32_t bufLen);

/**
 ************************************************************************************
 * @brief           配置AT通道为透传模式
 *
 * @param[in]       channelId         通道ID
 * @param[in]       dataReceiveCB     透传模式下接收数据的回调函数
 *
 * @return
 * @note
 *   为了保证模式切换前的数据可以发送完成, 这里采用消息排队发给TASK处理, 而不是直接设置状态
 *   为保证数据无残留, 调用该接口需要在命令接收完整之后, 在最终响应调用发送之前, 因为在这期间不会从设备读数据
 *   调用该接口会触发从设备读数据
 *   在从设备读到数据后会通过dataReceiveCB给对应模块, 该接口返回值大于0表示要继续从设备读数据
 ************************************************************************************
*/
int32_t AT_ChannelSetDataMode(uint8_t channelId, channel_datamode_cb dataReceiveCB);

/**
 ************************************************************************************
 * @brief           配置AT通道退出透传模式
 *
 * @param[in]       channelId         通道ID
 *
 * @return
 * @note
 ************************************************************************************
*/
int32_t AT_ChannelClearDataMode(uint8_t channelId);

/**
 ************************************************************************************
 * @brief           在数据透传模式下向通道发送数据
 *
 * @param[in]       channelId        通道ID
 * @param[in]       data             数据内容指针
 * @param[in]       dataLen          数据内容长度
 *
 * @return
 * @note      在用API AT_ChannelSetDataMode将通道切成数据透传模式后需要用这个API向通道发数据
 ************************************************************************************
*/
int32_t AT_SendData(uint8_t channelId, char *data, uint32_t dataLen);

/**
 ************************************************************************************
 * @brief           发送PPP数据
 *
 * @param[in]       data              数据内容指针
 * @param[in]       dataLen           数据内容长度
 *
 * @return 该API会阻塞直到data数据发送完成
 ************************************************************************************
*/
int32_t AT_PPP_SendData(uint8_t *data, uint32_t dataLen);

/**
 ************************************************************************************
 * @brief           配置通道进入PPP透传模式
 *
 * @param[in]       channelId         通道ID
 * @param[in]       dataReceiveCB     透传模式下接收数据的回调函数
 *
 * @return
 ************************************************************************************
*/
int32_t AT_ChannelSetPPPMode(uint8_t channelId, channel_datamode_cb dataReceiveCB);

/**
 ************************************************************************************
 * @brief           配置通道退出PPP透传模式
 *
 * @param[in]       channelId         通道ID
 *
 * @return
 ************************************************************************************
*/
int32_t AT_ChannelClearPPPMode(uint8_t channelId);

/**
 ************************************************************************************
 * @brief           AT 模块准备就绪
 *
 * @param[in]       none
 *
 * @return   true : 准备完成，可以收发命令   false : 没有准备好
 ************************************************************************************
*/
bool AT_ModuleisReady(void);

/**
 ************************************************************************************
 * @brief           AT框架支持的配置项,          配置值不保存NV
 ************************************************************************************
*/
typedef enum{
    AT_G_CFG_ATE       = 0,             ///<   配置ATE功能的全局默认值,             默认值0,      仅支持(0,1)
    AT_G_CFG_CMEE,                      ///<   配置CMEE功能的全局默认值,             默认值1,      仅支持(0,1)
    AT_G_CFG_MAX_URC_IN_DATAMODE,       ///<   配置数据透传模式下缓存的最大主动上报个数,默认值20,                    支持范围[0-255]，0表示不缓存URC，全部丢弃处理
} AT_GlobalConfigureItem;

/**
 ************************************************************************************
 * @brief           AT 模块全局配置设置
 *
 * @param[in]       配置项, 参考 AT_GlobalConfigureItem 定义
 * @param[in]       配置项对应的值
 *
 * @return   osOK : 设置成功,   其他  ：设置失败
 ************************************************************************************
*/
int32_t AT_GlobalConfigureSet(AT_GlobalConfigureItem item, int32_t value);

/**
 ************************************************************************************
 * @brief           AT 模块全局配置获取
 *
 * @param[in]       配置项, 参考 AT_GlobalConfigureItem 定义
 * @param[out]      配置项对应的值
 *
 * @return   osOK : 获取成功,   其他  ：获取失败
 ************************************************************************************
*/
int32_t AT_GlobalConfigureGet(AT_GlobalConfigureItem item, int32_t *value);

typedef enum{
    AT_URC_RI_NORMAL       = 0,             ///<   主动上报正常，立即发送
    AT_URC_RI_DROP,                         ///<   主动上报丢弃，不需要发送
    AT_URC_RI_DELAY,                        ///<   主动上报延时，超时后发送
} AT_ChannelURC_RIResult;

/**
 ************************************************************************************
 * @brief           AT 模块设置主动上报通知回调
 *
 * @param[in]       channelId         通道ID
 * @param[in]       urc_indicate      主动上报通知回调
 *
 * @return   osOK : 设置成功,   其他  ：设置失败
 * @note  主动上报通知回调返回值意义参考 AT_ChannelURC_RIResult
 ************************************************************************************
*/
int32_t AT_ChannelSetURCIndicateCB(uint8_t channelId, channel_cb urc_indicate);

/**
 ************************************************************************************
 * @brief           AT 模块通知AT框架对应通道RI超时
 *
 * @param[in]       channelId         通道ID
 *
 * @return   osOK : 设置成功,   其他  ：设置失败
 ************************************************************************************
*/
int32_t AT_ChannelUnsolicitedIndicateTimeOut(uint8_t channelId);
#ifdef __cplusplus
}
#endif
#endif /* __AT_API_H__ */
/** @} */

