-- vendor xmake 配置
-- 第三方库构建配置

-- 包含各个第三方库子目录
includes("lua", "lua-cjson", "gmssl", "lvgl-9.5.0", "miniz")

-- freetype 矢量字体库 (LVGL 9 内置了 tiny_ttf，暂不需要 freetype)
-- includes("freetype")