/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot3000 2022/7/14
 ********************************************************/
#include "cm_fs.h"
#include "cm_demo_fs.h"
#include "cm_demo_uart.h"

#define test_file1 "test1.txt"
#define test_file2 "test2.txt"


void cm_test_move_file(void)
{
    if (true == cm_fs_exist(test_file1) && false == cm_fs_exist(test_file2))
    {
        int ret = cm_fs_move(test_file1, test_file2);
        cm_demo_printf("[FMOVE]:%d\n",ret);
    }
    else if (false == cm_fs_exist(test_file1) && true == cm_fs_exist(test_file2))
    {
        int ret = cm_fs_move(test_file2, test_file1);
        cm_demo_printf("[FMOVE]:%d\n",ret);
    }
    else
    {
        cm_demo_printf("[FMOVE] Illegal operation\n");
    }
}

void cm_test_read_file(void)
{
    if (true == cm_fs_exist(test_file1))
    {
        char buff[32] = {0};
        int32_t fd = cm_fs_open(test_file1, CM_FS_RB);
        cm_demo_printf("[FREADE]cm_fs_open() fd:%d\n", fd);

        int32_t f_len = cm_fs_read(fd, buff, 32);
        cm_demo_printf("[FREADE] cm_fs_read() %s, len:%d, buff:%s\n", test_file1, f_len, buff);

        int32_t ret = cm_fs_close(fd);
        cm_demo_printf("[FREADE] cm_fs_close() ret:%d\n", ret);
    }

    if (true == cm_fs_exist(test_file2))
    {
        char buff[32] = {0};
        int32_t fd = cm_fs_open(test_file2, CM_FS_RB);
        cm_demo_printf("[FREADE]cm_fs_open() fd:%d\n", fd);

        int32_t f_len = cm_fs_read(fd, buff, 32);
        cm_demo_printf("[FREADE] cm_fs_read() %s, len:%d, buff:%s\n", test_file2, f_len, buff);

        int32_t ret = cm_fs_close(fd);
        cm_demo_printf("[FREADE] cm_fs_close() ret:%d\n", ret);
    }
}

void cm_test_write_file(void)
{
    int flag = CM_FS_WB;
    int32_t fd = cm_fs_open(test_file1, flag);
    cm_demo_printf("[FWRITE]cm_fs_open() fd:%d %x\n", fd, flag);

    int32_t f_len = cm_fs_write(fd, "test", 4);
    cm_demo_printf("[FWRITE] cm_fs_write() len:%d\n", f_len);

    int32_t ret = cm_fs_close(fd);
    cm_demo_printf("[FWRITE] cm_fs_close() ret:%d\n", ret);
}


void cm_test_delete_file(void)
{
    if (true == cm_fs_exist(test_file1))
    {
        int32_t ret = cm_fs_delete(test_file1);
        cm_demo_printf("[FDELETE] cm_fs_delete() %s ret:%d\n", test_file1, ret);
    }

    if (true == cm_fs_exist(test_file2))
    {
        int32_t ret = cm_fs_delete(test_file2);
        cm_demo_printf("[FDELETE] cm_fs_delete() %s ret:%d\n", test_file2, ret);
    }
}

void cm_test_fs(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);

    if (strcasecmp(cmd, "write") == 0)
    {
        cm_test_write_file();
    }
    if (strcasecmp(cmd, "read") == 0)
    {
        cm_test_read_file();
    }
    if (strcasecmp(cmd, "move") == 0)
    {
        cm_test_move_file();
    } 
    if (strcasecmp(cmd, "delete") == 0)
    {
        cm_test_delete_file();
    }   
}
