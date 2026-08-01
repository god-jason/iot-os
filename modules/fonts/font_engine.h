/**
 * @file font_engine.h
 * @brief 字体渲染引擎接口定义
 *
 * 本文件定义了字体渲染引擎的公开 API，包括引擎创建与销毁、字体与颜色
 * 设置、字符与字符串尺寸测量、以及将字体位图渲染到帧缓冲区等功能。支持
 * UTF-8 编码的多字节字符解析、字符间距和行高设置、自动换行等特性。
 * 引擎可通过字体名称快速查找已注册字体，或通过枚举接口获取所有可用字体。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef __IOT_FONT_ENGINE_H__
#define __IOT_FONT_ENGINE_H__

#include <stdint.h>
#include <stddef.h>
#include "font_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const iot_font_info_t* font;        /* 当前字体 */
    uint32_t color;                 /* 字体颜色 */
    uint32_t bg_color;              /* 背景颜色 */
    int spacing;                    /* 字符间距 */
    int line_height;                /* 行高 */
    uint8_t antialias;              /* 抗锯齿标志 */
} iot_font_engine_t;

/****************************************************************************
 * 初始化/销毁函数
 ****************************************************************************/

iot_font_engine_t* iot_font_engine_create(void);
void iot_font_engine_destroy(iot_font_engine_t* engine);

/****************************************************************************
 * 字体设置函数
 ****************************************************************************/

int iot_font_engine_set_font(iot_font_engine_t* engine, const iot_font_info_t* font);
int iot_font_engine_set_font_by_name(iot_font_engine_t* engine, const char* name);
int iot_font_engine_set_color(iot_font_engine_t* engine, uint32_t color);
int iot_font_engine_set_bg_color(iot_font_engine_t* engine, uint32_t bg_color);
int iot_font_engine_set_spacing(iot_font_engine_t* engine, int spacing);
int iot_font_engine_set_line_height(iot_font_engine_t* engine, int line_height);

/****************************************************************************
 * 文字测量函数
 ****************************************************************************/

int iot_font_engine_measure_char(iot_font_engine_t* engine, uint32_t ch, int* width, int* height);
int iot_font_engine_measure_string(iot_font_engine_t* engine, const char* str, int* width, int* height);
int iot_font_engine_measure_string_len(iot_font_engine_t* engine, const char* str, size_t len, int* width, int* height);

/****************************************************************************
 * 文字渲染函数
 ****************************************************************************/

int iot_font_engine_render_char(iot_font_engine_t* engine, uint32_t ch, uint8_t* buffer, int buf_width, int buf_height, int x, int y);
int iot_font_engine_render_string(iot_font_engine_t* engine, const char* str, uint8_t* buffer, int buf_width, int buf_height, int x, int y);
int iot_font_engine_render_string_len(iot_font_engine_t* engine, const char* str, size_t len, uint8_t* buffer, int buf_width, int buf_height, int x, int y);

/****************************************************************************
 * 字体管理函数
 ****************************************************************************/

const iot_font_info_t* iot_font_engine_get_font(iot_font_engine_t* engine);
const iot_font_info_t* iot_font_find_by_name(const char* name);
void iot_font_list_all(const iot_font_info_t*** fonts, int* count);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_FONT_ENGINE_H__ */