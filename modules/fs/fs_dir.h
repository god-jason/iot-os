/**
 * @file fs_dir.h
 * @brief 文件系统目录操作接口定义
 *
 * 提供文件系统目录操作的抽象接口，包括目录打开、关闭、
 * 读取目录条目、列出目录内容等功能。定义了目录条目结构体
 * fs_dir_entry_t 和目录句柄 fs_dir_t，用于遍历和管理文件系统目录。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_FS_DIR_H
#define IOT_FS_DIR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iot_fs_dir_entry {
    char name[256];
    int is_dir;
    long size;
    long mtime;
} iot_fs_dir_entry_t;

typedef struct iot_fs_dir iot_fs_dir_handle_t;

iot_fs_dir_handle_t* iot_fs_dir_open(const char* path);
void iot_fs_dir_close(iot_fs_dir_handle_t* dir);
int iot_fs_dir_read(iot_fs_dir_handle_t* dir, iot_fs_dir_entry_t* entry);
int iot_fs_dir_list(const char* path, iot_fs_dir_entry_t** entries, int* count);
void iot_fs_dir_free_list(iot_fs_dir_entry_t* entries);

#ifdef __cplusplus
}
#endif

#endif /* IOT_FS_DIR_H */