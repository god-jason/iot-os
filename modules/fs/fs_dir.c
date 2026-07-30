/**
 * @file fs_dir.c
 * @brief 文件系统目录操作实现
 *
 * 基于 IoT OS 底层文件系统 API 实现目录操作功能，
 * 包括目录打开、关闭、读取目录条目以及列出目录所有内容。
 * 通过 fs_path 模块辅助路径拼接，使用动态内存分配支持
 * 可变长度的目录列表返回，适用于嵌入式 IoT 设备的文件管理场景。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "fs_dir.h"
#include "iot.h"
#include "fs_path.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

struct fs_dir {
    iot_fs_dir_t dir;
    char path[512];
    int closed;
};

fs_dir_t* fs_dir_open(const char* path) {
    if (!path || path[0] == '\0') {
        return NULL;
    }

    fs_dir_t* dir = (fs_dir_t*)iot_malloc(sizeof(fs_dir_t));
    if (!dir) {
        return NULL;
    }

    memset(dir, 0, sizeof(fs_dir_t));
    strncpy(dir->path, path, sizeof(dir->path) - 1);

    char pattern[512];
    fs_path_join(pattern, sizeof(pattern), path, "*");

    dir->dir = iot_fs_opendir(pattern);
    if ((intptr_t)dir->dir == -1) {
        iot_free(dir);
        return NULL;
    }

    return dir;
}

void fs_dir_close(fs_dir_t* dir) {
    if (!dir) return;

    if (!dir->closed && dir->dir) {
        iot_fs_closedir(dir->dir);
    }

    iot_free(dir);
}

int fs_dir_read(fs_dir_t* dir, fs_dir_entry_t* entry) {
    if (!dir || !entry || dir->closed || !dir->dir) {
        return -1;
    }

    iot_fs_dirent_t raw_entry;
    if (iot_fs_readdir(dir->dir, &raw_entry) != 0) {
        return -1;
    }

    /* 获取文件名 */
    const char* name = iot_fs_dirent_name(raw_entry);
    if (!name || name[0] == '\0') {
        return -1;
    }

    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';

    char full_path[1024];
    fs_path_join(full_path, sizeof(full_path), dir->path, name);

    struct stat st;
    if (stat(full_path, &st) == 0) {
        entry->is_dir = S_ISDIR(st.st_mode);
        entry->size = st.st_size;
        entry->mtime = st.st_mtime;
    } else {
        entry->is_dir = 0;
        entry->size = 0;
        entry->mtime = 0;
    }

    return 0;
}

int fs_dir_list(const char* path, fs_dir_entry_t** entries, int* count) {
    if (!path || !entries || !count) {
        return -1;
    }

    *entries = NULL;
    *count = 0;

    fs_dir_t* dir = fs_dir_open(path);
    if (!dir) {
        return -1;
    }

    fs_dir_entry_t* list = NULL;
    int capacity = 0;
    int idx = 0;

    fs_dir_entry_t entry;
    while (fs_dir_read(dir, &entry) == 0) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        if (idx >= capacity) {
            capacity = capacity == 0 ? 16 : capacity * 2;
            fs_dir_entry_t* new_list = (fs_dir_entry_t*)iot_realloc(list, capacity * sizeof(fs_dir_entry_t));
            if (!new_list) {
                iot_free(list);
                fs_dir_close(dir);
                return -1;
            }
            list = new_list;
        }

        list[idx] = entry;
        idx++;
    }

    fs_dir_close(dir);

    *entries = list;
    *count = idx;

    return 0;
}

void fs_dir_free_list(fs_dir_entry_t* entries) {
    if (entries) {
        iot_free(entries);
    }
}