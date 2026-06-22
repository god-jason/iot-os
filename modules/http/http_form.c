/**
 * @file http_form.c
 * @brief HTTP 表单数据处理工具实现
 */

#include "http_form.h"
#include "http_url.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char* s_default_boundary = "----IOTFormBoundary7MA4YWxkTrZu0gW";

static void http_form_multipart_ensure_capacity(http_form_multipart_t* mp, size_t needed) {
    if (!mp) return;
    
    if (mp->data_len + needed > mp->data_capacity) {
        size_t new_cap = mp->data_capacity * 2;
        if (new_cap < mp->data_len + needed) {
            new_cap = mp->data_len + needed + 1024;
        }
        char* new_data = (char*)realloc(mp->data, new_cap);
        if (new_data) {
            mp->data = new_data;
            mp->data_capacity = new_cap;
        }
    }
}

int http_form_encode_param(const char* name, const char* value, char* encoded, size_t encoded_len) {
    if (!name || !encoded || encoded_len == 0) return -1;
    
    char* encoded_name = http_url_encode_alloc(name);
    char* encoded_value = value ? http_url_encode_alloc(value) : NULL;
    
    int len = snprintf(encoded, encoded_len, "%s=%s", 
                       encoded_name ? encoded_name : name,
                       encoded_value ? encoded_value : "");
    
    if (encoded_name) free(encoded_name);
    if (encoded_value) free(encoded_value);
    
    return (len >= (int)encoded_len) ? -1 : 0;
}

int http_form_encode_params(const http_form_param_t* params, size_t param_count,
                             char* encoded, size_t encoded_len) {
    if (!params || !encoded || encoded_len == 0) return -1;
    
    char* p = encoded;
    size_t remaining = encoded_len;
    
    for (size_t i = 0; i < param_count; i++) {
        if (i > 0) {
            if (remaining < 2) break;
            *p++ = '&';
            remaining--;
        }
        
        char encoded_pair[1024];
        if (http_form_encode_param(params[i].name, params[i].value, 
                                   encoded_pair, sizeof(encoded_pair)) == 0) {
            size_t len = strlen(encoded_pair);
            if (len >= remaining) {
                memcpy(p, encoded_pair, remaining - 1);
                p += remaining - 1;
                remaining = 1;
                break;
            }
            memcpy(p, encoded_pair, len);
            p += len;
            remaining -= len;
        }
    }
    
    *p = '\0';
    return 0;
}

char* http_form_encode_params_alloc(const http_form_param_t* params, size_t param_count) {
    if (!params || param_count == 0) return NULL;
    
    size_t total_len = 0;
    for (size_t i = 0; i < param_count; i++) {
        total_len += http_url_encode_len(params[i].name) + http_url_encode_len(params[i].value) + 2;
    }
    
    char* result = (char*)malloc(total_len);
    if (result) {
        if (http_form_encode_params(params, param_count, result, total_len) != 0) {
            free(result);
            return NULL;
        }
    }
    
    return result;
}

size_t http_form_decode_params(const char* encoded, http_form_param_t* params, size_t max_params) {
    if (!encoded || !params || max_params == 0) return 0;
    
    size_t count = 0;
    const char* p = encoded;
    
    while (*p && count < max_params) {
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '&') {
            if (*p) p++;
            continue;
        }
        
        const char* eq = strchr(p, '=');
        if (!eq) break;
        
        size_t name_len = eq - p;
        if (name_len >= sizeof(params[count].name)) {
            name_len = sizeof(params[count].name) - 1;
        }
        memcpy(params[count].name, p, name_len);
        params[count].name[name_len] = '\0';
        
        const char* value_start = eq + 1;
        const char* value_end = value_start;
        while (*value_end && *value_end != '&') value_end++;
        size_t value_len = value_end - value_start;
        if (value_len >= sizeof(params[count].value)) {
            value_len = sizeof(params[count].value) - 1;
        }
        memcpy(params[count].value, value_start, value_len);
        params[count].value[value_len] = '\0';
        
        char* decoded = http_url_decode_alloc(params[count].value);
        if (decoded) {
            strncpy(params[count].value, decoded, sizeof(params[count].value) - 1);
            params[count].value[sizeof(params[count].value) - 1] = '\0';
            free(decoded);
        }
        
        decoded = http_url_decode_alloc(params[count].name);
        if (decoded) {
            strncpy(params[count].name, decoded, sizeof(params[count].name) - 1);
            params[count].name[sizeof(params[count].name) - 1] = '\0';
            free(decoded);
        }
        
        count++;
        p = value_end;
        if (*p) p++;
    }
    
    return count;
}

const char* http_form_get_param(const http_form_param_t* params, size_t param_count, const char* name) {
    if (!params || !name) return NULL;
    
    for (size_t i = 0; i < param_count; i++) {
        if (strcmp(params[i].name, name) == 0) {
            return params[i].value;
        }
    }
    
    return NULL;
}

http_form_multipart_t* http_form_multipart_create(const char* boundary) {
    http_form_multipart_t* mp = (http_form_multipart_t*)malloc(sizeof(http_form_multipart_t));
    if (!mp) return NULL;
    
    memset(mp, 0, sizeof(http_form_multipart_t));
    
    if (boundary && boundary[0]) {
        strncpy(mp->boundary, boundary, sizeof(mp->boundary) - 1);
    } else {
        strcpy(mp->boundary, s_default_boundary);
    }
    
    mp->data_capacity = 4096;
    mp->data = (char*)malloc(mp->data_capacity);
    if (!mp->data) {
        free(mp);
        return NULL;
    }
    mp->data[0] = '\0';
    
    return mp;
}

void http_form_multipart_destroy(http_form_multipart_t* mp) {
    if (!mp) return;
    if (mp->data) free(mp->data);
    free(mp);
}

int http_form_multipart_add_field(http_form_multipart_t* mp, const char* name, const char* value) {
    if (!mp || !name) return -1;
    
    char line[512];
    int len = snprintf(line, sizeof(line), "--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n",
                       mp->boundary, name);
    
    http_form_multipart_ensure_capacity(mp, len + strlen(value ? value : "") + 10);
    if (!mp->data) return -1;
    
    strcat(mp->data, line);
    if (value) strcat(mp->data, value);
    strcat(mp->data, "\r\n");
    mp->data_len = strlen(mp->data);
    
    return 0;
}

int http_form_multipart_add_file(http_form_multipart_t* mp, const char* name, const char* filename,
                                const char* content_type, const void* data, size_t data_len) {
    if (!mp || !name || !filename || !data || data_len == 0) return -1;
    
    char header[512];
    int header_len = snprintf(header, sizeof(header),
                              "--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                              "Content-Type: %s\r\n\r\n",
                              mp->boundary, name, filename,
                              content_type ? content_type : "application/octet-stream");
    
    http_form_multipart_ensure_capacity(mp, header_len + data_len + 10);
    if (!mp->data) return -1;
    
    strcat(mp->data, header);
    
    size_t old_len = mp->data_len;
    memcpy(mp->data + mp->data_len, data, data_len);
    mp->data_len += data_len;
    mp->data[mp->data_len] = '\0';
    mp->data_len += 2;
    strcat(mp->data, "\r\n");
    
    return 0;
}

int http_form_multipart_add_file_path(http_form_multipart_t* mp, const char* name, 
                                       const char* filename, const char* content_type,
                                       const char* file_path) {
    if (!mp || !name || !filename || !file_path) return -1;
    
    int fd = -1;
    /* 文件读取功能需要文件系统支持，暂时使用占位符 */
    (void)fd;
    
    char placeholder[256];
    int len = snprintf(placeholder, sizeof(placeholder),
                       "[File content from %s]", file_path);
    
    const char* mime = content_type ? content_type : http_form_guess_content_type(filename);
    
    return http_form_multipart_add_file(mp, name, filename, mime, placeholder, len);
}

const char* http_form_multipart_get_data(http_form_multipart_t* mp, const char** data, size_t* len) {
    if (!mp) return NULL;
    
    http_form_multipart_ensure_capacity(mp, 20);
    if (!mp->data) return NULL;
    
    strcat(mp->data, "--");
    strcat(mp->data, mp->boundary);
    strcat(mp->data, "--\r\n");
    mp->data_len = strlen(mp->data);
    
    if (data) *data = mp->data;
    if (len) *len = mp->data_len;
    
    return mp->data;
}

int http_form_multipart_get_content_type(http_form_multipart_t* mp, char* content_type, size_t len) {
    if (!mp || !content_type) return -1;
    
    snprintf(content_type, len, "multipart/form-data; boundary=%s", mp->boundary);
    return 0;
}

size_t http_form_multipart_get_size(http_form_multipart_t* mp) {
    if (!mp) return 0;
    
    if (mp->data_len == 0) {
        const char* data = http_form_multipart_get_data(mp, NULL, NULL);
        (void)data;
    }
    
    return mp->data_len;
}

const char* http_form_guess_content_type(const char* filename) {
    if (!filename) return "application/octet-stream";
    
    const char* ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";
    
    ext++;
    
    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) return "text/html";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "json") == 0) return "application/json";
    if (strcmp(ext, "xml") == 0) return "application/xml";
    if (strcmp(ext, "txt") == 0) return "text/plain";
    
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "gif") == 0) return "image/gif";
    if (strcmp(ext, "bmp") == 0) return "image/bmp";
    if (strcmp(ext, "ico") == 0) return "image/x-icon";
    if (strcmp(ext, "svg") == 0) return "image/svg+xml";
    if (strcmp(ext, "webp") == 0) return "image/webp";
    
    if (strcmp(ext, "pdf") == 0) return "application/pdf";
    if (strcmp(ext, "zip") == 0) return "application/zip";
    if (strcmp(ext, "gz") == 0 || strcmp(ext, "gzip") == 0) return "application/gzip";
    
    if (strcmp(ext, "mp3") == 0) return "audio/mpeg";
    if (strcmp(ext, "wav") == 0) return "audio/wav";
    if (strcmp(ext, "ogg") == 0) return "audio/ogg";
    
    if (strcmp(ext, "mp4") == 0) return "video/mp4";
    if (strcmp(ext, "avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, "webm") == 0) return "video/webm";
    
    if (strcmp(ext, "woff") == 0) return "font/woff";
    if (strcmp(ext, "woff2") == 0) return "font/woff2";
    if (strcmp(ext, "ttf") == 0) return "font/ttf";
    if (strcmp(ext, "otf") == 0) return "font/otf";
    
    return "application/octet-stream";
}