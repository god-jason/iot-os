/**
 * @file zip.c
 * @brief ZIP归档文件解压实现
 *
 * 本文件实现ZIP格式归档文件的解压功能。
 * 支持ZIP标准的STORED（无压缩）和DEFLATED（DEFLATE压缩）两种格式，
 * 支持多文件压缩包和目录结构。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "zip.h"
#include "deflate.h"
#include "platform.h"
#include <string.h>

/* ZIP格式签名常量 */
#define ZIP_CENTRAL_DIR_SIG       0x02014B50    /**< 中央目录签名 */
#define ZIP_END_CENTRAL_DIR_SIG   0x06054B50    /**< 中央目录结束签名 */
#define ZIP_LOCAL_HEADER_SIG      0x04034B50    /**< 本地文件头签名 */

/* ZIP压缩方法定义 */
#define ZIP_STORED                0              /**< 无压缩（存储） */
#define ZIP_DEFLATED              8              /**< DEFLATE压缩 */

/**
 * @brief 从字节流读取16位小端整数
 * @param p 指向字节流的指针引用
 * @return 读取的16位整数
 *
 * 自动更新指针位置。
 */
static uint16_t zip_read_le16(const uint8_t **p) {
    uint16_t v = (*p)[0] | ((*p)[1] << 8);
    *p += 2;
    return v;
}

/**
 * @brief 从字节流读取32位小端整数
 * @param p 指向字节流的指针引用
 * @return 读取的32位整数
 *
 * 自动更新指针位置。
 */
static uint32_t zip_read_le32(const uint8_t **p) {
    uint32_t v = (*p)[0] | ((*p)[1] << 8) | ((*p)[2] << 16) | ((*p)[3] << 24);
    *p += 4;
    return v;
}

/**
 * @brief 递归创建目录
 * @param path 目录路径
 * @return 0成功，-1失败
 *
 * 根据路径逐级创建目录，确保父目录存在。
 */
static int zip_mkdir_recursive(const char *path) {
    char tmp[512];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/') tmp[len - 1] = '\0';
    
    /* 遍历路径，逐级创建目录 */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            iot_fs_mkdir(tmp, 0);
            *p = '/';
        }
    }
    return iot_fs_mkdir(tmp, 0);
}

/**
 * @brief 打开ZIP归档文件
 *
 * 打开指定的ZIP文件，读取中央目录，解析所有文件条目信息。
 * ZIP文件的中央目录位于文件末尾，包含所有文件的元数据。
 *
 * @param zip ZIP句柄指针
 * @param zip_path ZIP文件路径
 * @return ZIP_OK成功，其他值失败
 */
int zip_open(zip_t *zip, const char *zip_path) {
    if (!zip || !zip_path) {
        return ZIP_ERR_FORMAT;
    }
    
    memset(zip, 0, sizeof(zip_t));
    
    /* 打开ZIP文件 */
    iot_fs_file_t fd = iot_fs_open(zip_path, IOT_FS_RB);
    if (!fd) {
        return ZIP_ERR_FILE;
    }
    
    zip->fd = fd;
    
    /* 获取文件大小 */
    int32_t size = iot_fs_filesize(zip_path);
    if (size < 0) {
        iot_fs_close(fd);
        return ZIP_ERR_FILE;
    }
    zip->size = size;
    
    /* 读取文件末尾区域，查找中央目录结束标记 */
    uint8_t *buf = (uint8_t *)iot_malloc(46);
    if (!buf) {
        iot_fs_close(fd);
        return ZIP_ERR_MEM;
    }
    
    iot_fs_seek(fd, size - 46, IOT_FS_SEEK_SET);
    if (iot_fs_read(fd, buf, 46) < 22) {
        iot_free(buf);
        iot_fs_close(fd);
        return ZIP_ERR_FORMAT;
    }
    
    /* 搜索中央目录结束签名 */
    const uint8_t *p = buf + 22;
    int found = 0;
    for (int i = 0; i < 24; i++) {
        uint32_t sig = buf[i] | (buf[i+1] << 8) | (buf[i+2] << 16) | (buf[i+3] << 24);
        if (sig == ZIP_END_CENTRAL_DIR_SIG) {
            p = buf + i;
            found = 1;
            break;
        }
    }
    iot_free(buf);
    
    if (!found) {
        iot_fs_close(fd);
        return ZIP_ERR_FORMAT;
    }
    
    /* 解析中央目录结束记录 */
    uint32_t sig = zip_read_le32(&p);
    if (sig != ZIP_END_CENTRAL_DIR_SIG) {
        iot_fs_close(fd);
        return ZIP_ERR_FORMAT;
    }
    
    zip_read_le16(&p);  /* 磁盘编号 */
    zip_read_le16(&p);  /* 中央目录所在磁盘 */
    int entry_count = zip_read_le16(&p);      /* 中央目录条目数量 */
    zip_read_le16(&p);  /* 磁盘上条目总数 */
    uint32_t central_dir_size = zip_read_le32(&p);  /* 中央目录大小 */
    uint32_t central_dir_offset = zip_read_le32(&p); /* 中央目录偏移 */
    
    /* 验证中央目录位置的有效性 */
    if (central_dir_offset + central_dir_size > (size_t)zip->size) {
        iot_fs_close(fd);
        return ZIP_ERR_FORMAT;
    }
    
    /* 分配条目数组 */
    zip->entries = (zip_entry_t *)iot_malloc(entry_count * sizeof(zip_entry_t));
    if (!zip->entries) {
        iot_fs_close(fd);
        return ZIP_ERR_MEM;
    }
    zip->entry_count = entry_count;
    
    /* 读取中央目录 */
    buf = (uint8_t *)iot_malloc(central_dir_size);
    if (!buf) {
        iot_fs_close(fd);
        zip_close(zip);
        return ZIP_ERR_MEM;
    }
    
    iot_fs_seek(fd, central_dir_offset, IOT_FS_SEEK_SET);
    if (iot_fs_read(fd, buf, central_dir_size) != (int32_t)central_dir_size) {
        iot_free(buf);
        iot_fs_close(fd);
        zip_close(zip);
        return ZIP_ERR_FILE;
    }
    
    /* 解析每个中央目录条目 */
    p = buf;
    for (int i = 0; i < entry_count; i++) {
        sig = zip_read_le32(&p);
        if (sig != ZIP_CENTRAL_DIR_SIG) {
            iot_free(buf);
            iot_fs_close(fd);
            zip_close(zip);
            return ZIP_ERR_FORMAT;
        }
        
        zip_read_le16(&p);  /* 版本创建 */
        zip_read_le16(&p);  /* 版本解压 */
        zip_read_le16(&p);  /* 标志 */
        uint16_t compression_method = zip_read_le16(&p); /* 压缩方法 */
        zip_read_le16(&p);  /* 文件时间 */
        zip_read_le16(&p);  /* 文件日期 */
        zip_read_le32(&p);  /* CRC32 */
        uint32_t crc32 = zip_read_le32(&p);
        uint32_t compressed_size = zip_read_le32(&p);   /* 压缩后大小 */
        uint32_t uncompressed_size = zip_read_le32(&p); /* 原始大小 */
        uint16_t filename_len = zip_read_le16(&p);    /* 文件名长度 */
        uint16_t extra_len = zip_read_le16(&p);       /* 扩展字段长度 */
        uint16_t comment_len = zip_read_le16(&p);      /* 注释长度 */
        zip_read_le16(&p);  /* 磁盘起始 */
        zip_read_le16(&p);  /* 内部属性 */
        zip_read_le32(&p);  /* 外部属性 */
        uint32_t local_offset = zip_read_le32(&p);     /* 本地文件头偏移 */
        
        /* 分配文件名内存并复制 */
        zip->entries[i].filename = (char *)iot_malloc(filename_len + 1);
        if (!zip->entries[i].filename) {
            iot_free(buf);
            iot_fs_close(fd);
            zip_close(zip);
            return ZIP_ERR_MEM;
        }
        memcpy(zip->entries[i].filename, p, filename_len);
        zip->entries[i].filename[filename_len] = '\0';
        
        /* 保存条目信息 */
        zip->entries[i].compression_method = compression_method;
        zip->entries[i].crc32 = crc32;
        zip->entries[i].compressed_size = compressed_size;
        zip->entries[i].uncompressed_size = uncompressed_size;
        zip->entries[i].offset = local_offset;
        
        /* 移动到下一个条目 */
        p += filename_len + extra_len + comment_len;
    }
    
    iot_free(buf);
    return ZIP_OK;
}

/**
 * @brief 关闭ZIP归档文件
 *
 * 释放所有分配的内存和文件句柄。
 *
 * @param zip ZIP句柄指针
 */
void zip_close(zip_t *zip) {
    if (!zip) return;
    
    /* 释放所有条目文件名内存 */
    if (zip->entries) {
        for (int i = 0; i < zip->entry_count; i++) {
            if (zip->entries[i].filename) {
                iot_free(zip->entries[i].filename);
            }
        }
        iot_free(zip->entries);
    }
    
    /* 关闭文件句柄 */
    if (zip->fd) {
        iot_fs_close((iot_fs_file_t)zip->fd);
    }
    
    memset(zip, 0, sizeof(zip_t));
}

/**
 * @brief 获取ZIP文件中的条目数量
 * @param zip ZIP句柄指针
 * @return 条目数量，失败返回-1
 */
int zip_get_entry_count(zip_t *zip) {
    if (!zip) return -1;
    return zip->entry_count;
}

/**
 * @brief 获取指定索引的条目信息
 * @param zip ZIP句柄指针
 * @param index 条目索引（从0开始）
 * @param entry 输出参数，存储条目信息
 * @return ZIP_OK成功，其他值失败
 */
int zip_get_entry(zip_t *zip, int index, zip_entry_t *entry) {
    if (!zip || !entry || index < 0 || index >= zip->entry_count) {
        return ZIP_ERR_NOT_FOUND;
    }
    *entry = zip->entries[index];
    return ZIP_OK;
}

/**
 * @brief 解压指定条目到内存缓冲区
 *
 * 读取ZIP文件中指定条目的压缩数据，进行解压（如果需要），
 * 然后复制到用户提供的内存缓冲区。
 *
 * @param zip ZIP句柄指针
 * @param index 条目索引
 * @param buf 目标内存缓冲区
 * @param buf_size 缓冲区大小
 * @return ZIP_OK成功，其他值失败
 */
int zip_extract_entry_to_memory(zip_t *zip, int index, uint8_t *buf, size_t buf_size) {
    if (!zip || !buf || index < 0 || index >= zip->entry_count) {
        return ZIP_ERR_NOT_FOUND;
    }
    
    zip_entry_t *entry = &zip->entries[index];
    if (entry->uncompressed_size > buf_size) {
        return ZIP_ERR_MEM;
    }
    
    /* 读取本地文件头 */
    uint8_t header[30];
    iot_fs_file_t fd = (iot_fs_file_t)zip->fd;
    iot_fs_seek(fd, entry->offset, IOT_FS_SEEK_SET);
    if (iot_fs_read(fd, header, 30) != 30) {
        return ZIP_ERR_FILE;
    }
    
    /* 解析本地文件头 */
    const uint8_t *p = header;
    uint32_t sig = zip_read_le32(&p);
    if (sig != ZIP_LOCAL_HEADER_SIG) {
        return ZIP_ERR_FORMAT;
    }
    
    zip_read_le16(&p);  /* 版本 */
    zip_read_le16(&p);  /* 标志 */
    uint16_t compression_method = zip_read_le16(&p); /* 压缩方法 */
    zip_read_le16(&p);  /* 文件时间 */
    zip_read_le16(&p);  /* 文件日期 */
    zip_read_le32(&p);  /* CRC32 */
    uint32_t crc32 = zip_read_le32(&p);
    uint32_t compressed_size = zip_read_le32(&p);
    uint32_t uncompressed_size = zip_read_le32(&p);
    uint16_t filename_len = zip_read_le16(&p);
    uint16_t extra_len = zip_read_le16(&p);
    
    /* 计算数据偏移 */
    size_t data_offset = entry->offset + 30 + filename_len + extra_len;
    
    /* 根据压缩方法处理 */
    if (compression_method == ZIP_STORED) {
        /* 无压缩：直接复制 */
        iot_fs_seek(fd, data_offset, IOT_FS_SEEK_SET);
        if (iot_fs_read(fd, buf, uncompressed_size) != (int32_t)uncompressed_size) {
            return ZIP_ERR_FILE;
        }
    } else if (compression_method == ZIP_DEFLATED) {
        /* DEFLATE压缩：先读取压缩数据，再解压 */
        uint8_t *compressed = (uint8_t *)iot_malloc(compressed_size);
        if (!compressed) {
            return ZIP_ERR_MEM;
        }
        
        iot_fs_seek(fd, data_offset, IOT_FS_SEEK_SET);
        if (iot_fs_read(fd, compressed, compressed_size) != (int32_t)compressed_size) {
            iot_free(compressed);
            return ZIP_ERR_FILE;
        }
        
        if (zlib_deflate_decompress(compressed, compressed_size, buf, uncompressed_size) < 0) {
            iot_free(compressed);
            return ZIP_ERR_FORMAT;
        }
        iot_free(compressed);
    } else {
        return ZIP_ERR_FORMAT;
    }
    
    /* 验证CRC32 */
    uint32_t computed_crc = zlib_crc32(0xffffffff, buf, uncompressed_size) ^ 0xffffffff;
    if (computed_crc != crc32) {
        return ZIP_ERR_CRC;
    }
    
    return ZIP_OK;
}

/**
 * @brief 解压指定条目到文件
 *
 * 将ZIP中的指定文件解压到指定的输出路径。
 * 如果文件名以'/'结尾，则创建目录。
 * 会自动创建目标文件的父目录。
 *
 * @param zip ZIP句柄指针
 * @param index 条目索引
 * @param output_path 输出文件路径
 * @return ZIP_OK成功，其他值失败
 */
int zip_extract_entry_to_file(zip_t *zip, int index, const char *output_path) {
    if (!zip || !output_path || index < 0 || index >= zip->entry_count) {
        return ZIP_ERR_NOT_FOUND;
    }
    
    zip_entry_t *entry = &zip->entries[index];
    const char *name = entry->filename;
    size_t name_len = strlen(name);
    
    /* 目录条目：直接创建目录 */
    if (name_len > 0 && name[name_len - 1] == '/') {
        return zip_mkdir_recursive(output_path);
    }
    
    /* 创建父目录 */
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s", output_path);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        zip_mkdir_recursive(dir_path);
    }
    
    /* 分配内存并解压到内存 */
    uint8_t *buf = (uint8_t *)iot_malloc(entry->uncompressed_size);
    if (!buf) {
        return ZIP_ERR_MEM;
    }
    
    int ret = zip_extract_entry_to_memory(zip, index, buf, entry->uncompressed_size);
    if (ret != ZIP_OK) {
        iot_free(buf);
        return ret;
    }
    
    /* 写入目标文件 */
    iot_fs_file_t fd = iot_fs_open(output_path, IOT_FS_WB);
    if (!fd) {
        iot_free(buf);
        return ZIP_ERR_FILE;
    }
    
    int32_t written = iot_fs_write(fd, buf, entry->uncompressed_size);
    iot_fs_close(fd);
    iot_free(buf);
    
    if (written != (int32_t)entry->uncompressed_size) {
        return ZIP_ERR_FILE;
    }
    
    return ZIP_OK;
}

/**
 * @brief 解压ZIP文件中的所有条目
 *
 * 将ZIP中的所有文件解压到指定目录，保持ZIP中的目录结构。
 *
 * @param zip ZIP句柄指针
 * @param base_path 解压基础目录
 * @return ZIP_OK成功，其他值失败
 */
int zip_extract_all(zip_t *zip, const char *base_path) {
    if (!zip || !base_path) {
        return ZIP_ERR_FORMAT;
    }
    
    for (int i = 0; i < zip->entry_count; i++) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, zip->entries[i].filename);
        
        int ret = zip_extract_entry_to_file(zip, i, full_path);
        if (ret != ZIP_OK) {
            return ret;
        }
    }
    
    return ZIP_OK;
}

/**
 * @brief 直接解压ZIP文件到目录（便捷接口）
 *
 * 简化的解压接口，内部自动处理打开、解压、关闭流程。
 *
 * @param zip_path ZIP文件路径
 * @param dest_dir 目标解压目录
 * @return ZIP_OK成功，其他值失败
 */
int zip_decompress_file(const char *zip_path, const char *dest_dir) {
    if (!zip_path || !dest_dir) {
        return ZIP_ERR_FORMAT;
    }
    
    zip_t zip;
    int ret = zip_open(&zip, zip_path);
    if (ret != ZIP_OK) {
        return ret;
    }
    
    ret = zip_extract_all(&zip, dest_dir);
    zip_close(&zip);
    
    return ret;
}

/**
 * @brief 创建ZIP文件（便捷接口）
 *
 * 简化的压缩接口，直接将多个文件打包成ZIP文件。
 *
 * @param zip_path 目标ZIP文件路径
 * @param files 要添加的文件路径数组
 * @param file_count 文件数量
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 */
int zip_compress_file(const char *zip_path, const char **files, int file_count, int level) {
    if (!zip_path || !files || file_count <= 0) {
        return ZIP_ERR_FORMAT;
    }
    
    zip_t zip;
    int ret = zip_create(&zip, zip_path);
    if (ret != ZIP_OK) {
        return ret;
    }
    
    for (int i = 0; i < file_count; i++) {
        const char *file_path = files[i];
        if (!file_path) continue;
        
        /* 使用文件路径的basename作为条目名称 */
        const char *entry_name = strrchr(file_path, '/');
        if (entry_name) {
            entry_name++;
        } else {
            entry_name = file_path;
        }
        
        ret = zip_add_file(&zip, file_path, entry_name, level);
        if (ret != ZIP_OK) {
            zip_close(&zip);
            return ret;
        }
    }
    
    ret = zip_finish(&zip);
    return ret;
}

/* ==================== ZIP 创建相关函数 ==================== */

/**
 * @brief 写入16位小端整数到缓冲区
 * @param p 指向缓冲区的指针引用
 * @param v 要写入的值
 */
static void zip_write_le16(uint8_t **p, uint16_t v) {
    (*p)[0] = v & 0xff;
    (*p)[1] = (v >> 8) & 0xff;
    *p += 2;
}

/**
 * @brief 写入32位小端整数到缓冲区
 * @param p 指向缓冲区的指针引用
 * @param v 要写入的值
 */
static void zip_write_le32(uint8_t **p, uint32_t v) {
    (*p)[0] = v & 0xff;
    (*p)[1] = (v >> 8) & 0xff;
    (*p)[2] = (v >> 16) & 0xff;
    (*p)[3] = (v >> 24) & 0xff;
    *p += 4;
}

/**
 * @brief 创建ZIP文件
 *
 * 创建一个新的ZIP文件，准备添加条目。
 *
 * @param zip ZIP句柄指针
 * @param zip_path 目标ZIP文件路径
 * @return ZIP_OK成功，其他值失败
 */
int zip_create(zip_t *zip, const char *zip_path) {
    if (!zip || !zip_path) {
        return ZIP_ERR_FORMAT;
    }
    
    memset(zip, 0, sizeof(zip_t));
    
    /* 打开输出文件 */
    iot_fs_file_t fd = iot_fs_open(zip_path, IOT_FS_WB);
    if (!fd) {
        return ZIP_ERR_FILE;
    }
    
    zip->fd = fd;
    return ZIP_OK;
}

/**
 * @brief 向ZIP文件添加内存数据
 *
 * 将内存中的数据添加到ZIP文件，可以选择压缩级别。
 *
 * @param zip ZIP句柄指针
 * @param data 数据缓冲区
 * @param data_len 数据长度
 * @param entry_name 在ZIP中的条目名称（含路径）
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 */
int zip_add_memory(zip_t *zip, const uint8_t *data, size_t data_len, const char *entry_name, int level) {
    if (!zip || !data || !entry_name || !zip->fd) {
        return ZIP_ERR_FORMAT;
    }
    
    size_t filename_len = strlen(entry_name);
    if (filename_len == 0) {
        return ZIP_ERR_FORMAT;
    }
    
    /* 限制压缩级别 */
    if (level < 0) level = 0;
    if (level > 9) level = 9;
    
    uint16_t compression_method = (level == 0) ? ZIP_STORED : ZIP_DEFLATED;
    
    /* 计算CRC32 */
    uint32_t crc32 = zlib_crc32(0xffffffff, data, data_len) ^ 0xffffffff;
    
    /* 压缩数据 */
    size_t compressed_size = data_len;
    uint8_t *compressed_data = NULL;
    
    if (compression_method == ZIP_DEFLATED && data_len > 0) {
        compressed_size = zlib_deflate_bound(data_len);
        compressed_data = (uint8_t *)iot_malloc(compressed_size);
        if (!compressed_data) {
            return ZIP_ERR_MEM;
        }
        
        int ret = zlib_deflate_compress(data, data_len, compressed_data, &compressed_size, level);
        if (ret != ZLIB_DEFLATE_OK) {
            iot_free(compressed_data);
            return ZIP_ERR_MEM;
        }
    } else {
        compressed_data = (uint8_t *)data;
        compressed_size = data_len;
    }
    
    /* 计算本地文件头大小 */
    size_t header_size = 30 + filename_len;
    uint8_t *header = (uint8_t *)iot_malloc(header_size);
    if (!header) {
        if (compression_method == ZIP_DEFLATED) {
            iot_free(compressed_data);
        }
        return ZIP_ERR_MEM;
    }
    
    /* 构建本地文件头 */
    uint8_t *p = header;
    zip_write_le32(&p, ZIP_LOCAL_HEADER_SIG);
    zip_write_le16(&p, 0x0014);      /* 版本 */
    zip_write_le16(&p, 0x0000);      /* 标志 */
    zip_write_le16(&p, compression_method);
    zip_write_le16(&p, 0x0000);      /* 文件时间（暂设为0） */
    zip_write_le16(&p, 0x0000);      /* 文件日期（暂设为0） */
    zip_write_le32(&p, crc32);
    zip_write_le32(&p, (uint32_t)compressed_size);
    zip_write_le32(&p, (uint32_t)data_len);
    zip_write_le16(&p, (uint16_t)filename_len);
    zip_write_le16(&p, 0x0000);      /* 扩展字段长度 */
    memcpy(p, entry_name, filename_len);
    
    /* 记录当前写入位置（用于中央目录） */
    size_t local_header_offset = zip->size;
    
    /* 写入本地文件头 */
    iot_fs_file_t fd = (iot_fs_file_t)zip->fd;
    if (iot_fs_write(fd, header, header_size) != (int32_t)header_size) {
        iot_free(header);
        if (compression_method == ZIP_DEFLATED) {
            iot_free(compressed_data);
        }
        return ZIP_ERR_FILE;
    }
    zip->size += header_size;
    iot_free(header);
    
    /* 写入压缩数据 */
    if (iot_fs_write(fd, compressed_data, compressed_size) != (int32_t)compressed_size) {
        if (compression_method == ZIP_DEFLATED) {
            iot_free(compressed_data);
        }
        return ZIP_ERR_FILE;
    }
    zip->size += compressed_size;
    
    /* 释放压缩数据（如果是动态分配的） */
    if (compression_method == ZIP_DEFLATED) {
        iot_free(compressed_data);
    }
    
    /* 保存条目信息到内存（用于中央目录） */
    zip_entry_t *new_entries = (zip_entry_t *)iot_realloc(zip->entries, (zip->entry_count + 1) * sizeof(zip_entry_t));
    if (!new_entries) {
        return ZIP_ERR_MEM;
    }
    zip->entries = new_entries;
    
    zip->entries[zip->entry_count].filename = (char *)iot_malloc(filename_len + 1);
    if (!zip->entries[zip->entry_count].filename) {
        return ZIP_ERR_MEM;
    }
    memcpy(zip->entries[zip->entry_count].filename, entry_name, filename_len);
    zip->entries[zip->entry_count].filename[filename_len] = '\0';
    zip->entries[zip->entry_count].compressed_size = (uint32_t)compressed_size;
    zip->entries[zip->entry_count].uncompressed_size = (uint32_t)data_len;
    zip->entries[zip->entry_count].offset = (uint32_t)local_header_offset;
    zip->entries[zip->entry_count].compression_method = compression_method;
    zip->entries[zip->entry_count].crc32 = crc32;
    
    zip->entry_count++;
    
    return ZIP_OK;
}

/**
 * @brief 向ZIP文件添加文件
 *
 * 将指定文件添加到ZIP文件，可以选择压缩级别。
 *
 * @param zip ZIP句柄指针
 * @param file_path 要添加的文件路径
 * @param entry_name 在ZIP中的条目名称（含路径）
 * @param level 压缩级别（0-9，0=存储，1-9=DEFLATE压缩级别）
 * @return ZIP_OK成功，其他值失败
 */
int zip_add_file(zip_t *zip, const char *file_path, const char *entry_name, int level) {
    if (!zip || !file_path || !entry_name || !zip->fd) {
        return ZIP_ERR_FORMAT;
    }
    
    /* 打开源文件 */
    iot_fs_file_t src_fd = iot_fs_open(file_path, IOT_FS_RB);
    if (!src_fd) {
        return ZIP_ERR_FILE;
    }
    
    /* 获取文件大小 */
    int32_t file_size = iot_fs_filesize(file_path);
    if (file_size < 0) {
        iot_fs_close(src_fd);
        return ZIP_ERR_FILE;
    }
    
    /* 读取文件内容 */
    uint8_t *data = (uint8_t *)iot_malloc(file_size);
    if (!data) {
        iot_fs_close(src_fd);
        return ZIP_ERR_MEM;
    }
    
    if (iot_fs_read(src_fd, data, file_size) != file_size) {
        iot_free(data);
        iot_fs_close(src_fd);
        return ZIP_ERR_FILE;
    }
    iot_fs_close(src_fd);
    
    /* 添加到ZIP */
    int ret = zip_add_memory(zip, data, file_size, entry_name, level);
    iot_free(data);
    
    return ret;
}

/**
 * @brief 完成ZIP文件创建
 *
 * 写入中央目录和结束记录，完成ZIP文件的创建。
 *
 * @param zip ZIP句柄指针
 * @return ZIP_OK成功，其他值失败
 */
int zip_finish(zip_t *zip) {
    if (!zip || !zip->fd) {
        return ZIP_ERR_FORMAT;
    }
    
    iot_fs_file_t fd = (iot_fs_file_t)zip->fd;
    size_t central_dir_offset = zip->size;
    
    /* 写入中央目录条目 */
    for (int i = 0; i < zip->entry_count; i++) {
        zip_entry_t *entry = &zip->entries[i];
        size_t filename_len = strlen(entry->filename);
        size_t entry_size = 46 + filename_len;
        
        uint8_t *buf = (uint8_t *)iot_malloc(entry_size);
        if (!buf) {
            return ZIP_ERR_MEM;
        }
        
        uint8_t *p = buf;
        zip_write_le32(&p, ZIP_CENTRAL_DIR_SIG);
        zip_write_le16(&p, 0x0014);      /* 版本创建 */
        zip_write_le16(&p, 0x0014);      /* 版本解压 */
        zip_write_le16(&p, 0x0000);      /* 标志 */
        zip_write_le16(&p, entry->compression_method);
        zip_write_le16(&p, 0x0000);      /* 文件时间 */
        zip_write_le16(&p, 0x0000);      /* 文件日期 */
        zip_write_le32(&p, entry->crc32);
        zip_write_le32(&p, entry->compressed_size);
        zip_write_le32(&p, entry->uncompressed_size);
        zip_write_le16(&p, (uint16_t)filename_len);
        zip_write_le16(&p, 0x0000);      /* 扩展字段长度 */
        zip_write_le16(&p, 0x0000);      /* 注释长度 */
        zip_write_le16(&p, 0x0000);      /* 磁盘起始 */
        zip_write_le16(&p, 0x0000);      /* 内部属性 */
        zip_write_le32(&p, 0x00000000);  /* 外部属性 */
        zip_write_le32(&p, entry->offset);
        memcpy(p, entry->filename, filename_len);
        
        if (iot_fs_write(fd, buf, entry_size) != (int32_t)entry_size) {
            iot_free(buf);
            return ZIP_ERR_FILE;
        }
        zip->size += entry_size;
        iot_free(buf);
    }
    
    /* 写入中央目录结束记录 */
    size_t central_dir_size = zip->size - central_dir_offset;
    uint8_t end_record[22];
    uint8_t *p = end_record;
    zip_write_le32(&p, ZIP_END_CENTRAL_DIR_SIG);
    zip_write_le16(&p, 0x0000);          /* 磁盘编号 */
    zip_write_le16(&p, 0x0000);          /* 中央目录所在磁盘 */
    zip_write_le16(&p, (uint16_t)zip->entry_count);
    zip_write_le16(&p, (uint16_t)zip->entry_count);
    zip_write_le32(&p, (uint32_t)central_dir_size);
    zip_write_le32(&p, (uint32_t)central_dir_offset);
    zip_write_le16(&p, 0x0000);          /* 注释长度 */
    
    if (iot_fs_write(fd, end_record, 22) != 22) {
        return ZIP_ERR_FILE;
    }
    zip->size += 22;
    
    /* 关闭文件 */
    iot_fs_close(fd);
    zip->fd = NULL;
    
    return ZIP_OK;
}
