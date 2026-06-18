/*
@module  font_external
@summary 外部点阵字库支持
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
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