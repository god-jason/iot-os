/**
 * @file tar.h
 * @brief TAR归档文件头文件
 *
 * 定义TAR和TAR.GZ格式的打包和解包接口。
 * 支持标准USTAR格式的TAR文件，以及使用GZIP压缩的TAR.GZ文件。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef TAR_H
#define TAR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TAR错误码定义 */
#define TAR_OK              0   /**< 成功 */
#define TAR_ERR_MEM        -1   /**< 内存分配失败 */
#define TAR_ERR_FORMAT     -2   /**< TAR格式错误 */
#define TAR_ERR_FILE       -3   /**< 文件操作失败 */
#define TAR_ERR_NOT_FOUND  -4   /**< 条目未找到 */

/* TAR压缩级别定义 */
#define TAR_COMPRESS_FAST    1   /**< 快速压缩 */
#define TAR_COMPRESS_DEFAULT 6   /**< 默认压缩 */
#define TAR_COMPRESS_BEST   9   /**< 最佳压缩 */

/**
 * @brief TAR文件条目信息结构体
 *
 * 存储TAR文件中单个文件/目录的信息。
 */
typedef struct tar_entry_t tar_entry_t;
struct tar_entry_t {
    char *filename;     /**< 文件名（含路径） */
    uint64_t size;      /**< 文件大小 */
    uint32_t mode;      /**< 文件权限模式（八进制） */
    uint64_t mtime;     /**< 修改时间（时间戳） */
    uint8_t type;       /**< 条目类型（'0'=文件，'5'=目录，'2'=符号链接） */
};

/**
 * @brief TAR归档文件句柄结构体
 *
 * 维护TAR文件的上下文信息。
 */
typedef struct tar_t tar_t;
struct tar_t {
    void *fd;           /**< 文件句柄 */
    tar_entry_t *entries;   /**< 条目数组 */
    int entry_count;    /**< 条目数量 */
};

/**
 * @brief 打开TAR归档文件
 * @param tar TAR句柄指针
 * @param tar_path TAR文件路径
 * @return TAR_OK成功，其他值失败
 *
 * 扫描TAR文件的所有条目，建立条目索引。
 */
int tar_open(tar_t *tar, const char *tar_path);

/**
 * @brief 关闭TAR归档文件
 * @param tar TAR句柄指针
 *
 * 释放所有分配的内存和文件句柄。
 */
void tar_close(tar_t *tar);

/**
 * @brief 获取TAR文件中的条目数量
 * @param tar TAR句柄指针
 * @return 条目数量，失败返回-1
 */
int tar_get_entry_count(tar_t *tar);

/**
 * @brief 获取指定索引的条目信息
 * @param tar TAR句柄指针
 * @param index 条目索引（从0开始）
 * @param entry 输出参数，存储条目信息
 * @return TAR_OK成功，其他值失败
 */
int tar_get_entry(tar_t *tar, int index, tar_entry_t *entry);

/**
 * @brief 解压指定条目到内存缓冲区
 * @param tar TAR句柄指针
 * @param index 条目索引
 * @param buf 目标内存缓冲区
 * @param buf_size 缓冲区大小
 * @return TAR_OK成功，其他值失败
 */
int tar_extract_entry_to_memory(tar_t *tar, int index, uint8_t *buf, size_t buf_size);

/**
 * @brief 解压指定条目到文件
 * @param tar TAR句柄指针
 * @param index 条目索引
 * @param output_path 输出文件路径
 * @return TAR_OK成功，其他值失败
 *
 * 会自动创建必要的目录结构。
 */
int tar_extract_entry_to_file(tar_t *tar, int index, const char *output_path);

/**
 * @brief 解压TAR文件中的所有条目
 * @param tar TAR句柄指针
 * @param base_path 解压基础目录
 * @return TAR_OK成功，其他值失败
 *
 * 将TAR中的所有文件解压到指定目录，保持目录结构。
 */
int tar_extract_all(tar_t *tar, const char *base_path);

/**
 * @brief 创建TAR归档文件
 * @param tar_path 目标TAR文件路径
 * @param files 文件路径数组
 * @param file_count 文件数量
 * @return TAR_OK成功，其他值失败
 *
 * 将多个文件打包成TAR格式。
 */
int tar_create(const char *tar_path, const char **files, int file_count);

/**
 * @brief 解压TAR或TAR.GZ文件
 * @param src_path 源文件路径（.tar或.tar.gz）
 * @param dst_dir 目标解压目录
 * @return TAR_OK成功，其他值失败
 *
 * 根据文件扩展名自动识别是TAR还是TAR.GZ格式。
 * 支持的后缀：.tar、.tar.gz、.tgz
 */
int tar_decompress_file(const char *src_path, const char *dst_dir);

/**
 * @brief 创建TAR或TAR.GZ归档文件
 * @param src_dir 源文件所在目录
 * @param files 要打包的文件名数组
 * @param file_count 文件数量
 * @param dst_path 目标文件路径
 * @param level 压缩级别（仅对.tar.gz有效）
 * @return TAR_OK成功，其他值失败
 *
 * 根据目标文件扩展名自动选择是否压缩。
 * 支持的后缀：.tar、.tar.gz、.tgz
 */
int tar_compress_file(const char *src_dir, const char **files, int file_count, const char *dst_path, int level);

#ifdef __cplusplus
}
#endif

#endif
