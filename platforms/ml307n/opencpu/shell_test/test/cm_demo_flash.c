/**
 *  @file    cm_demo_flash.c
 *  @brief   OpenCPU flash操作示例
 *  @copyright copyright © 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot0752
 *  @date 2025/06/09
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cm_flash.h"
#include "cm_demo_uart.h"
#include "cm_demo_flash.h"


void cm_test_flash(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);

    if (strcmp(cmd, "info") == 0)
    {
        cm_flash_info_t info = {0};
        int ret = cm_flash_info(&info);
        cm_demo_printf("ret:%d, Block: %d, Size: %d\r\n", ret, info.block_amount, info.block_size);
    }
    else if (strcmp(cmd, "erase") == 0)
    {
        uint32_t block = atoi(embeddedCliGetToken(args, 2));
        uint32_t count = atoi(embeddedCliGetToken(args, 3));
        int ret = cm_flash_erase(block, count);
        cm_demo_printf("ret:%d", ret);
    }
    else if (strcmp(cmd, "read") == 0)
    {
        uint32_t addr = strtoul(embeddedCliGetToken(args, 2), NULL, 16);
        uint32_t size = atoi(embeddedCliGetToken(args, 3));
        uint8_t buf[256] = {0};
        int len = cm_flash_read(addr, buf, size);
        cm_demo_printf("Read Data:%d, %s\r\n", len, buf);
    }
    else if (strcmp(cmd, "write") == 0)
    {
        uint32_t addr = strtoul(embeddedCliGetToken(args, 2), NULL, 16);
        const char *data = embeddedCliGetToken(args, 3);
        int len = strlen(data);
        int written = cm_flash_write(addr, data, len);
        cm_demo_printf("written:%d", written);
    }
    else
    {
        cm_demo_printf("Usage: flash <info/erase/read/write>\r\n");
    }
}