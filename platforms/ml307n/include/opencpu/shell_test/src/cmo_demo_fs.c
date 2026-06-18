/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_FS_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_fs.h"

#define cm_demo_printf osPrintf
#define TEST_FILE_MAX_NUM 6
#define test_file1 "/fs/test1.bin"
#define test_file2 "/fs/test2.bin"
#define test_file3 "/fs/test3.bin"
#define test_file4 "/fs/test4.bin"
#define test_file5 "/fs/test5.bin"
#define test_file6 "/fs/test6.bin"

#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

static char *file_array[TEST_FILE_MAX_NUM] = {test_file1, test_file2,test_file3,test_file4,test_file5,test_file6};

void print_fs_file_data(cm_fs_file_data_t* file_data)
{
    cm_demo_printf("attr[%d] ", file_data->file_attr);
    cm_demo_printf("size[%d] ", file_data->file_size);
    cm_demo_printf("name[%s] ", file_data->file_name);
    cm_demo_printf("\r\n");
}

void SHELL_testFs(char argc, char **argv)
{
    char test[] = "test the world is wonderful!";
    char buff[32] = {0};
    cm_fs_system_info_t info = {0};
    int32_t fsize_len = 0;

    cm_demo_printf("CM OpenCPU testFs Starts\n");

    int32_t ret = cm_fs_mkdir("/fs/");
    cm_demo_printf("cm_fs_mkdir() ret:%d\n", ret);

    ret = cm_fs_rmdir("/fs/");
    cm_demo_printf("cm_fs_rmdir() ret:%d\n", ret);

    ret = cm_fs_mkdir("/fs/");
    cm_demo_printf("cm_fs_mkdir() ret:%d\n", ret);
    cm_fs_t fp = cm_fs_open(test_file1, CM_FS_WBPLUS);
    cm_demo_printf("cm_fs_open() fp:%p\n", fp);

    int32_t f_len = cm_fs_write(fp, test, strlen(test));
    cm_demo_printf("cm_fs_write() len:%d\n", f_len);

    ret = cm_fs_sync(fp);
    cm_demo_printf("cm_fs_sync() ret:%d\n", ret);

    ret = cm_fs_close(fp);
    cm_demo_printf("cm_fs_close() ret:%d\n", ret);

    if (true == cm_fs_exist(test_file1))
    {
        fp = cm_fs_open(test_file1, CM_FS_RB);
        cm_demo_printf("cm_fs_open() fp:%p\n", fp);

        f_len = cm_fs_read(fp, buff, 32);
        cm_demo_printf("cm_fs_read() %s, len:%d, buff:%s\n", test_file1, f_len, buff);

        ret = cm_fs_close(fp);
        cm_demo_printf("cm_fs_close() ret:%d\n", ret);
    }

    fsize_len = cm_fs_filesize(test_file1);
    cm_demo_printf("cm_fs_filesize() len:%d\n", fsize_len);

    fp =  cm_fs_open(test_file1, CM_FS_RB);
    if(fp == NULL)
    {
        cm_demo_printf("cm_fs_open() fail\n");
        return;
    }

    /* 读end 10个字节数据 */
    ret = cm_fs_seek(fp, fsize_len-10, CM_FS_SEEK_SET);
    if(ret < 0)
    {
        cm_demo_printf("cm_fs_filesize() fail\n");
        return;
    }

    memset(buff, 0, 32);
    f_len = cm_fs_read(fp, buff, 10);
    cm_demo_printf("cm_fs_read() %s, len:%d, buff:%s\n", test_file2, f_len, buff);

    ret = cm_fs_close(fp);
    cm_demo_printf("cm_fs_close() ret:%d\n", ret);

    ret = cm_fs_getinfo(&info);
    cm_demo_printf("cm_fs_getinfo() ret:%d, free_size:%d, total_size:%d\n", ret,info.free_size,info.total_size);

    if (true == cm_fs_exist(test_file1))
    {
        if(true == cm_fs_exist(test_file2))
        {
            ret = cm_fs_delete(test_file2);
            cm_demo_printf("cm_fs_delete() %s ret:%d\n", test_file2, ret);
        }

        ret = cm_fs_move(test_file1, test_file2);
        cm_demo_printf("cm_fs_move ret:%d\n",ret);
        if(true == cm_fs_exist(test_file1))
        {
            cm_demo_printf("cm_fs_move() %s:false\n",test_file1);
        }
    }

    if (true == cm_fs_exist(test_file2))
    {
        ret = cm_fs_delete(test_file2);
        cm_demo_printf("cm_fs_delete() %s ret:%d\n", test_file2, ret);
    }
    if(cm_fs_getinfo(NULL)==0)
    {
        cm_demo_printf("cm_fs_getinfo() abnormal fail\n");
    }

    for(int i = 0; i < TEST_FILE_MAX_NUM; i++)
    {

        fp = cm_fs_open(file_array[i], CM_FS_WBPLUS);
        cm_demo_printf("cm_fs_open() fp:%u\n", fp);

        int32_t f_len = cm_fs_write(fp, test, (10+i));
        cm_demo_printf("cm_fs_write() len:%d\n", f_len);
        ret = cm_fs_close(fp);
        cm_demo_printf("cm_fs_close() ret:%d\n", ret);
    }

    cm_fs_file_data_t file_data = {0};
    /* 获取文件列表信息 */
    /* 打开查找并进行首次查找 */
    int32_t find_fd = cm_fs_find_first("/fs/", &file_data);
    if (find_fd == 0)
    {
        cm_demo_printf("[%s] fs find first fail\r\n", __func__);
        return;
    }

    cm_demo_printf("=======================================================\r\n");

    print_fs_file_data(&file_data);

    /* 循环查找 */
    do {
        ret = cm_fs_find_next(find_fd, &file_data);
        if (ret == 0)
        {
            print_fs_file_data(&file_data);
        }
        else
        {
            break;
        }
    } while (ret == 0);

    cm_demo_printf("=======================================================\r\n");

    /* 关闭查找 */
    ret = cm_fs_find_close(find_fd);
    if (ret < 0)
    {
        cm_demo_printf("[%s] fs find close fail\r\n", __func__);
        return;
    }

    for(int i = 0; i < TEST_FILE_MAX_NUM; i++)
    {
        ret = cm_fs_delete(file_array[i]);
        cm_demo_printf("cm_fs_delete() %s ret:%d\n", file_array[i], ret);
    }

    ret = cm_fs_rmdir("/fs/");
    cm_demo_printf("cm_fs_rmdir() ret:%d\n", ret);

    cm_demo_printf("CM OpenCPU testFs Ends\n");

}

NR_SHELL_CMD_EXPORT(cmo_fs, SHELL_testFs);
#endif
#endif
