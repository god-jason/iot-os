/*
@module  font_data
@summary 字体数据定义
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
*/

#ifndef __FONT_DATA_H__
#define __FONT_DATA_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 字体类型定义
 ****************************************************************************/

typedef enum {
    FONT_FORMAT_BITMAP = 0,   /* 点阵字体 */
    FONT_FORMAT_VECTOR,        /* 矢量字体 */
    FONT_FORMAT_COMPRESSED,    /* 压缩字体 */
} font_format_e;

typedef struct {
    const char* name;          /* 字体名称 */
    font_format_e format;      /* 字体格式 */
    uint16_t width;            /* 字符宽度(0表示可变宽度) */
    uint16_t height;           /* 字符高度 */
    uint8_t bpp;               /* 每像素位数 */
    uint16_t first_char;       /* 首字符编码 */
    uint16_t last_char;        /* 末字符编码 */
    uint32_t glyph_count;      /* 字符数量 */
    const uint8_t* data;       /* 字体数据 */
    const uint32_t* offsets;   /* 字符偏移表(可变宽度字体) */
} font_info_t;

/****************************************************************************
 * 内置字体声明
 ****************************************************************************/

/* 6x8 像素字体 */
extern const font_info_t font_6x8;

/* 6x10 像素字体 */
extern const font_info_t font_6x10;

/* 7x14 像素字体 */
extern const font_info_t font_7x14;

/* 8x8 像素字体 */
extern const font_info_t font_8x8;

/* 8x13 像素字体 */
extern const font_info_t font_8x13;

/* 8x16 像素字体 */
extern const font_info_t font_8x16;

/* 10x20 像素字体 */
extern const font_info_t font_10x20;

/* 12x24 像素字体 */
extern const font_info_t font_12x24;

/* 字体列表 */
extern const font_info_t* font_list[];
extern const int font_count;

#ifdef __cplusplus
}
#endif

#endif /* __FONT_DATA_H__ */