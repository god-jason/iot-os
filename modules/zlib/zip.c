/**
 * @file zip.c
 * @brief ZIP 归档（miniz 封装）
 */

#include "zip.h"
#include "miniz.h"
#include "iot.h"
#include "iot_log.h"
#include <string.h>
#include <stdio.h>

static int zip_mkdir_recursive(const char *path)
{
    char tmp[512];
    char *p = NULL;
    size_t len;

    if (!path) {
        return -1;
    }

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (len > 0 && (tmp[len - 1] == '/' || tmp[len - 1] == '\\')) {
        tmp[len - 1] = '\0';
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char saved = *p;
            *p = '\0';
            iot_fs_mkdir(tmp, 0);
            *p = saved;
        }
    }

    iot_fs_mkdir(tmp, 0);
    return 0;
}

static int zip_mkdir_parent(const char *file_path)
{
    char parent[512];
    char *slash;

    snprintf(parent, sizeof(parent), "%s", file_path);
    slash = strrchr(parent, '/');
    if (!slash) {
        slash = strrchr(parent, '\\');
    }
    if (!slash) {
        return 0;
    }
    *slash = '\0';
    return zip_mkdir_recursive(parent);
}

static int zip_read_file(const char *path, uint8_t **out, size_t *out_len)
{
    iot_fs_file_t fd;
    int32_t size;

    if (!path || !out || !out_len) {
        return -1;
    }

    fd = iot_fs_open(path, IOT_FS_RB);
    if (!fd) {
        return -1;
    }

    size = iot_fs_filesize(path);
    if (size <= 0) {
        iot_fs_close(fd);
        return -1;
    }

    *out = (uint8_t *)iot_malloc((size_t)size);
    if (!*out) {
        iot_fs_close(fd);
        return -1;
    }

    if (iot_fs_read(fd, *out, (size_t)size) != size) {
        iot_free(*out);
        *out = NULL;
        iot_fs_close(fd);
        return -1;
    }

    iot_fs_close(fd);
    *out_len = (size_t)size;
    return 0;
}

static int zip_write_file(const char *path, const void *data, size_t len)
{
    iot_fs_file_t fd;

    fd = iot_fs_open(path, IOT_FS_WB);
    if (!fd) {
        return -1;
    }

    if (iot_fs_write(fd, data, len) != (int32_t)len) {
        iot_fs_close(fd);
        return -1;
    }

    iot_fs_close(fd);
    return 0;
}

static const char *zip_basename(const char *path)
{
    const char *name = strrchr(path, '/');
    if (!name) {
        name = strrchr(path, '\\');
    }
    return name ? (name + 1) : path;
}

int zip_decompress_file(const char *zip_path, const char *dest_dir)
{
    uint8_t *data = NULL;
    size_t size = 0;
    mz_zip_archive zip;
    mz_uint num;
    mz_uint i;

    if (!zip_path || !dest_dir) {
        return ZIP_ERR_FORMAT;
    }

    if (zip_read_file(zip_path, &data, &size) != 0) {
        return ZIP_ERR_FILE;
    }

    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_reader_init_mem(&zip, data, size, 0)) {
        iot_free(data);
        return ZIP_ERR_FORMAT;
    }

    num = mz_zip_reader_get_num_files(&zip);
    for (i = 0; i < num; i++) {
        mz_zip_archive_file_stat stat;
        char outpath[512];

        if (!mz_zip_reader_file_stat(&zip, i, &stat)) {
            continue;
        }

        snprintf(outpath, sizeof(outpath), "%s/%s", dest_dir, stat.m_filename);
        for (char *p = outpath; *p; p++) {
            if (*p == '\\') {
                *p = '/';
            }
        }

        if (stat.m_is_directory) {
            zip_mkdir_recursive(outpath);
            continue;
        }

        zip_mkdir_parent(outpath);
        {
            size_t uncomp_size = 0;
            void *uncomp = mz_zip_reader_extract_to_heap(&zip, i, &uncomp_size, 0);
            if (!uncomp) {
                mz_zip_reader_end(&zip);
                iot_free(data);
                return ZIP_ERR_FILE;
            }

            if (zip_write_file(outpath, uncomp, uncomp_size) != 0) {
                mz_free(uncomp);
                mz_zip_reader_end(&zip);
                iot_free(data);
                return ZIP_ERR_FILE;
            }
            mz_free(uncomp);
        }
    }

    mz_zip_reader_end(&zip);
    iot_free(data);
    return ZIP_OK;
}

int zip_compress_file(const char *zip_path, const char **files, int file_count, int level)
{
    mz_zip_archive zip;
    void *archive_buf = NULL;
    size_t archive_size = 0;
    int i;

    if (!zip_path || !files || file_count <= 0) {
        return ZIP_ERR_FORMAT;
    }

    if (level < 0) {
        level = 0;
    }
    if (level > 9) {
        level = 9;
    }

    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_writer_init_heap(&zip, 0, 1024 * 1024)) {
        return ZIP_ERR_MEM;
    }

    for (i = 0; i < file_count; i++) {
        uint8_t *file_data = NULL;
        size_t file_size = 0;
        const char *entry_name = zip_basename(files[i]);

        if (zip_read_file(files[i], &file_data, &file_size) != 0) {
            mz_zip_writer_end(&zip);
            return ZIP_ERR_FILE;
        }

        if (!mz_zip_writer_add_mem(&zip, entry_name, file_data, file_size, (mz_uint)level)) {
            iot_free(file_data);
            mz_zip_writer_end(&zip);
            return ZIP_ERR_FILE;
        }
        iot_free(file_data);
    }

    if (!mz_zip_writer_finalize_heap_archive(&zip, &archive_buf, &archive_size)) {
        mz_zip_writer_end(&zip);
        return ZIP_ERR_FILE;
    }

    if (zip_write_file(zip_path, archive_buf, archive_size) != 0) {
        mz_free(archive_buf);
        mz_zip_writer_end(&zip);
        return ZIP_ERR_FILE;
    }

    mz_free(archive_buf);
    mz_zip_writer_end(&zip);
    return ZIP_OK;
}

int zip_open(zip_t *zip, const char *zip_path)
{
    (void)zip;
    (void)zip_path;
    return ZIP_ERR_FORMAT;
}

void zip_close(zip_t *zip)
{
    (void)zip;
}

int zip_get_entry_count(zip_t *zip)
{
    (void)zip;
    return -1;
}

int zip_get_entry(zip_t *zip, int index, zip_entry_t *entry)
{
    (void)zip;
    (void)index;
    (void)entry;
    return ZIP_ERR_FORMAT;
}

int zip_extract_entry_to_memory(zip_t *zip, int index, uint8_t *buf, size_t buf_size)
{
    (void)zip;
    (void)index;
    (void)buf;
    (void)buf_size;
    return ZIP_ERR_FORMAT;
}

int zip_extract_entry_to_file(zip_t *zip, int index, const char *output_path)
{
    (void)zip;
    (void)index;
    (void)output_path;
    return ZIP_ERR_FORMAT;
}

int zip_extract_all(zip_t *zip, const char *base_path)
{
    (void)zip;
    (void)base_path;
    return ZIP_ERR_FORMAT;
}

int zip_create(zip_t *zip, const char *zip_path)
{
    (void)zip;
    (void)zip_path;
    return ZIP_ERR_FORMAT;
}

int zip_add_file(zip_t *zip, const char *file_path, const char *entry_name, int level)
{
    (void)zip;
    (void)file_path;
    (void)entry_name;
    (void)level;
    return ZIP_ERR_FORMAT;
}

int zip_add_memory(zip_t *zip, const uint8_t *data, size_t data_len, const char *entry_name, int level)
{
    (void)zip;
    (void)data;
    (void)data_len;
    (void)entry_name;
    (void)level;
    return ZIP_ERR_FORMAT;
}

int zip_finish(zip_t *zip)
{
    (void)zip;
    return ZIP_ERR_FORMAT;
}
