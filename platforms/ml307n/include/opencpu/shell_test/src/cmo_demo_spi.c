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
#include "cm_iomux.h"
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
#define SPI_PAGE_SIZE   256
#define SPI_SECTOR_SIZE 4096

#define SPI_RDID_CMD    0x9f
#define SPI_WREN_CMD    0x06
#define SPI_READ_CMD    0x03
#define SPI_PP_CMD      0x02
#define SPI_RDSR_CMD    0x05
#define SPI_SE_CMD      0x20
#define SPI_DEVICE_ID   0x90


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
static int spi_flash_read(uint32_t addr, char *data, int len);

/**
 * \brief 读取spi flash寄存器状态
 *
 * \param [in] status 读取到的寄存器状态
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_read_status(uint8_t *status)
{
    char w_cmd[] = {SPI_RDSR_CMD};
    int ret = -1;

    ret = cm_spi_write_then_read(SPI_FLASH_BUS, w_cmd, sizeof(w_cmd),(char *)status, 1);
	osDelay(10);

    return ret;
}

/**
 * \brief 判断flash状态
 * \param none
 *
 * \return true:busy;false:idle
 */
static bool spi_flash_is_busy(void)
{
    uint8_t l_status = 0;

    spi_flash_read_status(&l_status);
	osDelay(10);

    if (l_status & 0x01)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * \brief 读取spi flash ID
 *
 * \param [in] id 读取到的flash ID
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_read_id(uint32_t *id)
{
    char w_cmd[1] = {SPI_DEVICE_ID};
    char r_data[3] = {0};
    int ret = -1;
#if 0
ret = SPI_MasterSend(hspi, (uint8_t *)w_data, 1, false, 100000);
    if(ret != 0)
    {
        app_printf("[CMO_SPI]spi write failed\r\n");
        return -1;
    }
#endif
    ret = cm_spi_write_then_read(SPI_FLASH_BUS, w_cmd, 1, r_data, 3);

    *id = (r_data[0] << 16) + (r_data[1] << 8) + r_data[2];
    osPrintf("id = %d \r\n",*id);

    return ret;
}

/**
 * \brief 设置spi flash允许写入数据
 *
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_write_enable(void)
{
    char w_cmd[] = {SPI_WREN_CMD};
	osDelay(10);

    return cm_spi_write(SPI_FLASH_BUS, w_cmd, 1);
}

/**
 * \brief 写入一页数据，若数据超出本页地址范围则会从本页开始覆盖写入
 *
 * \param [in] addr: 页地址起始位置
 * \param [in] data: 待写入数据首地址
 * \param [in] len: 待写入数据长度
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_page_write(uint32_t addr, char *data, int len)
{
    char w_cmd[300] = {SPI_PP_CMD, (char)(addr >> 16), (char)(addr >> 8), (char)addr};
    int i = 0;
    int ret = -1;

    for (i = 0; i < len; i++)
    {
        w_cmd[i + 4] = *data++;
    }

    ret = spi_flash_write_enable();//enable write

    if (ret < 0)
    {
        return ret;
    }
	osDelay(10);

    return cm_spi_write(SPI_FLASH_BUS, w_cmd, len + 4);//写入数据
}

/**
 * \brief 擦除一个sector数据，4Kbyte
 *
 * \param [in] addr: sector地址起始位置
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_sector_erase(uint32_t addr)
{
    char w_cmd[4] = {SPI_SE_CMD, (char)(addr >> 16), (char)(addr >> 8), (char)addr};

    if (spi_flash_write_enable() < 0)
    {
        return -1;
    }

    return cm_spi_write(SPI_FLASH_BUS, w_cmd, 4);//写入数据
}

/**
 * \brief 写入数据
 *
 * \param [in] addr: 起始地址
 * \param [in] data: 待写入数据首地址
 * \param [in] len: 待写入数据长度
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_write(uint32_t addr, char *data, int len)
{
    int ret = -1;
    char *tmp = (char *)cm_malloc(SPI_SECTOR_SIZE);
    int sector_start = addr / SPI_SECTOR_SIZE;
    int sector_end = (addr + len) / SPI_SECTOR_SIZE;
    int i = 0, j = 0, k = 0;

    if ((len <= 0) || (data == NULL))
    {
        cm_free(tmp);
        return -1;
    }

    for (i = sector_start; (i <= sector_end) && (len > 0); i++)
    {
        uint32_t current_sector_addr = i * SPI_SECTOR_SIZE;

        //写入数据至缓冲区
        int current_len = SPI_SECTOR_SIZE - (addr % SPI_SECTOR_SIZE);
        int count = min(len, current_len);
        int addr_start = addr % SPI_SECTOR_SIZE;
        int addr_end = addr_start + count;

        for (j = addr_start; j < addr_end; j++)
        {
            tmp[j] = *data++;
        }

        //擦除sector
        ret = spi_flash_sector_erase(current_sector_addr);

        if (ret < 0)
        {
            CM_SPI_DEMO_LOG("spi erase err\r\n");
            cm_free(tmp);
            return ret;
        }

        //cm_sleep_ms(150);
        count =0;
        while (spi_flash_is_busy()) //等待擦除完成
        {
            osDelay(10);
            count++;
            if(count > 200)
            {
                cm_free(tmp);
                return -1;
            }
        }

        //缓冲区写入,4kbyte / 256 byte = 16 page
        for (k = 0; k < 16; k++)
        {
            ret = spi_flash_page_write(current_sector_addr + k * SPI_PAGE_SIZE, &tmp[k * SPI_PAGE_SIZE], SPI_PAGE_SIZE);

            if (ret < 0)
            {
                CM_SPI_DEMO_LOG("spi write err\r\n");
                cm_free(tmp);
                return ret;
            }
            count = 0;
            while (spi_flash_is_busy())
            {
                osDelay(10);
                count++;
                if(count > 200)
                {
                    cm_free(tmp);
                    return -1;
                }
            } //等待写入完成
        }

        //下一轮地址，长度
        addr = (i + 1) * SPI_SECTOR_SIZE;
        len -= count;
    }

    cm_free(tmp);
    return 0;
}

/**
 * \brief 读取数据
 *
 * \param [in] addr: 读取地址
 * \param [in] data: 数据存放，需事先分配内存
 * \param [in] len: 读取长度
 *
 * \return  0：成功，其他：失败
 */
static int spi_flash_read(uint32_t addr, char *data, int len)
{
    char w_cmd[4] = {0};
    int ret = -1;

    while (len > 0) //每次最多读取0xffff字节数据
    {
        w_cmd[0] = SPI_READ_CMD;
        w_cmd[1] = (char)(addr >> 16);
        w_cmd[2] = (char)(addr >> 8);
        w_cmd[3] = (char)addr;

        uint16_t count = min(0xff, len);
        ret = cm_spi_write_then_read(SPI_FLASH_BUS, w_cmd, sizeof(w_cmd), data, count);

        if (ret != 0)
        {
            CM_SPI_DEMO_LOG("spi read err");
            return ret;
        }

        addr += 0xffff;
        len -= 0xffff;
        data += 0xffff;
    }

    return 0;
}

static char rData[100] = {0};
static uint8_t sData[8] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};

static void TEST_SPI(char argc, char **argv)
{
    if(argc == 1) {
        return;
    }
    if(strcmp(argv[1], "on") == 0)
    {
        if(strcmp(argv[2], "master") == 0)
        {
            cm_spi_cfg_t config = {
                CM_SPI_MODE_MASTER,
                CM_SPI_WOKR_MODE_0,
                CM_SPI_DATA_WIDTH_8BIT,
                CM_SPI_NSS_HARD,
                CM_SPI_CLK_13MHZ};
            int ret = cm_spi_open(SPI_FLASH_BUS, &config);
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
                CM_SPI_WOKR_MODE_0,
                CM_SPI_DATA_WIDTH_8BIT,
                CM_SPI_NSS_HARD,
                CM_SPI_CLK_26MHZ};
            int ret = cm_spi_open(CM_SPI_DEV_0, &config);
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
    if(strcmp(argv[1], "recv") == 0 )
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
    if(strcmp(argv[1], "flash") == 0 )
    {
        sData[0] = 0x9F;
        int len = (int)strtol(argv[2], OS_NULL, 0);
        int ret = cm_spi_write_then_read(CM_SPI_DEV_0, sData, 1, rData, len);
        osPrintf("ret = %d ", ret);
        osPrintf("spi recv ");
        for(int i = 0; i < 8 ; i++)
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

NR_SHELL_CMD_EXPORT(cm_spi, TEST_SPI);
#endif
#endif
