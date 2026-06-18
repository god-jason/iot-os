
#include "yopen_debug.h"
#include "yopen_os.h"


/********************************************************************************************
*  说明：
*  1. 本demo使用的flash空间是在原代码区中划分了一块48KB空间，所以需要相应缩小代码区空间
*  2. 需要修改mem_map_716s_app.h中的AP_FLASH_LOAD_SIZE：减少0xC000
*  3. 需要修改pkg_716s_mapdef_default.h 中AP_PKGIMG_LIMIT_SIZE 减少0xC000
**********************************************************************************************
*  4. 修改后，*****需要clean 重新编译***********************************************************
**********************************************************************************************
*********************************************************************************************/

#define DEMO_FLASH_START_ADDR 0x1AA000 //==AP_FLASH_LOAD_SIZE+AP_FLASH_LOAD_ADDR-0x800000
#define DEMO_FLASH_SIZE 0xc000 //FLASH_FS_REGION_START-DEMO_FLASH_START_ADDR

// 地址和大小需要4KB对齐，返回0表示成功，其他值表示失败
extern uint8_t FLASH_eraseSafe(uint32_t SectorAddress, uint32_t Size);
// 任意地址和大小，返回0表示成功，其他值表示失败
extern uint8_t FLASH_writeSafe(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
// 任意地址和大小，返回0表示成功，其他值表示失败
extern uint8_t FLASH_readSafe(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);

static void __flash_dump_data_10(uint8_t *data)
{
    yopen_trace("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", data[0], data[1], data[2],
                data[3], data[4], data[5], data[6], data[7], data[8], data[9]);
}

void flash_demo_thread(void* argv)
{
    uint8_t ret = 0;
    uint8_t data_write[10] = {'1','2','3','4','5','6','7','8','9','0'};
    uint8_t data_read[10] = {0};
    // 

    yopen_rtos_task_sleep_ms(5000);

    do{
        for(uint32_t block = DEMO_FLASH_START_ADDR; block < DEMO_FLASH_START_ADDR+DEMO_FLASH_SIZE; block += 0x1000)
        {
           
            ret = FLASH_eraseSafe(block, 0x1000);
            if(ret == 0)
            {
                yopen_trace("[FLASH_DEMO] erase addr 0x%08x size 0x%08x success", block, 0x1000);
                FLASH_readSafe(data_read, block, 10);
                __flash_dump_data_10(data_read);
            }
            else
            {
                yopen_trace("[FLASH_DEMO] erase addr 0x%08x size 0x%08x fail ret %d", block, 0x1000, ret);
            }
            
            data_write[0]++;

            ret = FLASH_writeSafe(data_write, block, sizeof(data_write));
            if(ret == 0)
            {
                yopen_trace("[FLASH_DEMO] write addr 0x%08x size 0x%08x success", block, sizeof(data_write));
                FLASH_readSafe(data_read, block, 10);
                __flash_dump_data_10(data_read);
            }
            else
            {
                yopen_trace("[FLASH_DEMO] write addr 0x%08x size 0x%08x fail ret %d", block, sizeof(data_write), ret);
            }

            yopen_rtos_task_sleep_ms(1000);
        }
        yopen_rtos_task_sleep_ms(5000);
    }while(1);
    
    yopen_rtos_task_delete(NULL);
}