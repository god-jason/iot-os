/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        nvparam_pubcfg.h
 *
 * @brief       公共 NV 顶层定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef NVPARAM_PUBCFG_LAYOUT_H
#define NVPARAM_PUBCFG_LAYOUT_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <nv/nvparam_config.h>


/*****************************************************************************f*******
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/* @NV-PLAT {
        "name": "设备信息",
        "description": "设备信息"
   }
*/
typedef struct {
    /********************************************************************************
    出厂值:      由出产厂商决定
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "软件版本",
            "range": "",
            "description": "软件版本, 小于40个字符"
        }
    */
    char    softVersion[VERSION_NUMBER_LEN];

    /********************************************************************************
    出厂值:      由出产厂商决定
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "硬件版本",
            "range": "",
            "description": "硬件版本, 小于40个字符"
        }
    */
    char    hardVersion[VERSION_NUMBER_LEN];

    /********************************************************************************
    出厂值:      由出产厂商决定
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "产品信息",
            "range": "",
            "description": "产品信息, 小于40个字符"
        }
    */
    char    prodInfo[VERSION_NUMBER_LEN];

    /********************************************************************************
    出厂值：由出产厂商决定
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "制造商信息",
            "range": "",
            "description": "制造商信息, 小于40个字符"
        }
    */
    char    mfrInfo[VERSION_NUMBER_LEN];
}NV_DevInfo;

/* @NV-PLAT {
        "name": "日志配置",
        "description": "日志配置"
   }
*/
typedef struct {
    /********************************************************************************
    功能：LOG输出控制.
         logCtrlEnable配置为1, 此功能启用, 并忽略如下设置：
         1) PC LOG工具下发的跟踪过滤规则失效
         2) 与PC LOG工具失去心跳关闭LOG功能失效
    取值范围: 0 ~ 65535. 采用BitMap形式. 某个Bit的取值含义:0-不打印;1-打印
    出厂值：0;
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "LOG输出控制",
            "range": "0-65535",
            "description": "Bit0:协议栈打印,Bit1:物理层打印,Bit2:协议栈标准LOG,Bit3:物理层标准LOG,Bit4:平台窗口打印,Bit5:PRINT窗口非DUMP打印,Bit6:DUMP打印(如IP数据包打印)"
        }
    */
    uint16_t logCtrlValue;

    /*  @NV-ITEM {
            "name": "LOGCTRL功能开关",
            "range": "0-1",
            "description": "0:关闭LOGCTRL功能, 1:开启LOGCTRL功能"
        }
    */
    uint8_t logCtrlEnable;

    /*  @NV-ITEM {
            "name": "日志输出通道,非HDLC模式下有效",
            "range": "1-2",
            "description": "0:保留, 1:UART输出日志, 2:SPI输出日志"
        }
    */
    uint8_t logChannel;
}NV_LogConfig;

/* @NV-PLAT {
        "name": "省电配置",
        "description": "省电配置"
   }
*/
typedef struct {
    /*  @NV-ITEM {
            "name": "省电模式等级",
            "range": "0-2",
            "description": "0:不支持休眠;1:支持deepsleep，不支持standby;2:支持deepsleep，支持standby"
        }
    */
    uint8_t psmLevel;

    /*  @NV-ITEM {
            "name": "省电日志",
            "range": "0-1",
            "description": "0:关闭省电日志输出, 1:打开省电日志输出"
        }
    */
    uint8_t logLevel;

    /*  @NV-ITEM {
            "name": "短按唤醒的保持时间",
            "range": "0-3600",
            "description": "单位秒, 0表示唤醒后立刻睡下去"
        }
    */
    uint16_t spWakeupTime;

    /*  @NV-ITEM {
        "name": "deepsleep功耗优化开关",
        "range": "0-1",
        "description": "0:关闭deepsleep功耗优化, 1:打开deepsleep功耗优化"
    }
    */
    uint8_t dsOptimizeEn;

    /*  @NV-ITEM {
        "name": "deepsleep下DCXO26M时钟开关",
        "range": "0-1",
        "description": "0:关闭DCXO26M时钟, 1:打开DCXO26M时钟"
    }
    */
    uint8_t ds26MDcxoEn;

    /*  @NV-ITEM {
            "name": "sby门限时长",
            "range": "0-65535",
            "description": "单位秒"
        }
    */
    uint16_t sbyThreholdTime;

    /*  @NV-ITEM {
            "name": "SBY唤醒保持时间",
            "range": "0-255"
            "description": "单位秒, 0表示唤醒后立刻睡下去"
        }
    */
    uint8_t     psmWakeupTime;

}NV_PsmConfig;

/* @NV-PLAT {
        "name": "串口配置",
        "description": "串口配置"
   }
*/
typedef struct {
    /*  @NV-ITEM {
            "name": "LPUART波特率",
            "range": "0,2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,460800,921600,3000000,6000000,12000000",
            "description": "LPUART波特率"
        }
    */
    uint32_t baudrate;

    /*  @NV-ITEM {
            "name": "切换工作时钟标记",
            "range": "0,1",
            "description": "波特率自适应后是否切换工作时钟"
        }
    */
    uint8_t adaptChClock;
    /*  @NV-ITEM {
        "name": "9600波特率时钟选择",
        "range": "0-1",
        "description": "0:9600波特率使用32K时钟, 1:9600波特率使用26M时钟"
    }
    */
    uint8_t select_Clock;
}NV_UartCfg;

/* @NV-PLAT {
        "name": "驱动配置",
        "description": "驱动配置"
   }
*/
typedef struct {
    /*  @NV-ITEM {
            "name": "异常重启标记",
            "range": "0-1",
            "description": "0-关闭异常重启, 1-开启异常重启"
        }
    */
    uint8_t     exceptReset;

    /*  @NV-ITEM {
            "name": "中断线程轨迹标记",
            "range": "0-3",
            "description": "Bit0 - AP, Bit1 - CP"
        }
    */
    uint8_t     traceCtrl;

    /*  @NV-ITEM {
            "name": "看门狗标记",
            "range": "0,5-255",
            "description": "0-关闭看门狗, 5 -255 开启看门狗超时时间"
        }
    */
    uint8_t     watchDog;

    /*  @NV-ITEM {
            "name": "SIM卡热拔插标记",
            "range": "0-1",
            "description": "0-关闭热拔插支持, 1-开启热拔插支持"
        }
    */
    uint8_t     uiccHotPlug;

    union {
    NV_UartCfg  uartConfig;
    uint8_t     uartConfigArea[12];
    };

    /*  @NV-ITEM {
            "name": "MUX使能标记",
            "range": "0-1",
            "description": "0-关闭MUX, 1-开启MUX"
        }
    */
    uint8_t     muxCtrl;
    /*  @NV-ITEM {
            "name": "CONSOLE使能标记",
            "range": "0-1",
            "description": "0-关闭, 1-开启"
        }
    */
    uint8_t     consoleCtrl;
    /*  @NV-ITEM {
            "name": "RRC超时拆链使能标记",
            "range": "0-1",
            "description": "0-开机默认关闭, 1打开"
        }
    */
    uint8_t     fastReleaseRrcCtrl;

    /*  @NV-ITEM {
            "name": "PSM使用的PWM时钟",
            "range": "0,1",
            "description": "0-pstimer, 1-cptimer"
        }
    */
    uint8_t     pwmTimer;
}NV_DrvConfig;

typedef enum
{
    CODEC_MUSIC_PARA,
    CODEC_VOLTE_WB_PARA,
    CODEC_VOLTE_NB_PARA,
    CODEC_MAX_PARA,
}CodecParaType;

#define     AUDIO_VOLUME_MAX_LEVEL      10
/**
 * @struct NV_AudioCfg
 * @brief  音频NV空间布局定义结构体
 */
typedef struct
{
    uint32_t codecID;
    int32_t AdcDigitGain;
    int32_t DacDigitGain;
    int32_t AdcPgaGain;
    int32_t MicGain;
    int32_t NrGate;
    uint8_t SideTone;
    uint8_t AdcEQ[23][2];
    uint8_t DacEQ[16][2];
    uint8_t Drc[5][2];
    uint8_t Alc[6][2];
    int32_t volumeLevel[AUDIO_VOLUME_MAX_LEVEL];
    uint32_t magicNumber;
    uint8_t userVolumeLevel;
}NV_AudioCfgItem;

typedef struct
{
    NV_AudioCfgItem MusicCfg;
    NV_AudioCfgItem VolteWbCfg;
    NV_AudioCfgItem VolteNbCfg;
}NV_AudioCfg;


typedef struct {
    /*  @NV-ITEM {
            "name": "开机自动发AT+CFUN=1",
            "range": "0-1",
            "description": "0-开机不自动发AT+CFUN=1, 1-开机自动发AT+CFUN=1"
        }
    */
    uint8_t     auto_cfun1;
    /*  @NV-ITEM {
            "name": "开机自动拨号连接的CID",
            "range": "0-8",
            "description": "0-开机不自动拨号连接, 1-8开机自动拨号连接的CID"
        }
    */
    uint8_t     auto_connect_cid;
    /*  @NV-ITEM {
            "name": "时区信息",
            "range": "-96 ~ +96",
            "description": "按1/4小时计算即15分钟一时区"
        }
    */
    int8_t     time_zone;
    /*  @NV-ITEM {
            "name": "按键唤醒保持",
            "range": "0-1",
            "description": "0-按键唤醒不保持, 1-按键唤醒保持"
        }
    */
    uint8_t     wakeup_keep;
    /*  {
        "name": "芯片温度门控",
        "range": , default 0xFFFF
        "description": "当温度超过门控时关机"
    }
    */
    uint16_t     OtpThreshold;
   /*  {
        "name": "睡眠模式",
        "range": 0-不支持睡眠；1.支持睡眠不支持standby；2.支持standby
        "description": "睡眠模式设置"
    }
    */
    uint8_t    sleep_mode;
     /*  @NV-ITEM {
            "name": 网络指示灯状态",
            "range": "0-1",
            "description": "0-灯熄, 1-等亮"
        }
    */
    uint8_t    led_net;
     /*  @NV-ITEM {
            "name": 系统指示灯状态",
            "range": "0-1",
            "description": "0-灯熄, 1-等亮"
        }
    */
    uint8_t     led_status;
     /*  @NV-ITEM {
            "name": 系统指示灯状态自动联网功能",
            "range": "0-1",
            "description": "0-禁止自动联网, 1-使能自动联网"
        }
    */
    uint8_t    dial_mode;
         /*  @NV-ITEM {
     /*  @NV-ITEM {
            "name": 上网模式",
            "range": "0-1",
            "description": "0-禁止自动拨号上网, 1-拨号上网"
        }
    */
    uint8_t     auto_conn;
         /*  @NV-ITEM {
            "name":自动网络搜索,
            "range": "0-1",
            "description": "0-禁止强制扫频, 1-使能强制扫频"
        }
    */
    uint8_t     force_scan_earfcn;
          /*  @NV-ITEM {
     /*  @NV-ITEM {
            "name": 网络工作模式",
            "range": "0-1",
            "description": "0-网卡, 1-路由"
        }
    */
    uint8_t   work_mode;

}NV_AppConfig;

/* @NV-PLAT {
        "name": "调试口功能设置",
        "description": "调试口功能设置"
   }
*/
typedef struct {
    /********************************************************************************
    功能:muxPort功能设置
    取值范围:0 ~ 3. 0:mux功能关闭, 1:AT, 2:控制台, 3:LOG_INFO
    出厂值：0
    *********************************************************************************/
    /*  @NV-ITEM {
            "name": "muxPort功能设置",
            "range": "0-3",
            "description": "0:mux功能关闭, 1:AT, 2:控制台, 3:LOG_INFO"
        }
    */
    uint8_t    portSet;
    uint8_t    rsvd[3];
}NV_MuxPortCfg;

#define RI_PULSE_CFG_TYPE_MAX   (3)

/*  {
    "name": "指定来电类,短信类，通用类 URC RI行为"
    "description": "指定来电类URC RI行为"
}
*/
typedef struct {
    /*  {
        "name": "指定URC RI行为的脉冲周期, 仅<riType>为"pulse"时有效"
        "range": 1~2000ms,
        "description": "指定URC RI行为, 默认值是120ms"
    }
    */
    uint16_t    pulseDuration;
    /*  {
        "name": "指定URC RI行为的脉冲个数", 仅<riType>为"pulse"时有效"
        "range": 1~5,
        "description": "指定URC RI行为, 默认值是1"
    }
    */
    uint8_t     pulseCount;
        /*  {
        "name": "指定URC RI行为",
        "range": "off, pulse",
        "description": "指定URC RI行为, 默认值是pulse"
    }
    */
    uint8_t     riType;
}NV_AT_RiPulseCfg;

typedef struct {
    /*  {
        "name": "QIND类型控制",
        "range": , default
        "description": "配置QIND主动上报的类型"
    }
    */
    uint32_t     QIndCfg;
    /*  {
        "name": "RI的输出位置",
        "range": "respective, physical", 
        "description": "配置URC输出端口, 默认是respective"
    }
    */
    uint8_t     riSignalType;
    /*  {
        "name": "RI脉冲结束后URC上报的延迟时间",
        "range": 参数 urc/delay 范围为0~120ms, 参数 urcdelay 范围为0~120000ms
        "description": "配置RI脉冲结束后URC上报的延迟时间, 默认是0，无延迟"
    }
    */
    uint32_t    urcDelayMS;
    /*  {
        "name": "指定来电类,短信类，通用类 URC RI行为"
        "description": "指定来电类URC RI行为"
    }
    */
    NV_AT_RiPulseCfg riPulseCfg[RI_PULSE_CFG_TYPE_MAX];
}NV_ATConfig;

/**
 * @struct NV_PubConfig
 * @brief 公共 NV 空间布局定义结构体, 在此顺序增加 NV 子项
 */
typedef struct {
    NV_DevInfo      devInfo;

    union {
    NV_LogConfig    logConfig;
    uint8_t         logConfigArea[4];
    };

    union {
    NV_PsmConfig    psmConfig;
    uint8_t         psmConfigArea[12];
    };

    union {
    NV_DrvConfig    drvConfig;
    uint8_t         drvConfigArea[64];
    };

    union {
    NV_AppConfig    appConfig;
    uint8_t         appConfigArea[64];
    };

    union {
    NV_AudioCfg     AudioCfg;
    uint8_t         AudioCfgArea[CODEC_MAX_PARA*196];
    };

    union {
    NV_MuxPortCfg   muxPortCfg;
    uint8_t         MuxPortCfgArea[8];
    };

    union {
    NV_ATConfig     ATConfig;
    uint8_t         ATConfigArea[64];
    };
}NV_PubConfig;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
static void NV_CheckPubConfigSize(void)
{
    BUILD_BUG_ON(MEMBER_SIZE(NV_PubConfig, logConfig) > MEMBER_SIZE(NV_PubConfig, logConfigArea));
    BUILD_BUG_ON(MEMBER_SIZE(NV_PubConfig, psmConfig) > MEMBER_SIZE(NV_PubConfig, psmConfigArea));
    BUILD_BUG_ON(MEMBER_SIZE(NV_PubConfig, drvConfig) > MEMBER_SIZE(NV_PubConfig, drvConfigArea));
    BUILD_BUG_ON(MEMBER_SIZE(NV_PubConfig, appConfig) > MEMBER_SIZE(NV_PubConfig, appConfigArea));
    BUILD_BUG_ON(MEMBER_SIZE(NV_PubConfig, AudioCfg ) > MEMBER_SIZE(NV_PubConfig, AudioCfgArea));

}

#endif /*NVPARAM_PUBCFG_LAYOUT_H*/
