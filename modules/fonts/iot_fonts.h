/**
 * @file iot_fonts.h
 * @brief IOT-OS 字体库主头文件
 *
 * 本文件是 IOT-OS 字体库的统一入口头文件，聚合了字体数据、字体引擎、
 * 矢量字体和外部字体等子模块的头文件引用。同时声明了全局内置字体对象
 * （font_6x8、font_8x8、font_8x16）和字体列表数组，方便应用层统一调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef __IOT_FONTS_H__
#define __IOT_FONTS_H__

#include "font_data.h"
#include "font_engine.h"
#include "font_vector.h"
#include "font_external.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOT_FONTS_VERSION "1.0.0"

extern const font_info_t font_6x8;
extern const font_info_t font_8x8;
extern const font_info_t font_8x16;

extern const font_info_t* font_list[];
extern const int font_count;

#ifdef __cplusplus
}
#endif

#endif /* __IOT_FONTS_H__ */