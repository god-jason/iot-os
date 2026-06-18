/*****************************************************************************
 *  版本所有 (C)2023 
 *
 *  实现功能：
 *  作    者：
 *  其它说明：
 *

 *  
 *
 *****************************************************************************/

#ifndef    _PS_PHY_NV_H
#define    _PS_PHY_NV_H

/**************************************************************************
 *                        头文件包含                                      *
 **************************************************************************/


/**************************************************************************
 *                          宏定义                                           *
 **************************************************************************/
#define AT_NV_SIZE_WORD_PER_MODE        32

/**************************************************************************
 *                         全局数据结构声明                                *
 **************************************************************************/

/* AT+CMSET="UlpowerAdapter"[[<UlpowerAdapterflg>],[<wPading>]]*/
/* @NV-PS {
        "name": "设置基站不断下调UE功能的规避补丁的开关",
        "description": "设置基站不断下调UE功能的规避补丁的开关"
   }
*/
/* 客户不可见 */
typedef struct
{
    /* @NV-ITEM {
        "name": "基站不断下调UE功能的规避补丁的开关",
        "range": "0-1",
        "description": "基站不断下调UE功能的规避补丁的开关,0:打开,1:关闭"
       }
    */
    uint8_t wFlg;  /* 1打开Iot补丁；0关闭*/
    uint8_t wPading;
}T_PS_EPHY_NV_UL_Power_Adapter_FLAG;

/* @NV-PS {
        "name": "LTE锁频锁小区信息",
        "description": "LTE锁频锁小区信息"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "LTE锁频锁小区开关",
        "range": "0-1",
        "description": "LTE锁频锁小区开关,0:未设置,1:已设置,默认0"
       }
    */
    /* 客户可见可修改 */
    uint8_t   bLteLCAction;       /* 0: no lock configuration, 1: lock cell or frequency, default:0 */

    /* @NV-ITEM {
        "name": "LTE锁频的个数",
        "range": "0-9",
        "description": "LTE锁频个数,取值0-9,默认0"
       }
    */
    /* 客户可见可修改 */
    uint8_t   bLteLCUarfcnNum;       /* 0-9 */

    /* @NV-ITEM {
        "name": "锁定的小区",
        "range": "0-503,65535",
        "description": "锁定的小区,UINT16类型,默认0xFFFF表示不锁小区"
       }
    */
    /* 客户可见可修改 */
    uint16_t  wLteLCCellParaId;   /* lock PCI, default：0xFFFF */

    /* @NV-ITEM {
        "name": "锁定的频点",
        "range": "",
        "description": "锁定的频点,UINT32类型数组,默认0xFFFFFFFF"
       }
    */
    /* 客户可见可修改 */
    uint32_t  dwLteLCUarfcn[9];       /* lock frequency, default：0xFFFFFFFF */
}S_Ps_P_NV_LteLcCfg;

/* @NV-PS {
        "name": "AT配置的LTE相关NV",
        "description": "AT配置的LTE相关NV"
   }
*/
typedef struct
{
    /* @NV-ITEM {
        "name": "设置基站不断下调UE功能的规避补丁的开关",
        "range": "",
        "description": "设置基站不断下调UE功能的规避补丁的开关"
       }
    */
    /* 客户不可见 */
    T_PS_EPHY_NV_UL_Power_Adapter_FLAG tUlpowerAdapterFlg;
    uint8_t wPsmTest;/*AT+CMSET="POWER_SAVE"[,<n>] 省电测试开关，0:关闭，1:打开,默认为0*/

    /* @NV-ITEM {
        "name": "物理层功能配置",
        "range": "",
        "description": "物理层功能配置,bit位按从1开始,由低到高：bit1:测量策略打开,bit2:寻呼下移打开,bit3:RF单天线接收开关,
            bit4:csr的两个策略(测量周期压缩和连接态滤波系数), bit5:csr快速搜索1v1使能 bit位 0:关闭，1:打开"
       }
    */
    /* 客户不可见 */
    uint8_t wphyCfgFlg;/* AT+CMSET="phyCfg"[,<n>]物理层功能配置，bit1:测量策略打开，bit2:寻呼下移打开，bit3:RF单天线接收开关,bit位 0:关闭，1:打开，默认0 */

    /* @NV-ITEM {
        "name": "LTE锁频锁小区信息",
        "range": "",
        "description": "LTE锁频锁小区信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_P_NV_LteLcCfg tLteLcCfg; /*+CMLTELC configuration for cell locking*/

    /* @NV-ITEM {
        "name": "静止类终端标识",
        "range": "0-1",
        "description": "静止类终端标识,0:不支持,1:支持,默认值0"
       }
    */
    /* 客户可见可修改 */
    uint8_t bStationaryMeterCtrl; /*Stationary Ammeter or Gasmeter Control, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
        "name": "Lte服务小区和邻区S值偏移",
        "range": "0-30,100-130",
        "description": "Lte服务小区和邻区S值偏移，数组下标0是服务小区，数组下标1是邻小区，取值0-30或100-130，默认值0"
       }
    */
    /* 客户可见可修改 */
    uint8_t abLteSValueOffset[2]; /* AT+CMSET="LTE_CELL_SVALUE_OFFSET", S  offset: [0]for servercell ,[1]for ncell. default: 0*/
    /* @NV-ITEM {
        "name": "极弱场优化参数",
        "range": "0，1-34",
        "description": "极弱场优化参数,0:关闭极弱场优化,1-34 打开优化,默认值0"
       }
    */
    /* 客户可见可修改 */
    uint8_t bWeakFieldEndhanceParam; /* weak field enhance param, default: 0*/
    /* @NV-ITEM {
        "name": "大频偏优化参数1",
        "range": "0-10",
        "description": "大频偏优化参数1,默认值0"
       }
    */
    /* 客户可见可修改 */
    uint8_t bFreqShift_1; /* freq shift optimize param1, default: 0*/
    /* @NV-ITEM {
        "name": "大频偏优化参数2",
        "range": "0-10",
        "description": "大频偏优化参数2,默认值0"
       }
    */
    /* 客户可见可修改 */
    uint8_t bFreqShift_2; /* freq shift optimize param2, default: 0*/
}T_PS_P_ATNV_LTE;

/*AT+CMSET="com_phyfunc",[[<wValue>],[<wPading1>],[<wPading2>],[<wPading3>]]*/
typedef struct
{
    uint16_t wValue;
    uint16_t wModelType;//类型
    uint16_t wPading[2];
}T_PS_P_NV_COMMON_PHYFUNC;

/* @NV-PS {
        "name": "温度保护功能开关及温度控制",
        "description": "温度保护功能开关及温度控制"
   }
*/
/* 客户可见可修改 */
typedef struct
{
    /* @NV-ITEM {
        "name": "过温保护开关",
        "range": "0-2",
        "description": "0:关闭过温保护; 1:过温重启; 2:过温关机;默认值: 0"
       }
    */
    /* 客户可见可修改 */
    uint8_t enable:4; /*过温保护开关, 0:关闭过温保护; 1:过温重启; 2:过温关机;默认值: 0*/

    /* @NV-ITEM {
        "name": "功率控制标志",
        "range": "0-1",
        "description": "0:表示无效，不启动功率调整; 1:表示有效,启动功率调整;默认值: 0"
       }
    */
    /* 客户可见可修改 */
    uint8_t powerctrl:1; /*功率控制标志, 0:表示无效，不启动功率调整; 1:表示有效,启动功率调整;默认值: 0*/ 
    uint8_t padding:3;

    /* @NV-ITEM {
        "name": "temperature1",
        "range": "0-255",
        "description": "powerctrl为1时有效,取值范围=[0,255],单位:摄氏度"
       }
    */
    /* 客户可见可修改 */
    uint8_t temperature1; /*powerctrl为1时有效,取值范围=[0,255],单位:摄氏度*/

    /* @NV-ITEM {
        "name": "temperature2",
        "range": "0-255",
        "description": "powerctrl为1时有效,取值范围=[0,255],单位:摄氏度, temperature2必须大于等于temperature1"
       }
    */
    /* 客户可见可修改 */
    uint8_t temperature2; /*powerctrl为1时有效,取值范围=[0,255],单位:摄氏度, temperature2必须大于等于temperature1*/

    /* @NV-ITEM {
        "name": "temperature3",
        "range": "0-255",
        "description": "powerctrl为1时有效,取值范围=[0,255],单位:摄氏度, temperature3必须大于等于temperature2"
       }
    */
    /* 客户可见可修改 */
    uint8_t temperature3; /*powerctrl为1时有效,取值范围=[0,255],单位:摄氏度, temperature3必须大于等于temperature2*/

    /* @NV-ITEM {
        "name": "实际发送功率回退值参数1",
        "range": "0-255",
        "description": "温度=(temperature1, temperature2], UE实际发送功率回退值powerbackoffvalue1,单位1dB"
       }
    */
    /* 客户可见可修改 */
    uint8_t powerbackoffvaluel; /*温度=(temperature1, temperature2], UE实际发送功率回退值powerbackoffvalue1,单位1dB*/

     /* @NV-ITEM {
        "name": "实际发送功率回退值参数2",
        "range": "0-255",
        "description": "温度=(temperature2, temperature3], UE实际发送功率回退值powerbackoffvalue2,单位1dB"
       }
    */
    /* 客户可见可修改 */
    uint8_t powerbackoffvalue2; /*温度=(temperature2, temperature3], UE实际发送功率回退值powerbackoffvalue2,单位1dB*/

    /* @NV-ITEM {
        "name": "实际发送功率回退值参数3",
        "range": "0-255",
        "description": "温度> temperature3, UE实际发送功率回退值powerbackoffvalue3，单位1dB"
       }
    */
    /* 客户可见可修改 */
    uint8_t powerbackoffvalue3; /*温度> temperature3, UE实际发送功率回退值powerbackoffvalue3，单位1dB*/ 

    /* @NV-ITEM {
        "name": "最大发送功率1",
        "range": "0-23, or 255",
        "description": "温度=(temperature1, temperature2],最大发送功率为maxpowervalue1,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准"
       }
    */
    /* 客户可见可修改 */
    uint8_t maxpowervaluel; /*温度=(temperature1, temperature2],最大发送功率为maxpowervalue1,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准*/

    /* @NV-ITEM {
        "name": "最大发送功率2",
        "range": "0-23, or 255",
        "description": "温度=(temperature2, temperature3],最大发送功率为maxpowervalue2,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准"
       }
    */
    /* 客户可见可修改 */
    uint8_t maxpowervalue2; /*温度=(temperature2, temperature3],最大发送功率为maxpowervalue2,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准*/

    /* @NV-ITEM {
        "name": "最大发送功率3",
        "range": "0-23, or 255",
        "description": "温度> temperature3,最大发送功率为maxpowervalue3,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准"
       }
    */
    /* 客户可见可修改 */
    uint8_t maxpowervalue3; /*温度> temperature3,最大发送功率为maxpowervalue3,取值范围=[0-23] or 255,单位: dB;默认值255:表示以版本默认值为准*/

    /* @NV-ITEM {
        "name": "温度跨档时的迟滞时间",
        "range": "1-255",
        "description": "温度跨档时的迟滞时间,需要持续满足delaytime秒后(默认值5秒),才按照当前档位调整发送功率,取值范围[1-255],其他值按照默认值处理"
       }
    */
    /* 客户可见可修改 */
    uint8_t delaytime; /*温度跨档时的迟滞时间,需要持续满足delaytime秒后(默认值5秒),才按照当前档位调整发送功率*/
}temperatureProtect;


/* @NV-PS {
        "name": "公共控制功能开关",
        "description": "公共控制功能开关"
   }
*/
typedef struct
{
    T_PS_P_NV_COMMON_PHYFUNC tComPhyFunc;//公共控制功能

    /* @NV-ITEM {
        "name": "入网测试开关",
        "range": "",
        "description": "入网测试开关,1:打开,0:关闭"
       }
    */
    /* 客户可见可修改 */
    uint8_t mtnetTestFlag;      /* AT+CMSET="TECT_TEST"[,<n>] 入网，1打开，0 关闭 */
    /* @NV-ITEM {
        "name": "RF复杂项测试",
        "range": "",
        "description": "RF复杂项测试,bit0:RX，bit1:TX，bit2:LDO2=1.52v"
       }
    */
    uint8_t rfComplex;          /* AT+CMSET="RF_COMPLEX"[,<n>] RF复杂项测试，bit0:RX，bit1:TX，bit2:LDO2=1.52v */
    /* @NV-ITEM {
        "name": "上行功率偏移设置",
        "range": "",
        "description": "上行功率偏移设置，最大支持16个band，band顺序与at+cmlteamtband命令相同，单位0.25dB"
       }
    */
     int8_t wApcOffset[16];     /* AT+CMSET="PowerOffset"[,<band>,[offset]] 单位0.25dB */
    /* @NV-ITEM {
        "name": "版本标志",
        "range": "",
        "description": "bit0：38/40/41 20M 1rb回退1db"
       }
    */
    uint8_t verFlag;
    
    /* @NV-ITEM {
        "name": "温度保护功能开关及温度控制",
        "range": "",
        "description": "温度控制3档模式,1档模式:temerature1配置小于255的温度值,temerature2、temerature3配置255;2档模式: temerature1、temerature2配置小于255的温度值, temerature3配置255;3档模式: temeraturel、temerature2、 temerature3都配置小于255温度值;"
       }
    */
    /* 客户可见可修改 */
    temperatureProtect tTemperProtect;
    /* @NV-ITEM {
        "name": "晶体老化关闭标志",
        "range": "",
        "description": "=1，关闭晶体老化；=0，默认开启老化"
       }
    */
    uint8_t xoAgeDis;       /* AT+CMSET="XO_AGE"[,<disable>,[value]] 单位ppg */

    
    /* @NV-ITEM {
        "name": "模组类型",
        "range": "0-255",
        "description": "模组类型,0:通用,1:BL H1模组,其他:预留,默认值0"
       }
    */
    uint8_t modulType; 

    /* @NV-ITEM {
        "name": "BL电压门限1",
        "range": "0-255",
        "description": "BL电压门限,单位0.1V,默认值30(表示3V)"
       }
    */
    uint8_t BLVoltageThresh1; 

    /* @NV-ITEM {
        "name": "BL电压门限2",
        "range": "0-255",
        "description": "BL电压门限,单位0.1V,默认值25(表示2.5V)"
       }
    */
    uint8_t BLVoltageThresh2; 
    /* @NV-ITEM {
        "name": "AuxADC是否常开",
        "range": "",
        "description": "bit0：0：默认间歇开关，1：常开；bit1，0：默认关闭打印，1：打开打印; "
       }
    */
    uint8_t auxAdc;       /* AT+CMSET="AuxADC"[,<flag>] */
}T_PS_P_ATNV_COMMON;

/* @NV-PS {
        "name": "协议栈和物理层NV",
        "description": "协议栈和物理层NV"
   }
*/
typedef struct
{
    /* @NV-ITEM {
        "name": "AT配置的LTE相关NV",
        "range": "",
        "description": "AT配置的LTE相关NV"
       }
    */
    T_PS_P_ATNV_LTE tAtNvLte;
    uint16_t wLtePading[AT_NV_SIZE_WORD_PER_MODE - (sizeof(T_PS_P_ATNV_LTE)/sizeof(uint16_t))];

    /* @NV-ITEM {
        "name": "公共控制功能开关",
        "range": "",
        "description": "公共控制功能开关"
       }
    */    
    T_PS_P_ATNV_COMMON tAtNvCom;
    uint16_t wComPading[AT_NV_SIZE_WORD_PER_MODE - (sizeof(T_PS_P_ATNV_COMMON)/sizeof(uint16_t))];
   
}T_PS_P_ATNV;
/**************************************************************************
 *                         全局变量声明                                *
 **************************************************************************/


#endif  /*_PS_PHY_NV_H*/
