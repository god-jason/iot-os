/**
 * @file font_external.h
 * @brief 外部字库加载接口定义
 *
 * 本文件定义了外部字库加载模块的 API，支持 BDF、PCF、BIN、RAW 等多种
 * 字体文件格式的解析与加载。提供外部字体对象的引用计数管理和信息查询
 * 接口，允许用户从文件系统加载自定义字体以扩展内置字体库。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef __FONT_EXTERNAL_H__
#define __FONT_EXTERNAL_H__

#include <stdint.h>
#include <stddef.h>
#include "font_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FONT_FORMAT_BDF = 0,
    FONT_FORMAT_PCF,
    FONT_FORMAT_BIN,
    FONT_FORMAT_RAW,
} external_font_format_e;

typedef struct {
    font_info_t info;
    uint8_t* data;
    uint32_t* offsets;
    int ref_count;
} external_font_t;

int font_external_load(const char* path, external_font_format_e format, external_font_t** font);
void font_external_unload(external_font_t* font);

int font_external_load_bdf(const char* path, external_font_t** font);
int font_external_load_pcf(const char* path, external_font_t** font);
int font_external_load_bin(const char* path, external_font_t** font);
int font_external_load_raw(const char* path, int width, int height, int bpp, external_font_t** font);

const font_info_t* font_external_get_info(external_font_t* font);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_EXTERNAL_H__ */