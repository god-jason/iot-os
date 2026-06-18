/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        ps_com.h
 *
 * @brief       This file contains ps common processing
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef    PS_COM_H
#define    PS_COM_H

#include "ps.h"
#include "ap_ps_interface.h"
#include "psevent.h"
#include "ps_database.h"
#ifndef _OS_WIN
#include "drv_lpm.h"
#endif
#define SUBMDL_NUM                      (uint8_t)((SUBMDL_PSEND - SUBMDL_PSBASE))

#ifdef SIM_UICC
#define DUMMYUICC_THREAD_ID             (uint8_t)(PS_L2_THREAD_ID+1)
#endif
#ifdef TTCN_SIMPHY
#ifdef  DUMMYUICC_THREAD_ID
#define TTCNSIMPHY_THREAD_ID            (uint8_t)(DUMMYUICC_THREAD_ID+1)
#else
#define TTCNSIMPHY_THREAD_ID            (uint8_t)(PS_L2_THREAD_ID+1)
#endif
#endif

#ifdef  TTCNSIMPHY_THREAD_ID
#define THREAD_ID_CNT                   (uint8_t)(TTCNSIMPHY_THREAD_ID+1)
#elif defined(DUMMYUICC_THREAD_ID)
#define THREAD_ID_CNT                   (uint8_t)(DUMMYUICC_THREAD_ID+1)
#else
#define THREAD_ID_CNT                   (uint8_t)(PS_L2_THREAD_ID+1)
#endif

#define EPHY_THREAD_ID                  (uint8_t)(THREAD_ID_CNT)

#define SLOG_BASE_PSCOM                 0xF200    /*0xF200 ~ 0xF264,total:100*/
#define SLOG_BASE_PSDATABASE            (SLOG_BASE_PSCOM + 0x0033)  /*pscom:0xF200 ~ 0xF232,psdetabase:0xF233 ~ 0xF264*/

//@MSG(SLOG_MODULE_PS){{
/* ------ PS COM SLOG ID ----------------------------------------------*/
#define SLGID_PSCOM_SENDMSG_MSGERR            (SLOG_BASE_PSCOM + 0)
#define SLGID_PSCOM_SENDMSG_FAIL              (SLOG_BASE_PSCOM + 1)
#define SLGID_PSCOM_DESTORYLINK_PARAMNULL     (SLOG_BASE_PSCOM + 2)
#define SLGID_PSCOM_GETLINKCOUNT_PARAMNULL    (SLOG_BASE_PSCOM + 3)
#define SLGID_PSCOM_GETFIRSTITEM_PARAMNULL    (SLOG_BASE_PSCOM + 4)
#define SLGID_PSCOM_GETLASTITEM_PARAMNULL     (SLOG_BASE_PSCOM + 5)
#define SLGID_PSCOM_GETNEXTITEM_PARAMNULL     (SLOG_BASE_PSCOM + 6)
#define SLGID_PSCOM_INSERTITEM_PARAMNULL      (SLOG_BASE_PSCOM + 7)
#define SLGID_PSCOM_DELETEITEM_PARAMNULL      (SLOG_BASE_PSCOM + 8)
#define SLGID_PSCOM_SETCURRITEM_PARAMNULL     (SLOG_BASE_PSCOM + 9)

/* ------ PS database SLOG ID ------------------------------------------*/
#define SLOG_PSDATABASE_AMTNVBAND_EXP         (SLOG_BASE_PSDATABASE + 0)
#define SLOG_PSDATABASE_RWNVINIT_READ_FAIL    (SLOG_BASE_PSDATABASE + 1)
#define SLOG_PSDATABASE_RONVINIT_READ_FAIL    (SLOG_BASE_PSDATABASE + 2)
#define SLOG_PSDATABASE_NVREAD_FAILURE        (SLOG_BASE_PSDATABASE + 3)
#define SLOG_PSDATABASE_NVWRITE_FAILURE       (SLOG_BASE_PSDATABASE + 4)
#define SLOG_PSDATABASE_NVWRITE_PSEXT_FAILURE (SLOG_BASE_PSDATABASE + 5)
#define SLOG_PSDATABASE_NVINIT_PSEXT_READ_FAIL (SLOG_BASE_PSDATABASE + 6)

//}}

#define PSCOM_PRNTID(msgId)                    SLOG_PRINT_ID_NO_LEVEL(SLOG_MODULE_PS, SUBMDL_PSCOM, msgId)


#ifdef SLOG_TRACE
#define SLOG_PRT(level, printId, paraNum, ...)     SLOG_Printf(level, printId, paraNum, ##__VA_ARGS__)
#define SDTLOG_OUT(level, printId, addInfo, codeData, codeLen)      SLOG_StdLogOutput(level, printId, addInfo, codeData, codeLen)
#else
#define SLOG_PRT(level, printId, paraNum, ...)
#define SDTLOG_OUT(level, printId, addInfo, codeData, codeLen)
#endif

#ifdef _OS_WIN
#if defined(TTCN_SIMPHY) || defined(C_SHARP_2110) || defined(PS_IT_2110) || defined(_UT_SWITCH)
typedef struct
{
    UINT8 mode;                             //0: non instrument uicc card 1:instrument uicc card
    UINT8 padding[3];
} S_PS_InstruementCardFlag;
#endif
#ifdef TTCN_SIMPHY
extern uint16_t Ttcn_SimEphy_GetSubframe(void);
extern uint16_t Ttcn_SimEphy_GetHyperSfn(void);
#define    PS_LTE_GET_SUBFRAMENUM   Ttcn_SimEphy_GetSubframe()
#define    PS_LTE_GET_HYPERSFN      Ttcn_SimEphy_GetHyperSfn()
#else
#ifdef C_SHARP_2110
extern uint32_t           g_dwCsSfnCount;
extern uint32_t           g_dwCsHfnCount;
#define    PS_LTE_GET_SUBFRAMENUM   g_dwCsSfnCount
#define    PS_LTE_GET_HYPERSFN      g_dwCsHfnCount
#else
#define    PS_LTE_GET_SUBFRAMENUM   0
#define    PS_LTE_GET_HYPERSFN      0
#endif

#endif
#else
//TBD  ???
#define    PS_LTE_GET_SUBFRAMENUM     LPM_GetSfnLte()
#define    PS_LTE_GET_HYPERSFN        LPM_GetHfnLte()
#endif

#ifdef TRACE_TIME
#define MAC_TEST_TICK_MAX_NUM 16

typedef struct {
#define MAC_TEST_TICK_MAX_CNT 20
    osTime_t    timeTick[MAC_TEST_TICK_MAX_NUM][MAC_TEST_TICK_MAX_CNT];
    uint8_t     idx;
}S_Mac_TimeTick;

typedef struct {
#define MAC_TEST_SEND_TICK_MAX_CNT    2
    osTime_t    timeTick[MAC_TEST_TICK_MAX_NUM][MAC_TEST_SEND_TICK_MAX_CNT];
    uint32_t    msgId;
    uint8_t     idx;
}S_Mac_SendMsgTimeTick;

#define L2_PROC_TIME_MAX_MSG  64
typedef struct {
    uint32_t  msgId;
    osTime_t  startTime;
    uint32_t  ProcTimeLen;
}S_L2TimeTick;

typedef struct{
    S_Mac_TimeTick dlDataInd;
    S_Mac_TimeTick decodePdu;
    S_Mac_TimeTick decodeHeader;
    S_Mac_TimeTick crResultInd;
    S_Mac_SendMsgTimeTick sendPhyMsg;
    S_Mac_SendMsgTimeTick sendMsg;
    S_Mac_TimeTick ulGrant;
    uint32_t dwEl2MsgProcIndex;
    S_L2TimeTick  el2MsgProc[L2_PROC_TIME_MAX_MSG];
}S_Mac_TestTimeTick;

extern S_Mac_TestTimeTick g_Mac_TestTimeTick;
#endif

/*====================== signal trace directions =========================*/

typedef struct {
    uint16_t                      wMsgId;
    uint16_t                      wMsgLen;
    //uint8_t                     bSrcMod;
    //uint8_t                     bDestMod;
    //osThread_t                  tSrcTask;
    //osThread_t                  tDestTask;
    uint8_t                       bDestThreadIdx;
    uint8_t                       *pMsg;
}S_MsgInfo;

/**************************************************************************
*                            message sending structure                   *
**************************************************************************/

#ifdef SLOG_TRACE
#include "stdlog.h"
typedef struct {
    uint32_t           dwValidBitmap;
    uint32_t           dwStartTick[DELAY_CNTTYPE_MAXVALUE+1];
}S_DelayTest;
#endif



#define DEV_NV_AMTLTEBAND_NUM                            (uint8_t)16


#define MaxCidNum                                        (uint8_t)11
#define INVALID_BAND                                     (uint8_t)0xFF

#define PS_NoPtmsi                                       (uint8_t)0
#define PtmsiValidInCurLa                                (uint8_t)1
#define PtmsiValidInOtherLa                              (uint8_t)2
#define PtmsiValidInOtherPlmn                            (uint8_t)3

#define Uicc_AdLen                                       (uint8_t)1

/*************************************************************************/




/**************************************************************************/

#define LINEAR_LINK_TYPE                                (uint8_t)0
#define DOUBLE_LINK_TYPE                                (uint8_t)1
#define LINEAR_CIRCULAR_LINK_TYPE                       (uint8_t)2
#define DOUBLE_CIRCULAR_LINK_TYPE                       (uint8_t)3

/*************************************************************************/

/**************************************************************************/
/*0  PS mode 2 of operation*/
#define EPS_PSMODE2                                      (uint8_t)0
/*1  CS/PS mode 1 of operation*/
#define EPS_CSPSMODE1                                    (uint8_t)1
  /*2  CS/PS mode 2 of operation*/
#define EPS_CSPSMODE2                                    (uint8_t)2
 /*3  PS mode 1 of operation*/
#define EPS_PSMODE1                                      (uint8_t)3


#define TIN_INVALID                                      (uint8_t)0
#define TIN_P_TMSI                                       (uint8_t)1
#define TIN_GUTI                                         (uint8_t)2
#define TIN_RAT_RELATED                                  (uint8_t)3
#define EPS_NCS_KSI_INVALID                              (uint8_t)0xFF

/*************************************************************************/
#define Lte_SetRelativeTimer(TimerValue, ExpiredEv, TimerNo)\
        SetRelativeTimer(TimerValue, ExpiredEv, TimerNo)


#define ps_printf(fmt,...) printf("[%s][%lu] "fmt"\n", __FUNCTION__, __LINE__,##__VA_ARGS__)


#ifdef _USE_PSM
void Ps_PsmTimerWakeupCfgLoop(void);
#endif

#ifdef TRACE_TIME
extern osTime_t ps_trace_get_time(void);
#endif

extern void* Mem_GetUB(uint32_t dwSize, const char *pFile, uint32_t dwLine);

#define Ps_GetUB(size) Mem_GetUB(size, __FILE__, __LINE__)

extern void Ps_RetUB(void *pMem);
extern osTimer_t SetRelativeTimer(uint32_t dwTimerLen, uint32_t dwMsgId, uint32_t dwArg);
extern void SendMsg(S_MsgInfo *pMsgInfo, uint32_t *pCause);

extern uint8_t L2com_TxBuffStatus();
extern uint8_t L2com_T2BuffEmptyStatus();

extern uint32_t Pdcp_GetMaci(uint8_t bAlg, uint32_t dwCount, uint8_t  bBearId, uint8_t bDir, uint8_t *pbkey, uint8_t *pbInData, uint16_t wDataLen);
extern uint8_t Pdcp_Verify(uint8_t bAlg, uint32_t dwCount, uint8_t  bBearId, uint8_t bDir, uint8_t *pbkey, uint8_t *pbInData, uint16_t wDataLen);
extern void Pdcp_Cipher(uint8_t bAlg, uint32_t dwCount, uint8_t  bBearId, uint8_t bDir, uint8_t *pbkey, uint8_t *pbInData, uint16_t wDataLen);

extern void Hmbc_BuildNasMeasReport(uint16_t*pwLength,uint8_t* *pbdata );
extern void Ps_SetInstrumentCardFlag(uint8_t instrumentFlag);
#ifdef TTCN_SIMPHY
extern uint32_t ttcnGetTickCount();
#endif
#define OSS_NO_INHERIT         0x0
extern osMutexId_t oss_CreateMutex(const char *name_ptr, uint32_t priority_inherit);

#ifdef SLOG_TRACE
void SetDelayTestStart(uint8_t bCountType, uint32_t dwForeOffset);
void SetDelayTestEnd(uint8_t bCountType, uint8_t bRslt);
void AddDelayTestStartTickOff(uint8_t bCountType, uint32_t dwTimelen);
#endif

extern uint8_t g_L2_UlGrantWIndex;
extern uint8_t g_L2_UlGrantRIndex;
#endif
