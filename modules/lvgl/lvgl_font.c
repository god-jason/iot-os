/**
 * @file iot_lvgl_font.c
 * @brief LVGL字体工具
 *
 * 实现LVGL字体工具的Lua绑定，提供获取内置字体（Montserrat系列8-48号）的接口，
 * 以及基于FreeType的矢量字体（TTF/OTF）加载与卸载接口，用于样式设置中的字体指定。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* lv_freetype.h 已通过 lvgl_port.h → lvgl.h → lv_libs.h 链路包含，
 * 其声明在 LV_USE_FREETYPE=1 时可见 */
#if LV_USE_TINY_TTF
    #include "libs/tiny_ttf/lv_tiny_ttf.h"
#endif

/* ==================== 内置字体操作 ==================== */

/*
获取字体
@param name 字体名称(如"montserrat_12","montserrat_14","montserrat_16"等)
@return userdata 字体指针
@usage local font = lvgl.font.get("montserrat_14")
*/
static int iot_lvgl_font_get(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lv_font_t* font = NULL;

    if (0) {
#if LV_FONT_MONTSERRAT_8
    } else if (strcmp(name, "montserrat_8") == 0) {
        font = &lv_font_montserrat_8;
#endif
#if LV_FONT_MONTSERRAT_10
    } else if (strcmp(name, "montserrat_10") == 0) {
        font = &lv_font_montserrat_10;
#endif
#if LV_FONT_MONTSERRAT_12
    } else if (strcmp(name, "montserrat_12") == 0) {
        font = &lv_font_montserrat_12;
#endif
#if LV_FONT_MONTSERRAT_14
    } else if (strcmp(name, "montserrat_14") == 0) {
        font = &lv_font_montserrat_14;
#endif
#if LV_FONT_MONTSERRAT_16
    } else if (strcmp(name, "montserrat_16") == 0) {
        font = &lv_font_montserrat_16;
#endif
#if LV_FONT_MONTSERRAT_18
    } else if (strcmp(name, "montserrat_18") == 0) {
        font = &lv_font_montserrat_18;
#endif
#if LV_FONT_MONTSERRAT_20
    } else if (strcmp(name, "montserrat_20") == 0) {
        font = &lv_font_montserrat_20;
#endif
#if LV_FONT_MONTSERRAT_22
    } else if (strcmp(name, "montserrat_22") == 0) {
        font = &lv_font_montserrat_22;
#endif
#if LV_FONT_MONTSERRAT_24
    } else if (strcmp(name, "montserrat_24") == 0) {
        font = &lv_font_montserrat_24;
#endif
#if LV_FONT_MONTSERRAT_26
    } else if (strcmp(name, "montserrat_26") == 0) {
        font = &lv_font_montserrat_26;
#endif
#if LV_FONT_MONTSERRAT_28
    } else if (strcmp(name, "montserrat_28") == 0) {
        font = &lv_font_montserrat_28;
#endif
#if LV_FONT_MONTSERRAT_30
    } else if (strcmp(name, "montserrat_30") == 0) {
        font = &lv_font_montserrat_30;
#endif
#if LV_FONT_MONTSERRAT_32
    } else if (strcmp(name, "montserrat_32") == 0) {
        font = &lv_font_montserrat_32;
#endif
#if LV_FONT_MONTSERRAT_34
    } else if (strcmp(name, "montserrat_34") == 0) {
        font = &lv_font_montserrat_34;
#endif
#if LV_FONT_MONTSERRAT_36
    } else if (strcmp(name, "montserrat_36") == 0) {
        font = &lv_font_montserrat_36;
#endif
#if LV_FONT_MONTSERRAT_38
    } else if (strcmp(name, "montserrat_38") == 0) {
        font = &lv_font_montserrat_38;
#endif
#if LV_FONT_MONTSERRAT_40
    } else if (strcmp(name, "montserrat_40") == 0) {
        font = &lv_font_montserrat_40;
#endif
#if LV_FONT_MONTSERRAT_42
    } else if (strcmp(name, "montserrat_42") == 0) {
        font = &lv_font_montserrat_42;
#endif
#if LV_FONT_MONTSERRAT_44
    } else if (strcmp(name, "montserrat_44") == 0) {
        font = &lv_font_montserrat_44;
#endif
#if LV_FONT_MONTSERRAT_46
    } else if (strcmp(name, "montserrat_46") == 0) {
        font = &lv_font_montserrat_46;
#endif
#if LV_FONT_MONTSERRAT_48
    } else if (strcmp(name, "montserrat_48") == 0) {
        font = &lv_font_montserrat_48;
#endif
    } else {
        font = LV_FONT_DEFAULT;
    }

    lua_pushlightuserdata(L, (void*)font);
    return 1;
}

/* ==================== 矢量字体操作 ==================== */

/* 选择矢量字体后端：FreeType 优先，其次 tiny_ttf */
#if LV_USE_FREETYPE || LV_USE_TINY_TTF

#if LV_USE_FREETYPE
/* FreeType 库初始化状态（首次加载矢量字体时自动初始化，幂等） */
static int iot_lvgl_freetype_inited = 0;

/*
解析字体样式字符串
@param style 样式字符串("normal"/"italic"/"bold"，可组合如"italic|bold")
@return uint16_t 样式标志位
*/
static uint16_t iot_lvgl_font_parse_style(const char* style) {
    uint16_t flags = FT_FONT_STYLE_NORMAL;
    if (!style) return flags;

    /* 简单匹配：包含关键字即置位（支持 "bold"、"italic"、"bold|italic" 等写法） */
    if (strstr(style, "bold") || strstr(style, "BOLD")) {
        flags |= FT_FONT_STYLE_BOLD;
    }
    if (strstr(style, "italic") || strstr(style, "ITALIC")) {
        flags |= FT_FONT_STYLE_ITALIC;
    }
    return flags;
}
#endif /* LV_USE_FREETYPE */

/*
加载矢量字体（TTF/OTF）
@param path 字体文件路径（如 "WenQuanDengKuanWeiMiHei-1.ttf"）
@param size 字号（像素，如 24）
@param style 样式（可选，"normal"/"italic"/"bold"，默认"normal"；tiny_ttf 后端忽略此参数）
@return userdata 字体指针，失败返回 nil
@usage local font = lvgl.font.load_vector("WenQuanDengKuanWeiMiHei-1.ttf", 24)
@usage local font = lvgl.font.load_vector("WenQuanDengKuanWeiMiHei-1.ttf", 24, "bold")
*/
static int iot_lvgl_font_load_vector(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int size = (int)luaL_checkinteger(L, 2);
    const char* style_str = luaL_optstring(L, 3, "normal");
    (void)style_str; /* tiny_ttf 后端不使用 style */

    if (size <= 0) {
        luaL_error(L, "字体大小必须大于0");
        return 0;
    }

#if LV_USE_FREETYPE
    /* 首次使用时初始化 FreeType 库（不使用缓存：LV_FREETYPE_CACHE_SIZE = -1） */
    if (!iot_lvgl_freetype_inited) {
        if (!lv_freetype_init(0, 0, 0)) {
            luaL_error(L, "FreeType 库初始化失败");
            return 0;
        }
        iot_lvgl_freetype_inited = 1;
    }

    /* 构造字体加载信息（使用文件路径方式，mem = NULL） */
    lv_ft_info_t info;
    memset(&info, 0, sizeof(info));
    info.name = path;
    info.mem = NULL;
    info.mem_size = 0;
    info.weight = (uint16_t)size;
    info.style = iot_lvgl_font_parse_style(style_str);

    if (!lv_ft_font_init(&info)) {
        lua_pushnil(L);
        lua_pushfstring(L, "矢量字体加载失败: %s (size=%d)", path, size);
        return 2;
    }

    /* 返回字体指针（lightuserdata），可直接用于 lvgl.style.set_text_font */
    lua_pushlightuserdata(L, (void*)info.font);
    return 1;
#elif LV_USE_TINY_TTF
    /* tiny_ttf 后端：从文件加载（需要 LV_TINY_TTF_FILE_SUPPORT=1） */
#if LV_TINY_TTF_FILE_SUPPORT
    lv_font_t * font = lv_tiny_ttf_create_file(path, (int32_t)size);
    if (!font) {
        lua_pushnil(L);
        lua_pushfstring(L, "矢量字体加载失败: %s (size=%d)", path, size);
        return 2;
    }
    lua_pushlightuserdata(L, (void*)font);
    return 1;
#else
    lua_pushnil(L);
    lua_pushfstring(L, "tiny_ttf 未启用文件支持 (LV_TINY_TTF_FILE_SUPPORT=0)");
    return 2;
#endif
#endif
}

/*
卸载矢量字体
@param font 字体指针（load_vector 返回值）
@return boolean 成功返回 true
@usage lvgl.font.unload_vector(font)
*/
static int iot_lvgl_font_unload_vector(lua_State* L) {
    if (lua_isnil(L, 1)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lv_font_t* font = (lv_font_t*)luaL_checklightuserdata(L, 1);
    if (font) {
#if LV_USE_FREETYPE
        lv_ft_font_destroy(font);
#elif LV_USE_TINY_TTF
        lv_tiny_ttf_destroy(font);
#endif
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

#endif /* LV_USE_FREETYPE || LV_USE_TINY_TTF */

/* 注册 font 子模块 */
void iot_lvgl_register_font(lua_State* L) {
    REG_METHOD(L, "get", iot_lvgl_font_get);
#if LV_USE_FREETYPE || LV_USE_TINY_TTF
    REG_METHOD(L, "load_vector", iot_lvgl_font_load_vector);
    REG_METHOD(L, "unload_vector", iot_lvgl_font_unload_vector);
#endif
}
