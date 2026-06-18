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
 *  @brief 打开spi设备
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] cfg 配置
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_open(cm_spi_dev_e dev, cm_spi_cfg_t *cfg)
{
    uint32_t comm_arg = 0;
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
    if (cfg->clk == 13)
    {
        comm_arg |= SPI_CLK_SRC_13M;
    }
    else if (cfg->clk == 26)
    {
        comm_arg |= SPI_CLK_SRC_26M;
    }
    else if (cfg->clk == 39)
    {
        comm_arg |= SPI_CLK_SRC_39M;
    }
    else if (cfg->clk == 78)
    {
        comm_arg |= SPI_CLK_SRC_78M;
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport clk[%d]\r\n", cfg->clk);
        goto _cm_spi_open_err;
    }

    if(cfg->data_width == CM_SPI_DATA_WIDTH_8BIT)
    {
        comm_arg |= SPI_DATA_BITS(8);
    }
    else if(cfg->data_width == CM_SPI_DATA_WIDTH_16BIT)
    {
        comm_arg |= SPI_DATA_BITS(16);
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport data_width[%d]\r\n", cfg->data_width);
        goto _cm_spi_open_err;
    }
    if(cfg->work_mode == CM_SPI_WORK_MODE_0)
    {
        comm_arg |= SPI_POLPHA_00;
    }
    else if(cfg->work_mode == CM_SPI_WORK_MODE_1)
    {
        comm_arg |= SPI_POLPHA_01;
    }
    else if(cfg->work_mode == CM_SPI_WORK_MODE_2)
    {
        comm_arg |= SPI_POLPHA_10;
    }
    else if(cfg->work_mode == CM_SPI_WORK_MODE_3)
    {
        comm_arg |= SPI_POLPHA_11;
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport work_mode[%d]\r\n", cfg->work_mode);
        goto _cm_spi_open_err;
    }
    if(cfg->nss == CM_SPI_NSS_HARD)
    {
        comm_arg |= SPI_CS_MDOE_AUTO;
    }
    else if(cfg->nss == CM_SPI_NSS_SOFT)
    {
        comm_arg |= SPI_CS_MDOE_MANUAL;
    }
    else
    {
        osPrintf("[CMO_SPI] unsupport nss[%d]\r\n", cfg->nss);
        goto _cm_spi_open_err;
    }
/*    PIN_SetMux(PIN_RES(PIN_21), PIN_21_MUX_SSP0_CLK);
    PIN_SetMux(PIN_RES(PIN_22), PIN_22_MUX_SSP0_RXD);
    PIN_SetMux(PIN_RES(PIN_23), PIN_23_MUX_SSP0_TXD);
    PIN_SetMux(PIN_RES(PIN_24), PIN_24_MUX_SSP0_CS);
    PIN_SetDrCap(PIN_RES(PIN_21), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_22), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_23), PIN_DRV_3);
    PIN_SetDrCap(PIN_RES(PIN_24), PIN_DRV_3);
    */
    if(cfg->mode == CM_SPI_MODE_SLAVE)
    {
        g_spi_ms_mode = CM_SPI_MODE_SLAVE;
        comm_arg |= SPI_MS_MODE_SLAVE;
    }
    else if(cfg->mode == CM_SPI_MODE_MASTER)
    {
        g_spi_ms_mode = CM_SPI_MODE_MASTER;
        comm_arg |= SPI_MS_MODE_MASTER;
    }

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
    ret = SPI_Control(hspi, SPI_CONFIG_COMMON, comm_arg);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Control failed[%d] with arg[%X]\r\n", ret, comm_arg);
        goto _cm_spi_open_err;
    }

    g_spiIsOpen = true;

    return 0;
_cm_spi_open_err:
    SPI_PowerControl(hspi, DRV_POWER_OFF);
    SPI_Uninitialize(hspi);
    return -1;
}


/**
 *  @brief 关闭spi设备
 *
 *  @param [in] dev SPI设备ID
 *
 *  @return void
 *
 *  @details More details
 */
void cm_spi_close(cm_spi_dev_e dev)
{
    SPI_Handle *hspi;
    if(dev >= CM_SPI_DEV_NUM)
    {
        app_printf("[CMO_SPI] spi id not exist, please check it.\r\n");
        return;
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
        return;
#endif
    }
    SPI_PowerControl(hspi, DRV_POWER_OFF);
    SPI_Uninitialize(hspi);
    g_spiIsOpen = false;
}

/**
 *  @brief SPI写数据
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] data 待写数据
 *  @param [in] len 长度
 *
 *  @return
 *    = 实际写入长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_write(cm_spi_dev_e dev, const void *data, int32_t len)
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
 *  @details More details
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
 *  @details More details
 */
int32_t cm_spi_write_then_read(cm_spi_dev_e dev, const void *w_data, int32_t w_len, void *r_data, int32_t r_len)
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
    if(!w_data || !r_data)
    {
        app_printf("param invalid\r\n");
        return -1;
    }
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {
        ret = SPI_MasterTransfer(hspi, w_data, w_len, false, r_data, r_len, false, false, 1000000);
    }
    else
    {
        ret = SPI_SlaveTransfer(hspi, w_data, w_len, true, r_data, r_len, true, true, 1000000);
    }
    osWaitForCompletion(&spiCmpl, 1000000);
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

/**
 *  @brief 消息传输
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] msg 消息体
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_transfer_msg(cm_spi_dev_e dev, cm_spi_msg_t *msg)
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
    if(!msg || !msg->w_data || !msg->r_data)
    {
        app_printf("param invalid\r\n");
        return -1;
    }
    if(g_spi_ms_mode == CM_SPI_MODE_MASTER)
    {
        ret = SPI_MasterTransfer(hspi, msg->w_data, msg->w_len, false, msg->r_data, msg->r_len, false, false, 1000000);
    }
    else
    {
        ret = SPI_SlaveTransfer(hspi, msg->w_data, msg->w_len, true, msg->r_data, msg->r_len, true, true, 1000000);
    }
    osWaitForCompletion(&spiCmpl, 1000000);
    if(msg->cs_ctrl == 1)
    {
        SPI_CsSet(hspi, 0);
    }
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

/**
 * @brief SPI设备参数设置
 *
 * @param [in] dev spi设备ID
 * @param [in] type 配置类型，详见cm_spi_cfg_type_e
 * @param [in] info 配置信息指针
 *
 * @return
 *   = 0  - 成功 \n
 *   <0  - 失败, 返回值为错误码
 *
 * @details info需传入对应类型结构体
 */
int32_t cm_spi_set_cfg(cm_spi_dev_e dev, cm_spi_cfg_type_e type, void *info)
{
    uint32_t ret = 0;
    uint32_t comm_arg = 0;
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
    if(info == NULL)
    {
        app_printf("[CMO_SPI] info NULL\r\n");
        return -1;
    }
    if(type == CM_SPI_LINE)
    {
        uint8_t *line = (uint8_t *)info;
        if(*line != 4)
        {
            return -1;
        }
        return 0;
    }
    if(type == CM_SPI_CLK)
    {
        int *clk = (int *)info;

        if (*clk == 13)
        {
            comm_arg |= SPI_CLK_SRC_13M;
        }
        else if (*clk == 26)
        {
            comm_arg |= SPI_CLK_SRC_26M;
        }
        else if (*clk == 39)
        {
            comm_arg |= SPI_CLK_SRC_39M;
        }
        else if (*clk == 78)
        {
            comm_arg |= SPI_CLK_SRC_78M;
        }
        else
        {
            osPrintf("[CMO_SPI] unsupport clk[%d]\r\n", *clk);
            return -1;
        }
    }
    else if(type == CM_SPI_WORK_MODE)
    {
        cm_spi_work_mode_e *work_mode = (cm_spi_work_mode_e *)info; /*!< 工作模式 */

        if(*work_mode == CM_SPI_WORK_MODE_0)
        {
            comm_arg |= SPI_POLPHA_00;
        }
        else if(*work_mode == CM_SPI_WORK_MODE_1)
        {
            comm_arg |= SPI_POLPHA_01;
        }
        else if(*work_mode == CM_SPI_WORK_MODE_2)
        {
            comm_arg |= SPI_POLPHA_10;
        }
        else if(*work_mode == CM_SPI_WORK_MODE_3)
        {
            comm_arg |= SPI_POLPHA_11;
        }
        else
        {
            osPrintf("[CMO_SPI] unsupport work_mode[%d]\r\n", *work_mode);
            return -1;
        }
    }
    else if(type == CM_SPI_DATA_WIDTH)
    {
        cm_spi_data_width_e *data_width = (cm_spi_work_mode_e *)info;  /*!< SPI传输数据宽度 */
        if(*data_width == CM_SPI_DATA_WIDTH_8BIT)
        {
            comm_arg |= SPI_DATA_BITS(8);
        }
        else if(*data_width == CM_SPI_DATA_WIDTH_16BIT)
        {
            comm_arg |= SPI_DATA_BITS(16);
        }
        else
        {
            osPrintf("[CMO_SPI] unsupport data_width[%d]\r\n", *data_width);
            return -1;
        }
    }
    ret = SPI_Control(hspi, SPI_CONFIG_COMMON, comm_arg);
    if (ret != 0)
    {
        osPrintf("[CMO_SPI] SPI_Control failed[%d] with arg[%X]\r\n", ret, comm_arg);
        return -1;
    }
    return 0;
}


