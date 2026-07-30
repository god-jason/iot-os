/**
 * @file font_vector.h
 * @brief 矢量字体支持接口定义
 *
 * 本文件定义了矢量字体子系统的数据结构和 API，包括矢量点（vector_point_t）、
 * 字形轮廓（vector_glyph_t）、字体对象（vector_font_t）等类型定义，以及字体
 * 加载/卸载、字符尺寸测量、字形路径获取等操作接口。矢量字体使用贝塞尔曲线
 * 描述字符轮廓，支持任意尺寸缩放，为高质量文字渲染提供基础支撑。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef __FONT_VECTOR_H__
#define __FONT_VECTOR_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x;
    float y;
} vector_point_t;

typedef struct {
    uint8_t type;
    vector_point_t points[4];
} vector_glyph_t;

typedef struct {
    const char* name;
    float size;
    uint32_t first_char;
    uint32_t last_char;
    uint32_t glyph_count;
    const vector_glyph_t* glyphs;
    float ascent;
    float descent;
    float line_gap;
} vector_font_t;

typedef enum {
    VECTOR_CMD_MOVE = 0,
    VECTOR_CMD_LINE,
    VECTOR_CMD_QUAD,
    VECTOR_CMD_CUBIC,
    VECTOR_CMD_CLOSE,
} vector_cmd_e;

typedef struct {
    vector_cmd_e cmd;
    float x0, y0;
    float x1, y1;
    float x2, y2;
} vector_path_t;

int font_vector_load(const char* path, vector_font_t** font);
void font_vector_unload(vector_font_t* font);

int font_vector_measure_char(const vector_font_t* font, uint32_t ch, float* width, float* height);
int font_vector_measure_string(const vector_font_t* font, const char* str, float* width, float* height);

int font_vector_get_glyph(const vector_font_t* font, uint32_t ch, vector_path_t** path, int* count);
void font_vector_free_path(vector_path_t* path);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_VECTOR_H__ */