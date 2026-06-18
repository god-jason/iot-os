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

// #include "cm_common_inc.h"
// #include "cm_common_api.h"
#include "at_api.h"
#include "at_parser.h"
#include "cm_ftp.h"
#include "cm_mem.h"
#include "cm_demo_common.h"

typedef AT_CommandItem at_cmd_t;

#define FTP_LOG(fmt, ...) osPrintf("[OC_FTP_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define FTP_CMD_STR_LEN 512

#define CM_FTP_RET_CODE_OK 0 //成功
#define CM_FTP_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_FTP_RET_CODE_PARAM_ERROR 50 //参数错误 50

void ftp_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_ftp_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_create_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_remove_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_delete_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_rename_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_get_file_size(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_set_current_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_get_current_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_find_first(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_find_next(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_find_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_get_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_put_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ftp_get_state(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_ftp_oc[] =
{
    {.name = "+FTP",   .setFunc = ftp_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+FTP="XX", param1, param2, ...
// "XX" 为函数名
void ftp_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    FTP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        if (para_count < 1 || para_count > 7)
        {
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_ftp_open") == 0)
        {
            ret = func_cm_ftp_open(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_close") == 0)
        {
            ret = func_cm_ftp_close(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_create_dir") == 0)
        {
            ret = func_cm_ftp_create_dir(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_remove_dir") == 0)
        {
            ret = func_cm_ftp_remove_dir(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_delete_file") == 0)
        {
            ret = func_cm_ftp_delete_file(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_rename_file") == 0)
        {
            ret = func_cm_ftp_rename_file(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_get_file_size") == 0)
        {
            ret = func_cm_ftp_get_file_size(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_set_current_dir") == 0)
        {
            ret = func_cm_ftp_set_current_dir(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_get_current_dir") == 0)
        {
            ret = func_cm_ftp_get_current_dir(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_find_first") == 0)
        {
            ret = func_cm_ftp_find_first(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_find_next") == 0)
        {
            ret = func_cm_ftp_find_next(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_find_close") == 0)
        {
            ret = func_cm_ftp_find_close(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_get_file") == 0)
        {
            ret = func_cm_ftp_get_file(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_put_file") == 0)
        {
            ret = func_cm_ftp_put_file(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ftp_get_state") == 0)
        {
            ret = func_cm_ftp_get_state(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            FTP_LOG("Error func name[%s]", func_name);
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

// AT+FTP="cm_ftp_open","url",port,"username","passwd",rsptimeout
int func_cm_ftp_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t handle = -1;

    char* url = NULL;
    int port = 21;
    char* username = NULL;
    char* passwd = NULL;
    int rsptimeout = 10*1000;

    url = cm_malloc(FTP_CMD_STR_LEN);
    if (url == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", url, FTP_CMD_STR_LEN))
    {
        cm_free(url);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &port))
    {
        cm_free(url);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    username = cm_malloc(FTP_CMD_STR_LEN);
    if (username == NULL) {
        cm_free(url);
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", username, FTP_CMD_STR_LEN))
    {
        cm_free(url);
        cm_free(username);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    passwd = cm_malloc(FTP_CMD_STR_LEN);
    if (passwd == NULL) {
        cm_free(url);
        cm_free(username);
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", passwd, FTP_CMD_STR_LEN))
    {
        cm_free(url);
        cm_free(username);
        cm_free(passwd);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &rsptimeout))
    {
        cm_free(url);
        cm_free(username);
        cm_free(passwd);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }


    FTP_LOG("url[%s] port[%d] username[%s] passwd[%s] rsptimeout[%d]", \
            url, port, username, passwd, rsptimeout);


    /* 配置连接参数 */
    cm_ftp_config_t config = {0};
    config.url = (uint8_t *)url;       //需补充服务器地址
    config.port = port;                     //需补充服务器端口号
    config.username = (uint8_t *)username;  //需补充用户名
    config.passwd = (uint8_t *)passwd;    //需补充密码
    config.rsptimeout = rsptimeout;            //超时时间10秒
    config.cid = 0xff; // 使用默认cid

    /* 连接平台 */
    handle = cm_ftp_open(&config);

    if (handle < 0)
    {
        FTP_LOG("ftp open fail");
    }

    cm_uart_printf_urc("+FTPOPEN: %d", handle);
    cm_free(url);
    cm_free(username);
    cm_free(passwd);
    return 0;
}

// AT+FTP="cm_ftp_close", handle
int func_cm_ftp_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;
    int handle = -1;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    ret = cm_ftp_close(handle);

    if (ret < 0)
    {
        FTP_LOG("ftp close fail");
    }

    cm_uart_printf_urc("+FTPCLOSE: %d,%d", handle, ret);
    return 0;
}

// AT+FTP="cm_ftp_create_dir", handle, "dir"
int func_cm_ftp_create_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_create_dir(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp create dir fail");
    }

    cm_uart_printf_urc("+FTPMKD: %d,%d", handle, ret);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_remove_dir", handle, "dir"
int func_cm_ftp_remove_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_remove_dir(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp remove dir fail");
    }

    cm_uart_printf_urc("+FTPDEL: %d,%d", handle, ret);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_delete_file", handle, "dir"
int func_cm_ftp_delete_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_delete_file(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp del file fail");
    }

    cm_uart_printf_urc("+FTPDEL: %d,%d", handle, ret);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_rename_file", handle, "file", "new_file"
int func_cm_ftp_rename_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* file = NULL;
    char* new_file = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    file = cm_malloc(FTP_CMD_STR_LEN);
    if (file == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(file, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", file, FTP_CMD_STR_LEN))
    {
        cm_free(file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    new_file = cm_malloc(FTP_CMD_STR_LEN);
    if (new_file == NULL) {
        cm_free(file);
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(new_file, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", new_file, FTP_CMD_STR_LEN))
    {
        cm_free(file);
        cm_free(new_file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] file[%s] new_file[%s]", handle, file, new_file);

    /* 修改文件名 */
    ret = cm_ftp_rename_file(handle, file, new_file);

    if (ret < 0)
    {
        FTP_LOG("ftp rename fail");
    }

    cm_uart_printf_urc("+FTPRN: %d,%d", handle, ret);
    cm_free(file);
    cm_free(new_file);
    return 0;
}


// AT+FTP="cm_ftp_get_file_size", handle, "dir"
int func_cm_ftp_get_file_size(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_get_file_size(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp get file size fail");
    }

    cm_uart_printf_urc("+FTPFILESIZE: %d,%d", handle, ret);
    cm_free(dir);
    return 0;
}


// AT+FTP="cm_ftp_set_current_dir", handle, "dir"
int func_cm_ftp_set_current_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_set_current_dir(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp change dir fail");
    }

    cm_uart_printf_urc("+FTPCWD: %d,%d", handle, ret);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_get_current_dir", handle
int func_cm_ftp_get_current_dir(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    /* 获取当前工作目录 */
    ret = cm_ftp_get_current_dir(handle, dir);

    if (ret < 0)
    {
        FTP_LOG("ftp change dir fail");
        cm_uart_printf_urc("+FTPPWD: %d,%d", handle, ret);
        cm_free(dir);
        return 0;
    }

    FTP_LOG("current dir[%s]", dir);
    cm_uart_printf_urc("+FTPPWD: %d,%d,%s", handle, ret, dir);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_find_first", handle, "dir"
int func_cm_ftp_find_first(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;
    int32_t find_fd1 = -1;
    cmo_ftp_file_data_t file_data_test = {0};

    int32_t handle = -1;
    char* dir = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    dir = cm_malloc(FTP_CMD_STR_LEN);
    if (dir == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(dir, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", dir, FTP_CMD_STR_LEN))
    {
        cm_free(dir);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    FTP_LOG("handle[%d] dir[%s]", handle, dir);

    ret = cm_ftp_find_first(handle, dir, &file_data_test);

    if (ret < 0)
    {
        FTP_LOG("ftp find first fail");
        cm_uart_printf_urc("+FTPFINDFIRST: %d,%d", handle, ret);
        cm_free(dir);
        return 0;
    }

    cm_uart_printf_urc("+FTPFINDFIRST: %d,%d,%u,%u,%s,%u,%d", handle, ret, \
                                                              file_data_test.file_attr, \
                                                              file_data_test.file_size, \
                                                              file_data_test.file_name, \
                                                              file_data_test.file_permision, \
                                                              (long)file_data_test.file_modify_time);
    cm_free(dir);
    return 0;
}

// AT+FTP="cm_ftp_find_next", handle, find_fd1
int func_cm_ftp_find_next(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;
    cmo_ftp_file_data_t file_data_test = {0};

    int32_t handle = -1;
    int32_t find_fd1 = -1;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &find_fd1))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    ret = cm_ftp_find_next(handle, find_fd1, &file_data_test);
    if (ret < 0)
    {
        FTP_LOG("ftp find next fail");
        cm_uart_printf_urc("+FTPFINDNEXT: %d,%d", handle, ret);
        return 0;
    }

    cm_uart_printf_urc("+FTPFINDNEXT: %d,%d,%u,%u,%s,%u,%d", handle, ret, \
                                                              file_data_test.file_attr, \
                                                              file_data_test.file_size, \
                                                              file_data_test.file_name, \
                                                              file_data_test.file_permision, \
                                                              (long)file_data_test.file_modify_time);
    return 0;
}

// AT+FTP="cm_ftp_find_close", handle, find_fd1
int func_cm_ftp_find_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;
    cmo_ftp_file_data_t file_data_test = {0};

    int32_t handle = -1;
    int32_t find_fd1 = -1;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &find_fd1))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    /* 关闭查找 */
    ret = cm_ftp_find_close(handle, find_fd1);

    if (ret < 0)
    {
        FTP_LOG("ftp find close fail");
    }

    cm_uart_printf_urc("+FTPFINDCLOSE: %d,%d", handle, ret);
    return 0;
}

// AT+FTP="cm_ftp_get_file", handle, data_type, "file", offset, len
int func_cm_ftp_get_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    int handle = 0;
    int data_type = 0;
    char *file = NULL;
    int offset = 0;
    int len = 0;

    char *data = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &data_type))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    file = cm_malloc(FTP_CMD_STR_LEN);
    if (file == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(file, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", file, FTP_CMD_STR_LEN))
    {
        cm_free(file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &offset))
    {
        cm_free(file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &len))
    {
        cm_free(file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    data = cm_malloc(len + 1);
    if (data == NULL) {
        cm_free(file);
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(data, 0, len + 1);

    /* 下载文件内容 */
    /* 每次拉取需要建立连接，建立连接需要耗时，所以下载大文件时，单次拉取的长度越大效率越高 */
    ret = cm_ftp_get_file(handle, data_type, file, offset, (uint8_t*)data, len);

    if (ret < 0)
    {
        FTP_LOG("ftp get file fail");
        cm_uart_printf_urc("+FTPGET: %d,%d", handle, ret);
        cm_free(file);
        cm_free(data);
        return 0;
    }

    data[ret] = '\0';
    FTP_LOG("get_len[%d] data[%s]", ret, data);

    cm_uart_printf_urc("+FTPGET: %d,%d", handle, ret);
    cm_free(file);
    cm_free(data);
    return 0;
}

// AT+FTP="cm_ftp_put_file",handle,mode,"file","data"
int func_cm_ftp_put_file(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    int handle = 0;
    int mode = 0;
    char *file = NULL;
    char *data = NULL;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mode))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    file = cm_malloc(FTP_CMD_STR_LEN);
    if (file == NULL) {
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(file, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", file, FTP_CMD_STR_LEN))
    {
        cm_free(file);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    data = cm_malloc(FTP_CMD_STR_LEN);
    if (data == NULL) {
        cm_free(file);
        return CM_FTP_RET_CODE_MALLOC_FAIL;
    }
    memset(data, 0, FTP_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", data, FTP_CMD_STR_LEN))
    {
        cm_free(file);
        cm_free(data);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    ret = cm_ftp_put_file(handle, mode, file, (uint8_t*)data, strlen(data));

    if (ret < 0)
    {
        FTP_LOG("ftp put file fail");
    }

    cm_uart_printf_urc("+FTPPUT: %d,%d", handle, ret);
    cm_free(file);
    cm_free(data);
    return 0;
}



// AT+FTP="cm_ftp_get_state", handle
int func_cm_ftp_get_state(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;
    int handle = -1;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &handle))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    ret = cm_ftp_get_state(handle);

    if (ret < 0)
    {
        FTP_LOG("ftp get state fail");
    }

    cm_uart_printf_urc("+FTPSTATE: %d,%d", handle, ret);
    return 0;
}

#endif
#endif


