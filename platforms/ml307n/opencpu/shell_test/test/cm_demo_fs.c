/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot3000 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_FS_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include "cm_fs.h"

#define TEST_FILE_MAX_NUM 6
#define test_file1 "/test1.bin"
#define test_file2 "/test2.bin"
#define test_file3 "/test3.bin"
#define test_file4 "/test4.bin"
#define test_file5 "/test5.bin"
#define test_file6 "/test6.bin"
#define test_dir   "test"
#define cm_demo_printf osPrintf
#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

typedef struct
{
    const char* file_opcode;  /* 文件操作字符 */
    void (*func)(void);       /* 文件操作字符对应函数 */
} cm_fs_test_option_t;

static void cm_test_move_file(void);
static void cm_test_read_file(void);
static void cm_test_write_file(void);
static void cm_test_delete_file(void);
static void cm_test_sync_file(void);
static void cm_test_seek_file(void);
static void cm_test_get_file_size(void);
static void cm_test_mkdir(void);
static void cm_test_rmdir(void);
static void cm_test_get_filesystem_info(void);
static void cm_test_find(void);

static cm_fs_test_option_t g_fs_test_options[] = {
    {"write", cm_test_write_file},
    {"read",  cm_test_read_file},
    {"move", cm_test_move_file},
    {"delete", cm_test_delete_file},
    {"sync", cm_test_sync_file},
    {"seek", cm_test_seek_file},
    {"filesize", cm_test_get_file_size},
    {"mkdir", cm_test_mkdir},
    {"rmdir", cm_test_rmdir},
    {"systeminfo", cm_test_get_filesystem_info},
    {"find", cm_test_find},
    };

static char *file_array[TEST_FILE_MAX_NUM] = {test_file1, test_file2,test_file3,test_file4,test_file5,test_file6};

static void print_fs_file_info(cm_fs_file_data_t* file_data)
{
    if(file_data == NULL)
    {
        return;
    }

    cm_demo_printf("[FFIND]attr[%d] ", file_data->file_attr);
    cm_demo_printf("[FFIND]size[%d] ", file_data->file_size);
    cm_demo_printf("[FFIND]name[%s] ", file_data->file_name);
    cm_demo_printf("\r\n");
}

static void cm_test_move_file(void)
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

static void cm_test_read_file(void)
{
    if (true == cm_fs_exist(test_file1))
    {
        char buff[32] = {0};
        cm_fs_t fd = cm_fs_open(test_file1, CM_FS_RB);
        cm_demo_printf("[FREADE]cm_fs_open() fd:%u\n", fd);

        int32_t f_len = cm_fs_read(fd, buff, 32);
        cm_demo_printf("[FREADE] cm_fs_read() %s, len:%d, buff:%s\n", test_file1, f_len, buff);

        int32_t ret = cm_fs_close(fd);
        cm_demo_printf("[FREADE] cm_fs_close() ret:%d\n", ret);
    }

    if (true == cm_fs_exist(test_file2))
    {
        char buff[32] = {0};
        cm_fs_t fd = cm_fs_open(test_file2, CM_FS_RB);
        cm_demo_printf("[FREADE]cm_fs_open() fd:%u\n", fd);

        int32_t f_len = cm_fs_read(fd, buff, 32);
        cm_demo_printf("[FREADE] cm_fs_read() %s, len:%d, buff:%s\n", test_file2, f_len, buff);

        int32_t ret = cm_fs_close(fd);
        cm_demo_printf("[FREADE] cm_fs_close() ret:%d\n", ret);
    }
}

static void cm_test_write_file(void)
{
    int flag = CM_FS_WB;
    cm_fs_t fd = cm_fs_open(test_file1, flag);
    cm_demo_printf("[FWRITE]cm_fs_open() fd:%u %x\n", fd, flag);

    int32_t f_len = cm_fs_write(fd, "test", strlen("test"));
    cm_demo_printf("[FWRITE] cm_fs_write() len:%d\n", f_len);

    int32_t ret = cm_fs_close(fd);
    cm_demo_printf("[FWRITE] cm_fs_close() ret:%d\n", ret);
}


static void cm_test_delete_file(void)
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

static void cm_test_sync_file(void)
{
    int flag = CM_FS_WB;
    cm_fs_t fd = cm_fs_open(test_file1, flag);
    cm_demo_printf("[FSYNC]cm_fs_open() fd:%u %x\n", fd, flag);

    int32_t f_len = cm_fs_write(fd, "test", strlen("test"));
    cm_demo_printf("[FSYNC] cm_fs_write() len:%d\n", f_len);

    int32_t ret = cm_fs_sync(fd);
    cm_demo_printf("[FSYNC] cm_fs_sync() len:%d\n", ret);

    ret = cm_fs_close(fd);
    cm_demo_printf("[FSYNC] cm_fs_close() ret:%d\n", ret);
}

static void cm_test_seek_file(void)
{
    int flag = CM_FS_WB;
    cm_fs_t fd = cm_fs_open(test_file1, flag);
    cm_demo_printf("[FSEEK]cm_fs_open() fd:%u %x\n", fd, flag);

    int32_t ret = cm_fs_seek(fd, 4, CM_FS_SEEK_SET);
    cm_demo_printf("[FSEEK] cm_fs_seek() len:%d\n", ret);

    ret = cm_fs_close(fd);
    cm_demo_printf("[FSEEK] cm_fs_close() ret:%d\n", ret);
}

static void cm_test_get_file_size(void)
{
    if (true == cm_fs_exist(test_file1))
    {
        int file_size = cm_fs_filesize(test_file1);
        cm_demo_printf("[FGETFILESIZE]:%d\n",file_size);
    }
    else if (true == cm_fs_exist(test_file2))
    {
        int file_size = cm_fs_filesize(test_file2);
        cm_demo_printf("[FGETFILESIZE]:%d\n",file_size);
    }
    else
    {
        cm_demo_printf("[FGETFILESIZE] Illegal operation\n");
    }
}

static void cm_test_mkdir(void)
{
    int ret = cm_fs_mkdir(test_dir);
    cm_demo_printf("[FMKDIR]:%d\n",ret);
}

static void cm_test_rmdir(void)
{
    int ret = cm_fs_rmdir(test_dir);
    cm_demo_printf("[FRMDIR]:%d\n",ret);
}

static void cm_test_get_filesystem_info(void)
{
    cm_fs_system_info_t info = {0};
    int32_t ret = cm_fs_getinfo(&info);
    cm_demo_printf("[FGETINFO]:total size %d free size %d\n",info.total_size, info.free_size);
}

static void cm_test_find(void)
{
    char test[] = "test the world is wonderful!";
    int32_t ret = 0;
    for(int i = 0; i < TEST_FILE_MAX_NUM; i++)
    {

        cm_fs_t fp = cm_fs_open(file_array[i], CM_FS_WBPLUS);
        cm_demo_printf("[FFIND]cm_fs_open() fp:%u\n", fp);

        int32_t f_len = cm_fs_write(fp, test, (10+i));
        cm_demo_printf("[FFIND]cm_fs_write() len:%d\n", f_len);
        ret = cm_fs_close(fp);
        cm_demo_printf("[FFIND]cm_fs_close() ret:%d\n", ret);
    }

    cm_fs_file_data_t file_data = {0};
    /* 获取文件列表信息 */
    /* 打开查找并进行首次查找 */
    int32_t find_fd = cm_fs_find_first("/", &file_data);
    if (find_fd == 0)
    {
        cm_demo_printf("[FFIND][%s] fs find first fail\r\n", __func__);
        return;
    }

    cm_demo_printf("[FFIND]=======================================================\r\n");

    print_fs_file_info(&file_data);

    /* 循环查找 */
    do {
        ret = cm_fs_find_next(find_fd, &file_data);
        if (ret == 0)
        {
            print_fs_file_info(&file_data);
        }
        else
        {
            break;
        }
    } while (ret == 0);

    cm_demo_printf("[FFIND]=======================================================\r\n");

    /* 关闭查找 */
    ret = cm_fs_find_close(find_fd);
    if (ret < 0)
    {
        cm_demo_printf("[FFIND][%s] fs find close fail\r\n", __func__);
        return;
    }

    for(int i = 0; i < TEST_FILE_MAX_NUM; i++)
    {
        ret = cm_fs_delete(file_array[i]);
        cm_demo_printf("[FFIND]cm_fs_delete() %s ret:%d\n", file_array[i], ret);
    }
}

static void cm_test_file_printf(void)
{
    cm_demo_printf("Usage:\n");
    cm_demo_printf("  fs write                        \n");
    cm_demo_printf("  fs read                         \n");
    cm_demo_printf("  fs move                         \n");
    cm_demo_printf("  fs delete                       \n");
    cm_demo_printf("  fs sync                         \n");
    cm_demo_printf("  fs seek                         \n");
    cm_demo_printf("  fs filesize                     \n");
    cm_demo_printf("  fs mkdir                        \n");
    cm_demo_printf("  fs rmdir                        \n");
    cm_demo_printf("  fs systeminfo                   \n");
    cm_demo_printf("  fs find                         \n");
}

void SHELL_testFs(char argc, char **argv)
{
    if(argv == NULL)
    {
        cm_demo_printf("shell param error\n");
        return;
    }

    if(argc == 2)
    {
        int count = sizeof(g_fs_test_options) / sizeof(cm_fs_test_option_t);
        int i = 0;
        for(i = 0; i < count; i++)
        {
            if(0 == strcmp(g_fs_test_options[i].file_opcode, argv[1]))
            {
                g_fs_test_options[i].func();
                break;
            }
        }
    }
    else
    {
        cm_test_file_printf();
    }
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(fs, SHELL_testFs);
#endif

#endif

