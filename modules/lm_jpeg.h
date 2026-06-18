/*
@module  jpeg
@summary JPEG图像编解码模块
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
*/

/**
JPEG参考示例

-- 编码RGB数据为JPEG
local jpeg_data = jpeg.encode(rgb_data, width, height, quality)

-- 解码JPEG为RGB数据
local rgb_data, width, height = jpeg.decode(jpeg_data)

-- quality: 1-100, 默认75
*/

#ifndef IOT_JPEG_H
#define IOT_JPEG_H

#include "lua.h"

LUAMOD_API int luaopen_jpeg(lua_State* L);

#endif /* IOT_JPEG_H */