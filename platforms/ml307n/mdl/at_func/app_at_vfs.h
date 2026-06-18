/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file    app_at_vfs.h
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */
#ifndef __AT_VFS_H__
#define __AT_VFS_H__

#include <stddef.h>
#include <stdint.h>

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum {
    CME_ERR_VFS_OK = 0,
    CME_ERR_VFS_INVALID_INPUT = 400,
    CME_ERR_VFS_LARGER_THAN_FILE,
    CME_ERR_VFS_READ_ZERO,
    CME_ERR_VFS_DRIVE_FULL,
    CME_ERR_VFS_NOT_SUPPORT,
    CME_ERR_VFS_FILE_NEXIST,
    CME_ERR_VFS_INVALID_FILENAME,
    CME_ERR_VFS_FILE_EXIST,

    CME_ERR_VFS_WRITE_FILE = 409,
    CME_ERR_VFS_OPEN_FILE,
    CME_ERR_VFS_READ_FILE,

    CME_ERR_VFS_MAX_FILES = 413,
    CME_ERR_VFS_FILE_READONLY,

    CME_ERR_VFS_FILE_DESC = 416,
    CME_ERR_VFS_LIST_FILE,
    CME_ERR_VFS_DELETE_FILE,
    CME_ERR_VFS_DISK_INFO,
    CME_ERR_VFS_NO_SPACE,
    CME_ERR_VFS_TIMEOUT,

    CME_ERR_VFS_FILE_TOO_LARGE = 423,

    CME_ERR_VFS_INVALID_PARA = 425,
    CME_ERR_VFS_FILE_OPENED,

    CME_ERR_VFS_MAX = 9999          ///<   CME ERROR错误码支持最多4位
}CME_VFSErr;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void AT_VFSDiskInfoExec(uint8_t channelId);
void AT_VFSDiskInfoSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSDiskInfoTest(uint8_t channelId);

void AT_VFSListFileExec(uint8_t channelId);
void AT_VFSListFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSListFileTest(uint8_t channelId);

void AT_VFSDeleteFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSDeleteFileTest(uint8_t channelId);

void AT_VFSUploadFileRead(uint8_t channelId);
void AT_VFSUploadFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSUploadFileTest(uint8_t channelId);

void AT_VFSDownloadFileRead(uint8_t channelId);
void AT_VFSDownloadFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSDownloadFileTest(uint8_t channelId);

void AT_VFSOpenFileRead(uint8_t channelId);
void AT_VFSOpenFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSOpenFileTest(uint8_t channelId);

void AT_VFSReadFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSReadFileTest(uint8_t channelId);

void AT_VFSWriteFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSWriteFileTest(uint8_t channelId);

void AT_VFSSeekFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSSeekFileTest(uint8_t channelId);

void AT_VFSOffsetFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSOffsetFileTest(uint8_t channelId);

void AT_VFSTruncateFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSTruncateFileTest(uint8_t channelId);

void AT_VFSCloseFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSCloseFileTest(uint8_t channelId);

void AT_VFSMoveFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSMoveFileTest(uint8_t channelId);

void AT_VFSFlushFileSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSFlushFileTest(uint8_t channelId);

void AT_VFSChangeDirSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSChangeDirTest(uint8_t channelId);

void AT_VFSMakeDirSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSMakeDirTest(uint8_t channelId);

void AT_VFSPwdExec(uint8_t channelId);
void AT_VFSPwdSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_VFSPwdTest(uint8_t channelId);


#endif /* __AT_VFS_H__ */
