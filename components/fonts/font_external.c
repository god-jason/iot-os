/*
@module  font_external
@summary 外部点阵字库支持实现
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
*/

#include <string.h>
#include <stdlib.h>
#include "font_external.h"

#if defined(ENABLE_CM_FS)
#include "cm_fs.h"
#else
#include <stdio.h>
#endif

static void* font_file_read(const char* path, size_t* size) {
    *size = 0;
    
#if defined(ENABLE_CM_FS)
    cm_fs_file_t file = cm_fs_open(path, CM_FS_O_RDONLY);
    if (!file) return NULL;
    
    cm_fs_stat_t stat;
    if (cm_fs_fstat(file, &stat) != 0) {
        cm_fs_close(file);
        return NULL;
    }
    
    *size = stat.size;
    void* data = malloc(*size);
    if (!data) {
        cm_fs_close(file);
        return NULL;
    }
    
    cm_fs_read(file, data, *size);
    cm_fs_close(file);
    
    return data;
#else
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;
    
    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    void* data = malloc(*size);
    if (!data) {
        fclose(fp);
        return NULL;
    }
    
    fread(data, 1, *size, fp);
    fclose(fp);
    
    return data;
#endif
}

int font_external_load(const char* path, external_font_format_e format, external_font_t** font) {
    switch (format) {
        case FONT_FORMAT_BDF:
            return font_external_load_bdf(path, font);
        case FONT_FORMAT_PCF:
            return font_external_load_pcf(path, font);
        case FONT_FORMAT_BIN:
            return font_external_load_bin(path, font);
        default:
            return -1;
    }
}

void font_external_unload(external_font_t* font) {
    if (!font) return;
    
    font->ref_count--;
    if (font->ref_count <= 0) {
        if (font->data) free(font->data);
        if (font->offsets) free(font->offsets);
        free(font);
    }
}

int font_external_load_bdf(const char* path, external_font_t** font) {
    size_t size;
    uint8_t* data = font_file_read(path, &size);
    if (!data) return -1;
    
    *font = (external_font_t*)malloc(sizeof(external_font_t));
    if (!*font) {
        free(data);
        return -1;
    }
    
    memset(*font, 0, sizeof(external_font_t));
    
    (*font)->info.name = "bdf_font";
    (*font)->info.format = FONT_FORMAT_BITMAP;
    (*font)->info.width = 8;
    (*font)->info.height = 16;
    (*font)->info.bpp = 1;
    (*font)->info.first_char = 0x20;
    (*font)->info.last_char = 0x7E;
    (*font)->info.glyph_count = 95;
    
    int bytes_per_glyph = ((*font)->info.width + 7) / 8 * (*font)->info.height;
    (*font)->info.data = (*font)->data = (uint8_t*)malloc(bytes_per_glyph * (*font)->info.glyph_count);
    if (!(*font)->data) {
        free(data);
        free(*font);
        return -1;
    }
    
    memset((*font)->data, 0, bytes_per_glyph * (*font)->info.glyph_count);
    
    char* p = (char*)data;
    while (p < (char*)data + size) {
        if (strncmp(p, "ENCODING", 8) == 0) {
            int code = atoi(p + 9);
            if (code >= (*font)->info.first_char && code <= (*font)->info.last_char) {
                int idx = code - (*font)->info.first_char;
                char* bitmap_start = strstr(p, "BITMAP");
                if (bitmap_start) {
                    bitmap_start += 7;
                    for (int row = 0; row < (*font)->info.height; row++) {
                        bitmap_start += strspn(bitmap_start, "\n\r \t");
                        char byte_str[3] = {bitmap_start[0], bitmap_start[1], 0};
                        uint8_t byte = (uint8_t)strtol(byte_str, NULL, 16);
                        (*font)->data[idx * bytes_per_glyph + row] = byte;
                        bitmap_start += 2;
                    }
                }
            }
        }
        p++;
    }
    
    free(data);
    (*font)->ref_count = 1;
    
    return 0;
}

int font_external_load_pcf(const char* path, external_font_t** font) {
    *font = (external_font_t*)malloc(sizeof(external_font_t));
    if (!*font) return -1;
    
    memset(*font, 0, sizeof(external_font_t));
    
    (*font)->info.name = "pcf_font";
    (*font)->info.format = FONT_FORMAT_BITMAP;
    (*font)->info.width = 8;
    (*font)->info.height = 16;
    (*font)->info.bpp = 1;
    (*font)->info.first_char = 0x20;
    (*font)->info.last_char = 0x7E;
    (*font)->info.glyph_count = 95;
    
    int bytes_per_glyph = ((*font)->info.width + 7) / 8 * (*font)->info.height;
    (*font)->info.data = (*font)->data = (uint8_t*)calloc(bytes_per_glyph, (*font)->info.glyph_count);
    if (!(*font)->data) {
        free(*font);
        return -1;
    }
    
    (*font)->ref_count = 1;
    
    return 0;
}

int font_external_load_bin(const char* path, external_font_t** font) {
    size_t size;
    uint8_t* data = font_file_read(path, &size);
    if (!data) return -1;
    
    *font = (external_font_t*)malloc(sizeof(external_font_t));
    if (!*font) {
        free(data);
        return -1;
    }
    
    memset(*font, 0, sizeof(external_font_t));
    
    (*font)->info.name = "bin_font";
    (*font)->info.format = FONT_FORMAT_BITMAP;
    (*font)->info.width = 8;
    (*font)->info.height = 16;
    (*font)->info.bpp = 1;
    (*font)->info.first_char = 0x20;
    (*font)->info.glyph_count = size / (((*font)->info.width + 7) / 8 * (*font)->info.height);
    (*font)->info.last_char = (*font)->info.first_char + (*font)->info.glyph_count - 1;
    (*font)->info.data = (*font)->data = data;
    
    (*font)->ref_count = 1;
    
    return 0;
}

int font_external_load_raw(const char* path, int width, int height, int bpp, external_font_t** font) {
    size_t size;
    uint8_t* data = font_file_read(path, &size);
    if (!data) return -1;
    
    *font = (external_font_t*)malloc(sizeof(external_font_t));
    if (!*font) {
        free(data);
        return -1;
    }
    
    memset(*font, 0, sizeof(external_font_t));
    
    (*font)->info.name = "raw_font";
    (*font)->info.format = FONT_FORMAT_BITMAP;
    (*font)->info.width = width;
    (*font)->info.height = height;
    (*font)->info.bpp = bpp;
    (*font)->info.first_char = 0x20;
    
    int bytes_per_glyph = (width * bpp + 7) / 8 * height;
    (*font)->info.glyph_count = size / bytes_per_glyph;
    (*font)->info.last_char = (*font)->info.first_char + (*font)->info.glyph_count - 1;
    (*font)->info.data = (*font)->data = data;
    
    (*font)->ref_count = 1;
    
    return 0;
}

const font_info_t* font_external_get_info(external_font_t* font) {
    if (!font) return NULL;
    return &font->info;
}