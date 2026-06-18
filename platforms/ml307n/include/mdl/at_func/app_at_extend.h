/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __APP_AT_EXTEND_H__
#define __APP_AT_EXTEND_H__

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define APP_AT_INDCFG_ALL_BIT         0 //  all  default on
#define APP_AT_INDCFG_ALL             "all"
#define APP_AT_INDCFG_CSQ_BIT         1 //  csq   default off
#define APP_AT_INDCFG_CSQ             "csq"
#define APP_AT_INDCFG_DATASTATUS_BIT  2 //  datastatus  default off
#define APP_AT_INDCFG_DATASTATUS      "datastatus"
#define APP_AT_INDCFG_MODE_BIT        3 //  mode  default off
#define APP_AT_INDCFG_MODE            "mode"
#define APP_AT_INDCFG_SMSFULL_BIT     4 //  smsfull  default off
#define APP_AT_INDCFG_SMSFULL         "smsfull"
#define APP_AT_INDCFG_SMSINCOMING_BIT 5 //  smsincoming  default on
#define APP_AT_INDCFG_SMSINCOMING     "smsincoming"
#define APP_AT_INDCFG_ACT_BIT         6 //  act  default off
#define APP_AT_INDCFG_ACT             "act"
#define APP_AT_INDCFG_SQI_BIT         7 //  sqi  default off
#define APP_AT_INDCFG_SQI             "sqi"
#define APP_AT_INDCFG_PHONEBOOK_BIT   8 //  phonebook  default on
#define APP_AT_INDCFG_PHONEBOOK       "phonebook"
#define APP_AT_INDCFG_RING_BIT        9 //  ring  default on
#define APP_AT_INDCFG_RING            "ring"
#define APP_AT_INDCFG_NOCARRIER_BIT   10 //  nocarrier  default on
#define APP_AT_INDCFG_NOCARRIER       "nocarrier"
#define APP_AT_INDCFG_DEFAULT         (0x721U)
#define APP_AT_INDCFG_SET(value, bit)  ((value) |= (AT_INDCFG_t)(1 << bit))
#define APP_AT_INDCFG_CLEAR(value, bit)  ((value) &= (AT_INDCFG_t)(~(1 << bit)))
#define APP_AT_INDCFG_TEST(value, bit)  ((value) & (AT_INDCFG_t)(1 << bit))
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef uint32_t AT_INDCFG_t;
/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void APP_AT_Test_OK(uint8_t channelId);
void APP_AT_Set_OK(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_ATI_Exec(uint8_t channelId);

void APP_AT_GMI_Exec(uint8_t channelId);
void APP_AT_GMM_Exec(uint8_t channelId);
void APP_AT_GMR_Exec(uint8_t channelId);

void APP_AT_IGMR_Exec(uint8_t channelId);

void APP_AT_X_GSN_Exec(uint8_t channelId);

void APP_AT_X_CCID_Exec(uint8_t channelId);

void APP_AT_X_NWINFO_Exec(uint8_t channelId);

void APP_AT_X_ENG_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_ENG_Test(uint8_t channelId);

void APP_AT_X_CELL_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_CELLEX_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_X_URCCFG_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_URCCFG_Test(uint8_t channelId);

void APP_AT_X_SCLK_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_SCLK_Read(uint8_t channelId);
void APP_AT_X_SCLK_Test(uint8_t channelId);
/* 初始化QSCLK，会禁止系统sleep */
void APP_AT_X_SCLK_Init(void);

void APP_AT_X_SCLKEX_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_SCLKEX_Read(uint8_t channelId);
void APP_AT_X_SCLKEX_Test(uint8_t channelId);

void APP_AT_IPR_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_IPR_Read(uint8_t channelId);
void APP_AT_IPR_Test(uint8_t channelId);

void APP_AT_CCLK_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_CCLK_Read(uint8_t channelId);

void APP_AT_RAMDUMP_Exec(uint8_t channelId);
void APP_AT_REBOOT_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_REBOOT_Exec(uint8_t channelId);

void APP_AT_SHUTDOWN_Exec(uint8_t channelId);
void APP_AT_SHUTDOWN_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_SHUTDOWN_Test(uint8_t channelId);
void APP_AT_XADC_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_XADC_Test(uint8_t channelId);


void APP_AT_X_LTS_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_LTS_Exec(uint8_t channelId);
void APP_AT_X_LTS_Test(uint8_t channelId);

void APP_AT_X_DSIM_Read(uint8_t channelId);
void APP_AT_X_DSIM_Test(uint8_t channelId);
void APP_AT_X_DSIM_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_X_CELLINFO_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_CELLINFO_Read(uint8_t channelId);
void APP_AT_X_CELLINFO_Test(uint8_t channelId);

void APP_AT_X_INDCFG_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_INDCFG_Test(uint8_t channelId);
void APP_AT_INDCFG_Init(void);
AT_INDCFG_t APP_AT_INDCFG_GetCurCfg(void);

#ifdef __cplusplus
}
#endif
#endif


