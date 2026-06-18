/*
@module  qrcode
@summary QR码生成与解码模块
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
*/

/**
QRCode参考示例

-- 生成QR码
local qr_data = qrcode.encode("https://example.com")
-- qr_data是二进制数据，可以转换为图像

-- 解码QR码
local text = qrcode.decode(image_data, width, height)

-- 设置纠错等级
-- 0: L (低, 7%纠错)
-- 1: M (中, 15%纠错) - 默认
-- 2: Q (高, 25%纠错)
-- 3: H (最高, 30%纠错)
local qr_data = qrcode.encode("text", 1)  -- 使用M等级
*/

#ifndef IOT_QRCODE_H
#define IOT_QRCODE_H

#include "lua.h"

LUAMOD_API int luaopen_qrcode(lua_State* L);

#endif /* IOT_QRCODE_H */