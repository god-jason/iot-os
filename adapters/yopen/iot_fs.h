/**
 * @file iot_fs.h
 * @brief YOPEN 平台文件系统适配器头文件
 * @details 基于 yopen_fs 接口实现文件系统操作功能。
 */

#ifndef IOT_FS_YOPEN_H
#define IOT_FS_YOPEN_H

#include "../../iot_types.h"

/* 引入 yopen 平台文件系统头文件 */
#include "yopen_fs.h"

/* ============================================================
 * 文件系统宏定义
 * ============================================================ */

/** @brief 文件打开模式 */
#define IOT_FS_MODE_READ    YOPEN_FS_O_RDONLY
#define IOT_FS_MODE_WRITE   YOPEN_FS_O_WRONLY
#define IOT_FS_MODE_CREATE  YOPEN_FS_O_CREAT
#define IOT_FS_MODE_APPEND  YOPEN_FS_O_APPEND

/** @brief 文件句柄类型 */
typedef QFILE iot_file_t;

/** @brief 文件信息结构体 */
typedef struct {
    uint8_t type;                     /**< 文件类型 */
    uint32_t size;                    /**< 文件大小 */
    char name[256];                   /**< 文件名 */
} iot_fs_file_info_t;

/** @brief 目录句柄类型 */
typedef QDIR iot_dir_t;

/** @brief 目录项结构体 */
typedef qdirent iot_dirent_t;

/* ============================================================
 * 文件操作宏定义
 * ============================================================ */

/**
 * @brief 打开文件
 * @param[in] path 文件路径
 * @param[in] mode 打开模式
 * @return 文件句柄，失败返回NULL
 */
#define iot_fs_open(path, mode) \
    ((iot_file_t)yopen_fopen((const char *)(path), (const char *)(mode)))

/**
 * @brief 关闭文件
 * @param[in] file 文件句柄
 * @return 0 成功，<0 失败
 */
#define iot_fs_close(file) \
    yopen_fclose((QFILE)(file))

/**
 * @brief 读取文件
 * @param[in] file 文件句柄
 * @param[out] buf 读取缓冲区
 * @param[in] size 要读取的字节数
 * @return >=0 实际读取的字节数，<0 失败
 */
#define iot_fs_read(file, buf, size) \
    yopen_fread((void *)(buf), (uint32_t)(size), 1, (QFILE)(file))

/**
 * @brief 写入文件
 * @param[in] file 文件句柄
 * @param[in] buf 要写入的数据
 * @param[in] size 要写入的字节数
 * @return >=0 实际写入的字节数，<0 失败
 */
#define iot_fs_write(file, buf, size) \
    yopen_fwrite((const void *)(buf), (uint32_t)(size), 1, (QFILE)(file))

/**
 * @brief 文件定位
 * @param[in] file 文件句柄
 * @param[in] offset 偏移量
 * @param[in] whence 偏移起始位置
 * @return 0 成功，<0 失败
 */
#define iot_fs_seek(file, offset, whence) \
    yopen_fseek((QFILE)(file), (int32_t)(offset), (int)(whence))

/**
 * @brief 获取文件位置
 * @param[in] file 文件句柄
 * @return >=0 当前位置，<0 失败
 */
#define iot_fs_tell(file) \
    yopen_ftell((QFILE)(file))

/**
 * @brief 获取文件大小
 * @param[in] file 文件句柄
 * @return >=0 文件大小，<0 失败
 */
#define iot_fs_get_size(file) \
    yopen_fget_size((QFILE)(file))

/**
 * @brief 检查文件是否结束
 * @param[in] file 文件句柄
 * @return true 结束，false 未结束
 */
#define iot_fs_eof(file) \
    ((bool)(yopen_feof((QFILE)(file)) != 0))

/**
 * @brief 刷新文件缓冲区
 * @param[in] file 文件句柄
 * @return 0 成功，<0 失败
 */
#define iot_fs_flush(file) \
    yopen_fflush((QFILE)(file))

/* ============================================================
 * 目录操作宏定义
 * ============================================================ */

/**
 * @brief 打开目录
 * @param[in] path 目录路径
 * @return 目录句柄，失败返回NULL
 */
#define iot_fs_opendir(path) \
    ((iot_dir_t *)yopen_opendir((const char *)(path)))

/**
 * @brief 关闭目录
 * @param[in] dir 目录句柄
 * @return 0 成功，<0 失败
 */
#define iot_fs_closedir(dir) \
    yopen_closedir((QDIR *)(dir))

/**
 * @brief 读取目录项
 * @param[in] dir 目录句柄
 * @return 目录项结构体指针，NULL 结束或失败
 */
#define iot_fs_readdir(dir) \
    ((iot_dirent_t *)yopen_readdir((QDIR *)(dir)))

/* ============================================================
 * 文件/目录操作宏定义
 * ============================================================ */

/**
 * @brief 删除文件
 * @param[in] path 文件路径
 * @return 0 成功，<0 失败
 */
#define iot_fs_remove(path) \
    yopen_remove((const char *)(path))

/**
 * @brief 重命名文件
 * @param[in] old_path 原路径
 * @param[in] new_path 新路径
 * @return 0 成功，<0 失败
 */
#define iot_fs_rename(old_path, new_path) \
    yopen_rename((const char *)(old_path), (const char *)(new_path))

/**
 * @brief 创建目录
 * @param[in] path 目录路径
 * @return 0 成功，<0 失败
 */
#define iot_fs_mkdir(path) \
    yopen_mkdir((const char *)(path))

/**
 * @brief 删除目录
 * @param[in] path 目录路径
 * @return 0 成功，<0 失败
 */
#define iot_fs_rmdir(path) \
    yopen_rmdir((const char *)(path))

/**
 * @brief 检查文件/目录是否存在
 * @param[in] path 路径
 * @return true 存在，false 不存在
 */
bool iot_fs_exists(const char *path);

/**
 * @brief 获取磁盘信息
 * @param[in] path 路径（可为NULL）
 * @param[out] total 总空间(字节)
 * @param[out] free 空闲空间(字节)
 * @return 0 成功，<0 失败
 */
int32_t iot_fs_get_disk_info(const char *path, uint32_t *total, uint32_t *free);

#endif /* IOT_FS_YOPEN_H */
