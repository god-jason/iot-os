/**
 * @file        cm_demo_spi.c
 * @brief       OpenCPU SPI测试例程
 * @copyright   Copyright ? 2021 China Mobile IOT. All rights reserved.
 */

#ifdef CM_DEMO_UART_SUPPORT
#ifdef OS_USING_SHELL

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <os.h>
#include <cmsis_os2.h>
//#include "drv_spi.h"
#include <stdint.h>
#include <string.h>
#include "cm_spi.h"
#include <nr_micro_shell.h>
#include "cm_mem.h"
#include "drv_flash.h"
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_SPI_DEMO_LOG   osPrintf


#ifndef min
#define min(A,B) ((A) <= (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) < (B) ? (B) : (A))
#endif

#define SPI_FLASH_BUS   CM_SPI_DEV_0


/****************************************************************************
 * Private Types
 ****************************************************************************/



/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/
static char rData[100] = {0};
static uint8_t sData[8] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};

static void CM_TEST_SPI(char argc, char **argv)
{
    if(argc == 1 || argv == NULL) {
        return;
    }
    if(argv[1] == NULL) {
        return;
    }
    if(strcmp(argv[1], "on") == 0)
    {
        if(argc < 3 || argv[2] == NULL) {
            return;
        }
        if(strcmp(argv[2], "master") == 0)
        {
            cm_spi_cfg_t config = {
                CM_SPI_MODE_MASTER,
                CM_SPI_WORK_MODE_0,
                CM_SPI_DATA_WIDTH_8BIT,
                CM_SPI_NSS_HARD,
                13};
            uint8_t spi_line = 4;
            int ret = cm_spi_set_cfg(SPI_FLASH_BUS,CM_SPI_LINE,&spi_line);
            osPrintf("spi set_cfg err:%d\r\n", ret);
            ret = cm_spi_open(SPI_FLASH_BUS, &config);
            if (ret != 0)
            {
                osPrintf("spi init err:%d\r\n", ret);
                return -1;
            }
        }
        else if(strcmp(argv[2], "slave") == 0)
        {
            cm_spi_cfg_t config = {
                CM_SPI_MODE_SLAVE,
                CM_SPI_WORK_MODE_0,
                CM_SPI_DATA_WIDTH_8BIT,
                CM_SPI_NSS_HARD,
                26};
            uint8_t work_mode = 13;
            int ret = cm_spi_set_cfg(SPI_FLASH_BUS,CM_SPI_WORK_MODE,&work_mode);
            osPrintf("spi set_cfg err:%d\r\n", ret);
            ret = cm_spi_open(CM_SPI_DEV_0, &config);
            if (ret != 0)
            {
                osPrintf("spi init err:%d\r\n", ret);
                return -1;
            }
        }
        return 0;
    }
    if(strcmp(argv[1], "send") == 0 )
    {
        uint8_t s_data[8] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
        int ret = cm_spi_write(CM_SPI_DEV_0, s_data, 8);
        osPrintf("cm_spi_write rtn %d\r\n", ret);
        return 0;
    }
    if(strcmp(argv[1], "recv") == 0 && argv[2] != NULL)
    {
        char r_Data[100] = {0};
        int len = (int)strtol(argv[2], OS_NULL, 0);
        int ret = cm_spi_read(CM_SPI_DEV_0, r_Data, len);
        osPrintf("ret = %d ", ret);
        osPrintf("spi recv ");
        for(int i = 0; i < len ; i++)
        {
            osPrintf("%x ", r_Data[i]);
        }
        osPrintf("\r\n");
        return 0;
    }
    if(strcmp(argv[1], "flash") == 0 && argv[2] != NULL)
    {
        sData[0] = 0x9F;
        int len = (int)strtol(argv[2], OS_NULL, 0);
        int ret = cm_spi_write_then_read(CM_SPI_DEV_0, sData, 1, rData, len);
        osPrintf("ret = %d ", ret);
        osPrintf("spi recv ");
        for(int i = 0; i < min(len,100) ; i++)
        {
            osPrintf("%x ", rData[i]);
        }
        osPrintf("\r\n");
        return 0;
    }
    if(strcmp(argv[1], "msg_trans") == 0 && argv[2] != NULL)
    {
        sData[0] = 0x8F;
        int len = (int)strtol(argv[2], OS_NULL, 0);
        cm_spi_msg_t msg = {false,sData,rData,1,len};
        int ret = cm_spi_transfer_msg(CM_SPI_DEV_0, &msg);
        osPrintf("ret = %d ", ret);
        osPrintf("spi recv ");
        for(int i = 0; i < min(len,100) ; i++)
        {
            osPrintf("%x ", rData[i]);
        }
        osPrintf("\r\n");
        return 0;
    }
    if(strcmp(argv[1], "off") == 0 )
    {
        cm_spi_close(CM_SPI_DEV_0);
    }
}

NR_SHELL_CMD_EXPORT(spi, CM_TEST_SPI);
#endif
#endif
