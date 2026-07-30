/**
 * @file fs_path.h
 * @brief 文件系统路径处理工具接口定义
 *
 * 提供跨平台的文件路径处理工具函数接口，支持路径的
 * basename 提取、dirname 提取、扩展名获取、路径拼接、
 * 绝对路径判断、路径规范化、路径分割、末尾分隔符清理等
 * 常用操作。支持 Unix (/) 和 Windows (\) 两种路径分隔符。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_FS_PATH_H
#define IOT_FS_PATH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* fs_path_basename(const char* path);
char* fs_path_dirname(const char* path, char* buf, size_t buf_len);
const char* fs_path_extname(const char* path);
char* fs_path_join(char* buf, size_t buf_len, const char* path1, const char* path2);
int fs_path_is_absolute(const char* path);
char* fs_path_normalize(char* buf, size_t buf_len, const char* path);
char* fs_path_stem(char* buf, size_t buf_len, const char* path);
int fs_path_split(const char* path, char* dir_buf, size_t dir_len,
                  char* base_buf, size_t base_len);
char* fs_path_clean_trailing_sep(char* path);

#ifdef __cplusplus
}
#endif

#endif /* IOT_FS_PATH_H */