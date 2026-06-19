#ifndef IOT_FS_WINDOWS_H
#define IOT_FS_WINDOWS_H

#include <stdio.h>
#include <io.h>
#include <direct.h>

#define iot_fs_file_t            FILE*
#define iot_fs_dir_t             intptr_t
#define iot_fs_dirent_t          struct _finddata_t

#define iot_fs_open(path, mode) \
    fopen((path), (mode))

#define iot_fs_close(fp) \
    fclose((fp))

#define iot_fs_read(fp, buf, size) \
    fread((buf), 1, (size), (fp))

#define iot_fs_write(fp, buf, size) \
    fwrite((buf), 1, (size), (fp))

#define iot_fs_seek(fp, offset, whence) \
    fseek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    fflush((fp))

#define iot_fs_mkdir(path, mode) \
    (_mkdir((path)) == 0)

#define iot_fs_remove(path) \
    (remove((path)) == 0)

#define iot_fs_rename(oldpath, newpath) \
    (rename((oldpath), (newpath)) == 0)

#define iot_fs_access(path, mode) \
    (_access((path), 0) == 0)

#endif