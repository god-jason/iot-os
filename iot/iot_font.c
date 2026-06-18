/*
@module  font
@summary 字体渲染模块
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
@tag     FONT
*/

/*
FONT参考示例
local font = require("font")

-- 加载内置字体
font.loadbuiltin("font_6x8")
font.loadbuiltin("font_8x8")
font.loadbuiltin("font_8x16")

-- 设置默认字体
font.setfont("font_8x16")

-- 测量文字宽度
local w = font.measure("Hello")  -- 返回像素宽度

-- 渲染文字到位图
local bitmap = font.render("Hello World")
--[[
bitmap = {
    width = 80,
    height = 16,
    data = "\xFF\x00..."  -- 逐字节位图数据
}
]]

-- 获取字体列表
local fonts = font.list()
for _, name in ipairs(fonts) do
    print(name)
end

-- 获取字体信息
local info = font.info()
print("Font:", info.name, "Size:", info.width, "x", info.height)
*/

#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "iot_fonts.h"
#include <string.h>
#include <stdlib.h>

static font_engine_t* g_engine = NULL;
static external_font_t* g_external_font = NULL;

static int iot_font_loadbuiltin(lua_State* L);
static int iot_font_load(lua_State* L);
static int iot_font_setfont(lua_State* L);
static int iot_font_measure(lua_State* L);
static int iot_font_render(lua_State* L);
static int iot_font_info(lua_State* L);
static int iot_font_list(lua_State* L);

static const luaL_Reg font_lib[] = {
    {"loadbuiltin", iot_font_loadbuiltin},
    {"load",        iot_font_load},
    {"setfont",     iot_font_setfont},
    {"measure",     iot_font_measure},
    {"render",      iot_font_render},
    {"info",        iot_font_info},
    {"list",        iot_font_list},
    {NULL, NULL}
};

static void font_engine_init(void) {
    if (!g_engine) {
        g_engine = font_engine_create();
    }
}

static int iot_font_loadbuiltin(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    
    font_engine_init();
    
    const font_info_t* font = font_find_by_name(name);
    if (!font) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unknown builtin font");
        return 2;
    }
    
    font_engine_set_font(g_engine, font);
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_font_load(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int width = luaL_optinteger(L, 2, 8);
    int height = luaL_optinteger(L, 3, 16);
    int bpp = luaL_optinteger(L, 4, 1);
    
    font_engine_init();
    
    if (g_external_font) {
        font_external_unload(g_external_font);
        g_external_font = NULL;
    }
    
    int ret = font_external_load_raw(path, width, height, bpp, &g_external_font);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to load font file");
        return 2;
    }
    
    const font_info_t* font_info = font_external_get_info(g_external_font);
    font_engine_set_font(g_engine, font_info);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_font_setfont(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    
    font_engine_init();
    
    const font_info_t* font = font_find_by_name(name);
    if (!font) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "font not found");
        return 2;
    }
    
    font_engine_set_font(g_engine, font);
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_font_measure(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    
    font_engine_init();
    
    int width = 0, height = 0;
    font_engine_measure_string(g_engine, text, &width, &height);
    
    lua_pushinteger(L, width);
    return 1;
}

static int iot_font_render(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    
    font_engine_init();
    
    int width = 0, height = 0;
    font_engine_measure_string(g_engine, text, &width, &height);
    
    if (width <= 0 || height <= 0) {
        lua_pushnil(L);
        return 1;
    }
    
    int bytes_per_row = (width + 7) / 8;
    int total_size = height * bytes_per_row;
    
    uint8_t* bitmap = (uint8_t*)cm_malloc(total_size);
    if (!bitmap) {
        lua_pushnil(L);
        return 1;
    }
    memset(bitmap, 0, total_size);
    
    font_engine_render_string(g_engine, text, bitmap, width, height, 0, 0);
    
    lua_newtable(L);
    lua_pushinteger(L, width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, height);
    lua_setfield(L, -2, "height");
    lua_pushlstring(L, (const char*)bitmap, total_size);
    lua_setfield(L, -2, "data");
    
    cm_free(bitmap);
    return 1;
}

static int iot_font_info(lua_State* L) {
    font_engine_init();
    
    lua_newtable(L);
    
    const font_info_t* font = font_engine_get_font(g_engine);
    if (font) {
        lua_pushstring(L, font->name);
        lua_setfield(L, -2, "name");
        lua_pushinteger(L, font->width);
        lua_setfield(L, -2, "width");
        lua_pushinteger(L, font->height);
        lua_setfield(L, -2, "height");
        lua_pushinteger(L, font->bpp);
        lua_setfield(L, -2, "bpp");
    }
    
    return 1;
}

static int iot_font_list(lua_State* L) {
    const font_info_t** fonts;
    int count;
    
    font_list_all(&fonts, &count);
    
    lua_newtable(L);
    for (int i = 0; i < count; i++) {
        lua_pushstring(L, fonts[i]->name);
        lua_rawseti(L, -2, i + 1);
    }
    
    return 1;
}

LUAMOD_API int luaopen_font(lua_State* L) {
    luaL_newlibtable(L, font_lib);
    luaL_setfuncs(L, font_lib, 0);
    
    font_engine_init();
    
    return 1;
}