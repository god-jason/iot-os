/**
 * @file iot_fs.c
 * @brief ML307N 平台文件系统适配器实现
 * @details 基于 VFS 接口实现文件系统功能封装，
 *          提供统一的文件系统操作函数。
 */

#include "iot_fs.h"

/**
 * @brief 打开文件
 * @param[in] path 文件路径
 * @param[in] mode 打开模式
 * @return 文件句柄
 */
iot_fs_file_t *iot_fs_open(const char *path, const char *mode)
{
    return VFS_OpenFile(path, mode);
}

/**
 * @brief 关闭文件
 * @param[in] fp 文件句柄
 * @return 0 成功
 */
int32_t iot_fs_close(iot_fs_file_t *fp)
{
    return VFS_CloseFile(fp);
}

/**
 * @brief 读取文件
 * @param[in] fp  文件句柄
 * @param[out] buf 数据缓冲区
 * @param[in] size 读取大小
 * @return 实际读取长度
 */
int32_t iot_fs_read(iot_fs_file_t *fp, void *buf, uint32_t size)
{
    return VFS_ReadFile(buf, 1, size, fp);
}

/**
 * @brief 写入文件
 * @param[in] fp  文件句柄
 * @param[in] buf 数据缓冲区
 * @param[in] size 写入大小
 * @return 实际写入长度
 */
int32_t iot_fs_write(iot_fs_file_t *fp, const void *buf, uint32_t size)
{
    return VFS_WriteFile(buf, 1, size, fp);
}

/**
 * @brief 文件定位
 * @param[in] fp     文件句柄
 * @param[in] offset 偏移量
 * @param[in] whence 起始位置
 * @return 0 成功
 */
int32_t iot_fs_seek(iot_fs_file_t *fp, int32_t offset, int32_t whence)
{
    return VFS_SeekFile(fp, offset, whence);
}

/**
 * @brief 获取文件位置
 * @param[in] fp 文件句柄
 * @return 当前位置
 */
int32_t iot_fs_tell(iot_fs_file_t *fp)
{
    return VFS_GetFilePosition(fp);
}

/**
 * @brief 同步文件
 * @param[in] fp 文件句柄
 * @return 0 成功
 */
int32_t iot_fs_sync(iot_fs_file_t *fp)
{
    return VFS_SyncFile(fp);
}

/**
 * @brief 获取文件大小
 * @param[in] fp 文件句柄
 * @return 文件大小
 */
int32_t iot_fs_size(iot_fs_file_t *fp)
{
    return VFS_GetFileSize(fp);
}

/**
 * @brief 截断文件
 * @param[in] fp  文件句柄
 * @param[in] len 截断长度
 * @return 0 成功
 */
int32_t iot_fs_truncate(iot_fs_file_t *fp, int32_t len)
{
    return VFS_TruncateFile(fp, len);
}

/**
 * @brief 创建目录
 * @param[in] path 目录路径
 * @param[in] mode 权限模式
 * @return 0 成功
 */
int32_t iot_fs_mkdir(const char *path, uint32_t mode)
{
    return VFS_MakeDir(path, mode);
}

/**
 * @brief 打开目录
 * @param[in] path 目录路径
 * @return 目录句柄
 */
iot_fs_dir_t *iot_fs_opendir(const char *path)
{
    return VFS_OpenDir(path);
}

/**
 * @brief 关闭目录
 * @param[in] dp 目录句柄
 * @return 0 成功
 */
int32_t iot_fs_closedir(iot_fs_dir_t *dp)
{
    return VFS_CloseDir(dp);
}

/**
 * @brief 读取目录
 * @param[in] dp 目录句柄
 * @return 目录项
 */
iot_fs_dirent_t *iot_fs_readdir(iot_fs_dir_t *dp)
{
    return VFS_ReadDir(dp);
}

/**
 * @brief 删除文件
 * @param[in] path 文件路径
 * @return 0 成功
 */
int32_t iot_fs_remove(const char *path)
{
    return VFS_Unlink(path);
}

/**
 * @brief 重命名文件
 * @param[in] oldpath 原路径
 * @param[in] newpath 新路径
 * @return 0 成功
 */
int32_t iot_fs_rename(const char *oldpath, const char *newpath)
{
    return VFS_Rename(oldpath, newpath);
}

/**
 * @brief 检查文件访问权限
 * @param[in] path 文件路径
 * @param[in] mode 访问模式
 * @return 0 成功
 */
int32_t iot_fs_access(const char *path, int32_t mode)
{
    return VFS_Access(path, mode);
}

/**
 * @brief 获取文件状态
 * @param[in] path 文件路径
 * @param[out] stat 状态结构体
 * @return 0 成功
 */
int32_t iot_fs_stat(const char *path, void *stat)
{
    return VFS_Stat(path, stat);
}

/**
 * @brief 挂载文件系统
 * @param[in] dev  设备名
 * @param[in] fs   文件系统类型
 * @param[in] path 挂载路径
 * @param[in] flags 标志
 * @return 0 成功
 */
int32_t iot_fs_mount(const char *dev, const char *fs, const char *path, uint32_t flags)
{
    return VFS_Mount(dev, fs, path, flags);
}

/**
 * @brief 卸载文件系统
 * @param[in] path 挂载路径
 * @return 0 成功
 */
int32_t iot_fs_unmount(const char *path)
{
    return VFS_Unmount(path);
}