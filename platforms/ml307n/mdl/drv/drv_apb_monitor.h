#ifndef __DRV_APB_MONITOR__
#define __DRV_APB_MONITOR__

/**
 * @addtogroup ApbMonitor
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define AONSBY_APBMON_BASEADDR              (BASE_CPU_MEM_CRM + 0x14)
#define PDLSP_APBMON_BASEADDR               (BASE_CPU_MEM_CRM + 0x3C)

#define APBMON_AONADDR_START                (0xF1000000)
#define APBMON_AONADDR_END                  (0xF100A000)

#define APBMON_PDADDR_START                 (0xF2000000)
#define APBMON_PDADDR_END                   (0xF200A000)

#define APBMON_PD_IRQ              (OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_92))
#define APBMON_AON_IRQ             (OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_93))

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef void (*APBMonCallBack) (void);

typedef struct APBMON_Reg
{
    uint32_t clr:1;
    uint32_t clr_st:1;
    uint32_t enble:1;
    uint32_t time_st:1;
    uint32_t int_type:2;
    uint32_t ie:1;
    uint32_t is:1;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t timer_val;
    uint32_t int_addr;
    uint32_t int_offset;
    uint32_t int_cnt;
    uint32_t wr_num;
    uint32_t rd_num;
    uint32_t total_num;
} APBMON_Reg;

typedef struct APBMON_Cfg
{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t timer_val;
    uint32_t int_cnt;
    uint32_t int_addr;
    uint32_t int_offset;
    APBMonCallBack cb;
    uint32_t int_type:2;
    uint32_t int_enble:1;
    uint32_t domain:1;
} APBMON_Cfg;

typedef struct APBMON_Result
{
    uint32_t wr_num;
    uint32_t rd_num;
    uint32_t total_num;
    uint8_t domain:1;
    uint8_t time_over:1;
} APBMON_Result;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
************************************************************************************
 * @brief           配置APB monitor
 *
 * @param[in]       cfg :地址范围及中断配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void APBMON_Start(APBMON_Cfg *cfg);

/**
************************************************************************************
 * @brief           停止APB monitor
 *
 * @param[in]       cfg :地址范围及中断配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void APBMON_Stop(APBMON_Cfg *cfg);

/**
************************************************************************************
 * @brief           获取apb monitor统计计数
 *
 * @param[in]       cnt :apb monitor统计计数信息
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void APBMON_CntGet(APBMON_Result *cnt);

/**
************************************************************************************
 * @brief           获取apb monitor统计计数并重新开始计数
 *
 * @param[in]       cnt :apb monitor统计计数信息
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void APBMON_CntRead(APBMON_Result *cnt);
#endif

/** @} */
