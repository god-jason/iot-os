/*****************************************************************************
 *  版本所有 (C)2020 
 *
 *  实现功能：
 *  作    者：
 *  其它说明：
 *

 *  RD2021-221      2021-01-08        xxx        代码优化
 *
 *****************************************************************************/

#ifndef _RONVPARAM_AMT_H
#define _RONVPARAM_AMT_H

#include "oss_types.h"

typedef struct _st_AMT_CommNv_Info
{
    UINT8 imei[8];                  // 0x0000
    UINT8 imeiReserved[8];
    UINT8 imeiSv[8];                // 0x0010
    UINT8 imeiSvReserved[8];
    UINT8 boardNum[20];             // 0x0020
    UINT8 boardNumReserved[24];
    UINT8 validFlag[4];             // 0x004c
    UINT8 internalMAC[20];          // 0x0050
    UINT8 externalMAC[20];          // 0x0064
    UINT8 wifiMAC[20];              // 0x0078
    UINT8 wifiMAC2[20];             // 0x008c
    UINT8 ethMAC[20];               // 0x00a0
    UINT8 usbMAC[20];               // 0x00b4
    UINT8 GMAC[20];                 // 0x00c8
    UINT8 nvVersion[36];            // 0x00dc
    UINT8 testInfo[256];            // 0x0100
    UINT8 v4Key[84];                // 0x0200
    UINT8 v4KeyReserved[44];
    UINT8 saleState[2];             // 0x0280
    UINT8 saleStateReserved[14];
    UINT8 lockKey[20];              // 0x0290
    UINT8 lockKeyReserved[12];

    UINT8 imei2[8];                 // 0x02b0
    UINT8 imeiReserved2[8];

    UINT8 Reserved[320];            // 0x02c0

    UINT8 gb[1024];                 // 0x1000
    
    UINT8 user[1024];               // 0x2000
    
    UINT8 ref[1024];                // 0x3000
} T_AMT_CommNv_Info;                // 0x4000


#define OS_FLASH_AMT_COMM_RO_NONFAC_BASE_ADDR   0x4000

#define NV_AMTCOMM_ITEM_ADDR(item)      (OS_FLASH_AMT_COMM_RO_NONFAC_BASE_ADDR + (size_t)(&(((T_AMT_CommNv_Info*)(0x0))->item)))
#define NV_AMTCOMM_ITEM_SIZE(item)      (sizeof(((T_AMT_CommNv_Info*)(0x0))->item))

// AMT COMM ITEM
#define OS_FLASH_AMT_COMM_RO_IMEI_ADDRESS                   NV_AMTCOMM_ITEM_ADDR(imei)
#define OS_FLASH_AMT_COMM_RO_IMEI_SIZE                      NV_AMTCOMM_ITEM_SIZE(imei)

#define OS_FLASH_AMT_COMM_RO_IMEI_1_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(imeiReserved)
#define OS_FLASH_AMT_COMM_RO_IMEI_1_SIZE                    NV_AMTCOMM_ITEM_SIZE(imeiReserved)

#define OS_FLASH_AMT_COMM_RO_IMEISV_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(imeiSv)
#define OS_FLASH_AMT_COMM_RO_IMEISV_SIZE                    NV_AMTCOMM_ITEM_SIZE(imeiSv)

#define OS_FLASH_AMT_COMM_RO_BOARDNNUM_ADDRESS              NV_AMTCOMM_ITEM_ADDR(boardNum)
#define OS_FLASH_AMT_COMM_RO_BOARDNNUM_SIZE                 NV_AMTCOMM_ITEM_SIZE(boardNum)

#define OS_FLASH_AMT_COMM_RO_ValidFlag_ADDRESS              NV_AMTCOMM_ITEM_ADDR(validFlag)
#define OS_FLASH_AMT_COMM_RO_ValidFlag_SIZE                 NV_AMTCOMM_ITEM_SIZE(validFlag)

#define OS_FLASH_AMT_COMM_RO_InternalMAC_ADDRESS            NV_AMTCOMM_ITEM_ADDR(internalMAC)
#define OS_FLASH_AMT_COMM_RO_InternalMAC_SIZE               NV_AMTCOMM_ITEM_SIZE(internalMAC)

#define OS_FLASH_AMT_COMM_RO_ExternalMAC_ADDRESS            NV_AMTCOMM_ITEM_ADDR(externalMAC)
#define OS_FLASH_AMT_COMM_RO_ExternalMAC_SIZE               NV_AMTCOMM_ITEM_SIZE(externalMAC)

#define OS_FLASH_AMT_COMM_RO_WIFIMAC_ADDRESS                NV_AMTCOMM_ITEM_ADDR(wifiMAC)
#define OS_FLASH_AMT_COMM_RO_WIFIMAC_SIZE                   NV_AMTCOMM_ITEM_SIZE(wifiMAC)

#define OS_FLASH_AMT_COMM_RO_WIFIMAC2_ADDRESS               NV_AMTCOMM_ITEM_ADDR(wifiMAC2)
#define OS_FLASH_AMT_COMM_RO_WIFIMAC2_SIZE                  NV_AMTCOMM_ITEM_SIZE(wifiMAC2)

#define OS_FLASH_AMT_COMM_RO_ETHMAC_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(ethMAC)
#define OS_FLASH_AMT_COMM_RO_ETHMAC_SIZE                    NV_AMTCOMM_ITEM_SIZE(ethMAC)

#define OS_FLASH_AMT_COMM_RO_USBMAC_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(usbMAC)
#define OS_FLASH_AMT_COMM_RO_USBMAC_SIZE                    NV_AMTCOMM_ITEM_SIZE(usbMAC)

#define OS_FLASH_AMT_COMM_RO_GMAC_ADDRESS                   NV_AMTCOMM_ITEM_ADDR(GMAC)
#define OS_FLASH_AMT_COMM_RO_GMAC_SIZE                      NV_AMTCOMM_ITEM_SIZE(GMAC)

#define OS_FLASH_AMT_COMM_RO_NvVersion_ADDRESS              NV_AMTCOMM_ITEM_ADDR(nvVersion)
#define OS_FLASH_AMT_COMM_RO_NvVersion_SIZE                 NV_AMTCOMM_ITEM_SIZE(nvVersion)

#define OS_FLASH_AMT_COMM_RO_TestInfo_ADDRESS               NV_AMTCOMM_ITEM_ADDR(testInfo)
#define OS_FLASH_AMT_COMM_RO_TestInfo_SIZE                  NV_AMTCOMM_ITEM_SIZE(testInfo)

#define OS_FLASH_AMT_COMM_RO_V4Key_ADDRESS                  NV_AMTCOMM_ITEM_ADDR(v4Key)
#define OS_FLASH_AMT_COMM_RO_V4Key_SIZE                     NV_AMTCOMM_ITEM_SIZE(v4Key)

#define OS_FLASH_AMT_COMM_RO_SaleState_ADDRESS              NV_AMTCOMM_ITEM_ADDR(saleState)
#define OS_FLASH_AMT_COMM_RO_SaleState_SIZE                 NV_AMTCOMM_ITEM_SIZE(saleState)

#define OS_FLASH_AMT_COMM_RO_LockKey_ADDRESS                NV_AMTCOMM_ITEM_ADDR(lockKey)
#define OS_FLASH_AMT_COMM_RO_LockKey_SIZE                   NV_AMTCOMM_ITEM_SIZE(lockKey)

#define OS_FLASH_AMT_COMM_RO_IMEI_2_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(imei2)
#define OS_FLASH_AMT_COMM_RO_IMEI_2_SIZE                    NV_AMTCOMM_ITEM_SIZE(imei2)

#define OS_FLASH_AMT_COMM_RO_IMEI_3_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(imeiReserved2)
#define OS_FLASH_AMT_COMM_RO_IMEI_3_SIZE                    NV_AMTCOMM_ITEM_SIZE(imeiReserved2)


// 金机数据，单独计算
#define OS_FLASH_AMT_COMM_RO_GBDATA_ADDRESS                 NV_AMTCOMM_ITEM_ADDR(gb)
#define OS_FLASH_AMT_COMM_RO_GBDATA_SIZE                    NV_AMTCOMM_ITEM_SIZE(gb)
// 给外部客户分配1k字节用于存储数据
#define OS_FLASH_AMT_COMM_RO_USER_DEFINE_ADDRESS            NV_AMTCOMM_ITEM_ADDR(user)
#define OS_FLASH_AMT_COMM_RO_USER_DEFINE_SIZE               NV_AMTCOMM_ITEM_SIZE(user)
// 给参考设计分配1k字节用于存储数据
#define OS_FLASH_AMT_COMM_RO_REF_DEFINE_ADDRESS             NV_AMTCOMM_ITEM_ADDR(ref)
#define OS_FLASH_AMT_COMM_RO_REF_DEFINE_SIZE                NV_AMTCOMM_ITEM_SIZE(ref)


#define PS_ImeiLen                                          OS_FLASH_AMT_COMM_RO_IMEI_SIZE
#define PS_ImeiSvLen                                        OS_FLASH_AMT_COMM_RO_IMEISV_SIZE
#define PS_DEV_MSINFO_MAX_BORDNAME_LEN                      OS_FLASH_AMT_COMM_RO_BOARDNNUM_SIZE


typedef enum
{
    ABIMEI_NVPARAM,                 // 硬件设备号
    ABIMEISV_NVPARAM,               // 软件版本号已经不再使用
    ABBORDNUm_NVPARAM,              // 主板号
    ABMSERIALNUM_NVPARAM,           // 串号
    ABVALIDFLAG_NVPARAM,            // MAC地址选择指示位

    ABMAC_INT_NVPARAM,              // 内部MAC地址
    ABMAC_EXT_NVPARAM,              // 外部MAC地址
    ABMAC_WIFI_NVPARAM,             // WIFI MAC地址
    ABNV_VERSION_NVPARAM,           // NV版本信息
    ABTESTINFO_NVPARAM,             // 生产测试标志位
    ABSOFTVERSION_NVPARAM,          // 后续需要修改目前软件版本号不在只读区
    ABNET_V4KEY_NVPARAM,
    ABSALE_STATE_NVPARAM,           // 销量统计标志
    ABBANDBITMAP_NVPARAM,           // BAND BITMAP
    ABMAC_ETH_NVPARAM,              // ETH MAC地址
    ABNET_LCOKKEY_NVPARAM,          // 解锁码
    ABMAC_WIFI2_NVPARAM,            // 增加第二个WIFI地址
    ABMAC_USBMAC_NVPARAM,           // USB(Rndis,ECM)MAC地址
    ABMAC_GMAC_NVPARAM,             // GMAC(PHY/Switch) MAC地址

    ABIMEI1_NVPARAM,                // 第二个IMEI号
    ABIMEI2_NVPARAM,                // 第三个IMEI号
    ABIMEI3_NVPARAM,                // 第四个IMEI号

    ABGBDATA_NVPARAM,               // GBNV

    MAXABNVPARAM_NVPARAM
} NvParam_AMT;

/*****************************************************************************
 *                         生产测试标志位的定义                              *
 *****************************************************************************/
typedef enum
{
    LTE_CAL = 0,            LTE_FT,                 LTE_CPT,                LTE_FUN,
    WIFI_CAL,               WIFI_FT,                WIFI_CPT,               WIFI_FUN,
    BLUETOOTH_CAL,          BLUETOOTH_FT,           BLUETOOTH_CPT,          BLUETOOTH_FUN,
    GPS_FT,                 GPS_CPT,                GPS_FUN,                GPS_RESERVED,
    MOBILETV_FT,            MOBILETV_CPT,           MOBILETV_FUN,           MOBILETV_RESERVED,
    NFC_FT,                 NFC_FUN,                NFC_RESERVED1,          NFC_RESERVED2,
    WIMAX_CAL,              WIMAX_FT,               WIMAX_CPT,              WIMAX_RESERVED,
    BOARDTEST_CURRENT,      BOARDTEST_AUDIO,        BOARDTEST_BT,           BOARDTEST_RESERVED,
    CAC_PREVIEW,            CAC_FINALCHECK,         CAC_RESERVED2,          CAC_RESERVED3,
    DL_PV,                  DL_RELEASE,             DL_AMT,                 DL_RESERVED,

    /* 外设功能测试 */
    SOFTVERSION_CHECK,      TESTINFO_CHECK,         BUTTION_CHECK,          LCD_BACKLIT_CHECK,
    MOTOR_CHECK,            LCD_TEST,               LOUDSPEAKER_CHECK,      EARPHONE_CHECK,
    MICRO_PHONE_CHECK,      HEAD_SET_CHECK,         FM_CHECK,               SIMCARD_CHECK,
    CHARGING_CHECK,         FLASH_LIGHT_CHECK,      KEY_LIGHT_CHECK,        RESET_CHECK,
    HEADSET_KEY_CHECK,      BATTERT_VOLTAGE_CHECHK, SDCARD_CHECK,           CAMERA_BACK_CHECK,
    IDLE_CURRENT_CHECK,     OFF_CURRENT_CHECK,      DEVICETEST_RESERVED1,   DEVICETEST_RESERVED2,

    MAXAMTFALGS_NVPARAM = 255
} NvParam_AMTFlags;

typedef enum
{
    T_INITIAL    = 0x30,
    T_PASSED     = 0x31,
    T_FAILED     = 0x32,
    T_ERRORVALUE = 0x33
} TestResult;

/*****************************************************************************
 * 函数名称： amt_IsAmtMode
 * 功能描述： 判断是否为AMT模式
 * 参数说明：
 * 返 回 值："是"返回TRUE，"否"返回FALSE
 * 其它说明：
 *****************************************************************************/
BOOL amt_IsAmtMode(VOID);

/*****************************************************************************
 * 名称: amt_NvItemWrite
 * 功能描述: 写一项NV数据
 * 参数说明:
              (IN)
                  nvParam:      该NV项类型
                  NvItemData:   写入的数据
                  NvItemLen:    数据长度
              (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
UINT32  amt_NvItemWrite(NvParam_AMT nvParam, UINT8 *NvItemData, UINT32 NvItemLen);

/*****************************************************************************
 * 名称: amt_NvItemRead
 * 功能描述: 读出一项NV数据
 * 参数说明:
              (IN)
                  nvParam:     该NV项类型
                  NvItemData:  读出的数据
                  NvItemLen:   数据长度
              (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
UINT32  amt_NvItemRead(NvParam_AMT nvParam, UINT8 *NvItemData, UINT32 NvItemLen);

/*****************************************************************************
 * 名称: amt_ProdTestFlagsWrite
 * 功能描述:写生产测试标志位
 * 参数说明:
              (IN)
                  nvParam:    该NV项类型
                  NvItemData: 待写的数据
              (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
UINT32  amt_ProdTestFlagsWrite(NvParam_AMTFlags nvParam, UINT8 *NvItemData);

/*****************************************************************************
 * 名称: amt_ProdTestFlagsRead
 * 功能描述: 读生产测试标志位
 * 参数说明:
              (IN)
                  nvParam:     该NV项类型
                  NvItemData:  读取的数据
              (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
UINT32 amt_ProdTestFlagsRead(NvParam_AMTFlags nvParam, UINT8 *NvItemData);

/*****************************************************************************
 * 名称: amt_GetNvBandInfo
 * 功能描述:读生产测试频段信息
 * 参数说明:
              (IN)
                  NvItemData 读取信息的存放buffer
                  NvItemLen 长度
              (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
UINT32 amt_GetNvBandInfo(UINT8 *NvItemData, UINT32 NvItemLen);

/*****************************************************************************
 * 名称: amt_NvRoUserWrite
 * 功能描述: 用户自定义区域写函数
 * 参数说明:
             (IN)
                 dwStart : 0 ~ 1k
                 dwLen   : 0 ~ 1k
                 from    : 待写入的数据
             (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
SINT32 amt_NvRoUserWrite(UINT32 dwStart, UINT32 dwLen, UINT8 *from);

/*****************************************************************************
 * 名称: amt_NvRoUserRead
 * 功能描述: 用户自定义区域读函数
 * 参数说明:
             (IN)
                 dwStart : 0 ~ 1k
                 dwLen   : 0 ~ 1k
                 to      : 返回的数据

             (OUT)
 * 返 回 值: 成功返回IOS_SUCCESS；失败返回IOS_ERROR
 * 其它说明:
 *****************************************************************************/
SINT32 amt_NvRoUserRead(UINT32 dwStart, UINT32 dwLen, UINT8 *to);

/*****************************************************************************
 * 函数名称: amt_NvRoRefWrite
 * 功能描述: 参考设计自定义区域写函数
 * 参数说明:
             (IN)
                 dwStart : 0 ~ 16k
                 dwLen   : 0 ~ 16k
                 from    : 待写入的数据
             (OUT)
 * 返 回 值: 成功返回OS_SUCCESS；失败返回OS_ERROR
 * 其它说明:
 *****************************************************************************/
SINT32 amt_NvRoRefWrite(UINT32 dwStart, UINT32 dwLen, UINT8* from);

/*****************************************************************************
 * 函数名称: amt_NvRoRefRead
 * 功能描述: 参考设计自定义区域读函数
 * 参数说明:
             (IN)
                 dwStart : 0 ~ 16k
                 dwLen   : 0 ~ 16k
                 to      : 返回的数据

             (OUT)
 * 返 回 值: 成功返回OS_SUCCESS；失败返回OS_ERROR
 * 其它说明:
 *****************************************************************************/
SINT32 amt_NvRoRefRead(UINT32 dwStart, UINT32 dwLen, UINT8* to);

/*****************************************************************************
 * 函数名称： amt_GetPublicKey
 * 功能描述： 返回公钥
 * 参数说明：
              pE(in/out): 参数E指针
              pELen(in/out): 参数E长度指针
              pN(in/out): 参数N指针
              pModulusLen(in/out): 参数N长度指针
 * 返 回 值：成功返回0，失败返回-1
 * 其它说明：
 *****************************************************************************/
SINT32 amt_GetPublicKey(UINT8 *pE, UINT32 *pELen, UINT8 *pN, UINT32 *pModulusLen);

/*****************************************************************************
 * 名称: amt_GetCommNvSizeByNvParam
 * 功能描述: 获取comm nv中每个nv项的大小
 * 参数说明:
             (IN)
                 nvParam: comm nv项

             (OUT)
                 nvsize
 * 返 回 值: 成功返回真实的nvsize；失败返回0
 * 其它说明:
 *****************************************************************************/
UINT32 amt_GetCommNvSizeByNvParam(NvParam_AMT nvParam);

/*****************************************************************************
 * 名称: amt_UpdateFtData
 * 功能描述: 更新FT数据
 * 参数说明:
             (IN)
             (OUT)
                 nvsize
 * 返 回 值: 成功返回真实的nvsize；失败返回0
 * 其它说明:
 *****************************************************************************/
UINT32 amt_UpdateFtData();
/*****************************************************************************
 * 名称: amt_XoAgeUpdate
 * 功能描述: 更新老化数据
 * 参数说明:
             (IN)
             (OUT)
                 nvsize
 * 返 回 值: 成功返回真实的nvsize；失败返回0
 * 其它说明:
 *****************************************************************************/
UINT32 amt_XoAgeUpdate();

/*****************************************************************************
 * 名称: amt_XoAgeZero
 * 功能描述: 清空老化数据
 * 参数说明:
             (IN)
             (OUT)
                 nvsize
 * 返 回 值: 成功返回真实的nvsize；失败返回0
 * 其它说明:
 *****************************************************************************/
UINT32 amt_XoAgeZero();

#endif

