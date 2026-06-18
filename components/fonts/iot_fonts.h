/*
@module  iot_fonts
@summary iotos字体库主头文件
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
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