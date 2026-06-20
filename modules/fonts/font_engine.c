/*
@module  font_engine
@summary 字体渲染引擎实现
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
*/

#include <string.h>
#include <stdlib.h>
#include "font_engine.h"

static inline uint32_t utf8_to_unicode(const char** str) {
    unsigned char c = **str;
    uint32_t code = 0;
    
    if (c < 0x80) {
        code = c;
        (*str)++;
    } else if (c < 0xE0) {
        code = ((c & 0x1F) << 6) | ((*str)[1] & 0x3F);
        (*str) += 2;
    } else if (c < 0xF0) {
        code = ((c & 0x0F) << 12) | (((*str)[1] & 0x3F) << 6) | ((*str)[2] & 0x3F);
        (*str) += 3;
    } else {
        code = ((c & 0x07) << 18) | (((*str)[1] & 0x3F) << 12) | (((*str)[2] & 0x3F) << 6) | ((*str)[3] & 0x3F);
        (*str) += 4;
    }
    
    return code;
}

font_engine_t* font_engine_create(void) {
    font_engine_t* engine = (font_engine_t*)malloc(sizeof(font_engine_t));
    if (!engine) return NULL;
    
    memset(engine, 0, sizeof(font_engine_t));
    engine->font = &font_8x8;
    engine->color = 0xFFFFFFFF;
    engine->bg_color = 0x00000000;
    engine->spacing = 0;
    engine->line_height = 0;
    
    return engine;
}

void font_engine_destroy(font_engine_t* engine) {
    if (engine) {
        free(engine);
    }
}

int font_engine_set_font(font_engine_t* engine, const font_info_t* font) {
    if (!engine || !font) return -1;
    engine->font = font;
    return 0;
}

int font_engine_set_font_by_name(font_engine_t* engine, const char* name) {
    if (!engine || !name) return -1;
    
    const font_info_t* font = font_find_by_name(name);
    if (!font) return -1;
    
    engine->font = font;
    return 0;
}

int font_engine_set_color(font_engine_t* engine, uint32_t color) {
    if (!engine) return -1;
    engine->color = color;
    return 0;
}

int font_engine_set_bg_color(font_engine_t* engine, uint32_t bg_color) {
    if (!engine) return -1;
    engine->bg_color = bg_color;
    return 0;
}

int font_engine_set_spacing(font_engine_t* engine, int spacing) {
    if (!engine) return -1;
    engine->spacing = spacing;
    return 0;
}

int font_engine_set_line_height(font_engine_t* engine, int line_height) {
    if (!engine) return -1;
    engine->line_height = line_height;
    return 0;
}

int font_engine_measure_char(font_engine_t* engine, uint32_t ch, int* width, int* height) {
    if (!engine || !engine->font) return -1;
    
    const font_info_t* font = engine->font;
    
    if (ch < font->first_char || ch > font->last_char) {
        ch = '?';
    }
    
    if (width) {
        *width = font->width + engine->spacing;
    }
    if (height) {
        *height = font->height;
    }
    
    return 0;
}

int font_engine_measure_string(font_engine_t* engine, const char* str, int* width, int* height) {
    if (!engine || !str) return -1;
    return font_engine_measure_string_len(engine, str, strlen(str), width, height);
}

int font_engine_measure_string_len(font_engine_t* engine, const char* str, size_t len, int* width, int* height) {
    if (!engine || !str || !engine->font) return -1;
    
    const font_info_t* font = engine->font;
    int total_width = 0;
    int max_height = font->height;
    
    const char* p = str;
    const char* end = str + len;
    
    while (p < end) {
        uint32_t ch = utf8_to_unicode(&p);
        
        if (ch == '\n') {
            continue;
        }
        
        if (ch >= font->first_char && ch <= font->last_char) {
            total_width += font->width + engine->spacing;
        } else {
            total_width += font->width + engine->spacing;
        }
    }
    
    if (width) {
        *width = total_width;
    }
    if (height) {
        *height = max_height;
    }
    
    return 0;
}

int font_engine_render_char(font_engine_t* engine, uint32_t ch, uint8_t* buffer, int buf_width, int buf_height, int x, int y) {
    if (!engine || !buffer || !engine->font) return -1;
    
    const font_info_t* font = engine->font;
    
    if (ch < font->first_char || ch > font->last_char) {
        ch = '?';
    }
    
    int char_index = ch - font->first_char;
    int bytes_per_row = (font->width + 7) / 8;
    
    const uint8_t* glyph_data = font->data;
    if (font->offsets) {
        glyph_data += font->offsets[char_index];
    } else {
        glyph_data += char_index * bytes_per_row * font->height;
    }
    
    for (int row = 0; row < font->height; row++) {
        int buf_y = y + row;
        if (buf_y < 0 || buf_y >= buf_height) continue;
        
        for (int col = 0; col < font->width; col++) {
            int buf_x = x + col;
            if (buf_x < 0 || buf_x >= buf_width) continue;
            
            int byte_idx = row * bytes_per_row + (col / 8);
            int bit_idx = 7 - (col % 8);
            uint8_t pixel = (glyph_data[byte_idx] >> bit_idx) & 1;
            
            if (pixel) {
                buffer[buf_y * buf_width + buf_x] = 1;
            }
        }
    }
    
    return 0;
}

int font_engine_render_string(font_engine_t* engine, const char* str, uint8_t* buffer, int buf_width, int buf_height, int x, int y) {
    if (!engine || !str) return -1;
    return font_engine_render_string_len(engine, str, strlen(str), buffer, buf_width, buf_height, x, y);
}

int font_engine_render_string_len(font_engine_t* engine, const char* str, size_t len, uint8_t* buffer, int buf_width, int buf_height, int x, int y) {
    if (!engine || !str || !buffer || !engine->font) return -1;
    
    const font_info_t* font = engine->font;
    int current_x = x;
    int current_y = y;
    int line_h = engine->line_height > 0 ? engine->line_height : font->height;
    
    const char* p = str;
    const char* end = str + len;
    
    while (p < end) {
        uint32_t ch = utf8_to_unicode(&p);
        
        if (ch == '\n') {
            current_x = x;
            current_y += line_h;
            continue;
        }
        
        if (ch == '\r') {
            current_x = x;
            continue;
        }
        
        font_engine_render_char(engine, ch, buffer, buf_width, buf_height, current_x, current_y);
        current_x += font->width + engine->spacing;
        
        if (current_x >= buf_width) {
            current_x = x;
            current_y += line_h;
        }
        
        if (current_y >= buf_height) {
            break;
        }
    }
    
    return 0;
}

const font_info_t* font_engine_get_font(font_engine_t* engine) {
    if (!engine) return NULL;
    return engine->font;
}

const font_info_t* font_find_by_name(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < font_count; i++) {
        if (strcmp(font_list[i]->name, name) == 0) {
            return font_list[i];
        }
    }
    
    return NULL;
}

void font_list_all(const font_info_t*** fonts, int* count) {
    if (fonts) {
        *fonts = font_list;
    }
    if (count) {
        *count = font_count;
    }
}