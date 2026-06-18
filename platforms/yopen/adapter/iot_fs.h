/**
 * @file iot_fs.h
 * @brief YOPEN 平台文件系统适配器头文件
 * @details 封装 YOPEN 平台 yopen_fs.h 接口，提供统一文件系统操作宏定义。
 */

#ifndef IOT_FS_YOPEN_H
#define IOT_FS_YOPEN_H

#include "yopen_fs.h"
#include "yopen_type.h"

#define iot_fs_file_t            QFILE
#define iot_fs_dir_t             QDIR*
#define iot_fs_dirent_t          qdirent

#define iot_fs_open(path, mode) \
    yopen_fopen((path), (mode))

#define iot_fs_close(fp) \
    yopen_fclose((fp))

#define iot_fs_read(fp, buf, size) \
    yopen_fread((fp), (buf), (size))

#define iot_fs_write(fp, buf, size) \
    yopen_fwrite((fp), (buf), (size))

#define iot_fs_seek(fp, offset, whence) \
    yopen_fseek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    yopen_fflush((fp))

#define iot_fs_mkdir(path) \
    yopen_mkdir((path))

#define iot_fs_remove(path) \
    yopen_remove((path))

#define iot_fs_rename(oldpath, newpath) \
    yopen_frename((oldpath), (newpath))

#define iot_fs_access(path, mode) \
    yopen_file_exists((path))

#define iot_fs_tell(fp) \
    yopen_ftell((fp))

#define iot_fs_size(fp) \
    yopen_fsize((fp))

#endif /* IOT_FS_YOPEN_H */