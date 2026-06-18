#ifndef IOT_FS_ML307N_H
#define IOT_FS_ML307N_H

#include <stdint.h>
#include "cm_fs.h"

#define iot_fs_file_t            cm_fs_t
#define iot_fs_dir_t             uint32_t
#define iot_fs_dirent_t          cm_fs_file_data_t

#define iot_fs_open(path, mode) \
    cm_fs_open((path), (mode))

#define iot_fs_close(fp) \
    cm_fs_close((fp))

#define iot_fs_read(fp, buf, size) \
    cm_fs_read((fp), (buf), (size))

#define iot_fs_write(fp, buf, size) \
    cm_fs_write((fp), (buf), (size))

#define iot_fs_seek(fp, offset, whence) \
    cm_fs_seek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    cm_fs_sync((fp))

#define iot_fs_mkdir(path, mode) \
    cm_fs_mkdir((path))

#define iot_fs_remove(path) \
    cm_fs_delete((path))

#define iot_fs_rename(oldpath, newpath) \
    cm_fs_move((oldpath), (newpath))

#define iot_fs_access(path, mode) \
    cm_fs_exist((path))

#endif