/**
 * @file fs_dir.h
 * @brief 文件系统目录操作接口定义
 */

#ifndef IOT_FS_DIR_H
#define IOT_FS_DIR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fs_dir_entry {
    char name[256];
    int is_dir;
    long size;
    long mtime;
} fs_dir_entry_t;

typedef struct fs_dir fs_dir_t;

fs_dir_t* fs_dir_open(const char* path);
void fs_dir_close(fs_dir_t* dir);
int fs_dir_read(fs_dir_t* dir, fs_dir_entry_t* entry);
int fs_dir_list(const char* path, fs_dir_entry_t** entries, int* count);
void fs_dir_free_list(fs_dir_entry_t* entries);

#ifdef __cplusplus
}
#endif

#endif /* IOT_FS_DIR_H */