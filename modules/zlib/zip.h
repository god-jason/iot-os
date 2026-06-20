/**
 * @file zip.h
 * @brief ZIP归档文件头文件
 *
 * 定义ZIP格式归档文件的解压接口。
 * 支持多文件ZIP压缩包的解压，可直接解压到文件系统或内存缓冲区。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef ZIP_H
#define ZIP_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ZIP错误码定义 */
#define ZIP_OK              0   /**< 成功 */
#define ZIP_ERR_MEM        -1   /**< 内存分配失败 */
#define ZIP_ERR_FORMAT     -2   /**< ZIP格式错误 */
#define ZIP_ERR_CRC        -3   /**< CRC校验失败 */
#define ZIP_ERR_FILE       -4   /**< 文件操作失败 */
#define ZIP_ERR_NOT_FOUND  -5   /**< 条目未找到 */

/**
 * @brief ZIP文件条目信息结构体
 *
 * 存储ZIP文件中单个文件/目录的信息。
 */
typedef struct zip_entry_t zip_entry_t;
struct zip_entry_t {
    char *filename;             /**< 文件名（含路径） */
    uint32_t compressed_size;   /**< 压缩后大小 */
    uint32_t uncompressed_size; /**< 原始大小 */
    uint32_t offset;            /**< 在ZIP文件中的数据偏移 */
    uint16_t compression_method;/**< 压缩方法（0=存储，8=DEFLATE） */
    uint32_t crc32;             /**< CRC32校验码 */
};

/**
 * @brief ZIP归档文件句柄结构体
 *
 * 维护ZIP文件的上下文信息，包括文件句柄和条目列表。
 */
typedef struct zip_t zip_t;
struct zip_t {
    void *fd;                   /**< 文件句柄 */
    size_t size;                /**< ZIP文件总大小 */
    zip_entry_t *entries;       /**< 条目数组 */
    int entry_count;            /**< 条目数量 */
};

/**
 * @brief 打开ZIP归档文件
 * @param zip ZIP句柄指针
 * @param zip_path ZIP文件路径
 * @return ZIP_OK成功，其他值失败
 *
 * 读取ZIP文件的中央目录，解析所有文件条目信息。
 */
int zip_open(zip_t *zip, const char *zip_path);

/**
 * @brief 关闭ZIP归档文件
 * @param zip ZIP句柄指针
 *
 * 释放所有分配的内存和文件句柄。
 */
void zip_close(zip_t *zip);

/**
 * @brief 获取ZIP文件中的条目数量
 * @param zip ZIP句柄指针
 * @return 条目数量，失败返回-1
 */
int zip_get_entry_count(zip_t *zip);

/**
 * @brief 获取指定索引的条目信息
 * @param zip ZIP句柄指针
 * @param index 条目索引（从0开始）
 * @param entry 输出参数，存储条目信息
 * @return ZIP_OK成功，其他值失败
 */
int zip_get_entry(zip_t *zip, int index, zip_entry_t *entry);

/**
 * @brief 解压指定条目到内存缓冲区
 * @param zip ZIP句柄指针
 * @param index 条目索引
 * @param buf 目标内存缓冲区
 * @param buf_size 缓冲区大小
 * @return ZIP_OK成功，其他值失败
 */
int zip_extract_entry_to_memory(zip_t *zip, int index, uint8_t *buf, size_t buf_size);

/**
 * @brief 解压指定条目到文件
 * @param zip ZIP句柄指针
 * @param index 条目索引
 * @param output_path 输出文件路径
 * @return ZIP_OK成功，其他值失败
 *
 * 会自动创建必要的目录结构。
 */
int zip_extract_entry_to_file(zip_t *zip, int index, const char *output_path);

/**
 * @brief 解压ZIP文件中的所有条目
 * @param zip ZIP句柄指针
 * @param base_path 解压基础目录
 * @return ZIP_OK成功，其他值失败
 *
 * 将ZIP中的所有文件解压到指定目录，保持目录结构。
 */
int zip_extract_all(zip_t *zip, const char *base_path);

/**
 * @brief 直接解压ZIP文件到目录（便捷接口）
 * @param zip_path ZIP文件路径
 * @param dest_dir 目标解压目录
 * @return ZIP_OK成功，其他值失败
 *
 * 简化的解压接口，内部自动处理打开、解压、关闭流程。
 */
int zip_decompress_file(const char *zip_path, const char *dest_dir);

/**
 * @brief 创建ZIP文件（便捷接口）
 * @param zip_path 目标ZIP文件路径
 * @param files 要添加的文件路径数组
 * @param file_count 文件数量
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 *
 * 简化的压缩接口，直接将多个文件打包成ZIP文件。
 */
int zip_compress_file(const char *zip_path, const char **files, int file_count, int level);

/**
 * @brief 创建ZIP文件
 * @param zip ZIP句柄指针
 * @param zip_path 目标ZIP文件路径
 * @return ZIP_OK成功，其他值失败
 *
 * 创建一个新的ZIP文件，准备添加条目。
 */
int zip_create(zip_t *zip, const char *zip_path);

/**
 * @brief 向ZIP文件添加文件
 * @param zip ZIP句柄指针
 * @param file_path 要添加的文件路径
 * @param entry_name 在ZIP中的条目名称（含路径）
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 */
int zip_add_file(zip_t *zip, const char *file_path, const char *entry_name, int level);

/**
 * @brief 向ZIP文件添加内存数据
 * @param zip ZIP句柄指针
 * @param data 数据缓冲区
 * @param data_len 数据长度
 * @param entry_name 在ZIP中的条目名称（含路径）
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 */
int zip_add_memory(zip_t *zip, const uint8_t *data, size_t data_len, const char *entry_name, int level);

/**
 * @brief 完成ZIP文件创建
 * @param zip ZIP句柄指针
 * @return ZIP_OK成功，其他值失败
 *
 * 写入中央目录并关闭ZIP文件。
 */
int zip_finish(zip_t *zip);

#ifdef __cplusplus
}
#endif

#endif
