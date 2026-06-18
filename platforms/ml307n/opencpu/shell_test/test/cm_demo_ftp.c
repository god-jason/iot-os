/*********************************************************
 *  @file    cm_demo_ftp.c
 *  @brief   OpenCPU FTP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created By ZY 2024/12/9
 ********************************************************/
/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_FTP_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>

#include "cm_fs.h"
#include "cm_ftp.h"
#include "cm_mem.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Private Types
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
#define cm_demo_printf osPrintf
#define ftp_log(fmt, ...) osPrintf("[FTP][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

void print_download_help(const char *cmdName);
void print_upload_help(const char *cmdName);
int32_t cm_ftp_get_file_to_fs(int32_t handle, const char *remotefile, const char *localfile);
int32_t cm_ftp_put_file_from_fs(int32_t handle, const char *localfile, const char *remotefile);
void print_file_data(cmo_ftp_file_data_t* file_data);

/**
 *  FTP功能调试使用示例，注意使用前需先设置port，hostname，username，password
 *
 */
void SHELL_testFTP(char argc, char **argv)
{
    int32_t port = 21;   //测试端口
    char hostname[] = "8.137.154.246";    //测试地址
    char username[] = "fengshuaixing";    //测试用户名
    char password[] = "fengshuaixing";    //测试密码
    char dir[] = "opencpu_test_dir";   //测试文件夹名，请根据实际平台情况修改
    char home_dir[] = "/";    //测试文件夹名，请根据实际平台情况修改
    char file[] = "opencpu_ml307a_ftp_test7.txt";  //测试文件名，请根据实际平台情况修改
    char new_file[] = "ml307a_ftp_test8.txt";  //测试文件名，请根据实际平台情况修改
    char data[] = "1234567890";    //测试数据
    char out_data[32] = {0};

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
    char *get_dir = (char *)malloc(256);
    if(get_dir == NULL)
    {
        ftp_log("ftp malloc error");
        return;
    }

    ret = cm_ftp_get_current_dir(handle, get_dir);

    if (ret < 0)
    {
        ftp_log("ftp change dir fail");
        free(get_dir);
        return;
    }
    ftp_log("current_dir=[%s]", get_dir);
    free(get_dir);

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
    if(argv == NULL || argc < 4 || argv[2] == NULL || argv[3] == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

    int32_t port = 21;   //测试端口
    char hostname[] = "8.137.154.246";    //测试地址
    char username[] = "fengshuaixing";    //测试用户名
    char password[] = "fengshuaixing";    //测试密码
    int32_t ret = -1;
    int32_t handle = -1;

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
    char *remotefile = argv[2]; /* 例如:"/home/uftp/china.txt"*/
    char *localfile = argv[3];  /* 例如:"china.txt" */

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
    if(argv == NULL || argc < 4 || argv[2] == NULL || argv[3] == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

    int32_t port = 21;   //测试端口
    char hostname[] = "8.137.154.246";    //测试地址
    char username[] = "fengshuaixing";    //测试用户名
    char password[] = "fengshuaixing";    //测试密码

    int32_t ret = -1;
    int32_t handle = -1;

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
    char *localfile = argv[2];    /* 例如: "china.txt"*/
    char *remotefile = argv[3];   /* 例如: "/home/uftp/chinatest.txt" */

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
    if(argv == NULL || argc < 3 || argv[2] == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

    int32_t port = 21;   //测试端口
    char hostname[] = "8.137.154.246";    //测试地址
    char username[] = "fengshuaixing";    //测试用户名
    char password[] = "fengshuaixing";    //测试密码
    int32_t ret = -1;
    int32_t handle = -1;
    int32_t find_fd1 = -1;

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

    char *path = argv[2]; /*例如:home_dir */

    /* 获取文件列表信息 */
    /* 打开查找并进行首次查找 */
    cmo_ftp_file_data_t *file_data_test = (cmo_ftp_file_data_t *)malloc(sizeof(cmo_ftp_file_data_t));
    if(file_data_test == NULL)
    {
        ftp_log("ftp malloc error");
        return;
    }
    memset(file_data_test, 0, sizeof(cmo_ftp_file_data_t));

    find_fd1 = cm_ftp_find_first(handle, path, file_data_test);

    if (find_fd1 < 0)
    {
        cm_demo_printf("[%s] ftp find first fail\r\n", __func__);
        free(file_data_test);
        return;
    }

    ftp_log("=======================================================");

    print_file_data(file_data_test);

    /* 循环查找 */
    do {
        find_fd1 = cm_ftp_find_next(handle, find_fd1, file_data_test);
        if (find_fd1 >= 0)
        {
            print_file_data(file_data_test);
        }
    } while (find_fd1 >= 0);

    ftp_log("=======================================================");

    /* 关闭查找 */
    ret = cm_ftp_find_close(handle, find_fd1);

    if (ret < 0)
    {
        cm_demo_printf("[%s] ftp find close fail\r\n", __func__);
        free(file_data_test);
        return;
    }
    free(file_data_test);

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
    if(file_data == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

    ftp_log("attr[%d] ", file_data->file_attr);
    ftp_log("size[%d] ", file_data->file_size);
    ftp_log("name[%s] ", file_data->file_name);
    ftp_log("permision[%d] ", file_data->file_permision);
    ftp_log("modify_time[%d]\r\n", file_data->file_modify_time);
}

void print_download_help(const char *cmdName)
{
    if(cmdName == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

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
    if(cmdName == NULL)
    {
        ftp_log("ftp param error");
        return;
    }

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
    int fs_test_buff_size = 256;
    cm_fs_t fs_fd = NULL;
    int remotefile_size = 0;
    cm_fs_system_info_t fs_sys_info = {0, 0};
    int file_size_tmp = 0;
    int offset = 0;
    int get_ret = 0;
    int write_ret = 0;

    uint8_t *data_buff = (uint8_t *)cm_malloc(fs_test_buff_size);
    if (data_buff == NULL)
    {
        cm_demo_printf("[%s] cm_malloc fail\r\n", __func__);
        goto error;
    }
    memset(data_buff, 0, fs_test_buff_size);

    remotefile_size = cm_ftp_get_file_size(handle, remotefile);

    if (remotefile_size <= 0)
    {
        cm_demo_printf("[%s] get file size fail\r\n", __func__);
        goto error;
    }

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

    file_size_tmp = remotefile_size;
    while (file_size_tmp > 0)
    {
        get_ret = cm_ftp_get_file(handle, 0, remotefile, offset, data_buff, fs_test_buff_size);

        if (get_ret < 0)
        {
            cm_demo_printf("[%s] get file fail file_size_tmp: %d\r\n", __func__, file_size_tmp);
            goto error;
        }

        write_ret = cm_fs_write(fs_fd, data_buff, get_ret);
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
    int fs_test_buff_size = 256;
    cm_fs_t fs_fd = NULL;
    int localfile_size = 0;
    int file_size_tmp = 0;
    int offset = 0;
    int read_ret = 0;
    int put_ret = 0;
    int seek_ret = 0;

    uint8_t *data_buff = (uint8_t *)cm_malloc(fs_test_buff_size);
    if (data_buff == NULL)
    {
        cm_demo_printf("[%s] cm_malloc fail\n", __func__);
        goto error;
    }

    memset(data_buff, 0, fs_test_buff_size);

    localfile_size = cm_fs_filesize(localfile);
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

    file_size_tmp = localfile_size;
    while (file_size_tmp > 0)
    {
        read_ret = cm_fs_read(fs_fd, data_buff, fs_test_buff_size);
        if (read_ret < 0)
        {
            cm_demo_printf("[%s] fs read fail file_size_tmp: %d\n", __func__, file_size_tmp);
            goto error;
        }

        put_ret = cm_ftp_put_file(handle, 1, (char *)remotefile, data_buff, read_ret);
        if (put_ret < 0)
        {
            cm_demo_printf("[%s] ftp put fail\n", __func__);
            goto error;
        }

        file_size_tmp = file_size_tmp - read_ret;
        offset = offset + read_ret;
        seek_ret = cm_fs_seek(fs_fd, offset, CM_FS_SEEK_SET);
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

void SHELL_ftp_test(char argc, char **argv)
{
    if(argv == NULL && argv[1] == NULL)
    {
        shell_printf("shell param error\r\n");
        return;
    }

    if(argc == 1)
    {
        SHELL_testFTP(argc, argv);
    }
    else if(argc == 3 || argc == 4)
    {
        unsigned char operation[20] = {0};
        snprintf((char *)operation, sizeof(operation), "%.*s", sizeof(operation), argv[1]);
        if ((argc == 3) && (0 == strcmp((const char *)operation, "list")))
        {
            SHELL_testFTP_list(argc, argv);
        }
        else if ((argc == 4) && (0 == strcmp((const char *)operation, "download")))
        {
            SHELL_testFTP_download(argc, argv);
        }
        else if ((argc == 4) && (0 == strcmp((const char *)operation, "upload")))
        {
            SHELL_testFTP_upload(argc, argv);
        }
    }
    else
    {
        shell_printf("-----------------ftp help---------------------\r\n");
        shell_printf("1 ftp \r\n");
        shell_printf("2 ftp list dir_path\r\n");
        shell_printf("3 ftp download remotefile localfile\r\n");
        shell_printf("4 ftp upload localfile remotefile\r\n");
        shell_printf("-----------------ftp--------------------------\r\n");
    }
}

NR_SHELL_CMD_EXPORT(ftp, SHELL_ftp_test);

#endif
#endif

