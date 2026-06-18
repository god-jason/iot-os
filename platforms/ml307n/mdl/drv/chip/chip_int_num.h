/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        chip_int_num.h
 *
 * @brief       芯片中断号定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */

#ifndef _CHIP_INT_NUM
#define _CHIP_INT_NUM


/**
 * @addtogroup DrvCommon
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/** 
  * 大括号内容的字段说明:
  * {中断描述,              所属模块,       源触发模式, CPU侧触发模式, PCU是否唤醒}
  * 示例:
  * {sby cpss唤醒中断,      PD_SBY,     High,       High Level,    YES        }
  */

/******************* AP Interrupt Number *************************/
#define AP_INT_NUM_0            0     ///< name: sby_cpss_wakeup_int_mst              {sby cpss唤醒中断,PD_SBY,High Level,High Level,YES}
#define AP_INT_NUM_1            1     ///< name: wakeup                               {SBY域的外部管脚中断,PD_SBY,High Level,High Level,YES}
#define AP_INT_NUM_2            2     ///< name: llp_int                              {长长按中断,PD_SBY,Pos,High Level,YES}
#define AP_INT_NUM_3            3     ///< name: lp_int                               {长按中断,PD_SBY,Pos,High Level,YES}
#define AP_INT_NUM_4            4     ///< name: sp_int                               {短按中断,PD_SBY,Pos,High Level,YES}
#define AP_INT_NUM_5            5     ///< name: sby_rtc_timer_int_b                  {RTC定时中断,PD_SBY,Pos,High Level,YES}
#define AP_INT_NUM_6            6     ///< name: aon_cpss_wakeup_int_mst              {aon cpss唤醒中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_7            7     ///< name: key_int                              {KEYPAD中断,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_8            8     ///< name: lb_lsp_gpio_int                      {KEYPAD中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_9            9     ///< name: lb_cp_pwm_timer_int[0]               {CP Timer中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_10           10    ///< name: lb_cp_pwm_timer_int[1]               {CP Timer中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_11           11    ///< name: lb_ps_pwm_timer_int[0]               {AP Timer中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_12           12    ///< name: lb_ps_pwm_timer_int[1]               {AP Timer中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_13           13    ///< name: ext_int[0]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_14           14    ///< name: ext_int[1]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_15           15    ///< name: ext_int[2]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_16           16    ///< name: ext_int[3]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_17           17    ///< name: ext_int[4]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_18           18    ///< name: sby_lpuart_cts                       {LP_UART CTS唤醒中断,PD_AON,Neg,High Level,YES}
#define AP_INT_NUM_19           19    ///< name: sby_lpuart_int                       {LP_UART中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_20           20    ///< name: lte_lpm_subframe_int                 {LTE lpm timer5中断（新增给PS的子帧中断）, lte_ lpm_subframe_int,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_21           21    ///< name: lte_lpm_int_for_ps                   {LTE lpm timer4中断（最长40分钟睡眠中断）, lte_lpm_int_for_ps,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_22           22    ///< name: lte_ext_nmi                          {LTE lpm timer3中断, lte_ timer3_ind,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_23           23    ///< name: lte_time2_ind                        {LTE lpm timer2中断, lte_ timer2_ind,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_24           24    ///< name: lte_time1_ind                        {LTE lpm timer1中断, lte_ timer1_ind,PD_AON,Pos,High Level,YES}
#define AP_INT_NUM_25           25    ///< name: cp_icp_int                           {CP ICP中断,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_26           26    ///< name: ps_icp_int                             {PS ICP中断,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_27           27    ///< name: pd_int_in[0]                         {管脚中断0,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_28           28    ///< name: pd_int_in[1]                         {管脚中断1,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_29           29    ///< name: pd_int_in[2]                         {管脚中断2,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_30           30    ///< name: pd_int_in[3]                         {管脚中断3,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_31           31    ///< name: pd_int_in[4]                         {管脚中断4,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_32           32    ///< name: pd_int_in[5]                         {管脚中断5,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_33           33    ///< name: pd_int_in[6]                         {管脚中断6,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_34           34    ///< name: pd_int_in[7]                         {管脚中断7,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_35           35    ///< name: pd_gpio_int[0]                       {GPIO中断0,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_36           36    ///< name: pd_gpio_int[1]                       {GPIO中断1,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_37           37    ///< name: pd_lsp_icp_int[0]                    {ICP中断0,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_38           38    ///< name: pd_lsp_icp_int[1]                    {ICP中断1,PD_CORE,High Level,High Level,YES}
#define AP_INT_NUM_39           39    ///< name: ext_int[5]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_40           40    ///< name: ext_int[6]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define AP_INT_NUM_41           41    ///< name: i2c2_int                               {I2C2中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_42           42    ///< name: i2c3_int                               {I2C3中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_43           43    ///< name: usim_int                               {USIM中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_44           44    ///< name: i2s_int                                {I2S中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_45           45    ///< name: gp_ssp_int                             {通用SSP中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_46           46    ///< name: spi_cam_int                            {SPI camera中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_47           47    ///< name: lcd_int                                {SPI LCD中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_48           48    ///< name: uart0_int                              {uart0中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_49           49    ///< name: i2c1_int                               {I2C1中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_50           50    ///< name: i2c0_int                               {I2C0中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_51           51    ///< name: fb_efuse_int                         {fb_efuse_int中断                ,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_52           52    ///< name: pwm_int                              {pwm_int 中断                   ,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_53           53    ///< name: psram_phy_unlock_int                 {psram_phy_unlock_int中断        ,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_54           54    ///< name: xip_sfc_int                            {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_55           55    ///< name: rffe_ssc_int                           {rffe ssc中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_56           56    ///< name: modem_int[0]                         {modem中断0,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_57           57    ///< name: modem_int[1]                         {modem中断1,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_58           58    ///< name: modem_int[2]                         {modem中断2,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_59           59    ///< name: modem_int[3]                         {modem中断3,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_60           60    ///< name: modem_int[4]                         {modem中断4,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_61           61    ///< name: modem_int[5]                         {modem中断5,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_62           62    ///< name: modem_int[6]                         {modem中断6,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_63           63    ///< name: modem_int[7]                         {modem中断7,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_64           64    ///< name: lb_lte_irq_out0                        {lte中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_65           65    ///< name: lb_edcp_int_phy                        {edcp中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_66           66    ///< name: lb_edcp_int_ps                         {edcp中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_67           67    ///< name: ps_n300_timer0_int                   {cp timer中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_68           68    ///< name: gp_ssp1_int                          {通用SSP中断1,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_69           69    ///< name: ps_n300_timer1_int                   {ps timer中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_70           70    ///< name: uart3_int                              {uart3中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_71           71    ///< name: ps_n300_wdt_int                        {ps wdt中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_72           72    ///< name: apdma_int1                             {apdma中断1,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_73           73    ///< name: apdma_int2                             {apdma中断2,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_74           74    ///< name: ext_int[7]                             {AON域的外部管脚中断,PD_AON,High Level,High Level,NA}
#define AP_INT_NUM_75           75    ///< name: ext_int[8]                             {AON域的外部管脚中断,PD_AON,High Level,High Level,NA}
#define AP_INT_NUM_76           76    ///< name: pd_gpio_int[2]                         {GPIO中断2,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_77           77    ///< name: pd_int_in[8]                           {管脚中断8,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_78           78    ///< name: pd_int_in[9]                           {管脚中断9,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_79           79    ///< name: pd_int_in[10]                          {管脚中断10,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_80           80    ///< name: pd_int_in[11]                          {管脚中断11,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_81           81    ///< name: edma_axi_bus_err_wr_int              {edma总线写error中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_82           82    ///< name: edma_axi_bus_err_rd_int              {edma总线读error中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_83           83    ///< name: edma_int                             {edma中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_84           84    ///< name: aux_adc_stat_end_int                 {aux adc中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_85           85    ///< name: xspi_dfi_dqs_underrun                {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_86           86    ///< name: xspi_dfi_dqs_overflow                {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_87           87    ///< name: apm0_int                             {apm0中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_88           88    ///< name: apm1_int                             {apm1中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_89           89    ///< name: apm2_int                             {apm2中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_90           90    ///< name: apm3_int                             {apm3中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_91           91    ///< name: uart2_int                              {uart2中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_92           92    ///< name: pdlsp_apbmon_is                      {pd lsp apb monitor中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_93           93    ///< name: aosbylsp_apbmon_is                   {aon lsp apb monitor中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_94           94    ///< name: aux_dac_intr                         {aux dac中断,PD_CORE,High Level,High Level,NA}
#define AP_INT_NUM_95           95    ///< name: keyon_finish_int                     {keyon_finish_int中断,PD_CORE,pos edge,pos edge,NA}
#define AP_INT_NUM_96           96    ///< name: ethlite interupt for emulator
#define AP_INT_NUM_97           97    ///< name: kti keyboard interupt for emulator
#define AP_INT_NUM_98           98    ///< name: kti touch interupt for emulator

/******************* CP Interrupt Number *************************/
#define CP_INT_NUM_0            0     ///< name: sby_cpss_wakeup_int                  {sby cpss唤醒中断,PD_SBY,High Level,High Level,YES}
#define CP_INT_NUM_1            1     ///< name: wakeup                               {SBY域的外部管脚中断,PD_SBY,High Level,High Level,YES}
#define CP_INT_NUM_2            2     ///< name: llp_int                              {长长按中断,PD_SBY,Pos,High Level,YES}
#define CP_INT_NUM_3            3     ///< name: lp_int                               {长按中断,PD_SBY,Pos,High Level,YES}
#define CP_INT_NUM_4            4     ///< name: sp_int                               {短按中断,PD_SBY,Pos,High Level,YES}
#define CP_INT_NUM_5            5     ///< name: sby_rtc_timer_int_b                  {RTC定时中断,PD_SBY,Pos,High Level,YES}
#define CP_INT_NUM_6            6     ///< name: aon_cpss_wakeup_int                  {aon cpss唤醒中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_7            7     ///< name: key_int                              {KEYPAD中断,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_8            8     ///< name: lb_lsp_gpio_int                      {KEYPAD中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_9            9     ///< name: lb_cp_pwm_timer_int[0]               {CP Timer中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_10           10    ///< name: lb_cp_pwm_timer_int[1]               {CP Timer中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_11           11    ///< name: lb_ps_pwm_timer_int[0]               {AP Timer中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_12           12    ///< name: lb_ps_pwm_timer_int[1]               {AP Timer中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_13           13    ///< name: ext_int[0]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_14           14    ///< name: ext_int[1]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_15           15    ///< name: ext_int[2]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_16           16    ///< name: ext_int[3]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_17           17    ///< name: ext_int[4]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_18           18    ///< name: sby_lpuart_cts                       {LP_UART CTS唤醒中断,PD_AON,Neg,High Level,YES}
#define CP_INT_NUM_19           19    ///< name: sby_lpuart_int                       {LP_UART中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_20           20    ///< name: lte_lpm_subframe_int                 {LTE lpm timer5中断（新增给PS的子帧中断）, lte_ lpm_subframe_int,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_21           21    ///< name: lte_lpm_int_for_ps                   {LTE lpm timer4中断（最长40分钟睡眠中断）, lte_lpm_int_for_ps,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_22           22    ///< name: lte_ext_nmi                          {LTE lpm timer3中断, lte_ timer3_ind,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_23           23    ///< name: lte_time2_ind                        {LTE lpm timer2中断, lte_ timer2_ind,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_24           24    ///< name: lte_time1_ind                        {LTE lpm timer1中断, lte_ timer1_ind,PD_AON,Pos,High Level,YES}
#define CP_INT_NUM_25           25    ///< name: cp_icp_int                           {CP ICP中断,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_26           26    ///< name: ps_icp_int                             {PS ICP中断,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_27           27    ///< name: pd_int_in[0]                         {管脚中断0,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_28           28    ///< name: pd_int_in[1]                         {管脚中断1,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_29           29    ///< name: pd_int_in[2]                         {管脚中断2,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_30           30    ///< name: pd_int_in[3]                         {管脚中断3,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_31           31    ///< name: pd_int_in[4]                         {管脚中断4,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_32           32    ///< name: pd_int_in[5]                         {管脚中断5,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_33           33    ///< name: pd_int_in[6]                         {管脚中断6,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_34           34    ///< name: pd_int_in[7]                         {管脚中断7,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_35           35    ///< name: pd_gpio_int[0]                       {GPIO中断0,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_36           36    ///< name: pd_gpio_int[1]                       {GPIO中断1,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_37           37    ///< name: pd_lsp_icp_int[0]                    {ICP中断0,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_38           38    ///< name: pd_lsp_icp_int[1]                    {ICP中断1,PD_CORE,High Level,High Level,YES}
#define CP_INT_NUM_39           39    ///< name: ext_int[5]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_40           40    ///< name: ext_int[6]                           {AON域的外部管脚中断,PD_AON,High Level,High Level,YES}
#define CP_INT_NUM_41           41    ///< name: i2c2_int                               {I2C2中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_42           42    ///< name: i2c3_int                               {I2C3中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_43           43    ///< name: usim_int                               {USIM中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_44           44    ///< name: i2s_int                                {I2S中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_45           45    ///< name: gp_ssp_int                             {通用SSP中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_46           46    ///< name: spi_cam_int                            {SPI camera中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_47           47    ///< name: lcd_int                                {SPI LCD中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_48           48    ///< name: uart0_int                              {uart0中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_49           49    ///< name: i2c1_int                               {I2C1中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_50           50    ///< name: i2c0_int                               {I2C0中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_51           51    ///< name: fb_efuse_int                         {fb_efuse_int中断                ,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_52           52    ///< name: pwm_int                              {pwm_int 中断                   ,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_53           53    ///< name: psram_phy_unlock_int                 {psram_phy_unlock_int中断        ,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_54           54    ///< name: xip_sfc_int                            {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_55           55    ///< name: rffe_ssc_int                           {rffe ssc中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_56           56    ///< name: modem_int[0]                         {modem中断0,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_57           57    ///< name: modem_int[1]                         {modem中断1,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_58           58    ///< name: modem_int[2]                         {modem中断2,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_59           59    ///< name: modem_int[3]                         {modem中断3,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_60           60    ///< name: modem_int[4]                         {modem中断4,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_61           61    ///< name: modem_int[5]                         {modem中断5,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_62           62    ///< name: modem_int[6]                         {modem中断6,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_63           63    ///< name: modem_int[7]                         {modem中断7,PD_CORE,pos edge,pos edge,NA}
#define CP_INT_NUM_64           64    ///< name: lb_lte_irq_out0                        {lte中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_65           65    ///< name: lb_edcp_int_phy                        {edcp中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_66           66    ///< name: lb_edcp_int_ps                         {edcp中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_67           67    ///< name: cp_n300_timer0_int                   {cp timer中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_68           68    ///< name: gp_ssp1_int                          {通用SSP中断1,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_69           69    ///< name: cp_n300_timer1_int                   {cp timer中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_70           70    ///< name: uart3_int                              {uart3中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_71           71    ///< name: cp_n300_wdt_int                        {cp wdt中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_72           72    ///< name: apdma_int1                             {apdma中断1,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_73           73    ///< name: apdma_int2                             {apdma中断2,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_74           74    ///< name: ext_int[7]                             {AON域的外部管脚中断,PD_AON,High Level,High Level,NA}
#define CP_INT_NUM_75           75    ///< name: ext_int[8]                             {AON域的外部管脚中断,PD_AON,High Level,High Level,NA}
#define CP_INT_NUM_76           76    ///< name: pd_gpio_int[2]                         {GPIO中断2,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_77           77    ///< name: pd_int_in[8]                           {管脚中断8,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_78           78    ///< name: pd_int_in[9]                           {管脚中断9,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_79           79    ///< name: pd_int_in[10]                          {管脚中断10,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_80           80    ///< name: pd_int_in[11]                          {管脚中断11,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_81           81    ///< name: edma_axi_bus_err_wr_int              {edma总线写error中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_82           82    ///< name: edma_axi_bus_err_rd_int              {edma总线读error中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_83           83    ///< name: edma_int                             {edma中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_84           84    ///< name: aux_adc_stat_end_int                 {aux adc中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_85           85    ///< name: xspi_dfi_dqs_underrun                {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_86           86    ///< name: xspi_dfi_dqs_overflow                {xip spiflash中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_87           87    ///< name: apm0_int                             {apm0中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_88           88    ///< name: apm1_int                             {apm1中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_89           89    ///< name: apm2_int                             {apm2中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_90           90    ///< name: apm3_int                             {apm3中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_91           91    ///< name: uart2_int                              {uart2中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_92           92    ///< name: pdlsp_apbmon_is                      {pd lsp apb monitor中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_93           93    ///< name: aosbylsp_apbmon_is                   {aon lsp apb monitor中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_94           94    ///< name: aux_dac_intr                         {aux dac中断,PD_CORE,High Level,High Level,NA}
#define CP_INT_NUM_95           95    ///< name: keyon_finish_int                     {keyon_finish_int中断,PD_CORE,pos edge,pos edge,NA}


#if defined(_CPU_AP)
#define INT_MAX_NUM            AP_INT_NUM_98 
#else
#define INT_MAX_NUM            CP_INT_NUM_95 
#endif

#endif  // _CHIP_INT_NUM
/** @} */