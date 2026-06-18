/* ===========================================================================
** Copyright (C)  2023  by  ICT  . All rights reserved.
** ===========================================================================
**
** ===========================================================================
** Contents:
**
** ===========================================================================
*/

#ifndef    _PS_PHY_IF_H
#define    _PS_PHY_IF_H


#ifndef _OS_WIN
#include "top_ram_config.h"
#endif
#include "oss_types.h"

/**************************************************************************

 **************************************************************************/

#define PS_PHY_UL_TBS                     645     //Uplink UE Category
#define PS_PHY_DL_TBS                     1287    //Downlink UE Category
#define PS_PHY_UL_TBBUF_RESERVED_SIZE       96    //UL  malloc more reserved spcace for TBs payload

#define PS_PHY_MAX_SINR_RESULT_NUM          2
#define PS_PHY_MAX_RSSI_RESULT_NUM          1
#define PS_PHY_MAX_REPORT_CELL_PER_FREQ     16
#define PS_PHY_MAX_LIST_CELL_PER_FREQ       8
#define PS_PHY_MAX_INTERFREQ_CELL           32
#define PS_PHY_RESELECT_CELLNUM             4
#define PS_PHY_MAX_BLACK_CELL_PER_FREQ      8
#define PS_PHY_MAX_FREQ_NUM                 8
#define PS_PHY_MAX_INTER_FREQ_NUM           (PS_PHY_MAX_FREQ_NUM - 1)
#define PS_PHY_MAX_MBSFN_SUBFRAME_CFG_NUM   8
#define PS_PHY_MAX_BAND_NUM                 9
#define PS_PHY_SEARCH_P_INVALID             63
#define PS_PHY_SEARCH_Q_INVALID             63
#define PS_PHY_DEFAULT_SF_FACTOR            3
#define PS_PHY_INVALID_QUALMIN              0x7F
#define PS_PHY_THRESH_TYPE_RSRP             0
#define PS_PHY_THRESH_TYPE_RSRQ             1



#define PS_PHY_DL_BW_NOT_CFG                6

#define PS_PHY_SF_FACTOR_1DIV4              0
#define PS_PHY_SF_FACTOR_2DIV4              1
#define PS_PHY_SF_FACTOR_3DIV4              2
#define PS_PHY_SF_FACTOR_4DIV4              3


#define PS_PHY_SCELL_CFG_TYPE_SCELL_BAR                     0
#define PS_PHY_SCELL_CFG_TYPE_MID_MOBILITY_STATUS           1
#define PS_PHY_SCELL_CFG_TYPE_HIGH_MOBILITY_STATUS          2
#define PS_PHY_SCELL_CFG_TYPE_NORMAL_MOBILITY_STATUS        3
#define PS_PHY_SCELL_CFG_TYPE_RESEL_REJ                     4
#define PS_PHY_SCELL_CFG_TYPE_IDLE_SUSPEND_MEAS             5
#define PS_PHY_SCELL_CFG_TYPE_IDLE_RESUME_MEAS              6
#define PS_PHY_SCELL_CFG_TYPE_BAR_CELL_UPDATE               7
#define PS_PHY_SCELL_CFG_TYPE_LOST_SIB3                     8
#define PS_PHY_SCELL_CFG_TYPE_ENABLE_URC                    9
#define PS_PHY_SCELL_CFG_TYPE_DISABLE_URC                   10

#define PS_PHY_QCSQ_POLL_TIME_LEN 1
#define PS_PHY_QCSQ_TRIGGER_RANGE 2

#define PS_PHY_PRIO_NULL        0
#define PS_PHY_PRIO_LOWEST      1
#define PS_PHY_PRIO_HIGHEST     15

//#define PS_PHY_NON_DIFF_CFG     0
//#define PS_PHY_DIFF_CFG         1

#define PS_PHY_SPEED_STATE_NORMAL         0
#define PS_PHY_SPEED_STATE_MID            1
#define PS_PHY_SPEED_STATE_HIGH           2

#define  PS_PHY_MAX_NUM_OF_FREQSCAN_RANGE 16
#define  PS_PHY_MAX_NUM_OF_EARFCN_SCAN    48
#define  PS_PHY_MAX_NUM_OF_STORED_CELL     8
#define  PS_PHY_MAX_NUM_OF_CELL_SEARCH     8
#define  PS_PHY_MAX_NUM_OF_SI              4

#define   PS_PHY_MAX_SPS_N1PAN_NUM               4
#define   PS_PHY_MAX_SRS_CFG_NUM                 3
#define   PS_PHY_MAX_N3PAN_NUM                   4
#define   PS_PHY_MAX_CSI_MEAS_SF_PATTERN_NUM     2
#define   PS_PHY_MAX_N1PUCCH_AN_CSLISTP1_NUM     4
#define   PS_PHY_IMSI_LEN                        21

#define   PS_PHY_ATPHYCFG_PARAM_NUM              25

#define  PS_PHY_WIFI_CHANNEL_MAX                13

typedef struct
{
     UINT16                startPci;
     /* 从wStartPhyCellId开始的物理小区ID的个数,
     取值范围:1, 4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504  */
     UINT16                range;
} S_PS_PHY_PhyCellIdRange;

typedef struct
{
     UINT16                startPci:9;
     UINT16                rangePresent:1;
     UINT16                range:4;/* 0:4n,1:8n,2:12n,3:16n,4:24n,5:32n,6:48n,7:64n,8:84n,9:96n,10:128n,11:168n
                                      12:252n,13:504n,14:1008n*/
} S_PS_PHY_PhyCellIdRangeTiny;//2

typedef struct {
    UINT16        interFreqPrio:4;           /* 0~7+PRIO_OFFSET 0:PS_PHY_PRIO_NULL 1:PS_PHY_PRIO_LOWEST 15:PS_PHY_PRIO_HIGHEST */
    UINT16        interTreselect:3;          /* 0~7 单位s */
    UINT16        interTreselecSfMid:2;      /* ENUM,Treselect 中速缩放因子,0:1/4, 1:1/2, 2:3/4, 3:4/4 */
    UINT16        interTreselecSfHigh:2;     /* ENUM,Treselect 高速缩放因子,0:1/4, 1:1/2, 2:3/4, 3:4/4 */
    UINT16        interFreqOffset:5;         /*ENUM, 0:-24,1:-22,2:-20,3:-18,4:-16,5:-14,6:-12,7:-10,8:-8;9:-6;10:-5,
                                                11:-4,12:-3,13:-2,14:-1,15:0,16:1,17:2,18:3,19:4,20:5,21:6,22:8,
                                                23:10,24:12,25:14,26:16,27:18,28:20,29:22,30:24 */

    UINT16       interThreshX:5;         /* 0~31 */
    UINT16       interBlkRangeNum:4;          /* 0~8  */
    UINT16       interPcompensation:6;   /* 0~63 */
    UINT16       interPresenceAntennaPort1:1;/* 0:neighCells not use Antenna Port1;1:neighCells use Antenna Port1 */


    UINT8        interMeasBandwidth:3;/* ENUM,0:6RB,1:15RB,2:25RB,3:50RB,4:75RB,5:100RB,6:未配置 采用和服务小区相同的下行带宽进行测量 */
    UINT8        interCellNum:5;      /* 0~16 PS_PHY_MAX_LIST_CELL_PER_FREQ */

    SINT8        interQRxLevMin;             /* -70 ~ -22  */  //+6
    SINT8        interQualMin;               /* -34 ~ -3, 不存在时:127 PS_PHY_INVALID_QUALMIN*/
    UINT8        interEstFailOffsetBitmap;   /* 每个bit对应estFailOffset的第4bit */
    UINT8        interCellOffset[PS_PHY_MAX_LIST_CELL_PER_FREQ]; /* 高5bit listCellOffset Enum 低3bit estFailOffset的低3bit */
    UINT16       interPci[PS_PHY_MAX_LIST_CELL_PER_FREQ];  //+24
    S_PS_PHY_PhyCellIdRangeTiny  atInterBlackPciRangeList[PS_PHY_MAX_BLACK_CELL_PER_FREQ]; //+56
    UINT32       interEarfcn;//+72
}S_PS_PHY_InterFreqReselectPara;//76


/* PS_PHY_RESEL_PARA_CFG */
//@MTS(PS_PHY_RESEL_PARA_CFG)
typedef struct {
    UINT32       s_NonIntraSearchP:6;        /* RSRP 0~31 不存在时:63 PS_PHY_SEARCH_P_INVALID */
    UINT32       s_NonIntraSearchQ:6;        /* RSRQ 0~31 不存在时:63 PS_PHY_SEARCH_Q_INVALID */
    UINT32       s_IntraSearchP:6;           /* RSRP 0~31 不存在时:63 PS_PHY_SEARCH_P_INVALID */
    UINT32       s_IntraSearchQ:6;           /* RSRQ 0~31 不存在时:63 PS_PHY_SEARCH_Q_INVALID */
    UINT32       threshServingLow:5;         /* 0~31 */
    UINT32       intraMeasBandwidth:3;       /* ENUM,0:6RB,1:15RB,2:25RB,3:50RB,4:75RB,5:100RB,6:未配置 采用和服务小区相同的下行带宽进行测量 */

    UINT32       servPrio:4;                 /* 0~7+PRIO_OFFSET 0:PS_PHY_PRIO_NULL 1:PS_PHY_PRIO_LOWEST 15:PS_PHY_PRIO_HIGHEST */
    UINT32       intraCellNum:5;             /* 0~16 */
    UINT32       intraBlkRangeNum:4;         /* 0~8 */
    UINT32       interFreqNum:3;             /* 0~7 PS_PHY_MAX_INTER_FREQ_NUM */
    UINT32       threshType:1;               /* 0:PS_PHY_THRESH_TYPE_RSRP, 1:PS_PHY_THRESH_TYPE_RSRQ */
    UINT32       intraTreselSfMid:2;         /* ENUM,Treselect 中速缩放因子,0:1/4, 1:1/2, 2:3/4, 3:4/4 */
    UINT32       intraTreselSfHigh:2;        /* ENUM,Treselect 高速缩放因子,0:1/4, 1:1/2, 2:3/4, 3:4/4 */
    UINT32       qhystOffMid:2;              /* ENUM,eQ_Hyst 中速缩放因子,0:-6, 1:-4, 2:-2, 3:0 */
    UINT32       qhystOffHigh:2;             /* ENUM,eQ_Hyst 中速缩放因子,0:-6, 1:-4, 2:-2, 3:0 */
    UINT32       intraTreselect:3;           /* 0~7 单位s */
    UINT32       eQ_Hyst:4;                  /* ENUM */

    SINT8        pcellQRxLevMin;             /* -70 ~ -22 */
    UINT8        pcellPcompensation;         /* 0 ~ 63 */
    SINT8        pcellQualMin;               /* -34 ~ -3, 不存在时:127 PS_PHY_INVALID_QUALMIN  */
    UINT8        intraPcompensation;         /* 0 ~ 63 */

    SINT8        intraQRxLevMin;             /* -70 ~ -22  */
    SINT8        intraQualMin;               /* -34 ~ -3, 不存在时:127 PS_PHY_INVALID_QUALMIN */
    //UINT16       diffCfg:1;                  /* 0:PS_PHY_NON_DIFF_CFG     1 :PS_PHY_DIFF_CFG */
    UINT8        speedState:2;               /* PS_PHY_SPEED_STATE_NORMAL,PS_PHY_SPEED_STATE_MID, PS_PHY_SPEED_STATE_HIGH */
    UINT8        intraBar:1;
    UINT8        intraPresenceAntennaPort1:1;/* 0:neighCells not use Antenna Port1;1:neighCells use Antenna Port1 */
    UINT8        padding:4;
    UINT8        intraEstFailOffsetBitmap;        /* 每个bit对应estFailOffset的第4bit */

    UINT8        intraCellOffset[PS_PHY_MAX_LIST_CELL_PER_FREQ]; /* 高5bit listCellOffset Enum 低3bit estFailOffset的低3bit */
    UINT16       intraCellPci[PS_PHY_MAX_LIST_CELL_PER_FREQ];

    S_PS_PHY_PhyCellIdRangeTiny  atIntraBlackPciRangeList[PS_PHY_MAX_BLACK_CELL_PER_FREQ];
    S_PS_PHY_InterFreqReselectPara   atInterFreq[PS_PHY_MAX_INTER_FREQ_NUM];
}S_PS_PHY_ReselectParaCfg;//612

typedef struct
{
    UINT32                        dlEarfcn;      /* 取值范围为0~262143*/
    UINT32                        ulEarfcn;      /* 取值范围为0~262143*/
    UINT16                        phyCellId;     /* 取值范围为0-~503*/
    UINT8                         dlBandWidth;   /* 取值:6, 15, 25, 50, 75, 100, 获取不到：0xFF*/
    UINT8                         ulBandWidth;   /* 取值:6, 15, 25, 50, 75, 100， 获取不到：0xFF**/
} S_PS_PHY_PhyCellInfo;

/* PS_PHY_IDLE_SERV_CELL_CFG */
//@MTS(PS_PHY_IDLE_SERV_CELL_CFG)
typedef struct {
    UINT8       cfgType;
/*
PS_PHY_SCELL_CFG_TYPE_SCELL_BAR                     0
PS_PHY_SCELL_CFG_TYPE_MID_MOBILITY_STATUS           1
PS_PHY_SCELL_CFG_TYPE_HIGH_MOBILITY_STATUS          2
PS_PHY_SCELL_CFG_TYPE_NORMAL_MOBILITY_STATUS        3
PS_PHY_SCELL_CFG_TYPE_RESEL_REJ                     4
PS_PHY_SCELL_CFG_TYPE_IDLE_SUSPEND_MEAS             5
PS_PHY_SCELL_CFG_TYPE_IDLE_RESUME_MEAS              6
PS_PHY_SCELL_CFG_TYPE_BAR_CELL_UPDATE               7
*/
    UINT8       intraBarFg;
#define PS_PHY_SERV_CELL_CFG_BAR_CELL_NUM             8
    struct __barCellLst{
        UINT8 cellNum;
        S_PS_PHY_PhyCellInfo barCell[PS_PHY_SERV_CELL_CFG_BAR_CELL_NUM];
    }barCellLst;
}S_PS_PHY_IdleServCellCfg;

/* PHY MCM ->PS 小区选择结果上报PS
PS_PHY_RESEL_IND */
#define PS_PHY_RESEL_TYPE_BETTER_CELL           0
#define PS_PHY_RESEL_TYPE_QUICK_RESEL           1
#define PS_PHY_RESEL_TYPE_OOS                   2
#define PS_PHY_RESEL_TYPE_CESQ_UPDATE           3
typedef struct {
    UINT8        cellReSelectType;                    /* PHY小区选择类型 0:Better cell, 1:quickResel, 2:No Cell Ind(OOS),3:CesqUpdate */
    UINT8        reselNum;                            /* 1~4 */
    UINT8        rsrp[PS_PHY_RESELECT_CELLNUM];       /* 取值范围:0~97 */
    UINT8        rsrq[PS_PHY_RESELECT_CELLNUM];       /* 取值范围:0~34 */
    UINT16       pci[PS_PHY_RESELECT_CELLNUM];       /* wPci & dwEarfcn 数组一一对应  */
    UINT32       earfcn[PS_PHY_RESELECT_CELLNUM];
}S_PS_PHY_CellReSelectInd;

typedef struct
{
    UINT32              earfcn;
    UINT16              measBandWidth:7;/* 取值范围:{6,15,25,50,75,100}RB */
    /* 物理层没有使用 */
    //UINT8                rsrqMeasFlg:1;
    UINT16                presenceAntennaPort1:1;/* 0:neighCells not use Antenna Port1;1:neighCells use Antenna Port1 */
    /*用于CONNECTED态,提供intra邻区的MBSFN和TDD UL/DL信息   物理层没有使用*/
    //UINT8                nCellConfig:2;
    /*指定的黑名单物理小区ID范围的数目, 取值范围:0~16*/
    UINT16               blackCellRangeNum:5;
    UINT16               highPrioFlg:1;/*1:高于ServerCell Prio , 0:不高于服务小区*/
    UINT16               noDrxReportPeriod;/* 连接态下才会用 同频:200ms        异频:480ms */

    UINT16               drxReportPeriod;/*单位:ms   */

    S_PS_PHY_PhyCellIdRange  atBlackCellIdRangeList[PS_PHY_MAX_BLACK_CELL_PER_FREQ];
} S_PS_PHY_MeasFreqInfo;

/* PS->PHY MCM  测量配置接口
PS_PHY_MEAS_CONFIG_REQ */
//@MTS(PS_PHY_MEAS_CONFIG_REQ)
typedef struct
{
    UINT16                             freqNum:4; /* 1~8 */
    UINT16                             rxTxMeas:1;              /* 1表示开启Rx-Tx测量上报,0表示停止 */
    UINT16                             rsrpFilterFactor:5;      /* 0,1,2,3,4,5,6,7,8,9,11,13,15,17,19 默认值为4*/
    UINT16                             rsrqFilterFactor:5;      /* 0,1,2,3,4,5,6,7,8,9,11,13,15,17,19 默认值为4*/


    S_PS_PHY_MeasFreqInfo       atMeasFreqInfoList[PS_PHY_MAX_FREQ_NUM];  /* 测量对象的信息列表(含同频和异频), 元素0固定为同频频点的参数 */
} S_PS_PHY_MeasConfig_Req;

/* PS->PHY MCM  测量位图接口
PS_PHY_MEAS_MASK_SET_REQ */
//@MTS(PS_PHY_MEAS_MASK_SET_REQ)
typedef struct
{
     /*
     Bitn对应PS_PHY_MEAS_CONFIG_REQ中的atMeasFreqInfoList[n],n:0~3
     */
     UINT16           measBitMask;
     /*
     取值0表示按协议门限启动,取值1为自定义其他启动
     */
     UINT16           measType;
} S_PS_PHY_MeasMaskSet_Req;

/* PS->PHY MCM  连接态测量GAP配置
PS_PHY_MEAS_GAP_CONFIG_REQ */
//@MTS(PS_PHY_MEAS_GAP_CONFIG_REQ)
typedef struct
{
     /*
     wActivated:取值范围 1,0
     1:激活
     0:去活?
     */
     UINT8                   activated:4;
     /*
     wGapPatternId 取值范围:0,1
     0为pattern0,
     1为pattern1
     */
     UINT8                   gapPatternId:4;
     /*
     PATTEN_0的范围(0~39)
     PATTEN_1的范围(0~79)
     */
     UINT8                   gapOffset;
} S_PS_PHY_MeasGapConfig_Req;

/* PS->PHY 通知物理层删除测量配置及measmask ,仅保留服务小区结果上报
PS_PHY_ABORT_MEAS_REQ 空消息 */


typedef struct
{
    UINT16      rxTxTimeDiff;
    UINT16      curSFN;
} S_PS_PHY_RxTxResult;

typedef struct
{
     UINT16                phyCellId;    /* 取值范围:0~503 */
     UINT8                 rsrp;         /* 取值范围:0~97 */
     UINT8                 rsrq;         /* 取值范围:0~34 */
     SINT8                 snr;          /* 取值范围:-20~40 */
     UINT8                 padding[3];
} S_PS_PHY_MeasResult;

typedef struct
{
    SINT16       sinrInt;           //取值范围 -20~40  整数部分
    UINT16       sinrDec;           //取值范围 0~9 小数部分
} S_PS_PHY_SinrResult;

typedef struct
{
    SINT16       rssiInt;             //取值范围 -129~4,整数部分
    UINT16       rssiDec;             //取值范围 0~9,小数部分
} S_PS_PHY_RssiResult;

typedef struct
{
    S_PS_PHY_SinrResult         tSinrRslt[PS_PHY_MAX_SINR_RESULT_NUM];
    S_PS_PHY_RssiResult         tRssiRslt;
    S_PS_PHY_MeasResult         tCellRslt;
} S_PS_PHY_ServCellMeasResult;

/* PHY ->PS 连接态同频测量结果上报
PS_PHY_INTRA_MEAS_IND*/
//@MTS(PS_PHY_INTRA_MEAS_IND)
typedef struct
{
    UINT8                       cellNum:6;
    UINT8                       rxTxRsltFlg:1;
    UINT8                       neibExistFlg:1;
    S_PS_PHY_RxTxResult        tRxTxRslt;
    S_PS_PHY_SinrResult        tSinrRslt[PS_PHY_MAX_SINR_RESULT_NUM];
    S_PS_PHY_RssiResult        tRssiRslt[PS_PHY_MAX_RSSI_RESULT_NUM];
    S_PS_PHY_MeasResult        atResultList[PS_PHY_MAX_REPORT_CELL_PER_FREQ];
} S_PS_PHY_IntraMeas_Ind;

/* PHY ->PS 连接态异频频测量结果上报
PS_PHY_INTER_MEAS_IND */
//@MTS(PS_PHY_INTER_MEAS_IND)
typedef struct
{
    UINT8        freqNum;
    UINT8        aFreqCellNum[PS_PHY_MAX_INTER_FREQ_NUM];             /*频点下小区的数目指示,atCellRst[]中小区按频点及频点下小区数目依次排列 */
    UINT32       aEarfcn[PS_PHY_MAX_INTER_FREQ_NUM];

    S_PS_PHY_MeasResult     atCellRst[PS_PHY_MAX_INTERFREQ_CELL];
} S_PS_PHY_InterMeas_Ind;

/* PHY -> PS 上报的DRX状态
PS_PHY_DRX_STATE_IND */
//@MTS(PS_PHY_DRX_STATE_IND)
typedef struct
{
    /* 1:drx is used,0:no drx is used */
     UINT8           drxState;
} S_PS_PHY_ConnDrxStateInd;

typedef struct
{
    UINT16    n310:5;            /* 取值:1, 2, 3, 4, 6, 8, 10, 20 */
    UINT16    n311:4;            /* 取值:1, 2, 3, 4, 5, 6, 8, 10 */
    UINT16    t310:4;            /* enum:0,1,2,3,4,5,6,7,8  0:ms0,1:ms50,2:ms100,3:ms200,4:ms500,5:ms1000,6:ms2000,7:ms4000,8:ms6000*/
}S_PS_PHY_TimerConfig;

typedef struct
{
    SINT8                       SpreamInitialRecvTargetPower;/* 取值:{dBm-120, dBm-118, dBm-116, dBm-114,dBm-112, dBm-110, dBm-108,  dBm-106, dBm-104,
                                                                    dBm-102, dBm-100, dBm-98, dBm-96, dBm-94, dBm-92, dBm-90}, 无法获取时使用缺省值dBm-120 */

    UINT8                       preamTransMax;      /* 取值:真实值{n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200}, 无法获取时使用缺省值 n3 */

    UINT8                       powerRampStep:3;    /* 取值:dB0, dB2,dB4, dB6,无法获得参数时候填入最小值db0 */
    UINT8                       zeroCorrelZoneConfig:4;/* 取值范围:(0~15), 无法获得参数时候填入最小值0 */

    UINT8                       raResponseWinSize:4;/* 取值:{ sf2, sf3, sf4, sf5, sf6, sf7, sf8, sf10},无法获得参数时候填入最小值sf2 */
    UINT8                       maxHarqMsg3Tx:4;    /* 取值范围:(1~8), 无法获得参数时候填入最小值1 */

    UINT16                      rootSeqIdx:10;      /* 取值范围:(0~837) */
    UINT16                      highSpeedFlag:2;    /* 取值范围:1为高速,0为非高速,无法获得参数时缺省值0 */
    UINT16                      pwrOffsetGroupB:4;  /* 取值范围:(0~7),分别对应 minusinfinity, dB0, dB5, dB8, dB10, dB12, dB15, dB18,无法获得参数时缺省值 minusinfinity */

    UINT16                      prachConfigIdx:6;   /* 取值范围:(0~63),  无法获得参数时填入0 */
    UINT16                      msgSizeGroupA:9;    /* 取值:56、144、208、256,无法获得参数时填入56 */

    UINT16                      prachFreqOffset:7;  /* 取值范围:(0~94),无法获得参数时填入0 */
    UINT16                      numOfRAPreambles:7; /* 取值:4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,无法获得参数时填入4 */

    UINT8                       sizeOfRAPinGroupA;  /* 取值:4, 8, 12, 16 ,20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,无法获得参数时填入4 */
    UINT8                       conResoTimer;       /* 取值:{sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64},等同协议参数:321.5.1.1  mac-ContentionResolutionTimer,无法获得参数时候填入 sf8 */
} S_PS_PHY_PhyRaConfig;

typedef struct
{
    UINT8                          groupAssignPusch:5;  /* 取值范围:0~29, */
    UINT8                          cyclicShift:3;       /* 取值范围:0~7 */

    UINT8                          hoppingMode:1;        /* 0:interSubFrame, 1:intraAndInterSubFrame */
    UINT8                          enable64Qam:1;        /* 0:DISABLE,1:ENABLE */
    UINT8                          seqHoppingEnable:1;   /* 0:DISABLE,1:ENABLE */
    UINT8                          groupHoppingEnable:1; /* 0:DISABLE,1:ENABLE */
    UINT8                          nsb:3;                /* 取值范围:1~4 */

    UINT8                          puschHoppingOffset;   /* 取值范围:0~98 */
} S_PS_PHY_PuschConfigComm;

typedef struct
{
    UINT8                         betaOffsetAckIndex:4;         /* 取值范围:0-15,  缺省值10 */
    UINT8                         betaOffsetRiIndex:4;          /* 取值范围:0-15, 缺省值12 */
    UINT8                         betaOffsetCqiIndex:4;         /* 取值范围:0-15, 缺省值15 */

    UINT8                         groupHoppingDisabledFlag:1;   /* 取值:0~1 */
    UINT8                         dmrsWithOccActivatedFlag:1;   /* 取值:0~1 */
} S_PS_PHY_PuschConfigDedi;

typedef struct
{
    UINT8                         nrbCqi;             /* 取值范围:0~98, 无法获取时填充0 */
    UINT8                         deltPucchShift:3;   /* 取值:{ds1, ds2, ds3}  无法获得参数时候填入ds1 */
    UINT8                         ncsAn:4;            /* 取值范围:0~8, 无法获得参数时候填入0 */
    UINT16                        n1PucchAn;          /* 取值范围:0~2047,无法获得参数时候填入0 */
} S_PS_PHY_PucchConfigComm;

typedef struct
{
    UINT8                          ackNackRepetitionInd:1;              /* 1: repetitionFactor, n1PucchAnRep 存在, 0: 不存在 */
    UINT8                          tddAckNackFeedbackModePresent:1;      /* 1： tddAckNackFeedbackMode 存在， 0：不存在 */
    UINT8                          tddAckNackFeedbackMode:1;            /* 0:bundling, 1:multiplexing, 非TDD时忽略 */
    UINT8                          wPucchMultiplexingTable:1;           /*0: Table 10.1.3-2,3,4 ,1:Table:10.1.3-5/6/7  */
    UINT8                          repetitionFactor;                   /* 取值:{n2, n4, n6}, 0xff代表的是不存在 */
    UINT16                         n1PucchAnRep;                       /* (0~2047) */
} S_PS_PHY_PucchConfigDedi;

typedef struct
{
    UINT16                        ctrlFlag:1;              /* 取值:0:release, 1:setup */
    UINT16                        srsBandWidthConfig:3;     /* 取值:0,1,2,3,4,5,6,7, 分别对应: {bw0, bw1, bw2, bw3, bw4, bw5, bw6, bw7}  */
    UINT16                        srsSubFrameConfig:4;      /* 取值0~15，分别对应 {sc0, sc1, sc2, sc3, sc4, sc5, sc6, sc7,sc8, sc9, sc10, sc11, sc12, sc13, sc14, sc15}, */
    UINT16                        ackNackSrsSimulTrans:1;   /* 取值:0禁止 重配,1 使能 的重配  */
    UINT16                        srsMaxUpPts :1           /* 0~1, 非TDD忽略 */;
} S_PS_PHY_SrsUlConfigComm;

typedef struct
{
    UINT16                        ctrlFlag:1;              /* 取值:0:release,1:setup */
    UINT16                        srsBandWidthConfig:2;    /* {bw0, bw1, bw2, bw3} */
    UINT16                        freqDomainPos:5;         /* 取值范围:(0~23) */
    UINT16                        cyclicShift:3;           /* {cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7} */
    UINT16                        srsAntennaPortPresent:1; /* 指示srsAntennaPort是否存在 */
    UINT16                        srsAntennaPort:3;        /* {an1, an2, an4 } */
    UINT16                        srsHoppingBandWidth:2;   /* {hbw0, hbw1, hbw2, hbw3} */

    UINT16                        transComb:1;             /* 取值(0~1) */
    UINT16                        duration:1;              /* 取值:0~1 */
    UINT16                        srsConfigIndex:10;      /* (0~1023) */
} S_PS_PHY_SrsUlConfigDedi;

typedef struct
{
    UINT16                        alpha:6;               /* 扩大10倍取值:(0,4,5,6,7,8,9,10) ,对应真实值为{0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1}, 获取不到时缺省取值0 */
    UINT16                        deltaFPucchFormat1:2;  /* 取值:0:deltaF-2; 1:deltaF0; 2:deltaF2;  deltaF-2 代表 -2db, deltaF0 代表0db, 以此类推,获取不到时缺省取值0 */
    UINT16                        deltaFPucchFormat1b:2; /* 取值:0:deltaF1; 1:deltaF3; 2:deltaF5; 获取不到时缺省取值0 */
    UINT16                        deltaFPucchFormat2a:2; /* 取值:0:deltaF-2; 1:deltaF0; 2:deltaF2; 获取不到时缺省取值0 */
    UINT16                        deltaFPucchFormat2b:2; /* 取值:0:deltaF-2; 1:deltaF0; 2:deltaF2; 获取不到时缺省取值0 */
    UINT8                         deltaFPucchFormat2:3;  /* 取值:0:deltaF-2; 1:deltaF0; 2:deltaF1; 3:deltaF2  获取不到时缺省取值0 */

    SINT8                         sP0NominalPusch;       /* 取值范围(-126,24)dBm, 获取不到时缺省取值-126dbm */
    SINT8                         sP0NominalPucch;       /* 取值范围(-127,-96)dBm,获取不到时缺省取值-127dbm */
    SINT8                         sDeltaPreamMsg3;       /* 取值范围:(-1~6), 获取不到时缺省取值-1 */
} S_PS_PHY_UlPowerCtrlComm;

typedef struct
{
    SINT8                         sP0UePusch;          /* 取值范围:(-8,7) */
    SINT8                         sP0UePucch;          /* 取值范围:(-8,7) */

    UINT8                         psrsOffset:5;        /* 取值范围:(0~15) 无法获得参数时候,填入0 */
    UINT8                         deltaMcsEnabled:1;   /* {en0, en1} */
    UINT8                         accumulationEnabled:1;
    UINT8                         deltaTxDOffsetListPucchCtrlFlag:1;    /* 0 ~1,指示 deltaTxDOffsetListPucchFormat1,Format1a1b,Format22a2b,Format3是否存在 */

    UINT8                         psrsOffsetApCtrlFlag:1;               /* 0~1, 指示 psrsOffsetAp 是否存在 */
    UINT8                         psrsOffsetAp:5;                       /* 取值范围:(0~15) */

    UINT8                         deltaTxDOffsetListPucchFormat1:1;      /* 取值:0:dB0, 1:dB-2} */
    UINT8                         deltaTxDOffsetListPucchFormat1a1b:1;   /* 取值:0:dB0, 1:dB-2} */
    UINT8                         deltaTxDOffsetListPucchFormat22a2b:1;  /* 取值:0:dB0, 1:dB-2} */
    UINT8                         deltaTxDOffsetListPucchFormat3:1;      /* 取值:0:dB0, 1:dB-2} */

    UINT8                         filterCoefficient;   /* 取值范围:fc0, fc1, fc2, fc3, fc4, fc5,fc6, fc7, fc8, fc9, fc11, fc13,fc15, fc17, fc19,缺省取值fc4 */
} S_PS_PHY_UlPowerCtrlDedi;

typedef struct
{
    UINT16                        ctrlFlag:1;         /* 0:release, 1:setup (Setup时参数有效) */
    UINT16                        spsProcessNum:4;    /* 取值范围:(1~8) */
    UINT16                        spsIntervalDl:10;   /* 取值：10, 20, 32, 40, 64, 80, 128, 160, 320, 640, 单位：subframe */

    UINT8                         n1PAPNum;           /* 取值范围:(1,4) */
    UINT16                        awN1PucchAnPersistentList[PS_PHY_MAX_SPS_N1PAN_NUM];  /* 取值范围:(0~2047) */
} S_PS_PHY_SpsConfigDl;

typedef struct
{
    UINT16               ctrlFlag:1;                 /* 取值:0:release, 1:setup (Setup时参数有效) */
    UINT16               implicitReleaseAfter:4;     /* 取值:{2, 3, 4, 8} */
    UINT16               spsIntervalUl:10;           /* 取值:10, 20, 32, 40, 64, 80, 128, 160, 320, 640, 单位:subframe */
    UINT16               twoIntervalsConfig:1        /* 取值:0~1, TDD下根据配置置位,非TDD下置0 */;

    SINT8                sP0NominalPuschPersistent;  /* 取值范围:(-126,24) */
    SINT8                sP0UePuschPersistent;       /* 取值范围:(-8,7)dB */
} S_PS_PHY_SpsConfigUl;

typedef struct
{
	UINT16                           spsCRnti;      /*不存在时取值0xFFFF*/
    S_PS_PHY_SpsConfigDl             tSpsConfigDl;
    S_PS_PHY_SpsConfigUl             tSpsConfigUl;
}S_PS_PHY_SpsConfig;

typedef struct
{
    UINT8                ctrlFlag:1;              /* 取值:0:release, 1:setup  (Setup时参数有效) */
    UINT8                shortCycleFlag:1;        /* 取值:0~1,setup有效时,指示drxShortCycleTimer,shortDrxCycle 是否存在 */

    UINT8                onDurationTimer;         /* 取值:psf1, psf2, psf3, psf4, psf5, psf6,psf8, psf10, psf20, psf30, psf40,psf50,ps60,ps80,ps100,ps200 */

    UINT16               drxInactivityTimer;      /* 取值: psf0, psf1, psf2, psf3, psf4, psf5, psf6, psf8, psf10, psf20, psf30, psf40, psf50, psf60,
                                                    psf80, psf100, psf200, psf300, psf500, psf750, psf1280, psf1920, psf2560.*/


    UINT16               longDrxCycle;             /* 取值:(单位为子帧数):10、20、32、40、64、80、128、160、256、320、512、640、1024、1280、2048、2560 */
    UINT16               longDrxCycleStartOffset;  /* 取值范围:0~2559 子帧 */
	UINT8                drxRetransTimer;          /* 取值:psf1, psf2, psf4, psf6, psf8, psf16, psf24, psf33 */

	UINT8                drxShortCycleTimer;       /* 取值范围:(1~16), 没有配置短DRX周期时配置 0xFF */
    UINT16               shortDrxCycle;            /* 取值:sf2, sf5, sf8, sf10, sf16, sf20, sf32, sf40, sf64, sf80, sf128, sf160, sf256, sf320, sf512, sf640 子帧, 没有配置短DRX周期时配置 0xFFFF */
} S_PS_PHY_DrxConfig;

typedef struct
{
    UINT8                ctrlFlag:1;               /* 取值:1:setup, 0:release */
    UINT8                format3Or3A:1;            /* 取值:1:Format3, 0:Format3A */
    UINT8                tpcIndex:5;               /* 取值范围:Format3时:(1~15), Format3A时:(1~31) */
    UINT16               tpcRnti;
} S_PS_PHY_TpcPdcchConfig;


typedef struct
{
    UINT8                transModePresent:1;
    UINT8                transMode:4;             /* 取值范围:1~8,分别对应tm1, tm2, tm3, tm4, tm5, tm6,tm7, tm8-v920 */
    UINT8                ueTransAntCtrlFlag:1;    /* 取值：1为setup, 0为release , setup 时 ueTransAntSelect 有效 */
    UINT8                ueTransAntSelect:1;      /* 取值：0为CLOSELOOP,1为OPENLOOP */
    UINT8                cBRestricBitLenPresent:1;
    UINT8                cBRestricBitLen;         /* 取值范围：2~64  */
    UINT16               awCBRestricBit[8];       /* CodebookSubsetRestric的bit,最大109bit */
} S_PS_PHY_AntenInfoDedi;

typedef struct
{
    UINT16                ctrlFlag:1;                    /* 取值:1:setup, 0:release */
    UINT16                srPucchResourceIndex:13;       /* 取值范围:(0~2047) */

    UINT8                 srConfigIndex;                 /* 取值范围:(0~157) */
    UINT8                 dsrTransMax;                   /* 取值:4, 8, 16, 32, 64 */

    UINT16                srPucchResourceIndexP1Flag:1;  /* 0~1,指示 srPucchResourceIndexP1 是否存在 */
    UINT16                srPucchResourceIndexP1:13;     /* 取值范围:(0~2047) */

} S_PS_PHY_SrConfig;

typedef struct
{
    UINT8                                subframeAllocType:1;     /* 取值:0:oneFrame,1:fourFrames */
    UINT8                                radioFrameAllocPeriod:6; /* 取值:0~5, 对应 {n1, n2, n4, n8, n16, n32} */
    UINT8                                radioFrameAllocOffst;    /* 取值范围, (0~7) */
    UINT8                                aFrameBitStr[3];        /* BIT STRING (SIZE(6)) for oneFrame, BIT STRING (SIZE(24) for fourFrames */
} S_PS_PHY_MbsfnSubframeConfig;

typedef struct
{
    UINT8                                mbsfnSubframeConfigNum;  /* 最大取值为8, 如果SIB2为空,列表长度取0,忽略列表内容 */
    S_PS_PHY_MbsfnSubframeConfig         tMbsfnSubframeConfig[PS_PHY_MAX_MBSFN_SUBFRAME_CFG_NUM];
}S_PS_PHY_MbsfnSubframeConfigList;

typedef struct
{
    UINT16                                csiRsFlag:1;            /* 取值：1：setup, 0：release, Setup时 antennaPortsCount,resourceConfig,subframeConfig,sPc 有效 */
    UINT16                                zeroTxPowerCsiRsFlag:1; /* 取值：1：setup, 0：release, Setup时 zeroTxPowerResourceConfigList,zeroTxPowerSubframeConfig有效 */
    UINT16                                antennaPortsCount:4;    /* 取值：{an1, an2, an4, an8}, 缺省取值 an1 */
    UINT16                                resourceConfig:5;       /* 取值范围:(0~31) */

    SINT8                                 sPc;                              /* 取值范围:(-8~15) */
    UINT8                                 subframeConfig;                   /* 取值范围:(0~154) */
    UINT8                                 zeroTxPowerSubframeConfig;        /* (0~154) */
	UINT16                                zeroTxPowerResourceConfigList;    /* BIT STRING (SIZE (16)) */
} S_PS_PHY_CsiRsConfig;

typedef struct
{
    UINT16                                srsAntennaPortAp:3;        /* 取值:{an1, an2, an4, } */
    UINT16                                srsBandWidthConfigAp:3;    /* 取值:0~3, 对应{bw0, bw1, bw2, bw3} */
    UINT16                                freqDomainPosAp:5;         /* 取值范围:(0~23) */
    UINT16                                transCombAp:1;             /* (0~1) */
    UINT16                                cyclicShiftAp:4;           /* 取值0~7,对应{cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7} */
} S_PS_PHY_SrsConfigAp;

typedef struct
{
    UINT16                                ctrlFlag:1;                    /* 取值:1:setup, 0:release, Setup时结构所有参数有效 */
    UINT16                                srsConfigIndexAp:5;            /* (0~31)*/
    UINT16                                srsConfigApDciFormat4Num:4;    /* 取值范围为0-3, 指示tSrsConfigAp[]数组数目 */
    UINT16                                srsActivateApCtrlFlag:1;       /* 取值:1:setup, 0:release, setup 时 tSrsConfigApDciFormat0/Format1a2b2c 有效 */

    S_PS_PHY_SrsConfigAp                  tSrsConfigAp[PS_PHY_MAX_SRS_CFG_NUM];
    S_PS_PHY_SrsConfigAp                  tSrsConfigApDciFormat0;
    S_PS_PHY_SrsConfigAp                  tSrsConfigApDciFormat1a2b2c;
} S_PS_PHY_SrsUlConfigDediAp;

typedef struct
{
    UINT8                                  measSubframePattern;     /* 0代表的是FDD,1代表的是TDD */
    UINT16                                 aSubframeConfigBit[5];   /* 最大为BIT STRING (SIZE (70) */
} S_PS_PHY_MeasSubframePattern;

typedef struct
{
	UINT16                                cqiFormatIndrPeriodicType:1;     /* 取值:0:widebandCQI,1:subbandCQI */
	UINT16                                wideBandCsiRptModePresent:1;     /* 0:submode1, 1:submode2, cqiFormatIndrPeriodicType 为 widebandCQI, */
	UINT16                                wideBandCsiRptMode:1;            /* 0:submode1, 1:submode2, cqiFormatIndrPeriodicType 为 widebandCQI,present指示它是否有效(R10前版本无效) */
	UINT16                                subbandCqiK:3;                   /* 取值范围 (1~4), cqiFormatIndrPeriodicType 为 subbandCQI 时使用 */
    //UINT16                              subbandCqiPeriodicityFactor:3;   /* 取值:2,4 对应 {n2, n4}, cqiFormatIndrPeriodicType 为 subbandCQI 时,取值0x07为无效(R10前版本无效) */
    UINT16                                simulAckNackAndCQI:1;            /* 取值 0~1 */
    UINT16                                cqiMask:1;                       /* 取值:0:Release,1:Setup */
    UINT16                                csiConfigIndexFlag:1;            /* 取值:0:Release,1:Setup, Setup时 riConfigIndex2 cqiPmiConfigIndex2 有效 */  /* R10后才可能Setup */
    UINT16                                cqiPucchResourceIndex:11;        /* 取值范围(0~1185) */
    UINT16                                cqiPmiConfigIndex:10;            /* 取值范围(0~1023) */
    UINT16                                cqiPucchResourceIndexP1;         /* 取值范围 (0~1184),0xFFFF表示不存在 */ /* R10前不存在,R10后可选 */
    UINT16                                cqiPmiConfigIndex2;              /* 取值范围 (0~1023), 0xFFFF表示不存在 */ /* R10前不存在,R10后 csiConfigIndexFlag 有效时存在 */
	UINT16                                riConfigIndex;                   /* 取值范围 (0~1023),0xFFFF表示不存在 */
    UINT16                                riConfigIndex2;                  /* 取值范围 (0~1023),0xFFFF表示不存在 */
} S_PS_PHY_CqiReportPeriodicConfig;

typedef struct
{
    UINT8               cqiReportModeAPeriodicFlg:1;    /* 取值 0~1, 指示 cqiReportModeAperiodic 是否存在 */
    UINT8               aPeriodicCsiTriggerFlg:1;       /* 取值:1:setup, 0:release,setup时 aPeriodicCsiTrigger 有效, R10版本后 cqiReportModeAPeriodicFlg 有效时可能置位 */
    UINT8               cqiReportPeriodicFlg:1;         /* 取值:1:setup, 0:release, Setup时 tCqiReportPeriodic 有效 */
    UINT8               csiSubframePatternConfigFlg:1;  /* 取值:1为setup, 0为release, R10版本后可能setup,此时 atCsiMeasSubframeSet[]有效 */
    UINT8               pmiRiRpt:1;                     /* 取值:1:setup, 0:release */
    UINT8               cqiReportModeAperiodic:3;       /* 取值:0~7, 分别对应; rm12, rm20, rm22, rm30, rm31,rm32-v1250, rm10-v1310, rm11-v1310*/
    SINT8               sNomPdschRsEpreOffset;          /* 取值范围:(-1~6) */
    UINT16              aPeriodicCsiTrigger;            /* BIT STRING (SIZE (8)*2) */

    S_PS_PHY_CqiReportPeriodicConfig     tCqiReportPeriodic;
    S_PS_PHY_MeasSubframePattern         atCsiMeasSubframeSet[PS_PHY_MAX_CSI_MEAS_SF_PATTERN_NUM];
} S_PS_PHY_CqiReportConfig;

typedef struct{
    UINT16                   edrxInfoFlg:1;   /* EDRX信息是否有效,1:有效.0:无效 */
    UINT16                   edrxCycle:9;     /* 取值为0,1,2,4,6,8,10,12,14,16,32,64,128,256(2621.44S,44分钟)个超帧,
	                                                   其中0值有特殊含义为半个超帧5.12s */
    UINT16                   ptwLen:5;        /* edrx的接收窗,取值1-16 ,对应实际值 wPtwLen *128  ,单位帧 */
}S_PS_PHY_EdrxInfo;

typedef struct{
    UINT32                   sTMSI;
    UINT8                    mmeCode;
}S_PS_PHY_S_TMSI;

typedef struct{
    UINT8                    num;
    UINT8                    awList[PS_PHY_IMSI_LEN];      /* SEQUENCE (SIZE (6..21)) OF IMSI-Digit,IMSI-Digit ::=  (0..9) */
}S_PS_PHY_IMSI;

typedef struct
{
    UINT8                   imsiExistFlag:1;    /* 指示IMSI是否存在,取值1:IMSI存在；0:IMSI不存在 */
    UINT8                   sTmsiExistFlag:1;   /* 指示S-TMSI是否存在,取值1:S-TMSI存在；0:S-TMSI不存在 */
    UINT8                   cesqTrigRange:6;
    S_PS_PHY_IMSI           tImsi;
    S_PS_PHY_S_TMSI         tSTmsi;
} S_PS_PHY_UeId;


/* PS->PHY 公共物理层资源配置 */
/**************************************************************************
RRC配置EPHY CONNECT态无线资源参数,主要包含通用以及专用配置信息
主要用于切换场景.要求EPHY同步目标小区,同步成功或失败后,返回切换确认消息*
**************************************************************************/
//@MTS(PS_PHY_COMMON_CONFIG_REQ)
typedef struct
{
    UINT16                             ueId;                          /* IMSI,不存在时填充0 */

    UINT16                             additionalSpectrumEmissionPresent:1;  /* 取值:0~1, 0:additionalSpectrumEmission 不存在, 1:存在 */
    UINT16                             additionalSpectrumEmission:5;  /* 取值范围：(1~32) */
    UINT16                             subFrameAssign:3;              /* 取值范围：0~6,对应 {sa0, sa1, sa2, sa3, sa4, sa5, sa6}, TDD有效,无效配置0x07 */
    UINT16                             specialSubFramePatterns:4;     /* 取值范围：0~9,对应{ssp0, ssp1, ssp2, ssp3, ssp4,ssp5, ssp6, ssp7,ssp8, ssp9},TDD有效,无效配置0x0F */
    UINT16                             nb:3;                          /* 取值范围：0~7 分别对应{fourT, twoT, oneT, halfT, quarterT, oneEighthT,oneSixteenthT, oneThirtySecondT} */

    UINT16                             ulCpLen:1;                     /* 取值:0:LEN1(normal cyclic prefix)1:LEN2(extended cyclic prefix) */
    UINT16                             defaultPageCycle:9;             /* 取值范围：32,64,128,256, 单位：系统帧 */
    UINT16                             campInd:1;                     /* 取值：1,指示EPHY驻留；0,指示EPHY不进行驻留 */
    UINT16                             pb:2;                         /* (0~3) */
    UINT16                             subAct:1;                      /* 驻留小区ACT信息, 0:TDD, 1:FDD*/
    UINT16                             hyperSfnFlag:1;                 /* 取值:0~1,hyperSFN有效指示,0:表示不更新HSFN值,EPHY使用自行维护值 */
    UINT16                             hyperSfn:10;                    /* 取值范围:0~1023 */
    UINT32                             plmn;

    SINT8                              sPeMax;                        /* 取值范围:(-30~33) */
    SINT8                              sDlReferSignalPower;           /* 取值范围:(-60~50) */
    S_PS_PHY_TimerConfig               tTimeConfig;
    S_PS_PHY_PhyCellInfo               tPhyCellInfo;
    S_PS_PHY_PhyRaConfig               tPhyRaConfig;
    S_PS_PHY_PuschConfigComm           tPuschConfigComm;
    S_PS_PHY_PucchConfigComm           tPucchConfigComm;
    S_PS_PHY_SrsUlConfigComm           tSrsUlConfigComm;
    S_PS_PHY_UlPowerCtrlComm           tUlPowerCtrlComm;
    S_PS_PHY_MbsfnSubframeConfigList   tMbsfnSubframeConfigList;
    S_PS_PHY_EdrxInfo                  tEdrxInfo;
    S_PS_PHY_UeId                      tUeId;
} S_PS_PHY_CommonConfig_Req;

/* PS->PHY 专用物理层资源配置 */
//@MTS(PS_PHY_DEDICATED_CONFIG_REQ)
typedef struct
{
    UINT8                             reConfigFlag:1;          /* 取值 0~1, 指示当前配置是否重配置触发 */
    UINT8                             ttiBundlingInd:1;        /* 取值 0~1, MAC-MainConfig->ul-SCH-Config中指示 */
    UINT8                             maxHarqTx:5;             /* 取值:1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28,缺省取值5 */

    UINT8                             srProhibitTimer:3;       /* 取值范围:(0~7),不存在时填充0 */
    UINT8                             cifPresence:1;           /* 取值 0~1 */
    UINT8                             pa:3;                    /* 取值范围:0~7, 对应真实值{ -6, -4.77, -3, -1.77, 0, 1, 2, 3}, 缺省取值4 */
    S_PS_PHY_SpsConfig                tSpsConfig;
    S_PS_PHY_DrxConfig                tDrxConfig;

    S_PS_PHY_PuschConfigDedi         tPuschConfigDedi;
    S_PS_PHY_PucchConfigDedi         tPucchConfigDedi;
    S_PS_PHY_SrsUlConfigDedi         tSrsUlConfigDedi;
    S_PS_PHY_UlPowerCtrlDedi         tUlPowerCtrlDedi;
    S_PS_PHY_TpcPdcchConfig          tTpcPdcchConfigPucch;
    S_PS_PHY_TpcPdcchConfig          tTpcPdcchConfigPusch;
    S_PS_PHY_CqiReportConfig         tCqiReporting;
    S_PS_PHY_AntenInfoDedi           tAntenInfoDedi;
    S_PS_PHY_SrConfig                tSrConfig;
    S_PS_PHY_TimerConfig             tTimerConfig;

    S_PS_PHY_CsiRsConfig             tCsiRsConfig;
    S_PS_PHY_SrsUlConfigDediAp       tSrsUlConfigDediAp;

} S_PS_PHY_DedicatedConfig_Req;

/*PS->PHY 切换物理层资源配置*/
//@MTS(PS_PHY_HANDOVER_REQ)
typedef struct
{
    SINT8                             sDlReferSignalPower;   /* 取值范围:(-60~50), 无法获取时缺省取值-60 */
    SINT8                             sPeMax;                /* 取值范围(-30~33), 无法获取时缺省取值-30 */

    UINT16                            additionalSpectrumEmissionPresent:1;  /* 取值:0~1, 0:additionalSpectrumEmission 不存在, 1:存在 */
    UINT16                            additionalSpectrumEmission:5;   /* 取值范围：(1~32)， 获取不到时配置1 */
    UINT16                            maxHarqTx:5;          /* 取值:1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28,缺省取值5 */
    UINT16                            pa:3;                 /* 取值范围:(0~7), 对应真实值{ -6, -4.77, -3, -1.77, 0, 1, 2, 3}, 缺省取值4 */
    UINT16                            pb:2;                 /* 取值范围:(0~3), 无法获取时缺省值为0 */
    UINT16                            subAct:1;                        /* 驻留小区ACT信息, 0:TDD, 1:FDD*/

    UINT8                             subFrameAssign:3;      /* 取值范围:0~6,对应 {sa0, sa1, sa2, sa3, sa4, sa5, sa6}, TDD有效, 无法获取时取值0 */
    UINT8                             specialSubFramePatterns:4; /* 取值范围:0~8,对应{ssp0, ssp1, ssp2, ssp3, ssp4,ssp5, ssp6, ssp7,ssp8},TDD有效,无法获取时取值0 */
    UINT8                             ulCpLen:1;             /* 取值:0:LEN1(normal cyclic prefix)1:LEN2(extended cyclic prefix) */
    UINT16                            phichInfoFlag:1;       /* 取值:0~1, 指示phichDuration phichResource是否存在,若不存在,EPHY从MIB获取 */
    UINT16                            phichDuration:1;       /* 取值:0:normal, 1:extended} */
    UINT16                            phichResource:2;       /* 取值:0:oneSixth, 1:half, 2:one, 3:two} */
    UINT16                            srProhibitTimer:3;     /* 取值范围:(0~7), 不存在时填充0 */
    UINT16                            cifPresence:1;         /* 取值:0~1 */

    UINT16                            defaultPageCycle:9;    /* 取值:32,64,128,256(系统帧) */
    UINT16                            nb:3;                /* 取值范围:(0~7) 分别对应{fourT, twoT, oneT, halfT, quarterT, oneEighthT,oneSixteenthT, oneThirtySecondT},获取不到时缺省值:0 */
    UINT16                            antennaPortsCount:4;   /* 取值:{1 ,2, 4, 8}, 无法获取时缺省值1 */

    UINT16                            t304:14;              /*T304时长,取值 50, 100, 150, 200, 500, 1000, 2000, 10000(v1310)},单位ms, 缺省值:50 */
    UINT16                            ttiBundlingInd:1;      /* 取值:0~1, MAC-MainConfig->ul-SCH-Config中指示 */

    UINT16                            ueId;
    UINT16                            crnti;

    S_PS_PHY_PhyCellInfo             tPhyCellInfo;
    S_PS_PHY_SpsConfig               tSpsConfig;
    S_PS_PHY_DrxConfig               tDrxConfig;
    S_PS_PHY_PhyRaConfig             tPhyRaConfig;
    S_PS_PHY_PuschConfigComm         tPuschConfigComm;
    S_PS_PHY_PucchConfigComm         tPucchConfigComm;
    S_PS_PHY_SrsUlConfigComm         tSrsUlConfigComm;
    S_PS_PHY_UlPowerCtrlComm         tUlPowerCtrlComm;
    S_PS_PHY_PuschConfigDedi         tPuschConfigDedi;
    S_PS_PHY_PucchConfigDedi         tPucchConfigDedi;
    S_PS_PHY_SrsUlConfigDedi         tSrsUlConfigDedi;
    S_PS_PHY_UlPowerCtrlDedi         tUlPowerCtrlDedi;
    S_PS_PHY_TpcPdcchConfig          tTpcPdcchConfigPucch;
    S_PS_PHY_TpcPdcchConfig          tTpcPdcchConfigPusch;
    S_PS_PHY_CqiReportConfig         tCqiReporting;
    S_PS_PHY_AntenInfoDedi           tAntenInfoDedi;
    S_PS_PHY_SrConfig                tSrConfig;
    S_PS_PHY_TimerConfig             tTimerConfig;
    S_PS_PHY_MbsfnSubframeConfigList tMbsfnSubframeConfigList;
    S_PS_PHY_CsiRsConfig             tCsiRsConfig;
    S_PS_PHY_SrsUlConfigDediAp       tSrsUlConfigDediAp;
} S_PS_PHY_Handover_Req;

/* PHY -> PS 切换响应 */
//@MTS(PS_PHY_HANDOVER_CNF)
typedef struct
{
     UINT8       result;             /* 取值:1:切换成功   0:切换失败 */
} S_PS_PHY_Handover_Cnf;

/* PS -> PHY 释放资源响应 LTE_P_MAC_RESET_REQ */
/* 结构无 */

/* PS -> PHY 释放资源响应 */
//@MTS(PS_PHY_RELEASE_REQ)
typedef struct
{
    UINT8                          relRfFlg:1;
    UINT8                          keepServCellFlg:1;
} S_PS_PHY_Rel_Req;

/********************** MAC-PHY 接口****************************/
/* PS -> PHY, Access Req */
//@MTS(PS_PHY_ACCESS_REQ)
typedef struct
{
     UINT8                     resetPara:1;// 0 使用上次的RA参数  1 使用新参数发起RA
     UINT8                     raType:2;// NO_CR 0   CRNTI_CR 1   CCCH_CR   2
     UINT8                     raPrachMaskIndex:4;// prach资源掩码ID      0~15
     UINT8                     initRA:1;// PS_INIT    0  PHY_INIT  1
     UINT8                     preamId:7;// prach资源PreamId   0~63
     UINT16                    msgSize;// 预估msg3大小
     UINT16                    subFN;// ps发起RA时间
} S_PS_PHY_Access_Req;

/* PHY -> PS, Access Cnf */
//@MTS(PS_PHY_ACCESS_CNF)
typedef struct
{
    /*wResult
    1:非竞争接入成功
    2:竞争接入成功
    3:竞争定时器超时失败
    4:前导发送达到最大值:preamble_trans_counter = preambleTransMax+1
    */
     UINT8                     result;// RA结果
     UINT8                     preamCnt;// 前导码发送次数 3~200
     UINT16                     cRnti;
} S_PS_PHY_Access_Cnf;

/* PS -> PHY */
//@MTS(PS_PHY_ABORT_ACCESS_REQ)
typedef struct
{
   UINT16                      subFN;
} S_PS_PHY_AbortAccess_Req;

/* PS -> PHY */
//@MTS(PS_PHY_MAC_INIT_RA_REQ)
typedef struct
{
     UINT8                     raPrachMaskIndex;// prach资源掩码ID      0~15
     UINT8                     preamId;// prach资源PreamId   0~63
     UINT16                     subFN;
} S_PS_PHY_MacInitRa_Req;

/* PS -> PHY */
//@MTS(PS_PHY_TA_CMD_REQ)
typedef struct
{
     UINT16                     taCmdType:1;//0:6_BIT, 1:11_BIT
     UINT16                     timeAdvance:11;//TA调整值
     UINT16                     subFN;
} S_PS_PHY_TaCmd_Req;

//@MTS(PS_PHY_TA_TIMER_STOP_IND)
typedef struct
{
    UINT8                        cause;   /* 0 TA_TIMER_EXPIRE ,1 MAC_TA_TIMER_PS_STOP */
} S_PS_PHY_TATimerStop_Ind;

/* PHY -> PS */
//@MTS(PS_PHY_RA_RESPONSE_IND)
typedef struct
{
    UINT16                  taCmd;//TA调整值
    UINT16                  subFN;
} S_PS_PHY_RaResponse_Ind;

typedef struct
{
    UINT16                  subFN;// 物理层发送消息时间
    UINT16                  macPduLen;
    POINTER32               pbMacPduData;
} S_PS_PHY_DlSchData_Ind;

/*PS->PHY, (LTE_P_RESET_REQ)*/
/*No struct*/

/*PHY->PS, (异步消息) PS_PHY_ULSCH_INFO_IND*/
//@MTS(PS_PHY_ULSCH_INFO_IND)
typedef struct
{
    UINT8         spsActivatedFlag:1;// SPS激活标志
    UINT8         ulSchedType:2;// NO_GRANT   0  UL_GRANT  1   RA_GRANT   2
    UINT8         harqId:4;      //  0~7
    UINT8         powerHeadRoomType; //*PH:UE_MAX_POWER-PUSCH_POWER  0~63*/
    UINT16        ulGrantTbSize;// 上行授权大小 单位字节
    SINT16        sPathLoss;// 当前路损 [-16,190]
    POINTER32     pbUlDataPtr;// 写入TB和0xFFFFFFFF的内存地址
}S_PS_PHY_UlSchedInfoInd;

//@MTS(PS_PHY_UL_DATA_SEND_CTRL_INFO_REQ)
typedef struct
{
    POINTER32     pbDataSrc;     // TB净核首地址
    UINT16        dataSendSize;  // TB长度,单位字节
    UINT8         dataValid:1;    // UlDataSendCtrlInfo有效性标志 dataValid需放到最后一个字节才能保障所有参数有效性
    UINT8         sendType:2;    // NEW_DATA 0   NEW_DATA_WITHOUT_SDU 1    TRIGGER_SR_ONLY 2
    UINT8         harqId:4;      //  0~7
} S_PS_PHY_UlDataSendCtrlInfo;

//@MTS(PS_PHY_DLSCH_DATA_IND)
typedef struct
{
    UINT16                    subFN;
    UINT16                    tbLen;
    POINTER32                 pbMacPduData;
} S_PS_PHY_DlDataRecvCtrlInfo;

/*PS_>PHY:LTE_P_WAKEUP_REQ*/
/*No struct*/

/* PS_PHY_SI_DATA_IND, PHY -> PS */
//@MTS(PS_PHY_SI_DATA_IND)
typedef struct
{
    UINT8          procId;
    UINT8          bandwidth;
    UINT16         tblen;
    POINTER32      pbSiData;
    SINT16         wrsrp;
}S_PS_PHY_SiDataInd;

/* PS_PHY_PCH_DATA_IND, PHY -> PS */
//@MTS(PS_PHY_PCH_DATA_IND)
typedef struct
{
    UINT16         tblen;
    POINTER32      pbPchData;
}S_PS_PHY_PchDataInd;
//@MTS(PS_PHY_EPCG_REQ)
typedef struct
{
    UINT8        cmd;      /* 命令号,对应不同的物理层功能 */
    UINT8        mode;     /* 0表示设置,1表示查询 */
    UINT8        val1Fg;
    UINT8        val2Fg;
    UINT32       val1;
    UINT32       val2;
    UINT8        type;    //for 2210 cfw 0:cmepcg 1:sellinfo
}S_PS_PHY_AtPhyCfg_Req;

//@MTS(PS_PHY_EPCG_CNF)
typedef struct
{
    UINT8        cmd;
    UINT8        mode:4;     /* 0表示设置对应的响应,此时,后续参数bRslt有效,bVal2Fg,bVal1,bVal2均无效
                               1表示查询对应的响应,此时,后续参数bRslt无效,bVal2Fg,bVal1,bVal2均有效 */
    UINT8        rslt:4;     /* 表示设置结果,R_TRUE设置成功,R_FALSE设置失败  wMode=0时有效 */
    UINT8        val1Fg[PS_PHY_ATPHYCFG_PARAM_NUM];
    UINT32       val[PS_PHY_ATPHYCFG_PARAM_NUM];
    UINT8        type;    //for 2210 cfw 0:cmepcg 1:sellinfo
}S_PS_PHY_AtPhyCfg_Cnf;

typedef struct
{
    UINT16           measPeriodChgMask;
} S_PS_PHY_MeasPeriodChg_Req;


/* PS_PHY_RESET_REQ, PS -> PHY */
//@MTS(PS_PHY_RESET_REQ)
typedef struct
{
    UINT8                       resetState;    /* 1: PowerOn, 0: Power off */
} S_PS_PHY_Reset_Req;

/* PS_PHY_RESET_CNF, PHY -> PS */
/*No Struct*/

/* PS_PHY_BAND_INFO, PS -> PHY */
typedef struct
{
    UINT8      bandIdx;      /* 频段号,当前支持1,3,5,8,34,38,39,40,41 */
    UINT8      abPadding[3]; /* 增加对齐位，便于PHY实现的拷贝 */
    UINT16     dlBandRange;  /* 下行频段带宽 单位是  100K HZ */
    UINT16     dlStratHz;    /* 下行起始频率 单位是  100K HZ */
    UINT32     minDlEarfcn;  /* 最小下行频点 */

    UINT16     ulBandRange;  /* 上行频段带宽 单位是  100K HZ */
    UINT16     ulStratHz;    /* 上行起始频率 单位是  100K HZ */
    UINT32     minUlEarfcn;  /* 最小上行频点 */
}S_PS_PHY_BandInfo;

//@MTS(PS_PHY_EARFCN_BAND_INFO)
typedef struct
{
    UINT8                  bandNum;
    S_PS_PHY_BandInfo      atBandInfo[PS_PHY_MAX_BAND_NUM];
}S_PS_PHY_BandInfoList;

/* PS_PHY_FREQ_SCAN_REQ , PS -> PHY */
typedef struct
{
    UINT32          startEarfcn;/* 频段的开始频点号 */
    UINT32          endEarfcn;/* 频段的结束频点号 */
} S_PS_PHY_FreqBand;

//@MTS(PS_PHY_FREQ_SCAN_REQ)
typedef struct
{
    UINT8                        procId;     /* 处理号,用于区分各次请求 */
	UINT8                        bandNum:5;    /* 扫描频段的个数 */
    UINT8                        freqScanType:2;/* 0 扫描一个频点范围,1 包络扫描,2 plmnlist全频段扫描可用频点 */
    S_PS_PHY_FreqBand   atFreqBandList[PS_PHY_MAX_NUM_OF_FREQSCAN_RANGE]; /* 用于扫描的频段信息 */
    UINT8                        bMultiBinParam;
} S_PS_PHY_FreqScan_Req;

/* PS_PHY_FREQ_SCAN_CNF , PHY -> PS */
//@MTS(PS_PHY_FREQ_SCAN_CNF)
typedef struct
{
    UINT8                        reportEarfcnNum;/* 上报的频点个数 */
    UINT8                        procId;/* 处理号,用于区分各次请求 */
    UINT32                       aEarfcn[PS_PHY_MAX_NUM_OF_EARFCN_SCAN];/* 上报的频点信息 */
} S_PS_PHY_FreqScan_Cnf;

/* PS_PHY_CELL_SEARCH_REQ , PS -> PHY */
//@MTS(PS_PHY_CELL_SEARCH_REQ)
typedef struct
{
    UINT8                    procId;/* 处理号,用于区分各次请求 */
    UINT8                    searchType:3; /* 1 最强小区搜索, 2 小区列表, 3 指定小区搜索, 4 psmdeepsleep指定搜 */
    UINT8                    cellNum:3;/* 搜索指定小区数量 */
    UINT32                   earfcn;/* 本次扫小区的频点 */
    UINT16                   aPhyCellIdList[PS_PHY_MAX_NUM_OF_STORED_CELL]; /* 搜索指定小区信息 */
    UINT8                    bMultiBinParam;
} S_PS_PHY_CellSearch_Req;


/* PS_PHY_CELL_SEARCH_CNF , PHY -> PS */
typedef struct
{
    UINT16                phyCellId;/* 小区PCI */
    UINT8                 rsrp;/* 小区信号强度 */
    UINT8                 rsrq;/* 小区信号质量 */
    SINT16                wSINR;
} S_PS_PHY_CellSearchResult;

//@MTS(PS_PHY_CELL_SEARCH_CNF)
typedef struct
{
    UINT8                          procId;/* 处理号,用于区分各次请求 */
    UINT8                          cellNum;/* 上报的小区数量 */
    UINT32                         earfcn;/* 本次扫小区的频点 */
    S_PS_PHY_CellSearchResult      atCellSearchResultList[PS_PHY_MAX_NUM_OF_CELL_SEARCH]; /* 本次扫小区的结果 */
} S_PS_PHY_CellSearch_Cnf;

/* PS_PHY_ABORT_CELL_SEARCH_REQ PS -> PHY */
/*No Struct*/

/* PS_PHY_READ_SIB1_REQ , PS -> PHY */
//@MTS(PS_PHY_READ_SIB1_REQ)
typedef struct
{
    UINT8                 procId;/* 处理号,用于区分各次请求 */
    UINT8                 siRequestForHO;/* 本次读小区信息是否和HO相关 */
    UINT16                phyCellId;/* 小区PCI */
    UINT32                earfcn;/* 小区频点 */
} S_PS_PHY_ReadSib1_Req;

/* PS_PHY_SCHED_SI_REQ , PS -> PHY */
typedef struct
{
    UINT16          siPeriodicity;      /* 取值范围:rf8, rf16, rf32, rf64, rf128, rf256, rf512   (无线帧单位) */
    UINT8           siIndex;            /* 取值范围:1～32 */
} S_PS_PHY_SiSchedInfo;

//@MTS(PS_PHY_READ_SI_REQ)
typedef struct
{
    UINT32                         earfcn;/* 调度系统消息的频点 */
    UINT16                         subFrameAssign:3;/* 子帧配比 */
    UINT16                         specialSubFramePatterns:4;/* 特殊子帧配比 */
    UINT16                         phyCellId:9;/* 调度系统消息的PCI */
    UINT16                         siWindowLen:6;   /* 取值:1, 2, 5, 10, 15, 20, 40*/
    UINT16                         siSchedNum:3;/* 调度系统消息的数量 */
    S_PS_PHY_SiSchedInfo           atSiSchedInfoList[PS_PHY_MAX_NUM_OF_SI]; /* 调度信息 */
} S_PS_PHY_SchedSi_Req;

/* PS_PHY_ABORT_SI_READ_REQ , PS -> PHY */
/* no struct */

/* PS_PHY_PBCH_READ_FAIL_IND , PHY -> PS */
//@MTS(PS_PHY_PBCH_READ_FAIL_IND)
typedef struct
{
    UINT8                        procId;/* 处理号,用于区分各次请求 */
} S_PS_PHY_PbchReadFail_Ind;

typedef struct {
    UINT8                      bScanTime;
    UINT8                      bScanNum;
    UINT8                      bChannelNum;
    UINT8                      abChannel[PS_PHY_WIFI_CHANNEL_MAX];
}S_PS_WifiScan_Req;

typedef struct {
    UINT8                      bScanTime;
    UINT8                      abMac[6];
}S_PS_WifiScanAp_Req;

typedef struct {
    UINT8                      bChannel;
    UINT8                      abMac[6];
    SINT8                      cRssi;
    UINT8                      abSsid[32];
}S_PS_WifiScancInd_ChApParam;

typedef struct {
    UINT8                        bApNum;
    S_PS_WifiScancInd_ChApParam  atApNum[20];
}S_PS_WifiScan_Ind;

/*
================================================================================
  Structure  : S_PS_WifiSentestStartReq_t
--------------------------------------------------------------------------------
  Direction  : PS -> PHY
  Scope      : 
  Comment    : PS_PHY_WIFISENTEST_START_REQ, Used to start WiFiSentest Service
================================================================================
*/
typedef struct
{
    UINT8 mode;       //0: signle mod 1:con mod
    UINT8 ch_id;      // channel no
    UINT16 loop_time; 
    UINT16 max_loop;  //max loop cnt
    UINT16 resvered;
} S_PS_WifiSentestStartReq;

/*
================================================================================
  Structure  : S_PS_WifiSentestResultInd_t
--------------------------------------------------------------------------------
  Direction  : PHY -> PS
  Scope      : 
  Comment    : PS_PHY_WIFISENTEST_RESULT_IND, Used to Report WiFiSentest Result
================================================================================
*/
//wifi sensitivity test
typedef struct
{
    UINT8 mode;                             //0: signle mod 1:con mod
    UINT8 ch_id;                            // channel no
    SINT16 rssi;
    UINT16 loop_cnt;                        // for con mod,  executed loop cnt
    UINT16 max_loop;                        // for con mod, max loop cnt
    UINT16 result[10]; // test result (beacon frame counter), for single mod, the result fixed in u16_result[0]
    UINT16 resvered;
} S_PS_WifiSentestResultInd;


//Coment add for MTS use
//@MTS(PS_PHY_UL_SR_SEND_REQ->S_PS_PHY_UL_SRSEND_REQ)
//@MTS(PS_PHY_MTS_STOP_REQ->S_PS_PHY_MTS_STOP_REQ)
//@MTS(PS_PHY_RESET_CNF->S_PS_PHY_RESET_CNF)
//@MTS(PS_PHY_STOP_SI_READ_REQ->S_PS_PHY_STOP_SI_READ_REQ)
//@MTS(PS_PHY_STOP_SCAN_SEARCH_REQ->S_PS_PHY_STOP_SCAN_SEARCH_REQ)
//@MTS(PS_PHY_ABORT_MEAS_REQ->S_PS_PHY_ABORT_MEAS_REQ)
//@MTS(PS_PHY_MAC_RESET_REQ->S_PS_PHY_MAC_RESET_REQ)
//@MTS(PS_PHY_DRX_CMD_REQ->S_PS_PHY_DRX_CMD_REQ)
//@MTS(PS_PHY_WAKEUP_REQ->S_PS_PHY_WAKEUP_REQ)
//@MTS(PS_PHY_OUT_OF_SYNC_IND->S_PS_PHY_OUT_OF_SYNC_IND)
//@MTS(PS_PHY_RECOVERY_SYNC_IND->S_PS_PHY_RECOVERY_SYNC_IND)
//@MTS(PS_PHY_PUCCH_SRS_REL_IND->S_PS_PHY_PUCCH_SRS_REL_IND)
//@MTS(PS_PHY_RELEASE_CNF->S_PS_PHY_RELEASE_CNF)
//@MTS(PS_PHY_PHYWAKEUPPS_REQ->S_PS_PHY_PHYWAKEUPPS_REQ)
//@MTS(PS_PHY_ICP_REQ->S_PS_PHY_ICP_REQ)
//@MTS(PS_PHY_AMT_MSG_IND->S_PS_PHY_AMT_MSG_IND)

/*******************************************************************************************************/

#ifndef _OS_WIN

#define CTRL_INFO_PS_EPHY_ADDR_BASE             IRAM_BASE_ADDR_LTE_NC
#define PS_LTE_GET_ULSCHEDIND_PTR               CTRL_INFO_PS_EPHY_ADDR_BASE
#define CTRL_INFO_LTE_ULDATASEND_ADDR           PS_LTE_GET_ULSCHEDIND_PTR //ps to phy ul send data ctrl info ram addr

#define CTRL_INFO_LTE_SUBFRAMENUM_ADDR          IRAM_BASE_ADDR_LTE_SUBFRAMENUM
#define PS_LTE_GET_HFN                          (*(volatile UINT32 *)(CTRL_INFO_LTE_SUBFRAMENUM_ADDR))

#define UL_DATA_PROTECT                         0x5A5A5A5A

#define PS_LTE_SCELL_MEAS_RSLT_ADDR             DRAM_BASE_ADDR_SCELL_MEAS_RSLT_NC   //sizeof(S_PS_PHY_ServCellMeasResult)

#define CTRL_INFO_LTE_ULSRSEND_ADDR             IRAM_BASE_ADDR_LTE_ULSRSEND_FLAG    //SR  info ram addr

typedef struct
{
    UINT8 mode;                             //0: non instrument uicc card 1:instrument uicc card
    UINT8 padding[3];
} S_PS_InstruementCardFlag;
#define PS_LTE_INSTRUMENT_CARD_FLAG_ADDR        DRAM_BASE_ADDR_INSTRUMENT_CARD_FLAG_NC //sizeof(S_PS_InstruementCardFlag)
#endif

#endif  /*_PS_EPHY_INTERFACE_H*/
