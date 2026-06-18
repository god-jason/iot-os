/*
@module  font_engine
@summary 字体渲染引擎
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
*/

#ifndef __FONT_ENGINE_H__
#define __FONT_ENGINE_H__

#include <stdint.h>
#include <stddef.h>
#include "font_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const font_info_t* font;        /* 当前字体 */
    uint32_t color;                 /* 字体颜色 */
    uint32_t bg_color;              /* 背景颜色 */
    int spacing;                    /* 字符间距 */
    int line_height;                /* 行高 */
    uint8_t antialias;              /* 抗锯齿标志 */
} font_engine_t;

/****************************************************************************
 * 初始化/销毁函数
 ****************************************************************************/

font_engine_t* font_engine_create(void);
void font_engine_destroy(font_engine_t* engine);

/****************************************************************************
 * 字体设置函数
 ****************************************************************************/

int font_engine_set_font(font_engine_t* engine, const font_info_t* font);
int font_engine_set_font_by_name(font_engine_t* engine, const char* name);
int font_engine_set_color(font_engine_t* engine, uint32_t color);
int font_engine_set_bg_color(font_engine_t* engine, uint32_t bg_color);
int font_engine_set_spacing(font_engine_t* engine, int spacing);
int font_engine_set_line_height(font_engine_t* engine, int line_height);

/****************************************************************************
 * 文字测量函数
 ****************************************************************************/

int font_engine_measure_char(font_engine_t* engine, uint32_t ch, int* width, int* height);
int font_engine_measure_string(font_engine_t* engine, const char* str, int* width, int* height);
int font_engine_measure_string_len(font_engine_t* engine, const char* str, size_t len, int* width, int* height);

/****************************************************************************
 * 文字渲染函数
 ****************************************************************************/

int font_engine_render_char(font_engine_t* engine, uint32_t ch, uint8_t* buffer, int buf_width, int buf_height, int x, int y);
int font_engine_render_string(font_engine_t* engine, const char* str, uint8_t* buffer, int buf_width, int buf_height, int x, int y);
int font_engine_render_string_len(font_engine_t* engine, const char* str, size_t len, uint8_t* buffer, int buf_width, int buf_height, int x, int y);

/****************************************************************************
 * 字体管理函数
 ****************************************************************************/

const font_info_t* font_engine_get_font(font_engine_t* engine);
const font_info_t* font_find_by_name(const char* name);
void font_list_all(const font_info_t*** fonts, int* count);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_ENGINE_H__ */