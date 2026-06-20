/*
@module  font_vector
@summary 矢量字体支持实现
@version 1.0
@date    2026.06.11
@author  杰神 & TRAE & ChatGPT
*/

#include <string.h>
#include <stdlib.h>
#include "font_vector.h"

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

int font_vector_load(const char* path, vector_font_t** font) {
    (void)path;
    
    *font = (vector_font_t*)malloc(sizeof(vector_font_t));
    if (!*font) return -1;
    
    memset(*font, 0, sizeof(vector_font_t));
    (*font)->name = "vector_font";
    (*font)->size = 16.0f;
    (*font)->first_char = 0x20;
    (*font)->last_char = 0x7E;
    (*font)->glyph_count = 95;
    (*font)->ascent = 14.0f;
    (*font)->descent = 4.0f;
    (*font)->line_gap = 2.0f;
    
    return 0;
}

void font_vector_unload(vector_font_t* font) {
    if (font) {
        free(font);
    }
}

int font_vector_measure_char(const vector_font_t* font, uint32_t ch, float* width, float* height) {
    if (!font) return -1;
    
    if (ch < font->first_char || ch > font->last_char) {
        ch = '?';
    }
    
    float char_width = font->size * 0.6f;
    
    if (ch == ' ' || ch == '\t') {
        char_width = font->size;
    }
    
    if (width) {
        *width = char_width;
    }
    if (height) {
        *height = font->ascent + font->descent;
    }
    
    return 0;
}

int font_vector_measure_string(const vector_font_t* font, const char* str, float* width, float* height) {
    if (!font || !str) return -1;
    
    float total_width = 0.0f;
    float max_height = font->ascent + font->descent;
    
    const char* p = str;
    while (*p) {
        uint32_t ch = utf8_to_unicode(&p);
        
        if (ch == '\n') {
            continue;
        }
        
        float w, h;
        font_vector_measure_char(font, ch, &w, &h);
        total_width += w;
    }
    
    if (width) {
        *width = total_width;
    }
    if (height) {
        *height = max_height;
    }
    
    return 0;
}

int font_vector_get_glyph(const vector_font_t* font, uint32_t ch, vector_path_t** path, int* count) {
    if (!font) return -1;
    
    if (ch < font->first_char || ch > font->last_char) {
        ch = '?';
    }
    
    int path_count = 4;
    *path = (vector_path_t*)malloc(path_count * sizeof(vector_path_t));
    if (!*path) return -1;
    
    float s = font->size / 16.0f;
    
    switch (ch) {
        case 'A':
            (*path)[0] = (vector_path_t){VECTOR_CMD_MOVE, 4*s, 0, 0, 0, 0, 0};
            (*path)[1] = (vector_path_t){VECTOR_CMD_LINE, 12*s, 0, 0, 0, 0, 0};
            (*path)[2] = (vector_path_t){VECTOR_CMD_LINE, 12*s, 8*s, 0, 0, 0, 0};
            (*path)[3] = (vector_path_t){VECTOR_CMD_LINE, 4*s, 8*s, 0, 0, 0, 0};
            break;
        case 'B':
            (*path)[0] = (vector_path_t){VECTOR_CMD_MOVE, 4*s, 0, 0, 0, 0, 0};
            (*path)[1] = (vector_path_t){VECTOR_CMD_LINE, 12*s, 0, 0, 0, 0, 0};
            (*path)[2] = (vector_path_t){VECTOR_CMD_LINE, 12*s, 16*s, 0, 0, 0, 0};
            (*path)[3] = (vector_path_t){VECTOR_CMD_LINE, 4*s, 16*s, 0, 0, 0, 0};
            break;
        default:
            (*path)[0] = (vector_path_t){VECTOR_CMD_MOVE, 2*s, 2*s, 0, 0, 0, 0};
            (*path)[1] = (vector_path_t){VECTOR_CMD_LINE, 14*s, 2*s, 0, 0, 0, 0};
            (*path)[2] = (vector_path_t){VECTOR_CMD_LINE, 14*s, 14*s, 0, 0, 0, 0};
            (*path)[3] = (vector_path_t){VECTOR_CMD_LINE, 2*s, 14*s, 0, 0, 0, 0};
            break;
    }
    
    if (count) {
        *count = path_count;
    }
    
    return 0;
}

void font_vector_free_path(vector_path_t* path) {
    if (path) {
        free(path);
    }
}