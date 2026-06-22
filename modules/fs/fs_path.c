/**
 * @file fs_path.c
 * @brief 文件系统路径处理工具
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "platform.h"

/*===========================================================
 * 路径处理工具函数
 *===========================================================*/

/**
 * @brief 获取路径中的文件名
 * @param path 文件路径
 * @return 文件名指针，失败返回 NULL
 * @note 返回值指向原路径内存，不可修改或释放
 */
const char* fs_path_basename(const char* path) {
    if (!path || path[0] == '\0') {
        return NULL;
    }

    const char* last_slash = strrchr(path, '/');
#ifdef _WIN32
    const char* last_backslash = strrchr(path, '\\');
    if (last_backslash && (!last_slash || last_backslash > last_slash)) {
        last_slash = last_backslash;
    }
#endif

    if (last_slash) {
        return last_slash + 1;
    }

    return path;
}

/**
 * @brief 获取路径中的目录名
 * @param path 文件路径
 * @param buf 存储结果的缓冲区
 * @param buf_len 缓冲区长度
 * @return 成功返回 buf，失败返回 NULL
 */
char* fs_path_dirname(const char* path, char* buf, size_t buf_len) {
    if (!path || !buf || buf_len == 0) {
        return NULL;
    }

    size_t len = strlen(path);
    if (len == 0) {
        buf[0] = '\0';
        return buf;
    }

    size_t pos = len - 1;
    while (pos > 0) {
        if (path[pos] == '/' || (path[pos] == '\\' && strchr(path, '\\'))) {
            pos--;
            break;
        }
        pos--;
    }

    while (pos > 0 && (path[pos] == '/' || (path[pos] == '\\' && strchr(path, '\\')))) {
        pos--;
    }

    if (pos == 0 && (path[0] == '/' || path[0] == '\\')) {
        pos = 1;
    }

    if (pos >= buf_len) {
        pos = buf_len - 1;
    }

    memcpy(buf, path, pos);
    buf[pos] = '\0';

    return buf;
}

/**
 * @brief 获取文件扩展名
 * @param path 文件路径
 * @return 扩展名指针（包含点号），失败返回 NULL
 * @note 返回值指向原路径内存，不可修改或释放
 */
const char* fs_path_extname(const char* path) {
    if (!path || path[0] == '\0') {
        return NULL;
    }

    const char* last_dot = strrchr(path, '.');
    if (!last_dot) {
        return NULL;
    }

    const char* last_slash = strrchr(path, '/');
#ifdef _WIN32
    const char* last_backslash = strrchr(path, '\\');
    if (last_backslash && (!last_slash || last_backslash > last_slash)) {
        last_slash = last_backslash;
    }
#endif

    if (last_slash && last_dot < last_slash) {
        return NULL;
    }

    return last_dot;
}

/**
 * @brief 拼接路径
 * @param buf 存储结果的缓冲区
 * @param buf_len 缓冲区长度
 * @param path1 第一个路径
 * @param path2 第二个路径
 * @return 成功返回 buf，失败返回 NULL
 */
char* fs_path_join(char* buf, size_t buf_len, const char* path1, const char* path2) {
    if (!buf || buf_len == 0 || !path1 || !path2) {
        return NULL;
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);

    if (len1 + len2 + 2 >= buf_len) {
        return NULL;
    }

    memcpy(buf, path1, len1);

    int need_sep = 0;
    if (len1 > 0 && path1[len1 - 1] != '/' && path1[len1 - 1] != '\\') {
        need_sep = 1;
    }

    if (len2 > 0 && (path2[0] == '/' || path2[0] == '\\')) {
        need_sep = 0;
        len2--;
        path2++;
    }

    if (need_sep) {
        buf[len1] = '/';
        len1++;
    }

    memcpy(buf + len1, path2, len2);
    buf[len1 + len2] = '\0';

    return buf;
}

/**
 * @brief 判断是否为绝对路径
 * @param path 文件路径
 * @return 是绝对路径返回 1，否则返回 0
 */
int fs_path_is_absolute(const char* path) {
    if (!path || path[0] == '\0') {
        return 0;
    }

    if (path[0] == '/') {
        return 1;
    }

#ifdef _WIN32
    if (path[0] == '\\') {
        return 1;
    }

    if (path[1] == ':') {
        return 1;
    }
#endif

    return 0;
}

/**
 * @brief 规范化路径
 * @param buf 存储结果的缓冲区
 * @param buf_len 缓冲区长度
 * @param path 原始路径
 * @return 成功返回 buf，失败返回 NULL
 */
char* fs_path_normalize(char* buf, size_t buf_len, const char* path) {
    if (!buf || buf_len == 0 || !path) {
        return NULL;
    }

    size_t len = strlen(path);
    if (len == 0) {
        buf[0] = '\0';
        return buf;
    }

    char* p = buf;
    char* end = buf + buf_len - 1;

    if (fs_path_is_absolute(path)) {
#ifdef _WIN32
        if (path[1] == ':') {
            *p++ = path[0];
            *p++ = path[1];
            *p++ = '/';
            path += 2;
            len -= 2;
            if (path[0] == '/' || path[0] == '\\') {
                path++;
                len--;
            }
        } else if (path[0] == '/' || path[0] == '\\') {
            *p++ = '/';
            path++;
            len--;
            if (path[0] == '/' || path[0] == '\\') {
                path++;
                len--;
            }
        }
#else
        *p++ = '/';
        path++;
        len--;
#endif
    }

    const char* start = path;
    while (len > 0 && p < end) {
        while (len > 0 && (*path == '/' || *path == '\\')) {
            path++;
            len--;
        }

        if (len == 0) break;

        const char* seg_start = path;
        size_t seg_len = 0;
        while (len > 0 && *path != '/' && *path != '\\') {
            path++;
            len++;
            seg_len++;
        }

        if (seg_len == 1 && seg_start[0] == '.') {
            continue;
        }

        if (seg_len == 2 && seg_start[0] == '.' && seg_start[1] == '.') {
            while (p > start && *(p - 1) != '/') {
                p--;
            }
            if (p > start) {
                p--;
            }
            continue;
        }

        if (p > start) {
            *p++ = '/';
        }

        size_t copy_len = (seg_len < (size_t)(end - p)) ? seg_len : (size_t)(end - p);
        memcpy(p, seg_start, copy_len);
        p += copy_len;
    }

    if (p == buf) {
        *p++ = '.';
    }

    *p = '\0';

    return buf;
}

/**
 * @brief 获取不含扩展名的文件名
 * @param buf 存储结果的缓冲区
 * @param buf_len 缓冲区长度
 * @param path 文件路径
 * @return 成功返回 buf，失败返回 NULL
 */
char* fs_path_stem(char* buf, size_t buf_len, const char* path) {
    if (!buf || buf_len == 0 || !path) {
        return NULL;
    }

    const char* base = fs_path_basename(path);
    if (!base) {
        buf[0] = '\0';
        return buf;
    }

    const char* ext = fs_path_extname(path);
    if (!ext) {
        strncpy(buf, base, buf_len - 1);
        buf[buf_len - 1] = '\0';
        return buf;
    }

    size_t stem_len = ext - base;
    if (stem_len >= buf_len) {
        stem_len = buf_len - 1;
    }

    memcpy(buf, base, stem_len);
    buf[stem_len] = '\0';

    return buf;
}

/**
 * @brief 分割路径为目录和文件名
 * @param path 文件路径
 * @param dir_buf 存储目录的缓冲区
 * @param dir_len 目录缓冲区长度
 * @param base_buf 存储文件名的缓冲区
 * @param base_len 文件名缓冲区长度
 * @return 成功返回 0，失败返回 -1
 */
int fs_path_split(const char* path, char* dir_buf, size_t dir_len, 
                  char* base_buf, size_t base_len) {
    if (!path) {
        return -1;
    }

    const char* base = fs_path_basename(path);
    if (base_buf) {
        strncpy(base_buf, base, base_len - 1);
        base_buf[base_len - 1] = '\0';
    }

    if (dir_buf) {
        return fs_path_dirname(path, dir_buf, dir_len) ? 0 : -1;
    }

    return 0;
}

/**
 * @brief 清理路径末尾的分隔符
 * @param path 文件路径（会被修改）
 * @return 返回处理后的路径
 */
char* fs_path_clean_trailing_sep(char* path) {
    if (!path || path[0] == '\0') {
        return path;
    }

    size_t len = strlen(path);
    while (len > 0) {
        if (path[len - 1] == '/' || path[len - 1] == '\\') {
            len--;
        } else {
            break;
        }
    }

    if (len == 0) {
        path[0] = '/';
        path[1] = '\0';
    } else {
        path[len] = '\0';
    }

    return path;
}