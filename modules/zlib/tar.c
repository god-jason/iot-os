/**
 * @file tar.c
 * @brief TAR归档文件打包解包实现
 *
 * 本文件实现TAR和TAR.GZ格式的打包和解包功能。
 * 支持标准USTAR格式的TAR文件，以及使用GZIP压缩的TAR.GZ文件。
 * 可自动根据文件名后缀判断处理方式。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "tar.h"
#include "gzip.h"
#include "iot.h"
#include <string.h>
#include <time.h>

#define IOT_TAR_BLOCK_SIZE       512             /**< TAR块大小（512字节） */
#define IOT_TAR_MAGIC            "ustar\x00"      /**< USTAR格式魔术串 */

typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
} iot_tar_header_t;

/**
 * @brief 将八进制字符串转换为数字
 * @param str 输入的八进制字符串
 * @param len 字符串长度
 * @return 转换后的数值
 *
 * TAR格式使用八进制ASCII编码存储数字，本函数解析这种编码。
 */
static uint64_t tar_str_to_num(const char *str, int len) {
    uint64_t val = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '\0' || str[i] == ' ') break;
        val = val * 8 + (str[i] - '0');
    }
    return val;
}

/**
 * @brief 将数字转换为八进制字符串
 * @param str 输出缓冲区
 * @param val 要转换的数值
 * @param len 输出缓冲区长度
 *
 * 使用空格填充，TAR格式要求八进制数字以空格填充而非零。
 */
static void iot_tar_num_to_str(char *str, uint64_t val, int len) {
    memset(str, ' ', len);
    str[len - 1] = '\0';
    for (int i = len - 2; i >= 0 && val > 0; i--) {
        str[i] = '0' + (val & 7);
        val >>= 3;
    }
}

/**
 * @brief 计算TAR头部校验和
 * @param h TAR头部指针
 * @return 校验和值
 *
 * TAR头部校验和计算方法：对前512字节求和，
 * 但chksum字段位置填入8个空格（' '）后再计算。
 */
static uint32_t iot_tar_checksum(const iot_tar_header_t *h) {
    uint32_t sum = 0;
    const uint8_t *p = (const uint8_t *)h;
    for (int i = 0; i < IOT_TAR_BLOCK_SIZE; i++) {
        sum += p[i];
    }
    for (int i = 0; i < 8; i++) {
        sum -= h->chksum[i];
        sum += ' ';
    }
    return sum;
}

/**
 * @brief 递归创建目录
 * @param path 目录路径
 * @return 0成功，-1失败
 *
 * 逐级解析路径，为每级目录调用文件系统mkdir接口。
 */
static int iot_tar_mkdir_recursive(const char *path) {
    char tmp[512];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/') tmp[len - 1] = '\0';
    
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
 * @brief 检查路径是否有指定后缀
 * @param path 文件路径
 * @param suffix 要检查的后缀
 * @return 1有后缀，0没有后缀
 */
static int iot_tar_has_suffix(const char *path, const char *suffix) {
    size_t path_len = strlen(path);
    size_t suffix_len = strlen(suffix);
    if (path_len < suffix_len) return 0;
    return strcmp(path + path_len - suffix_len, suffix) == 0;
}

int iot_tar_open(iot_tar_t *tar, const char *tar_path) {
    if (!tar || !tar_path) {
        return IOT_TAR_ERR_FORMAT;
    }
    
    memset(tar, 0, sizeof(iot_tar_t));
    
    iot_fs_file_t fd = iot_fs_open(tar_path,IOT_FS_RBPLUS);
    if (!fd) {
        return IOT_TAR_ERR_FILE;
    }
    
    tar->fd = fd;
    
    iot_tar_header_t hdr;
    int entry_count = 0;
    
    while (1) {
        if (iot_fs_read(fd, &hdr, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
            break;
        }
        
        if (hdr.name[0] == '\0') {
            uint8_t zero_block[IOT_TAR_BLOCK_SIZE] = {0};
            if (iot_fs_read(fd, zero_block, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
                break;
            }
            break;
        }
        
        if (strncmp(hdr.magic, IOT_TAR_MAGIC, 6) != 0) {
            iot_fs_close(fd);
            return IOT_TAR_ERR_FORMAT;
        }
        
        entry_count++;
        uint64_t size = tar_str_to_num(hdr.size, 12);
        size = (size + IOT_TAR_BLOCK_SIZE - 1) & ~(IOT_TAR_BLOCK_SIZE - 1);
        iot_fs_seek(fd, size, IOT_FS_SEEK_CUR);
    }
    
    iot_fs_seek(fd, 0, IOT_FS_SEEK_SET);
    
    tar->entries = (iot_tar_entry_t *)iot_malloc(entry_count * sizeof(iot_tar_entry_t));
    if (!tar->entries) {
        iot_fs_close(fd);
        return IOT_TAR_ERR_MEM;
    }
    
    tar->entry_count = entry_count;
    
    for (int i = 0; i < entry_count; i++) {
        if (iot_fs_read(fd, &hdr, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
            iot_tar_close(tar);
            return IOT_TAR_ERR_FILE;
        }
        
        tar->entries[i].filename = (char *)iot_malloc(256);
        if (!tar->entries[i].filename) {
            iot_tar_close(tar);
            return IOT_TAR_ERR_MEM;
        }
        
        if (hdr.prefix[0] != '\0') {
            snprintf(tar->entries[i].filename, 256, "%s/%s", hdr.prefix, hdr.name);
        } else {
            snprintf(tar->entries[i].filename, 256, "%s", hdr.name);
        }
        
        tar->entries[i].size = tar_str_to_num(hdr.size, 12);
        tar->entries[i].mode = (uint32_t)tar_str_to_num(hdr.mode, 8);
        tar->entries[i].mtime = tar_str_to_num(hdr.mtime, 12);
        tar->entries[i].type = hdr.typeflag;
        
        uint64_t size = tar->entries[i].size;
        size = (size + IOT_TAR_BLOCK_SIZE - 1) & ~(IOT_TAR_BLOCK_SIZE - 1);
        iot_fs_seek(fd, size, IOT_FS_SEEK_CUR);
    }
    
    return IOT_TAR_OK;
}

/**
 * @brief 关闭TAR归档文件
 * @param tar TAR句柄指针
 *
 * 释放所有条目内存，关闭文件句柄。
 */
void iot_tar_close(iot_tar_t *tar) {
    if (!tar) return;
    
    if (tar->entries) {
        for (int i = 0; i < tar->entry_count; i++) {
            if (tar->entries[i].filename) {
                iot_free(tar->entries[i].filename);
            }
        }
        iot_free(tar->entries);
    }
    
    if (tar->fd) {
        iot_fs_close((iot_fs_file_t)tar->fd);
    }
    
    memset(tar, 0, sizeof(iot_tar_t));
}

/**
 * @brief 获取TAR归档中的条目数量
 * @param tar TAR句柄指针
 * @return 条目数量，失败返回-1
 */
int iot_tar_get_entry_count(iot_tar_t *tar) {
    if (!tar) return -1;
    return tar->entry_count;
}

/**
 * @brief 获取指定索引的条目信息
 * @param tar TAR句柄指针
 * @param index 条目索引
 * @param entry 输出条目信息
 * @return IOT_TAR_OK成功，其他值失败
 */
int iot_tar_get_entry(iot_tar_t *tar, int index, iot_tar_entry_t *entry) {
    if (!tar || !entry || index < 0 || index >= tar->entry_count) {
        return IOT_TAR_ERR_NOT_FOUND;
    }
    *entry = tar->entries[index];
    return IOT_TAR_OK;
}

/**
 * @brief 查找指定索引的条目头部和偏移
 * @param tar TAR句柄指针
 * @param index 要查找的条目索引
 * @param hdr 输出头部信息
 * @param offset 输出文件偏移
 * @return 0成功，-1失败
 *
 * 内部函数，用于定位指定条目的头部位置和数据偏移。
 */
static int iot_tar_find_entry(iot_tar_t *tar, int index, iot_tar_header_t *hdr, int64_t *offset) {
    iot_fs_file_t fd = (iot_fs_file_t)tar->fd;
    iot_fs_seek(fd, 0, IOT_FS_SEEK_SET);
    
    for (int i = 0; i <= index; i++) {
        if (iot_fs_read(fd, hdr, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
            return -1;
        }
        
        if (i == index) {
            *offset = iot_fs_seek(fd, 0, IOT_FS_SEEK_CUR);
            return 0;
        }
        
        uint64_t size = tar_str_to_num(hdr->size, 12);
        size = (size + IOT_TAR_BLOCK_SIZE - 1) & ~(IOT_TAR_BLOCK_SIZE - 1);
        iot_fs_seek(fd, size, IOT_FS_SEEK_CUR);
    }
    
    return -1;
}

/**
 * @brief 将指定条目解压到内存缓冲区
 * @param tar TAR句柄指针
 * @param index 条目索引
 * @param buf 目标内存缓冲区
 * @param buf_size 缓冲区大小
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 将TAR归档中指定索引的文件内容读取到内存缓冲区。
 */
int iot_tar_extract_entry_to_memory(iot_tar_t *tar, int index, uint8_t *buf, size_t buf_size) {
    if (!tar || !buf || index < 0 || index >= tar->entry_count) {
        return IOT_TAR_ERR_NOT_FOUND;
    }
    
    iot_tar_entry_t *entry = &tar->entries[index];
    if (entry->size > buf_size) {
        return IOT_TAR_ERR_MEM;
    }
    
    iot_tar_header_t hdr;
    int64_t offset;
    
    if (iot_tar_find_entry(tar, index, &hdr, &offset) != 0) {
        return IOT_TAR_ERR_FILE;
    }
    
    iot_fs_file_t fd = (iot_fs_file_t)tar->fd;
    iot_fs_seek(fd, offset, IOT_FS_SEEK_SET);
    
    if (iot_fs_read(fd, buf, entry->size) != (int32_t)entry->size) {
        return IOT_TAR_ERR_FILE;
    }
    
    return IOT_TAR_OK;
}

/**
 * @brief 将指定条目解压到文件
 * @param tar TAR句柄指针
 * @param index 条目索引
 * @param output_path 输出文件路径
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 自动创建父目录，处理目录类型条目（创建目录）。
 */
int iot_tar_extract_entry_to_file(iot_tar_t *tar, int index, const char *output_path) {
    if (!tar || !output_path || index < 0 || index >= tar->entry_count) {
        return IOT_TAR_ERR_NOT_FOUND;
    }
    
    iot_tar_entry_t *entry = &tar->entries[index];
    
    if (entry->type == '5' || entry->type == '4') {
        return iot_tar_mkdir_recursive(output_path);
    }
    
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s", output_path);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        iot_tar_mkdir_recursive(dir_path);
    }
    
    uint8_t *buf = (uint8_t *)iot_malloc(entry->size);
    if (!buf) {
        return IOT_TAR_ERR_MEM;
    }
    
    int ret = iot_tar_extract_entry_to_memory(tar, index, buf, entry->size);
    if (ret != IOT_TAR_OK) {
        iot_free(buf);
        return ret;
    }
    
    iot_fs_file_t fd = iot_fs_open(output_path, IOT_FS_WB);
    if (!fd) {
        iot_free(buf);
        return IOT_TAR_ERR_FILE;
    }
    
    int32_t written = iot_fs_write(fd, buf, entry->size);
    iot_fs_close(fd);
    iot_free(buf);
    
    if (written != (int32_t)entry->size) {
        return IOT_TAR_ERR_FILE;
    }
    
    return IOT_TAR_OK;
}

/**
 * @brief 解压所有TAR条目到指定目录
 * @param tar TAR句柄指针
 * @param base_path 目标目录路径
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 遍历TAR归档中的所有条目，解压到base_path目录下。
 */
int iot_tar_extract_all(iot_tar_t *tar, const char *base_path) {
    if (!tar || !base_path) {
        return IOT_TAR_ERR_FORMAT;
    }
    
    for (int i = 0; i < tar->entry_count; i++) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, tar->entries[i].filename);
        
        int ret = iot_tar_extract_entry_to_file(tar, i, full_path);
        if (ret != IOT_TAR_OK) {
            return ret;
        }
    }
    
    return IOT_TAR_OK;
}

/**
 * @brief 创建TAR归档文件
 * @param tar_path 目标TAR文件路径
 * @param files 文件路径数组
 * @param file_count 文件数量
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 将多个文件打包成标准USTAR格式的TAR文件。
 */
int iot_tar_create(const char *tar_path, const char **files, int file_count) {
    if (!tar_path || !files || file_count <= 0) {
        return IOT_TAR_ERR_FORMAT;
    }
    
    iot_fs_file_t fd = iot_fs_open(tar_path, IOT_FS_WB);
    if (!fd) {
        return IOT_TAR_ERR_FILE;
    }
    
    for (int i = 0; i < file_count; i++) {
        const char *file = files[i];
        const char *basename = strrchr(file, '/');
        basename = basename ? basename + 1 : file;
        
        iot_tar_header_t hdr;
        memset(&hdr, 0, sizeof(hdr));
        snprintf(hdr.name, sizeof(hdr.name), "%s", basename);
        
        iot_fs_file_t src_fd = iot_fs_open(file, IOT_FS_RBPLUS);
        if (!src_fd) {
            iot_fs_close(fd);
            return IOT_TAR_ERR_FILE;
        }
        
        int32_t file_size = iot_fs_filesize(file);
        iot_tar_num_to_str(hdr.size, file_size, 12);
        
        iot_tar_num_to_str(hdr.mode, 0644, 8);
        iot_tar_num_to_str(hdr.uid, 0, 8);
        iot_tar_num_to_str(hdr.gid, 0, 8);
        
        time_t now = time(NULL);
        iot_tar_num_to_str(hdr.mtime, (uint64_t)now, 12);
        
        hdr.typeflag = '0';
        snprintf(hdr.magic, sizeof(hdr.magic), "%s", IOT_TAR_MAGIC);
        snprintf(hdr.version, sizeof(hdr.version), "00");
        
        uint32_t chksum = iot_tar_checksum(&hdr);
        iot_tar_num_to_str(hdr.chksum, chksum, 8);
        
        if (iot_fs_write(fd, &hdr, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
            iot_fs_close(src_fd);
            iot_fs_close(fd);
            return IOT_TAR_ERR_FILE;
        }
        
        uint8_t buf[IOT_TAR_BLOCK_SIZE];
        int32_t bytes_read;
        while ((bytes_read = iot_fs_read(src_fd, buf, IOT_TAR_BLOCK_SIZE)) > 0) {
            if (bytes_read < IOT_TAR_BLOCK_SIZE) {
                memset(buf + bytes_read, 0, IOT_TAR_BLOCK_SIZE - bytes_read);
            }
            if (iot_fs_write(fd, buf, IOT_TAR_BLOCK_SIZE) != IOT_TAR_BLOCK_SIZE) {
                iot_fs_close(src_fd);
                iot_fs_close(fd);
                return IOT_TAR_ERR_FILE;
            }
        }
        iot_fs_close(src_fd);
    }
    
    uint8_t zero_block[IOT_TAR_BLOCK_SIZE] = {0};
    iot_fs_write(fd, zero_block, IOT_TAR_BLOCK_SIZE);
    iot_fs_write(fd, zero_block, IOT_TAR_BLOCK_SIZE);
    
    iot_fs_close(fd);
    return IOT_TAR_OK;
}

/**
 * @brief 解压TAR或TAR.GZ文件
 * @param src_path 源文件路径（.tar或.tar.gz）
 * @param dst_dir 目标解压目录
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 根据文件扩展名自动识别是TAR还是TAR.GZ格式。
 * 支持的后缀：.tar、.tar.gz、.tgz
 */
int iot_tar_decompress_file(const char *src_path, const char *dst_dir) {
    if (!src_path || !dst_dir) {
        return IOT_TAR_ERR_FORMAT;
    }
    
    if (iot_tar_has_suffix(src_path, ".tar.gz") || iot_tar_has_suffix(src_path, ".tgz")) {
        iot_fs_file_t src_fd = iot_fs_open(src_path, IOT_FS_RBPLUS);
        if (!src_fd) {
            return IOT_TAR_ERR_FILE;
        }
        
        int32_t file_size = iot_fs_filesize(src_path);
        if (file_size <= 0) {
            iot_fs_close(src_fd);
            return IOT_TAR_ERR_FILE;
        }
        
        uint8_t *gz_buf = (uint8_t *)iot_malloc(file_size);
        if (!gz_buf) {
            iot_fs_close(src_fd);
            return IOT_TAR_ERR_MEM;
        }
        
        if (iot_fs_read(src_fd, gz_buf, file_size) != file_size) {
            iot_free(gz_buf);
            iot_fs_close(src_fd);
            return IOT_TAR_ERR_FILE;
        }
        iot_fs_close(src_fd);
        
        size_t tar_size = file_size * 4;
        uint8_t *tar_buf = (uint8_t *)iot_malloc(tar_size);
        if (!tar_buf) {
            iot_free(gz_buf);
            return IOT_TAR_ERR_MEM;
        }
        
        int ret = iot_gzip_decompress(gz_buf, file_size, tar_buf, &tar_size);
        iot_free(gz_buf);
        if (ret != IOT_GZIP_OK) {
            iot_free(tar_buf);
            return IOT_TAR_ERR_FORMAT;
        }
        
        char tmp_path[512];
        snprintf(tmp_path, sizeof(tmp_path), "%s/.tmp.tar", dst_dir);
        
        iot_fs_file_t tmp_fd = iot_fs_open(tmp_path, IOT_FS_WB);
        if (!tmp_fd) {
            iot_free(tar_buf);
            return IOT_TAR_ERR_FILE;
        }
        
        if (iot_fs_write(tmp_fd, tar_buf, tar_size) != (int32_t)tar_size) {
            iot_fs_close(tmp_fd);
            iot_free(tar_buf);
            return IOT_TAR_ERR_FILE;
        }
        iot_fs_close(tmp_fd);
        iot_free(tar_buf);
        
        iot_tar_t tar;
        ret = iot_tar_open(&tar, tmp_path);
        if (ret != IOT_TAR_OK) {
            return ret;
        }
        
        ret = iot_tar_extract_all(&tar, dst_dir);
        iot_tar_close(&tar);

        return ret;
    } else if (iot_tar_has_suffix(src_path, ".tar")) {
        iot_tar_t tar;
        int ret = iot_tar_open(&tar, src_path);
        if (ret != IOT_TAR_OK) {
            return ret;
        }
        
        ret = iot_tar_extract_all(&tar, dst_dir);
        iot_tar_close(&tar);
        
        return ret;
    }
    
    return IOT_TAR_ERR_FORMAT;
}

/**
 * @brief 创建TAR或TAR.GZ压缩文件
 * @param src_dir 源文件所在目录
 * @param files 要压缩的文件名数组
 * @param file_count 文件数量
 * @param dst_path 目标压缩文件路径
 * @param level 压缩级别（1-9，仅对.tar.gz有效）
 * @return IOT_TAR_OK成功，其他值失败
 *
 * 根据目标文件扩展名自动选择创建.tar或.tar.gz格式。
 * 支持的后缀：.tar、.tar.gz、.tgz
 */
int iot_tar_compress_file(const char *src_dir, const char **files, int file_count, const char *dst_path, int level) {
    if (!src_dir || !files || !dst_path || file_count <= 0) {
        return IOT_TAR_ERR_FORMAT;
    }
    
    if (iot_tar_has_suffix(dst_path, ".tar.gz") || iot_tar_has_suffix(dst_path, ".tgz")) {
        char tmp_path[512];
        snprintf(tmp_path, sizeof(tmp_path), "%s/.tmp.tar", src_dir);
        
        char **full_paths = (char **)iot_malloc(file_count * sizeof(char *));
        if (!full_paths) {
            return IOT_TAR_ERR_MEM;
        }
        
        for (int i = 0; i < file_count; i++) {
            full_paths[i] = (char *)iot_malloc(512);
            if (!full_paths[i]) {
                for (int j = 0; j < i; j++) {
                    iot_free(full_paths[j]);
                }
                iot_free(full_paths);
                return IOT_TAR_ERR_MEM;
            }
            snprintf(full_paths[i], 512, "%s/%s", src_dir, files[i]);
        }
        
        int ret = iot_tar_create(tmp_path, (const char **)full_paths, file_count);
        
        for (int i = 0; i < file_count; i++) {
            iot_free(full_paths[i]);
        }
        iot_free(full_paths);
        
        if (ret != IOT_TAR_OK) {
            return ret;
        }
        
        iot_fs_file_t tar_fd = iot_fs_open(tmp_path, IOT_FS_RBPLUS);
        if (!tar_fd) {
            return IOT_TAR_ERR_FILE;
        }
        
        int32_t tar_size = iot_fs_filesize(tmp_path);
        if (tar_size <= 0) {
            iot_fs_close(tar_fd);
            return IOT_TAR_ERR_FILE;
        }
        
        uint8_t *tar_buf = (uint8_t *)iot_malloc(tar_size);
        if (!tar_buf) {
            iot_fs_close(tar_fd);
            return IOT_TAR_ERR_MEM;
        }
        
        if (iot_fs_read(tar_fd, tar_buf, tar_size) != tar_size) {
            iot_free(tar_buf);
            iot_fs_close(tar_fd);
            return IOT_TAR_ERR_FILE;
        }
        iot_fs_close(tar_fd);
        
        size_t gz_size = tar_size + (tar_size >> 12) + (tar_size >> 14) + (tar_size >> 25) + 21;
        uint8_t *gz_buf = (uint8_t *)iot_malloc(gz_size);
        if (!gz_buf) {
            iot_free(tar_buf);
            return IOT_TAR_ERR_MEM;
        }
        
        ret = iot_gzip_compress(tar_buf, tar_size, gz_buf, &gz_size, level);
        iot_free(tar_buf);
        if (ret != IOT_GZIP_OK) {
            iot_free(gz_buf);
            return ret;
        }
        
        iot_fs_file_t dst_fd = iot_fs_open(dst_path, IOT_FS_WB);
        if (!dst_fd) {
            iot_free(gz_buf);
            return IOT_TAR_ERR_FILE;
        }
        
        if (iot_fs_write(dst_fd, gz_buf, gz_size) != (int32_t)gz_size) {
            iot_fs_close(dst_fd);
            iot_free(gz_buf);
            return IOT_TAR_ERR_FILE;
        }
        
        iot_fs_close(dst_fd);
        iot_free(gz_buf);
        
        return IOT_TAR_OK;
    } else if (iot_tar_has_suffix(dst_path, ".tar")) {
        char **full_paths = (char **)iot_malloc(file_count * sizeof(char *));
        if (!full_paths) {
            return IOT_TAR_ERR_MEM;
        }
        
        for (int i = 0; i < file_count; i++) {
            full_paths[i] = (char *)iot_malloc(512);
            if (!full_paths[i]) {
                for (int j = 0; j < i; j++) {
                    iot_free(full_paths[j]);
                }
                iot_free(full_paths);
                return IOT_TAR_ERR_MEM;
            }
            snprintf(full_paths[i], 512, "%s/%s", src_dir, files[i]);
        }
        
        int ret = iot_tar_create(dst_path, (const char **)full_paths, file_count);
        
        for (int i = 0; i < file_count; i++) {
            iot_free(full_paths[i]);
        }
        iot_free(full_paths);
        
        return ret;
    }
    
    return IOT_TAR_ERR_FORMAT;
}
