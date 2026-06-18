/*********************************************************
 *  @file    cm_demo_ftp.c
 *  @brief   OpenCPU FTP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created By ZY 2024/12/9
 ********************************************************/

#ifdef CM_DEMO_FTP_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>

#include "cm_fs.h"
#include "cm_ftp.h"
#include "cm_mem.h"

#define cm_demo_printf osPrintf
#define ftp_log(fmt, ...) osPrintf("[FTP][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

void print_download_help(const char *cmdName);
void print_upload_help(const char *cmdName);
int32_t cm_ftp_get_file_to_fs(int32_t handle, const char *remotefile, const char *localfile);
int32_t cm_ftp_put_file_from_fs(int32_t handle, const char *localfile, const char *remotefile);
int32_t cm_ftp_get_list(int32_t handle, const char *path, char *match, cmo_ftp_file_data_t file_info_list[], int max_file_num);
void print_file_data(cmo_ftp_file_data_t* file_data);


/* 测试前请先补充如下参数 */
static int32_t port = 21;   //测试端口
static char hostname[16] = "8.137.154.246";    //测试地址
static char username[16] = "fengshuaixing";    //测试用户名
static char password[16] = "fengshuaixing";    //测试密码
static char dir[32] = "opencpu_test_dir";   //测试文件夹名，请根据实际平台情况修改
static char home_dir[32] = "/";    //测试文件夹名，请根据实际平台情况修改
static char file[32] = "opencpu_ml307a_ftp_test7.txt";  //测试文件名，请根据实际平台情况修改
static char new_file[32] = "ml307a_ftp_test8.txt";  //测试文件名，请根据实际平台情况修改
static char get_dir[256] = {0};
static char data[32] = "1234567890";    //测试数据
static char out_data[32] = {0};
static cmo_ftp_file_data_t file_data_test = {0};

static int fs_test_buff_size = 256;

/**
 *  FTP功能调试使用示例，注意使用前需先设置port，hostname，username，password
 *
 */
void SHELL_testFTP(char argc, char **argv)
{
    int32_t handle = -1;
    int32_t ret = -1;
    int32_t find_fd1 = -1;

    cmo_ftp_state_e state = 0;

    /* 配置连接参数 */
    cm_ftp_config_t config = {0};
    config.url = (uint8_t *)hostname;       //需补充服务器地址
    config.port = port;                     //需补充服务器端口号
    config.username = (uint8_t *)username;  //需补充用户名
    config.passwd = (uint8_t *)password;    //需补充密码
    config.rsptimeout = 10*1000;            //超时时间10秒
    config.cid = 1;

    /* 连接平台 */
    handle = cm_ftp_open(&config);

    if (handle < 0)
    {
        ftp_log("ftp open fail!!!");
        return;
    }

    /* 创建一个新文件夹 */
    ret = cm_ftp_create_dir(handle, dir);

    if (ret < 0)
    {
        ftp_log("ftp create dir fail");
        return;
    }

    /* 设置之前的新建文件夹为当前工作目录 */
    ret = cm_ftp_set_current_dir(handle, dir);

    if (ret < 0)
    {
        ftp_log("ftp change dir fail");
        return;
    }

    /* 获取当前工作目录 */
    ret = cm_ftp_get_current_dir(handle, get_dir);

    if (ret < 0)
    {
        ftp_log("ftp change dir fail");
        return;
    }

    ftp_log("current_dir=[%s]", get_dir);

    /* 设置其他文件夹作为当前工作目录 */
    ret = cm_ftp_set_current_dir(handle, home_dir);

    if (ret < 0)
    {
        ftp_log("ftp change dir fail");
        return;
    }

    /* 删除之前新建的文件夹 */
    ret = cm_ftp_remove_dir(handle, dir);

    if (ret < 0)
    {
        ftp_log("ftp remove dir fail");
        return;
    }

    /* 上传文件，模式0：新建或覆盖 */
    ret = cm_ftp_put_file(handle, 0, file, (uint8_t*)data, strlen(data));

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp put 0 fail\r\n", __func__);
        return;
    }

    /* 上传文件，模式1：新建或追加 */
    ret = cm_ftp_put_file(handle, 1, file, (uint8_t*)data, strlen(data));

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp put 1 fail\r\n", __func__);
        return;
    }

    /* 获取文件大小 */
    ret = cm_ftp_get_file_size(handle, file);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp get file size fail\n", __func__);
        return;
    }

    cm_demo_printf("[%s] file size=%d\r\n", __func__, ret);

    /* 下载文件内容 */
    /* 每次拉取需要建立连接，建立连接需要耗时，所以下载大文件时，单次拉取的长度越大效率越高 */
    ret = cm_ftp_get_file(handle, 0, file, 0, (uint8_t*)out_data, 20);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp get file fail\r\n",__func__);
        return;
    }

    out_data[ret] = '\0';

    cm_demo_printf("[%s] len=%d data=[%s]\r\n", __func__, ret, out_data);

    /* 修改文件名 */
    ret = cm_ftp_rename_file(handle, file, new_file);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp rename fail\r\n", __func__);
        return;
    }

    /* 删除文件 */
    ret = cm_ftp_delete_file(handle, new_file);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp del file fail\r\n", __func__);
        return;
    }

    /* 获取文件列表信息 */
    /* 打开查找并进行首次查找 */
    find_fd1 = cm_ftp_find_first(handle, ".", &file_data_test);

    if (find_fd1 < 0)
    {
        cm_demo_printf("[%s] ftp find first fail\r\n", __func__);
        return;
    }

    print_file_data(&file_data_test);

    /* 循环查找 */
    do {
        find_fd1 = cm_ftp_find_next(handle, find_fd1, &file_data_test);
        if (find_fd1 >= 0)
        {
            print_file_data(&file_data_test);
        }

    } while (find_fd1 >= 0);

    /* 关闭查找 */
    ret = cm_ftp_find_close(handle, find_fd1);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp find close fail\r\n", __func__);
        return;
    }

    // cmo_ftp_file_data_t ftp_file_data_list[5] = {0};
    cmo_ftp_file_data_t *ftp_file_data_list = osCalloc(1, sizeof(cmo_ftp_file_data_t) * 5);

    ret = cm_ftp_get_list(handle, "/", NULL, ftp_file_data_list, 5);

    cm_demo_printf("[%s] ftp get list no match %d\r\n", __func__, ret);

    for (int i = 0; i < ret; i++)
    {
        cm_demo_printf("[%s,%d] name=%s,size=%d,time=%d,attr=%d,permision=%d\r\n", __func__, __LINE__,
         ftp_file_data_list[i].file_name, ftp_file_data_list[i].file_size, ftp_file_data_list[i].file_modify_time,
         ftp_file_data_list[i].file_attr, ftp_file_data_list[i].file_permision);
    }

    osFree(ftp_file_data_list);

    /* 获取连接状态 */
    state = cm_ftp_get_state(handle);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp get state fail\r\n", __func__);
        return;
    }

    cm_demo_printf("[%s] ftp state=%d\r\n", __func__, state);

    /* 关闭连接 */
    ret = cm_ftp_close(handle);

    if (ret < 0)
    {
        ftp_log("ftp close fail");
        return;
    }

    return;
}

void SHELL_testFTP_download(char argc, char **argv)
{
    int32_t ret = -1;
    int32_t handle = -1;

    // shell_ftp_test_download <remotefile> <localfile>
    if (argc != 3 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        return print_download_help(argv[0]);
    }

    /* 配置连接参数 */
    cm_ftp_config_t config = {0};
    config.url = (uint8_t *)hostname;       //需补充服务器地址
    config.port = port;                     //需补充服务器端口号
    config.username = (uint8_t *)username;  //需补充用户名
    config.passwd = (uint8_t *)password;    //需补充密码
    config.rsptimeout = 10*1000;            //超时时间10秒
    config.cid = 1;

    /* 连接平台 */
    handle = cm_ftp_open(&config);

    if (handle < 0)
    {
        ftp_log("ftp open fail!!!");
        return;
    }

    /* FTP从服务器拉取文件并存入文件系统 */
    char *remotefile = argv[1];
    char *localfile = argv[2];

    ret = cm_ftp_get_file_to_fs(handle, remotefile, localfile);

    if (ret < 0)
    {
        ftp_log("ftp get file fail");
        return;
    }

    /* 关闭连接 */
    ret = cm_ftp_close(handle);

    if (ret < 0)
    {
        ftp_log("ftp close fail");
        return;
    }

    return;
}

void SHELL_testFTP_upload(char argc, char **argv)
{
    int32_t ret = -1;
    int32_t handle = -1;

    // shell_ftp_test_download <localfile> <remotefile>
    if (argc != 3 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        return print_upload_help(argv[0]);
    }

    /* 配置连接参数 */
    cm_ftp_config_t config = {0};
    config.url = (uint8_t *)hostname;       //需补充服务器地址
    config.port = port;                     //需补充服务器端口号
    config.username = (uint8_t *)username;  //需补充用户名
    config.passwd = (uint8_t *)password;    //需补充密码
    config.rsptimeout = 10*1000;            //超时时间10秒
    config.cid = 1;

    /* 连接平台 */
    handle = cm_ftp_open(&config);

    if (handle < 0)
    {
        ftp_log("ftp open fail!!!");
        return;
    }

    /* FTP从服务器拉取文件并存入文件系统 */
    char *localfile = argv[1];
    char *remotefile = argv[2];

    ret = cm_ftp_put_file_from_fs(handle, localfile, remotefile);

    if (ret < 0)
    {
        ftp_log("ftp put file fail");
        return;
    }

    /* 关闭连接 */
    ret = cm_ftp_close(handle);

    if (ret < 0)
    {
        ftp_log("ftp close fail");
        return;
    }

    return;
}

void SHELL_testFTP_list(char argc, char **argv)
{
    int32_t ret = -1;
    int32_t handle = -1;

    int32_t find_fd1 = -1;

    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        cm_demo_printf("Usage: %s <path>\r\n", argv[0]);
        return;
    }

    /* 配置连接参数 */
    cm_ftp_config_t config = {0};
    config.url = (uint8_t *)hostname;       //需补充服务器地址
    config.port = port;                     //需补充服务器端口号
    config.username = (uint8_t *)username;  //需补充用户名
    config.passwd = (uint8_t *)password;    //需补充密码
    config.rsptimeout = 10*1000;            //超时时间10秒
    config.cid = 1;

    /* 连接平台 */
    handle = cm_ftp_open(&config);

    if (handle < 0)
    {
        ftp_log("ftp open fail!!!");
        return;
    }

    char *path = argv[1];

    /* 获取文件列表信息 */
    /* 打开查找并进行首次查找 */
    find_fd1 = cm_ftp_find_first(handle, path, &file_data_test);

    if (find_fd1 < 0)
    {
        cm_demo_printf("[%s] ftp find first fail\r\n", __func__);
        return;
    }

    ftp_log("=======================================================");

    print_file_data(&file_data_test);

    /* 循环查找 */
    do {
        find_fd1 = cm_ftp_find_next(handle, find_fd1, &file_data_test);
        if (find_fd1 >= 0)
        {
            print_file_data(&file_data_test);
        }
    } while (find_fd1 >= 0);

    ftp_log("=======================================================");

    /* 关闭查找 */
    ret = cm_ftp_find_close(handle, find_fd1);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp find close fail\r\n", __func__);
        return;
    }

    /* 关闭连接 */
    ret = cm_ftp_close(handle);

    if (ret < 0)
    {
        ftp_log("ftp close fail");
        return;
    }

    return;
}

void print_file_data(cmo_ftp_file_data_t* file_data)
{
    ftp_log("attr[%d] ", file_data->file_attr);
    ftp_log("size[%d] ", file_data->file_size);
    ftp_log("name[%s] ", file_data->file_name);
    ftp_log("permision[%d] ", file_data->file_permision);
    ftp_log("modify_time[%d]\r\n", file_data->file_modify_time);
}

void print_download_help(const char *cmdName)
{
    cm_demo_printf("Usage: %s <remotefile> <localfile>\r\n", cmdName);
    cm_demo_printf("\r\n");
    cm_demo_printf("Parameters: \r\n");
    cm_demo_printf("    <remotefile>    files in ftp server, absolute or relative path\r\n");
    cm_demo_printf("    <localfile>     local file name\r\n");
    cm_demo_printf("\r\n");
    cm_demo_printf("Examples: \r\n");
    cm_demo_printf("    %s /home/ftptest/zzz123.txt /tmp/test.txt\r\n", cmdName);
}

void print_upload_help(const char *cmdName)
{
    cm_demo_printf("Usage: %s <localfile> <remotefile>\r\n", cmdName);
    cm_demo_printf("\r\n");
    cm_demo_printf("Parameters: \r\n");
    cm_demo_printf("    <localfile>     local file name\r\n");
    cm_demo_printf("    <remotefile>    files in ftp server, absolute or relative path\r\n");
    cm_demo_printf("\r\n");
    cm_demo_printf("Examples: \r\n");
    cm_demo_printf("    %s /tmp/test.txt /home/ftptest/zzz123.txt\r\n", cmdName);
}

/**
 *  @brief FTP从服务器拉取文件并存入文件系统
 *
 *	@param [in]	 handle        FTP连接句柄
 *  @param [in]  remotefile    需要获取的ftp服务器上文件的文件名，可以包含相对或绝对路径。
 *  @param [in]  localfile     存储到本地的文件名
 *
 *  @return
 *   >= 0 - 成功，返回文件大小 \n
 *   <  0 - 失败
 *
 *  @details 通过修改全局静态变量fs_test_buff_size的大小可调整单次业务处理数据的长度，适当增加可加快速度
 */
int32_t cm_ftp_get_file_to_fs(int32_t handle, const char *remotefile, const char *localfile)
{
    uint8_t *data_buff = (uint8_t *)cm_malloc(fs_test_buff_size);
    cm_fs_t fs_fd = NULL;

    if (data_buff == NULL)
    {
        cm_demo_printf("[%s] cm_malloc fail\r\n", __func__);
        goto error;
    }

    memset(data_buff, 0, fs_test_buff_size);

    int remotefile_size = cm_ftp_get_file_size(handle, remotefile);

    if (remotefile_size <= 0)
    {
        cm_demo_printf("[%s] get file size fail\r\n", __func__);
        goto error;
    }

    cm_fs_system_info_t fs_sys_info = {0, 0};
    cm_fs_getinfo(&fs_sys_info);

    if (remotefile_size > fs_sys_info.free_size)
    {
        cm_demo_printf("[%s] free size is not enough\r\n", __func__);
        goto error;
    }

    fs_fd = cm_fs_open(localfile, CM_FS_AB);
    if (fs_fd == NULL)
    {
        cm_demo_printf("[%s] fs open fail\r\n", __func__);
        goto error;
    }

    int file_size_tmp = remotefile_size;
    int offset = 0;
    while (file_size_tmp > 0)
    {
        int get_ret = cm_ftp_get_file(handle, 0, remotefile, offset, data_buff, fs_test_buff_size);

        if (get_ret < 0)
        {
            cm_demo_printf("[%s] get file fail file_size_tmp: %d\r\n", __func__, file_size_tmp);
            goto error;
        }

        int write_ret = cm_fs_write(fs_fd, data_buff, get_ret);
        if (get_ret != write_ret)
        {
            cm_demo_printf("[%s] write file fail file_size_tmp: %d, get_ret: %d, write_ret: %d\r\n", __func__, file_size_tmp, get_ret, write_ret);
            goto error;
        }

        file_size_tmp = file_size_tmp - get_ret;
        offset = offset + get_ret;
        osDelay(5);
    }

    if (data_buff)
    {
        cm_free(data_buff);
    }
    if (fs_fd != NULL)
    {
        cm_fs_close(fs_fd);
    }
    return remotefile_size;

error:

    if (data_buff)
    {
        cm_free(data_buff);
    }
    if (fs_fd != NULL)
    {
        cm_fs_close(fs_fd);
    }
    return -1;
}

/**
 *  @brief 从文件系统向FTP服务器推送文件
 *
 *	@param [in]	 handle        FTP连接句柄
 *  @param [in]  localfile     需要上传的本地文件名
 *  @param [in]  remotefile    存储到ftp服务器上文件的文件名，可以包含相对或绝对路径
 *
 *  @return
 *   >= 0 - 成功，返回文件大小 \n
 *   <  0 - 失败
 *
 *  @details 通过修改全局静态变量fs_test_buff_size的大小可调整单次业务处理数据的长度，适当增加可加快速度
 */
int32_t cm_ftp_put_file_from_fs(int32_t handle, const char *localfile, const char *remotefile)
{
    uint8_t *data_buff = (uint8_t *)cm_malloc(fs_test_buff_size);
    cm_fs_t fs_fd = NULL;

    if (data_buff == NULL)
    {
        cm_demo_printf("[%s] cm_malloc fail\n", __func__);
        goto error;
    }

    memset(data_buff, 0, fs_test_buff_size);

    int localfile_size = cm_fs_filesize(localfile);
    if (localfile_size <= 0)
    {
        cm_demo_printf("[%s] get file size fail\n", __func__);
        goto error;
    }

    fs_fd = cm_fs_open(localfile, CM_FS_RB);
    if (fs_fd == NULL)
    {
        cm_demo_printf("[%s] fs open fail\n", __func__);
        goto error;
    }

    int file_size_tmp = localfile_size;
    int offset = 0;
    while (file_size_tmp > 0)
    {
        int read_ret = cm_fs_read(fs_fd, data_buff, fs_test_buff_size);
        if (read_ret < 0)
        {
            cm_demo_printf("[%s] fs read fail file_size_tmp: %d\n", __func__, file_size_tmp);
            goto error;
        }

        int put_ret = cm_ftp_put_file(handle, 1, (char *)remotefile, data_buff, read_ret);
        if (put_ret < 0)
        {
            cm_demo_printf("[%s] ftp put fail\n", __func__);
            goto error;
        }

        file_size_tmp = file_size_tmp - read_ret;
        offset = offset + read_ret;
        int seek_ret = cm_fs_seek(fs_fd, offset, CM_FS_SEEK_SET);
        if (seek_ret < 0)
        {
            cm_demo_printf("[%s] fs seek fail\n", __func__);
            goto error;
        }
    }

    if (data_buff)
    {
        cm_free(data_buff);
    }
    if (fs_fd != NULL)
    {
        cm_fs_close(fs_fd);
    }
    return localfile_size;
error:

    if (data_buff)
    {
        cm_free(data_buff);
    }
    if (fs_fd != NULL)
    {
        cm_fs_close(fs_fd);
    }
    return -1;
}

/**
 *  @brief 获取指定ftp服务器上目录下文件和目录列表
 *
 *	@param [in]	 handle          FTP连接句柄
 *  @param [in]  path            指定路径，可以是相对或绝对路径
 *  @param [in]  match           若不为NULL，则只列出包含match字符的结果
 *  @param [out] file_info_list  获取到的文件和目录列表。
 *  @param [in]  max_file_num    file_info_list可以存储结果的最大个数
 *
 *  @return
 *   >= 0 - 成功，返回获取到的结果个数 \n
 *   <  0 - 失败
 */
int32_t cm_ftp_get_list(int32_t handle, const char *path, char *match, cmo_ftp_file_data_t file_info_list[], int max_file_num)
{
    static cmo_ftp_file_data_t file_data_tmp = {0};
    int count = 0;

    int find_fd1 = cm_ftp_find_first(handle, path, &file_data_tmp);

    if (find_fd1 < 0)
    {
        cm_demo_printf("[%s] cm_ftp find first fail\n", __func__);
        return -1;
    }

    if (match && max_file_num)
    {
        if (strstr((const char *)file_data_tmp.file_name, match))
        {
            memcpy(&file_info_list[0], &file_data_tmp, sizeof(cmo_ftp_file_data_t));
            max_file_num--;
            count++;
        }
    }
    else if (max_file_num)
    {
        memcpy(&file_info_list[0], &file_data_tmp, sizeof(cmo_ftp_file_data_t));
        max_file_num--;
        count++;
    }

    do{
        memset(&file_data_tmp, 0, sizeof(cmo_ftp_file_data_t));
        if (match && max_file_num)
        {
            find_fd1 = cm_ftp_find_next(handle, find_fd1, &file_data_tmp);
            if (strstr((const char *)file_data_tmp.file_name, match))
            {
                memcpy(&file_info_list[count], &file_data_tmp, sizeof(cmo_ftp_file_data_t));
                max_file_num--;
                count++;
            }
        }
        else if (max_file_num)
        {
            find_fd1 = cm_ftp_find_next(handle, find_fd1, &file_info_list[count]);
            max_file_num--;
            count++;
        }

    }while (find_fd1 >= 0 && max_file_num);

    /* 关闭查找 */
    int ret = cm_ftp_find_close(handle, find_fd1);

    if (ret < 0)
    {
        cm_demo_printf("[%s] cm_ftp find close fail\n", __func__);
    }

    return count;
}


NR_SHELL_CMD_EXPORT(cm_ftp_test_list, SHELL_testFTP_list);
NR_SHELL_CMD_EXPORT(cm_ftp_test_download, SHELL_testFTP_download);
NR_SHELL_CMD_EXPORT(cm_ftp_test, SHELL_testFTP);
NR_SHELL_CMD_EXPORT(cm_ftp_test_upload, SHELL_testFTP_upload);

#endif
#endif

