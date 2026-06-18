#ifndef     _BUS_CONFIG_H
#define     _BUS_CONFIG_H

/************************************************************************************
 *                                 全局宏
 ************************************************************************************/
#define BUS_MACRO_ON                    (1)
#define BUS_MACRO_OFF                   (0)

/************************************************************************************
 *                                 MUX宏开关
 ************************************************************************************/
#define BUSMUX_KEEP_ALIVE_ENABLE        (BUS_MACRO_ON)
#define BUSMUX_CORE_FRAME_ENABLE        (BUS_MACRO_ON)  // 是否使用帧结构
#define BUSMUX_EXCEPTION_CATCH_ENABLE   (BUS_MACRO_ON)  // 是否检查空指针回调
#define BUSMUX_PING_ENABLE              (BUS_MACRO_ON)
#define BUSMUX_STATIS_ENABLE            (BUS_MACRO_ON)

/************************************************************************************
 *                                 FRAME宏开关
 ************************************************************************************/
#define BUS_FRAME_CRC_ENABLE            (BUS_MACRO_ON)  // 是否开启CRC校验
#define BUS_RRAME_CRC_FAST_ENABLE       (BUS_MACRO_OFF) // 是否使用快速CRC校验，耗费更大的内存
#define BUS_FRAME_CRC_HW_ENABLE         (BUS_MACRO_ON)

/************************************************************************************
 *                                 TRACE宏开关
 ************************************************************************************/
#define BUS_TRACE_USE_EXT_LOG           (BUS_MACRO_OFF)  // 是否使用外部LOG工具
#define BUS_TRACE_USE_RAM_LOG           (BUS_MACRO_ON)  // 是否使用RAM LOG工具


/************************************************************************************
 *                                 MUX宏定义
 ************************************************************************************/
#define BUSMUX_PRIORITY_MAX             (8)         // 通道最大优先级(0 ~ BUSMUX_PRIORITY_MAX - 1)
#define BUSMUX_BUFFER_NUM_MAX           (512)       // 缓冲的最大数据个数
#define BUSMUX_BUFFER_MODIFY_NUM        (32)        // 每次申请（释放）动态内存池的个数

#define BUSMUX_EVENT_MSG_NUMBER_MAX     (FRAME_PACKET_MAX + 2)      // 多分配两个余量
#define BUSMUX_AGGRE_MAX                (FRAME_PACKET_MAX)

#if BUSMUX_PING_ENABLE
#define BUSMUX_CHANNEL_ID_MAX           (0xe)
#define BUSMUX_PING_CHANNEL             (0xf)
#define BUSMUX_PING_PRIORITY            (BUSMUX_PRIORITY_MAX - 1)
#define BUSMUX_PING_NAME                "ping"
#define BUSMUX_PING_VAL                 (0x5a)
#define BUSMUX_PING_LEN                 (32)
#define BUSMUX_KEEP_ALIVE_MSG           "alive"
#else
#define BUSMUX_CHANNEL_ID_MAX           (0xf)
#endif

/************************************************************************************
 *                                 MUX优先级调度策略
 ************************************************************************************/
#define BUSMUX_PRIORITY_LOW_PREMPTION_WAITING_CYCLE (5)     // 低优先级允许被抢占的最大次数，超过这个次数即便有高优先级数据，也发送当前超时的低优先级（数字越小，高优先级实时性越低）
#define BUSMUX_PRIORITY_FORCE_AGGRE_CYCLE           (8)     // 高优先级强制组包，高优先级轮空次数，推荐值：4（数字越大，高优先级实时性越低）
#define BUSMUX_PRIORITY_FORCE_AGGRE_HIGH_LEN        (8)     // 高优先级强制组包，要求低优先级缓存的包个数，推荐值：4（数字越大，高优先级实时性越低）
#define BUSMUX_PRIORITY_FORCE_AGGRE_LOW_LEN         (8)     // 高优先级强制组包，要求低优先级缓存的包个数，超过这几个才允许发送低优先级（数字越小，高优先级实时性越低）

/************************************************************************************
 *                                 FRAME宏定义
 ************************************************************************************/
#define FRAME_HEADER                    (0xf9)      // 帧头
#define FRAME_PACKET_MAX                (8)         // 一个帧最大组包数
#define FRAME_LEN_MAX                   (8000)      // 一个帧最大的长度

#define FRAME_TOTAL_ALIGNED_LEN         (4)         // 一个帧的对齐长度，为0则表示不需要对齐
#define FRAME_NODE_ALIGNED_LEN          (0)         // 每个节点的对齐长度，为0则表示不需要对齐

#if BUS_FRAME_CRC_ENABLE
#define FRAME_CRC_LEN                   (2)         // CRC长度
#else
#define FRAME_CRC_LEN                   (0)
#endif

#if BUS_FRAME_CRC_ENABLE && (!BUS_FRAME_CRC_HW_ENABLE)
#define FRAME_TRANS_PACKET_MAX          (FRAME_PACKET_MAX + 2)      // 加上帧头和CRC校验两个数据包
#else
#define FRAME_TRANS_PACKET_MAX          (FRAME_PACKET_MAX + 1)      // 加上帧头
#endif

/************************************************************************************
 *                                 TRACE宏定义
 ************************************************************************************/
#define BUS_TRACE_DEPTH_MAX             (100)      // 最大异常跟踪数量

#define BUS_TRACE_GLOBAL_LEVEL          BUS_TRACE_LEVEL_D               // 全局日志等级
#define BUS_TRACE_LEVEL_MUX             BUS_TRACE_LEVEL_D               // MUX模块日志等级
#define BUS_TRACE_LEVEL_TUNNEL          BUS_TRACE_LEVEL_I               // TUNNEL模块日志等级
#define BUS_TRACE_LEVEL_FRAME           BUS_TRACE_LEVEL_D               // FRAME模块日志等级

#endif