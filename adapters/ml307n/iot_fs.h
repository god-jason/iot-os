#ifndef IOT_FS_ML307N_H
#define IOT_FS_ML307N_H

#include "../../iot_types.h"
#include "vfs.h"

#define iot_fs_file_t            VFS_File
#define iot_fs_dir_t             VFS_Dir
#define iot_fs_dirent_t          VFS_Dirent

#define iot_fs_open(path, mode) \
    VFS_OpenFile(path, mode)

#define iot_fs_close(fp) \
    VFS_CloseFile(fp)

#define iot_fs_read(fp, buf, size) \
    VFS_ReadFile(buf, 1, size, fp)

#define iot_fs_write(fp, buf, size) \
    VFS_WriteFile(buf, 1, size, fp)

#define iot_fs_seek(fp, offset, whence) \
    VFS_SeekFile(fp, offset, whence)

#define iot_fs_tell(fp) \
    VFS_GetFilePosition(fp)

#define iot_fs_sync(fp) \
    VFS_SyncFile(fp)

#define iot_fs_size(fp) \
    VFS_GetFileSize(fp)

#define iot_fs_truncate(fp, len) \
    VFS_TruncateFile(fp, len)

#define iot_fs_mkdir(path, mode) \
    VFS_MakeDir(path, mode)

#define iot_fs_opendir(path) \
    VFS_OpenDir(path)

#define iot_fs_closedir(dp) \
    VFS_CloseDir(dp)

#define iot_fs_readdir(dp) \
    VFS_ReadDir(dp)

#define iot_fs_remove(path) \
    VFS_Unlink(path)

#define iot_fs_rename(oldpath, newpath) \
    VFS_Rename(oldpath, newpath)

#define iot_fs_access(path, mode) \
    VFS_Access(path, mode)

#define iot_fs_stat(path, stat) \
    VFS_Stat(path, stat)

#define iot_fs_mount(dev, fs, path, flags) \
    VFS_Mount(dev, fs, path, flags)

#define iot_fs_unmount(path) \
    VFS_Unmount(path)

#endif
