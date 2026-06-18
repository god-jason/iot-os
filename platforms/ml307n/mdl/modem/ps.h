/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        ps.h
 *
 * @brief       Headfile of ps
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef    PS_PUB_H
#define    PS_PUB_H

#include "ap_ps_interface.h"
#include "nvparam_ps.h"

#ifdef OS_USING_PM
#define _USE_PSM
#endif

#define NULL_BYTE              (uint8_t)0xff
#define NULL_WORD              (uint16_t)0xffff
#define NULL_DWORD             (unsigned int)0xffffffff

#define OSS_MS_PER_TICK        1

#define EV_INIT                0


/*========================== LTE system ==========================*/
#define NO_ACT                  (uint8_t)0x00
#define LTE_ACT                 (uint8_t)0x10


#ifndef OSS_NULL
#define OSS_NULL                 0
#endif
#ifndef OSS_INVALID_TIMER_ID
#define OSS_INVALID_TIMER_ID     OSS_NULL
#endif
#ifndef OSS_INVALID_TASK_ID
#define OSS_INVALID_TASK_ID      OSS_NULL
#endif

#ifndef OSS_SUCCESS
#define OSS_SUCCESS              0x0
#endif
#ifndef OSS_ERROR
#define OSS_ERROR                0xffffffff
#endif

#define KC128_LEN                           (uint8_t)32
#define KC64_LEN                            (uint8_t)8
#define CK_LEN                              (uint8_t)16
#define IK_LEN                              (uint8_t)16
#define KASME_LEN                           (uint8_t)32
/*EM  max ncell num*/
#define EM_MAX_NLCELL_NUM                   (uint8_t)10
#define PS_MaxCallNum_Len                   (uint8_t)40 
#define PS_MaxSubAddr_Len                   (uint8_t)20 

/*max freq num*/
#define MAX_STORED_EFRQE_NUM                PS_MAX_STORED_EFRQE_NUM

#define MAX_BAND_NUM                        (uint8_t)10
#define MAX_STORED_FRQE_NUM                 (uint8_t)16
/*max band num*/
#define MAX_BAND_LIST_LENGTH                (uint8_t)66 
#define MAX_LEN_ASME_KEY                    (uint8_t)32
#define MAX_STANDBY_NUM                     (uint8_t)1

#define MAX_TAI_NUM                         (uint8_t)16        /*TAI list count*/

#define MAX_IMSI_LEN                        (uint8_t)16


/* PS SUBSYSTEM MODULE */
//@SUBMODULE(SLOG_MODULE_PS){{
#define SUBMDL_PSBASE                       (uint8_t)(0)
#define SUBMDL_NASCOM                       (uint8_t)(SUBMDL_PSBASE + 1)
#define SUBMDL_EMM                          (uint8_t)(SUBMDL_PSBASE + 2)
#define SUBMDL_ESM                          (uint8_t)(SUBMDL_PSBASE + 3)
#define SUBMDL_SMS                          (uint8_t)(SUBMDL_PSBASE + 4)
#define SUBMDL_USIM                         (uint8_t)(SUBMDL_PSBASE + 5)
#define SUBMDL_L3CDEC                       (uint8_t)(SUBMDL_PSBASE + 6)
#define SUBMDL_ROHCv1                       (uint8_t)(SUBMDL_PSBASE + 7)
#define SUBMDL_CMN                          (uint8_t)(SUBMDL_PSBASE + 8)
#define SUBMDL_RESC                         (uint8_t)(SUBMDL_PSBASE + 9)
#define SUBMDL_CSC                          (uint8_t)(SUBMDL_PSBASE + 10)
#define SUBMDL_MBC                          (uint8_t)(SUBMDL_PSBASE + 11)
#define SUBMDL_ASN                          (uint8_t)(SUBMDL_PSBASE + 12)
#define SUBMDL_PDCP                         (uint8_t)(SUBMDL_PSBASE + 13)
#define SUBMDL_RLC_UL                       (uint8_t)(SUBMDL_PSBASE + 14)
#define SUBMDL_RLC_DL                       (uint8_t)(SUBMDL_PSBASE + 15)
#define SUBMDL_MAC_UL                       (uint8_t)(SUBMDL_PSBASE + 16)
#define SUBMDL_MAC_DL                       (uint8_t)(SUBMDL_PSBASE + 17)
#define SUBMDL_EPHY                         (uint8_t)(SUBMDL_PSBASE + 18)
#define SUBMDL_UICC_DATA                    (uint8_t)(SUBMDL_PSBASE + 19)
#define SUBMDL_MME_EMM                      (uint8_t)(SUBMDL_PSBASE + 20)
#define SUBMDL_MME_ESM                      (uint8_t)(SUBMDL_PSBASE + 21)
#define SUBMDL_ENMAC_UL                     (uint8_t)(SUBMDL_PSBASE + 22)
#define SUBMDL_ENMAC_DL                     (uint8_t)(SUBMDL_PSBASE + 23)
#define SUBMDL_ENRLC_UL                     (uint8_t)(SUBMDL_PSBASE + 24)
#define SUBMDL_ENRLC_DL                     (uint8_t)(SUBMDL_PSBASE + 25)
#define SUBMDL_ENPDCP                       (uint8_t)(SUBMDL_PSBASE + 26)
#define SUBMDL_ENRRC                        (uint8_t)(SUBMDL_PSBASE + 27)
#define SUBMDL_EL2                          (uint8_t)(SUBMDL_PSBASE + 28)
#define SUBMDL_ADAPTER                      (uint8_t)(SUBMDL_PSBASE + 29)
#define SUBMDL_ITT                          (uint8_t)(SUBMDL_PSBASE + 30)
#define SUBMDL_TTCNSIMPHY                   (uint8_t)(SUBMDL_PSBASE + 31)
#define SUBMDL_SOCKETTASK                   (uint8_t)(SUBMDL_PSBASE + 32)
#define SUBMDL_PSCOM                        (uint8_t)(SUBMDL_PSBASE + 33)
#define SUBMDL_IMSCOM                       (uint8_t)(SUBMDL_PSBASE + 34)
#define SUBMDL_IMSCC                        (uint8_t)(SUBMDL_PSBASE + 35)
#define SUBMDL_IMSSS                        (uint8_t)(SUBMDL_PSBASE + 36)
#define SUBMDL_IMSSMS                       (uint8_t)(SUBMDL_PSBASE + 37)
#define SUBMDL_IMSACC                       (uint8_t)(SUBMDL_PSBASE + 38)
#define SUBMDL_IMSSIP                       (uint8_t)(SUBMDL_PSBASE + 39)
#define SUBMDL_IMSSUA                       (uint8_t)(SUBMDL_PSBASE + 40)
#define SUBMDL_SDT                          (uint8_t)(SUBMDL_PSBASE + 60)
#define SUBMDL_PSEND                        (uint8_t)(SUBMDL_SDT)
//}}

/*CellID*/
typedef uint32_t                      S_CellId;
/*LAC*/
typedef uint16_t                      S_LAC;
/*RAC*/
typedef uint16_t                      S_Rac;          
/*CK*/
typedef uint8_t                       S_CK[CK_LEN];
/*IK*/
typedef uint8_t                       S_IK[IK_LEN];



/*LocationAreaIdentity*/
typedef struct {
    BITS                            bMCC1     :4;
    BITS                            bMCC2     :4;
    BITS                            bMCC3     :4;
	
    BITS                            bMNC3     :4;        
    BITS                            bMNC1     :4;
    BITS                            bMNC2     :4;

    uint16_t                        wLac;
}S_LAI;

/*RouteAreaIdentity*/
typedef struct {
    BITS                            bMCC1     :4;
    BITS                            bMCC2     :4;
    BITS                            bMCC3     :4;
	
    BITS                            bMNC3     :4;     
    BITS                            bMNC1     :4;
    BITS                            bMNC2     :4;

    uint8_t                         bRac;
    uint16_t                        wLac;
}S_PS_RAI;


typedef uint8_t                       S_LteUeMode;                         


/*LTE cell info*/
typedef struct{

    uint32_t                          dwEarfcn; 
    uint16_t                          wPhyCellId;   
}S_PS_LteCell;




/*CELL INFO*/
typedef struct{
    uint8_t                            bAct;
    uint8_t                            bPadding;
    uint16_t                           wPhyCellId;
    uint32_t                           dwEarfcn; 
}S_PS_CellInfo;


/*CALL SUB ADDR*/
typedef struct{
    uint8_t                            bTypeSubAddr;
    uint8_t                            bSubAddrLen;     
    uint8_t                            abPadding[2];

    uint8_t                            abSubAddr[PS_MaxSubAddr_Len];
}S_CallSubAddr;
 


typedef struct{
    uint8_t                            bNum;
    uint8_t                            abPadding[3];
    S_PS_TAI                           atTaiList[MAX_TAI_NUM];
}S_TaiList;



#endif

