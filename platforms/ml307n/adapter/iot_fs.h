#ifndef IOT_FS_ML307N_H
#define IOT_FS_ML307N_H

#include <stdint.h>
#include "cm_fs.h"

#define iot_fs_file_t            cm_fs_t
#define iot_fs_dir_t             uint32_t
#define iot_fs_dirent_t          cm_fs_file_data_t

int32_t iot_fs_open(const char *path, const char *mode);
int32_t iot_fs_close(iot_fs_file_t fp);
int32_t iot_fs_read(iot_fs_file_t fp, void *buf, uint32_t size);
int32_t iot_fs_write(iot_fs_file_t fp, const void *buf, uint32_t size);
int32_t iot_fs_seek(iot_fs_file_t fp, int32_t offset, int32_t whence);
int32_t iot_fs_sync(iot_fs_file_t fp);
int32_t iot_fs_mkdir(const char *path, uint32_t mode);
iot_fs_dir_t *iot_fs_opendir(const char *path);
int32_t iot_fs_closedir(iot_fs_dir_t dp);
iot_fs_dirent_t *iot_fs_readdir(iot_fs_dir_t dp);
int32_t iot_fs_remove(const char *path);
int32_t iot_fs_rename(const char *oldpath, const char *newpath);
int32_t iot_fs_access(const char *path, int32_t mode);

#endif