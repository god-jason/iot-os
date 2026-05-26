/**
 * @file        cm_demo_spi.c
 * @brief       OpenCPU SPI测试例程
 * @copyright   Copyright ? 2021 China Mobile IOT. All rights reserved.
 */


 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cm_iomux.h"
#include "cm_spi.h"
#include "cm_demo_uart.h"
#include "cm_mem.h"
#include "cm_demo_spi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_SPI_DEMO_LOG   cm_demo_printf


#ifndef min
#define min(A,B) ((A) <= (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) < (B) ? (B) : (A))
#endif

#define SPI_PAGE_SIZE   256
#define SPI_SECTOR_SIZE 4096

#define SPI_RDID_CMD    0x9f
#define SPI_WREN_CMD    0x06
#define SPI_READ_CMD    0x03
#define SPI_PP_CMD      0x02
#define SPI_RDSR_CMD    0x05
#define SPI_SE_CMD      0x20
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)

/****************************************************************************
 * Private Types
 ****************************************************************************/


 
/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static int spi_flash_read(uint32_t addr, char *data, int len);
 
/****************************************************************************
 * Private Data
 ****************************************************************************/
int spi_dev = 0;
 
/****************************************************************************
 * Private Functions
 ****************************************************************************/
static int parse_params(const char *args, const char **param, int param_num)
{
	int i = 0;
    for(i = 0; i < param_num; i++)
    {
        param[i] = embeddedCliGetToken(args, i + 2);
        if (param[i] == NULL)
        {
            cm_demo_printf("invalid param[%d]\n", i);
            return -1;
        }
    }
    return 0;
}

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

    ret = cm_spi_write_then_read(spi_dev, w_cmd, sizeof(w_cmd),(char *)status, 1);
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
    char w_cmd[] = {SPI_RDID_CMD};
    char r_data[10] = {0};
    int ret = -1;

    ret = cm_spi_write_then_read(spi_dev, w_cmd, sizeof(w_cmd), r_data, 3);

    *id = (r_data[0] << 16) + (r_data[1] << 8) + r_data[2];

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

    return cm_spi_write(spi_dev, w_cmd, 1);
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

    return cm_spi_write(spi_dev, w_cmd, len + 4);//写入数据
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

    return cm_spi_write(spi_dev, w_cmd, 4);//写入数据
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
    char *tmp = cm_malloc(SPI_SECTOR_SIZE);
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
        ret = cm_spi_write_then_read(spi_dev, w_cmd, sizeof(w_cmd), data, count);

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


/****************************************************************************
 * Public Functions
 ****************************************************************************/
void cm_test_spi(EmbeddedCli *cli, char *args, void *context)
{
    int ret = -1;
    cm_spi_cfg_t config = {CM_SPI_MODE_MASTER,CM_SPI_WOKR_MODE_0, CM_SPI_DATA_WIDTH_8BIT,CM_SPI_NSS_HARD,CM_SPI_CLK_3_25MHZ};
    uint32_t spi_id = 0;
    uint32_t test_addr = 0;
    char test_str[] = "Hello world!";
    int test_len = strlen(test_str);
    char tmp[1000] = {0};
    const char *param[4] = {NULL};
    int param_num = 1;
    int spi_line = 4;
    
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    
    if (strncmp(cmd, STR_ITEM("start")) == 0)
    {
        param_num = 1;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
        
        spi_dev = atoi((char *)param[0]);
        CM_SPI_DEMO_LOG("spi test running:%d\r\n",spi_dev);

        if(spi_dev == CM_SPI_DEV_0)
        {
            cm_iomux_set_pin_func(CM_IOMUX_PIN_44, CM_IOMUX_FUNC_FUNCTION1); 
            cm_iomux_set_pin_func(CM_IOMUX_PIN_41, CM_IOMUX_FUNC_FUNCTION1);
            cm_iomux_set_pin_func(CM_IOMUX_PIN_43, CM_IOMUX_FUNC_FUNCTION1); 
            cm_iomux_set_pin_func(CM_IOMUX_PIN_42, CM_IOMUX_FUNC_FUNCTION1); 
            cm_iomux_set_pin_cmd(CM_IOMUX_PIN_42, CM_IOMUX_PINCMD3_PULL, CM_IOMUX_PINCMD3_FUNC2_PULL_HIGH);
            cm_iomux_set_pin_cmd(CM_IOMUX_PIN_43, CM_IOMUX_PINCMD3_PULL, CM_IOMUX_PINCMD3_FUNC2_PULL_HIGH);
        }
        else if(spi_dev == CM_SPI_DEV_1)
        {
            cm_iomux_set_pin_func(CM_IOMUX_PIN_211, CM_IOMUX_FUNC_FUNCTION2); 
            cm_iomux_set_pin_func(CM_IOMUX_PIN_212, CM_IOMUX_FUNC_FUNCTION2);
            cm_iomux_set_pin_func(CM_IOMUX_PIN_213, CM_IOMUX_FUNC_FUNCTION2); 
            cm_iomux_set_pin_func(CM_IOMUX_PIN_214, CM_IOMUX_FUNC_FUNCTION2); 
            cm_iomux_set_pin_cmd(CM_IOMUX_PIN_212, CM_IOMUX_PINCMD3_PULL, CM_IOMUX_PINCMD3_FUNC2_PULL_HIGH);
            cm_iomux_set_pin_cmd(CM_IOMUX_PIN_213, CM_IOMUX_PINCMD3_PULL, CM_IOMUX_PINCMD3_FUNC2_PULL_HIGH);
        }
        else
        {
            CM_SPI_DEMO_LOG("spi test spi dev numbe:%d error\r\n",spi_dev);
            return;
        }
        
        ret = cm_spi_set_cfg(spi_dev,CM_SPI_LINE,&spi_line);
//        if (ret != 0)
//        {
//            CM_SPI_DEMO_LOG("cm_spi_set_cfg err:%d\r\n", ret);
//            return;
//        }
        
        ret = cm_spi_open(spi_dev, &config);
        if (ret != 0)
        {
            CM_SPI_DEMO_LOG("spi init err:%d\r\n", ret);
            return;
        }
    	
        //读取spi id
        ret = spi_flash_read_id(&spi_id);

        if (ret != 0 || spi_id == 0xffffff)
        {
            CM_SPI_DEMO_LOG("spi read id ret:%d spi_id:0x%x err\r\n", ret, spi_id);
            goto spi_stop;
        }

        CM_SPI_DEMO_LOG("spi id =0x%x\n", spi_id);

        //写入测试数据
        ret = spi_flash_write(test_addr, test_str, test_len);

        if (ret != 0)
        {
            CM_SPI_DEMO_LOG("spi write str err:%d\r\n", ret);
            goto spi_stop;
        }

        CM_SPI_DEMO_LOG("spi write len = %d\n", test_len);

        //读取数据
        ret = spi_flash_read(test_addr, tmp, test_len);

        if (ret != 0)
        {
            CM_SPI_DEMO_LOG("spi read err:%d\r\n", ret);
            goto spi_stop;
        }

        CM_SPI_DEMO_LOG("spi read len = %d,%s\n", strlen(tmp), tmp);

    spi_stop:
        cm_spi_close(spi_dev);
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\n",cmd);
    }
}

