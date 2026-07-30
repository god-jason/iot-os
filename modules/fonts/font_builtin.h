/**
 * @file font_builtin.h
 * @brief 内置 ASCII 点阵字体数据声明
 *
 * 本文件声明了一系列内置的 ASCII 点阵字体数据，涵盖 6x8、8x8、8x12、
 * 8x14、8x16、10x18、12x24、16x32 等多种尺寸规格。每个字体包含 0x20-0x7E
 * 范围的 95 个标准 ASCII 字符，提供统一的 builtin_font_info_t 结构体供字体
 * 引擎查询和使用。同时提供按名称和按索引获取字体信息的辅助函数。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef __FONT_BUILTIN_H__
#define __FONT_BUILTIN_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 6x8 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 6字节 (8行 x 6列 / 8位)
 ****************************************************************************/

extern const uint8_t fontdata_6x8[];
extern const uint16_t fontdata_6x8_offsets[];
extern const int fontdata_6x8_count;

/****************************************************************************
 * 8x8 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 8字节
 ****************************************************************************/

extern const uint8_t fontdata_8x8[];
extern const int fontdata_8x8_count;

/****************************************************************************
 * 8x12 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 12字节
 ****************************************************************************/

extern const uint8_t fontdata_8x12[];
extern const int fontdata_8x12_count;

/****************************************************************************
 * 8x14 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 14字节
 ****************************************************************************/

extern const uint8_t fontdata_8x14[];
extern const int fontdata_8x14_count;

/****************************************************************************
 * 8x16 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 16字节
 ****************************************************************************/

extern const uint8_t fontdata_8x16[];
extern const int fontdata_8x16_count;

/****************************************************************************
 * 10x18 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 20字节 (每行2字节，最后2字节填充0)
 ****************************************************************************/

extern const uint8_t fontdata_10x18[];
extern const int fontdata_10x18_count;

/****************************************************************************
 * 12x24 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 36字节
 ****************************************************************************/

extern const uint8_t fontdata_12x24[];
extern const int fontdata_12x24_count;

/****************************************************************************
 * 16x32 像素字体数据
 * 字符: 0x20-0x7E (95个字符)
 * 每字符: 64字节
 ****************************************************************************/

extern const uint8_t fontdata_16x32[];
extern const int fontdata_16x32_count;

/****************************************************************************
 * 字体信息结构
 ****************************************************************************/

typedef struct {
    const char* name;
    uint8_t width;
    uint8_t height;
    uint8_t bpp;
    uint16_t first_char;
    uint16_t last_char;
    const uint8_t* data;
    const uint16_t* offsets;  /* 可变宽度字体的偏移表 */
    int glyph_count;
} builtin_font_info_t;

extern const builtin_font_info_t builtin_fonts[];
extern const int builtin_font_count;

/* 获取字体信息 */
const builtin_font_info_t* builtin_font_get(const char* name);
const builtin_font_info_t* builtin_font_get_by_index(int index);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_BUILTIN_H__ */
