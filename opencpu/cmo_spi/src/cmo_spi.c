/**
 * @file        cmo_spi.c
 * @brief       SPI驱动接口
 * @copyright   Copyright @2021 China Mobile IOT. All rights reserved.
 * @author      By tw
 * @date        2021/05/18
 *  
 * @defgroup spi spi
 * @ingroup PI
 * @{
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <os.h>
#include "drv_gpio.h"
#include "drv_spi.h"
#include "cm_spi.h"
#include "cm_iomux.h"
#include <drv_pin.h>

/****************************************************************************
 * Public Data
 ****************************************************************************/

static SPI_Handle g_spiHdl[2] = {0};
static bool g_spiIsOpen = false;
static bool g_spi_ms_mode = CM_SPI_MODE_MASTER;

static osCompletion spiCmpl;


static void __attribute__ ((noinline)) SSP_Callback(void* data, uint32_t event)
{
    osCompletion *cmpl = (osCompletion *)data;
    if(cmpl) {
        osComplete(cmpl);
    }
}

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief spi开启指定通道
 *
 * @param [in] dev spi设备
 * @param [in] config spi配置
 *
 * @return  
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details 建议在mode2/3模式下配置clk低于13MHZ，否则可能收发数据不连续。
 */
int32_t cm_spi_open(cm_spi_dev_e dev, cm_spi_cfg_t *cfg)
{
    uint32_t arg = 0;
    int32_t ret = 0;
    SPI_Handle *hspi = NULL;

    uint8_t w_data[1];
    w_data[0] = 'a';

    if ((dev >= CM_SPI_DEV_NUM) || (cfg == NULL))
    {
        osPrintf("[CMO_SPI] spi id[%d] not exist, please check it.\r\n", dev);
        return -1;
    }

    if(dev == CM_SPI_DEV_0)
    {
        //cm_iomux_set_pin_func(CM_IOMUX_PIN_16, CM_IOMUX_FUNC_FUNCTION2);  //CLK  (PIN_21_MUX_SSP0_CLK)
        //cm_iomux_set_pin_func(CM_IOMUX_PIN_54, CM_IOMUX_FUNC_FUNCTION2);  //MOSI  (PIN_23_MUX_SSP0_TXD)
        //cm_iomux_set_pin_func(CM_IOMUX_PIN_56, CM_IOMUX_FUNC_FUNCTION2);  //MISO   (PIN_22_MUX_SSP0_RXD)
        //cm_iomux_set_pin_func(CM_IOMUX_PIN_55, CM_IOMUX_FUNC_FUNCTION2);  //CS     (PIN_24_MUX_SSP0_CS)
        //GPIO_SetDir(&g_PIN_24_Res, GPIO_OUTPUT);
        hspi = &g_spiHdl[0];
        hspi->res = DRV_RES(SPI, dev);
        osPrintf("[CMO_SPI] spi pin set success \r\n");
    }
    else
    {
        osPrintf("[CMO_SPI] spi id have not been support in this module till now\r\n");
        return -1;
    }
    osPrintf("[CMO_SPI] SPI_Initialize start ...\r\n");
    if(hspi->cbEvent != SSP_Callback){
        hspi->cbEvent = SSP_Callback;
        hspi->userData = (void *)(&spiCmpl);
        osInitCompletion(&spiCmpl);
    }
    ret = SPI_Initialize(hspi);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Initialize failed[%d]\r\n", ret);
        goto _cm_spi_open_err;
    }

    //clock setup
    if (cfg->clk == CM_SPI_CLK_13MHZ)
    {
        arg |= SPI_CLK_SRC_13M;
    }
    else if (cfg->clk == CM_SPI_CLK_26MHZ)
    {
        arg |= SPI_CLK_SRC_26M;
    }
    else if (cfg->clk == CM_SPI_CLK_39MHZ)
    {
        arg |= SPI_CLK_SRC_39M;
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport clk[%d]\r\n", cfg->clk);
        goto _cm_spi_open_err;
    }

    if(cfg->data_width == CM_SPI_DATA_WIDTH_8BIT)
    {
        arg |= SPI_DATA_BITS(8);
    }
    else if(cfg->data_width == CM_SPI_DATA_WIDTH_16BIT)
    {
        arg |= SPI_DATA_BITS(16);
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport data_width[%d]\r\n", cfg->data_width);
        goto _cm_spi_open_err;
    }
    PIN_SetMux(PIN_RES(PIN_21), PIN_21_MUX_SSP0_CLK);
    PIN_SetMux(PIN_RES(PIN_22), PIN_22_MUX_SSP0_RXD);
    PIN_SetMux(PIN_RES(PIN_23), PIN_23_MUX_SSP0_TXD);
    PIN_SetMux(PIN_RES(PIN_24), PIN_24_MUX_SSP0_CS);
    PIN_SetDrCap(PIN_RES(PIN_21), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_22), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_23), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_24), PIN_DRV_3);
    if(cfg->mode == CM_SPI_MODE_SLAVE)
        g_spi_ms_mode = CM_SPI_MODE_SLAVE;
    else if(cfg->mode == CM_SPI_MODE_MASTER)
        g_spi_ms_mode = CM_SPI_MODE_MASTER;
    

    ret = SPI_PowerControl(hspi, DRV_POWER_FULL);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_PowerControl failed[%d]\r\n", ret);
        goto _cm_spi_open_err;
    }

    ret = SPI_Control(hspi, SPI_CONFIG_SPEC, SPI_BIDIR_MODE_BI_DIR);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Control failed ...\r\n");
        goto _cm_spi_open_err;
    }
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {
        ret = SPI_Control(hspi, SPI_CONFIG_COMMON, arg);
    }
    else
    {
        ret = SPI_Control(hspi, SPI_CONFIG_COMMON, arg);
    }
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Control failed[%d] with arg[%X]\r\n", ret, arg);
        goto _cm_spi_open_err;
    }
    g_spiIsOpen = true;
    
#if 0    
    ret = SPI_Control(hspi, SPI_CONFIG_COMMON, SPI_CLK_SRC_13M | SPI_MS_MODE_MASTER);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Control failed ...\r\n");
    }    
    osPrintf("[CMO_SPI] SPI_Initialize end \r\n");
#endif    
#if 0
    SPI_CommonCfg(hspi, cfg->mode);
    

    if(cfg->mode == CM_SPI_MODE_SLAVE)
        arg |= SPI_MS_MODE_SLAVE;
    else if(cfg->mode == CM_SPI_MODE_MASTER)
        arg |= SPI_MS_MODE_MASTER;
    if(cfg->work_mode == CM_SPI_WOKR_MODE_0)
        arg |= SPI_POLPHA_00;
    if(cfg->work_mode == CM_SPI_WOKR_MODE_1)
        arg |= SPI_POLPHA_01;
    if(cfg->work_mode == CM_SPI_WOKR_MODE_2)
        arg |= SPI_POLPHA_10;
    if(cfg->work_mode == CM_SPI_WOKR_MODE_3)
        arg |= SPI_POLPHA_11;
    if(cfg->data_width == CM_SPI_DATA_WIDTH_8BIT)
        arg |= SPI_DATA_BITS(8);
    if(cfg->work_mode == CM_SPI_DATA_WIDTH_16BIT)
        arg |= SPI_DATA_BITS(16);
    if(cfg->work_mode == CM_SPI_DATA_WIDTH_32BIT)
        arg |= SPI_DATA_BITS(32);
    if(cfg->nss == CM_SPI_NSS_SOFT)
        arg |= SPI_CS_MDOE_MANUAL;
    if(cfg->nss == CM_SPI_NSS_HARD)
        arg |= SPI_CS_MDOE_AUTO;
    if(cfg->clk == CM_SPI_CLK_812_5KHZ)
    {
        arg |= SPI_CLK_SRC_13M;
        arg |= SPI_CLK_DIV(15);
    }  
    if(cfg->clk == CM_SPI_CLK_1_625MHZ)
    {
        arg |= SPI_CLK_SRC_13M;
        arg |= SPI_CLK_DIV(7);
    }  
    if(cfg->clk == CM_SPI_CLK_3_25MHZ)
    {
        arg |= SPI_CLK_SRC_13M;
        arg |= SPI_CLK_DIV(3);
    } 
    if(cfg->clk == CM_SPI_CLK_6_5MHZ)
    {
        arg |= SPI_CLK_SRC_13M;
        arg |= SPI_CLK_DIV(1);
    }
    if(cfg->clk == CM_SPI_CLK_13MHZ)
        arg |= SPI_CLK_SRC_13M;
    if(cfg->clk == CM_SPI_CLK_26MHZ)
        arg |= SPI_CLK_SRC_26M;

    ret = SPI_PowerControl(hspi, DRV_POWER_FULL);
    osPrintf("[CMO_SPI] SPI_Initialize ret = %d \r\n", ret);

    SPI_Control(hspi, SPI_CONFIG_COMMON, arg);
    osPrintf("[CMO_SPI] SPI_Control success in this module till now\r\n");
#endif
    return 0;
_cm_spi_open_err:
    SPI_PowerControl(hspi, DRV_POWER_OFF);
    SPI_Uninitialize(hspi);
    return -1;
}

/**
 * @brief spi关闭，去初始化
 *
 * @param [in] dev spi设备
 *
 * @return  
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 * 
 */
int32_t cm_spi_close(cm_spi_dev_e dev)
{
    SPI_Handle *hspi;
    if(dev >= CM_SPI_DEV_NUM)
    {
        app_printf("[CMO_SPI] spi id not exist, please check it.\r\n");
        return -1;
    }
    if(dev == CM_SPI_DEV_0)
    {
        hspi = &g_spiHdl[0];
    }
    else if(dev == CM_SPI_DEV_1)
    {
#if 0   
        hspi = &g_spiHdl[1];
#else 
        app_printf("[CMO_SPI] spi id have not been support in this module till now\r\n");
        return -1;
#endif
    }
    SPI_PowerControl(hspi, DRV_POWER_OFF);
    SPI_Uninitialize(hspi);
    g_spiIsOpen = false;
    return 0;
}

/**
 * @brief spi写数据 
 * 
 * @param [in] dev spi设备
 * @param [in] data 待写数据
 * @param [in] len  待写数据长度
 * @return  
 *    = 实际写入长度 - 成功 \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details 单次写数据不超过1024字节。
 * 
 */
int32_t cm_spi_write(cm_spi_dev_e dev, const void *data, uint32_t len)
{
    uint32_t ret = 0;
    SPI_Handle *hspi;
    if(dev >= CM_SPI_DEV_NUM)
    {
        app_printf("[CMO_SPI] spi id not exist, please check it.\r\n");
        return -1;
    }
    if(g_spiIsOpen == false)
    {
        app_printf("[CMO_SPI] spi is not opened.\r\n");
        return -1;
    }
    if(dev == CM_SPI_DEV_0)
    {
        hspi = &g_spiHdl[0];
    }
    else if(dev == CM_SPI_DEV_1)
    {
#if 0   
        hspi = &g_spiHdl[1];
#else 
        app_printf("[CMO_SPI] spi id have not been support in this module till now\r\n");
        return -1;
#endif
    }
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {
        ret = SPI_MasterSend(hspi, (uint8_t *)data, len, false, 1000000);
    }
    else
    {
        ret = SPI_SlaveSend(hspi, (uint8_t *)data, len, false, 1000000);
    }
    osWaitForCompletion(&spiCmpl, 1000000);
    if(ret)
    {
        app_printf("[CMO_SPI]spi write failed\r\n");
        return -1;
    }
    else
        return ret;
}

/**
 *  @brief SPI读数据
 *  
 *  @param [in] dev SPI设备ID
 *  @param [out] data 待读数据缓存
 *  @param [in] len 长度
 *  
 *  @return 
 *    = 实际读出长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *  
 *  @details 单次读取数据不超过1024字节。
 */
int32_t cm_spi_read(cm_spi_dev_e dev, void *data, int32_t len)
{
    uint32_t ret = 0;
    SPI_Handle *hspi;
    if(dev >= CM_SPI_DEV_NUM)
    {
        app_printf("[CMO_SPI] spi id not exist, please check it.\r\n");
        return -1;
    }
    if(g_spiIsOpen == false)
    {
        app_printf("[CMO_SPI] spi is not opened.\r\n");
        return -1;
    }
    if(dev == CM_SPI_DEV_0)
    {
        hspi = &g_spiHdl[0];
    }
    else if(dev == CM_SPI_DEV_1)
    {
#if 0   
        hspi = &g_spiHdl[1];
#else 
        app_printf("[CMO_SPI] spi id have not been support in this module till now\r\n");
        return -1;
#endif
    }
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {
        ret = SPI_MasterRecv(hspi, (uint8_t *)data, len, false, false, 1000000);
    }
    else
    {
        ret = SPI_SlaveRecv(hspi, (uint8_t *)data, len, false, false, 1000000);
    }
    osWaitForCompletion(&spiCmpl, 1000000);
    ret = SPI_RecvLengthActual(hspi);
    return ret;
}

/**
 *  @brief SPI写数据后读出数据
 *  
 *  @param [in] dev SPI设备ID
 *  @param [in] w_data 待写数据
 *  @param [in] w_len 写入长度
 *  @param [out] r_data 待读缓存
 *  @param [in] r_len 读取长度
 *  
 *  @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *  
 *  @details 单次读写长度之和不能超过1024字节。
 */
int32_t cm_spi_write_then_read(cm_spi_dev_e dev, const void *w_data, uint32_t w_len, void *r_data, uint32_t r_len)
{
    uint32_t ret = 0;
    SPI_Handle *hspi;
    uint32_t total_len = w_len + r_len;
    uint8_t SpiWrBuf[1024] = {0};
    static uint8_t SpiRdBuf[1024] = {0};
    if(dev >= CM_SPI_DEV_NUM)
    {
        app_printf("[CMO_SPI] spi id not exist, please check it.\r\n");
        return -1;
    }
    if(g_spiIsOpen == false)
    {
        app_printf("[CMO_SPI] spi is not opened.\r\n");
        return -1;
    }
    if(dev == CM_SPI_DEV_0)
    {
        hspi = &g_spiHdl[0];
    }
    else if(dev == CM_SPI_DEV_1)
    {
#if 0   
        hspi = &g_spiHdl[1];
#else 
        app_printf("[CMO_SPI] spi id have not been support in this module till now\r\n");
        return -1;
#endif
    }    
    if(!w_data || !r_data)
    {
        app_printf("param invalid\r\n");
        return -1;
    }
    memcpy(SpiWrBuf, w_data, w_len);
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {        
        ret = SPI_MasterTransfer(hspi, SpiWrBuf, total_len, false, SpiRdBuf, total_len, false, false, 1000000);
    }
    else
    {
        ret = SPI_SlaveTransfer(hspi, SpiWrBuf, total_len, true, SpiRdBuf, total_len, true, true, 1000000);
    }
    osWaitForCompletion(&spiCmpl, 1000000);
    memcpy(r_data, SpiRdBuf + w_len, r_len);
    if(ret)
    {
        app_printf("[CMO_SPI]spi write failed\r\n");
        return -1;
    }
    else
    {
        return ret;
    }
}

