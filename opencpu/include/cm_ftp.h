/**
 * @file        cm_ftp.h
 * @brief       FTP接口
 * @copyright   Copyright © 2022 China Mobile IOT. All rights reserved.
 * @author      By mxc
 * @date        2022/06/07
 *
 * @defgroup  FTP
 * @ingroup FTP
 * @{
 */

#ifndef __CM_FTP_H__
#define __CM_FTP_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "lwip/sockets.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

/**FTP配置*/
typedef struct
{
    uint8_t *url;       /*!< FTP服务器URL或IP*/
    int32_t port;       /*!< FTP服务器端口*/
    uint8_t *username;  /*!< FTP服务器用户名*/
    uint8_t *passwd;    /*!< FTP服务器密码*/
    int32_t trans_mode; /*!< FTP传输模式，0：主动模式，1：被动模式。（该参数不生效，目前仅支持被动模式）*/
    int32_t data_type;  /*!< FTP数据传输类型，0：Binary，1：ASCII*/
    int32_t rsptimeout; /*!< FTP响应超时时间，单位ms，建议设置为1s以上*/
    int32_t ssl_mode;   /*!< FTP SSL模式，0：无SSL（目前不支持SSL）*/
    int32_t ssl_id;     /*!< FTP SSL通道ID，取值范围参考SSL模块（目前不支持SSL）*/
    int32_t cid;        /*!< FTP PDP通道ID*/
    void *pfnEventFunc; /*!< 仅占位，无实际用处 */
} cm_ftp_config_t;

/**FTP文件信息*/
typedef struct
{
    uint32_t file_attr;                 /*!< 0：（相对）路径        1：文件 */
    uint32_t file_size;                 /*!< file_attr = 1时为文件大小，否则该参数无效 */
    uint8_t file_name[256];             /*!< file_attr = 1时为文件名称，  file_attr = 0时为（相对）路径名称 */
    uint8_t file_permision;             /*!< 第0位代表是否可执行，第1位代表是否可写，第2位代表是否可读。0：是 1：否*/
    time_t file_modify_time;          /*!< 修改时间，unix时间戳*/
} cmo_ftp_file_data_t;

/**FTP状态*/
typedef enum
{
    CMO_FTP_STATE_DISCONNECTED = 0, /*!< FTP连接断开 */
    CMO_FTP_STATE_CONNECTED,        /*!< FTP连接建立 */
    CMO_FTP_STATE_GETTING,          /*!< FTP正在下载文件*/
    CMO_FTP_STATE_PUTTING           /*!< FTP正在上传文件*/
} cmo_ftp_state_e;

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
    extern "C" {
#else
#define EXTERN extern
#endif

/****************************************************************************/

/**
 *  @brief 连接FTP服务器
 *
 *  @param [in] config FTP配置信息
 *
 *  @return
 *   >= 0 - 成功，返回FTP连接句柄 \n
 *   <  0 - 失败
 */
int32_t cm_ftp_open(cm_ftp_config_t *config);

/**
 *  @brief 断开一个FTP连接
 *
 *  @param [in] handle FTP连接句柄
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_ftp_close(int32_t handle);

/**
 *  @brief 在服务器上新建文件夹
 *
 *  @param [in]  handle   FTP连接句柄
 *  @param [in]  dir  	  需要新建的文件夹，可以是相对或绝对路径
 *
 *  @return
 *   =  0 - 成功 \n
 *   <  0 - 失败
 */
int32_t cm_ftp_create_dir(int32_t handle, const char *dir);

/**
 *  @brief 删除服务器上的文件夹
 *
 *  @param [in] handle   FTP连接句柄
 *  @param [in] dir      需要删除的文件夹，可以是相对或绝对路径
 *
 *  @return
 *   =  0 - 成功 \n
 *   <  0 - 失败
 */
int32_t cm_ftp_remove_dir(int32_t handle, const char *dir);

/**
 *  @brief 删除服务器上指定文件
 *
 *  @param [in] handle 	FTP连接句柄
 *  @param [in] file 	需要删除的文件的文件名，可以包含相对或绝对路径
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_ftp_delete_file(int32_t handle, const char *file);

/**
 *  @brief 设置服务器上的当前工作目录
 *
 *  @param [in] handle 	FTP连接句柄
 *  @param [in] dir 	工作目录路径，可以是相对或绝对路径
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_ftp_set_current_dir(int32_t handle, const char *dir);

/**
 *  @brief 获取服务器上的当前工作目录
 *
 *  @param [in] handle 	FTP连接句柄
 *  @param [out] dir 	工作目录路径，绝对路径。
 *
 *  @return
 *   =  0 - 成功 \n
 *   <  0 - 失败
 *
 *  @details dir用于输出工作目录路径，需要用户自行维护内存空间
 */
int32_t cm_ftp_get_current_dir(int32_t handle, char *dir);

/**
 *  @brief 重命名文件
 *
 *  @param [in] handle  	FTP连接句柄
 *  @param [in] old_file 	原文件名，可以包含相对或绝对路径
 *  @param [in] new_file 	新文件名，可以包含相对或绝对路径
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_ftp_rename_file(int32_t handle, const char *old_file, const char *new_file);

/**
 *  @brief 获取文件大小
 *
 *  @param [in] handle FTP连接句柄
 *  @param [in] file   文件路径，可以包含相对或绝对路径
 *
 *  @return
 *   >= 0 - 成功，返回文件大小 \n
 *   <  0 - 失败
 */
int32_t cm_ftp_get_file_size(int32_t handle, const char *file);

/**
 *  @brief FTP打开查找，并且获取指定路径下第一个文件或文件夹目录信息
 *
 *	@param [in]	 handle        FTP连接句柄
 *  @param [in]  path          指定路径，可以是相对或绝对路径
 *  @param [out] file_data     对应路径下的第一个文件或（相对）路径信息
 *
 *  @return
 *   >= 0 - 成功，返回FTP查找句柄 \n
 *   <  0 - 失败
 *
 *  @details 获取指定路径下的文件夹和文件列表，需要配合cm_ftp_find_next、cm_ftp_find_close共同使用
 */
int32_t cm_ftp_find_first(int32_t handle, const char *path, cmo_ftp_file_data_t *file_data);

/**
 *  @brief FTP获取对应路径下的一个文件或文件夹目录信息
 *
 *	@param [in]	 handle    FTP连接句柄
 *  @param [in]  find_fd   查找句柄（cm_ftp_find_first接口返回值）
 *  @param [out] file_data 对应路径下的一个文件或文件夹目录信息
 *
 *  @return
 *   >= 0 - 表明对应路径下还有文件或文件夹目录可以获取 \n
 *   <  0 - 表明对应路径下没有剩余文件或文件夹目录可以获取
 *
 *  @details 获取指定路径下的文件夹和文件列表，需要配合cm_ftp_find_first、cm_ftp_find_close共同使用
 */
int32_t cm_ftp_find_next(int32_t handle, int32_t find_fd, cmo_ftp_file_data_t *file_data);

/**
 *  @brief 关闭查找
 *
 *	@param [in]	 handle    FTP连接句柄
 *  @param [in]  find_fd   查找句柄（cm_ftp_find_first接口返回值）
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 *  @details 获取指定路径下的文件夹和文件列表，需要配合cm_ftp_find_first、cm_ftp_find_next共同使用
 */
int32_t cm_ftp_find_close(int32_t handle, int32_t find_fd);

/**
 *  @brief FTP从服务器获取文件
 *
 *	@param [in]	 handle        FTP连接句柄
 *  @param [in]  data_type     FTP数据传输类型，0：Binary，1：ASCII（本接口中该参数不生效）
 *  @param [in]  file    	   要获取的文件名，可以包含相对或绝对路径。
 *  @param [in]  offset    	   获取内容的偏移量
 *  @param [out] data    	   存储获取内容的地址
 *  @param [in]  len    	   要获取的长度
 *
 *  @return
 *   >= 0 - 实际获取的长度 \n
 *   <  0 - 失败
 *
 *  @details data_type未生效，传输类型实际生效参数为cm_ftp_open传入的配置参数 \n
 *           data用于输出获取内容，需要用户自行维护内存空间
 */
int32_t cm_ftp_get_file(int32_t handle, int32_t data_type, const char *file, int32_t offset, uint8_t *data, uint32_t len);

/**
 *  @brief FTP向服务器推送文件
 *
 *	@param [in]	 handle    FTP连接句柄
 *	@param [in]	 mode      0:新建或覆盖  1：新建或追加
 *	@param [in]	 file      要传输的文件名，可以包含相对或绝对路径。
 *  @param [in]  data      要传输的文件数据指针
 *  @param [in]  len       要传输的文件数据长度
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_ftp_put_file(int32_t handle, int32_t mode, char *file, uint8_t *data, uint32_t len);

/**
 *  @brief 获取FTP连接状态
 *
 *	@param [in]	 handle        FTP连接句柄
 *
 *  @return
 *   FTP连接状态，参考cmo_ftp_state_e
 */
cmo_ftp_state_e cm_ftp_get_state(int32_t handle);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_FTP_CLIENT_H__ */

/** @}*/
