/**
 * @file http_url.c
 * @brief HTTP URL 解析与编码工具实现
 */

#include "http_url.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static char http_url_hex_to_char(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static char http_url_char_to_hex(char c) {
    static const char hex_chars[] = "0123456789ABCDEF";
    if (c < 0 || c > 15) return '0';
    return hex_chars[(unsigned char)c];
}

int http_url_parse(const char* url, http_url_t* parsed) {
    if (!url || !parsed) return -1;
    
    memset(parsed, 0, sizeof(http_url_t));
    parsed->port = 80;
    
    const char* p = url;
    
    /* 解析 scheme */
    const char* scheme_end = strstr(p, "://");
    if (scheme_end) {
        size_t scheme_len = scheme_end - p;
        if (scheme_len >= sizeof(parsed->scheme)) scheme_len = sizeof(parsed->scheme) - 1;
        memcpy(parsed->scheme, p, scheme_len);
        parsed->scheme[scheme_len] = '\0';
        p = scheme_end + 3;
        
        if (strcmp(parsed->scheme, "https") == 0) {
            parsed->port = 443;
        }
    } else {
        strcpy(parsed->scheme, "http");
    }
    
    /* 解析 fragment */
    const char* fragment = strchr(p, '#');
    if (fragment) {
        size_t len = fragment - p;
        if (len >= sizeof(parsed->fragment)) len = sizeof(parsed->fragment) - 1;
        memcpy(parsed->fragment, fragment + 1, len);
        parsed->fragment[len] = '\0';
    }
    
    /* 解析 query */
    const char* query = strchr(p, '?');
    const char* query_end = fragment ? fragment : p + strlen(p);
    if (query && query < query_end) {
        size_t len = query_end - query - 1;
        if (len >= sizeof(parsed->query)) len = sizeof(parsed->query) - 1;
        memcpy(parsed->query, query + 1, len);
        parsed->query[len] = '\0';
    }
    
    /* 解析 path */
    const char* path = strchr(p, '/');
    const char* host_end = path ? path : query_end;
    if (path && path >= query_end) {
        path = NULL;
        host_end = query_end;
    }
    
    /* 解析 host 和 port */
    size_t host_len = host_end - p;
    if (host_len >= sizeof(parsed->host)) host_len = sizeof(parsed->host) - 1;
    
    const char* port_pos = memchr(p, ':', host_len);
    if (port_pos) {
        memcpy(parsed->host, p, port_pos - p);
        parsed->host[port_pos - p] = '\0';
        parsed->port = (uint16_t)atoi(port_pos + 1);
    } else {
        memcpy(parsed->host, p, host_len);
        parsed->host[host_len] = '\0';
    }
    
    /* 解析 path */
    if (path) {
        const char* path_end = query ? query : fragment;
        if (path_end) {
            size_t len = path_end - path;
            if (len >= sizeof(parsed->path)) len = sizeof(parsed->path) - 1;
            memcpy(parsed->path, path, len);
            parsed->path[len] = '\0';
        } else {
            strncpy(parsed->path, path, sizeof(parsed->path) - 1);
            parsed->path[sizeof(parsed->path) - 1] = '\0';
        }
    } else {
        strcpy(parsed->path, "/");
    }
    
    return 0;
}

char* http_url_get_query(const char* url) {
    if (!url) return NULL;
    
    const char* query = strchr(url, '?');
    if (!query) return NULL;
    
    const char* fragment = strchr(query, '#');
    size_t len = fragment ? (size_t)(fragment - query - 1) : strlen(query + 1);
    
    char* result = (char*)malloc(len + 1);
    if (result) {
        memcpy(result, query + 1, len);
        result[len] = '\0';
    }
    return result;
}

char* http_url_get_param(const char* query, const char* key) {
    if (!query || !key) return NULL;
    
    size_t key_len = strlen(key);
    const char* p = query;
    
    while (*p) {
        /* 跳过空格 */
        while (*p == ' ' || *p == '\t') p++;
        
        /* 检查键名匹配 */
        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            p += key_len + 1;
            
            /* 提取值 */
            const char* value_end = p;
            while (*value_end && *value_end != '&' && *value_end != ' ' && *value_end != '\t') {
                value_end++;
            }
            
            size_t value_len = value_end - p;
            char* value = (char*)malloc(value_len + 1);
            if (value) {
                memcpy(value, p, value_len);
                value[value_len] = '\0';
                
                /* URL 解码 */
                char* decoded = http_url_decode_alloc(value);
                free(value);
                if (decoded) {
                    return decoded;
                }
            }
            return NULL;
        }
        
        /* 跳到下一个参数 */
        while (*p && *p != '&') p++;
        if (*p == '&') p++;
    }
    
    return NULL;
}

size_t http_url_encode_len(const char* str) {
    if (!str) return 0;
    
    size_t len = 0;
    unsigned char c;
    
    while ((c = (unsigned char)*str)) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            len += 1;
        } else {
            len += 3; /* %XX */
        }
        str++;
    }
    
    return len + 1;
}

char* http_url_encode(const char* src, char* dst, size_t dst_len) {
    if (!src || !dst || dst_len == 0) return NULL;
    
    char* p = dst;
    unsigned char c;
    
    while ((c = (unsigned char)*src) && (size_t)(p - dst) < dst_len - 1) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            *p++ = (char)c;
        } else if ((size_t)(p - dst) < dst_len - 3) {
            *p++ = '%';
            *p++ = http_url_char_to_hex((c >> 4) & 0x0F);
            *p++ = http_url_char_to_hex(c & 0x0F);
        }
        src++;
    }
    
    *p = '\0';
    return dst;
}

char* http_url_encode_alloc(const char* str) {
    if (!str) return NULL;
    
    size_t len = http_url_encode_len(str);
    char* result = (char*)malloc(len);
    if (result) {
        http_url_encode(str, result, len);
    }
    return result;
}

size_t http_url_decode_len(const char* str) {
    if (!str) return 0;
    
    size_t len = 0;
    unsigned char c;
    
    while ((c = (unsigned char)*str)) {
        if (c == '%' && str[1] && str[2]) {
            str += 3;
        } else if (c == '+') {
            str++;
        } else {
            str++;
        }
        len++;
    }
    
    return len + 1;
}

char* http_url_decode(const char* src, char* dst, size_t dst_len) {
    if (!src || !dst || dst_len == 0) return NULL;
    
    char* p = dst;
    unsigned char c;
    
    while ((c = (unsigned char)*src) && (size_t)(p - dst) < dst_len - 1) {
        if (c == '%' && isxdigit((unsigned char)src[1]) && isxdigit((unsigned char)src[2])) {
            *p++ = (http_url_hex_to_char(src[1]) << 4) | http_url_hex_to_char(src[2]);
            src += 3;
        } else if (c == '+') {
            *p++ = ' ';
            src++;
        } else {
            *p++ = (char)c;
            src++;
        }
    }
    
    *p = '\0';
    return dst;
}

char* http_url_decode_alloc(const char* str) {
    if (!str) return NULL;
    
    size_t len = http_url_decode_len(str);
    char* result = (char*)malloc(len);
    if (result) {
        http_url_decode(str, result, len);
    }
    return result;
}

int http_url_build(char* url, size_t url_len, const char* scheme, const char* host,
                  uint16_t port, const char* path, const char* query) {
    if (!url || !scheme || !host) return -1;
    
    int len = snprintf(url, url_len, "%s://%s", scheme, host);
    
    /* 添加非标准端口 */
    if ((strcmp(scheme, "http") == 0 && port != 80) ||
        (strcmp(scheme, "https") == 0 && port != 443) ||
        (port != 80 && port != 443)) {
        len += snprintf(url + len, url_len - len, ":%d", port);
    }
    
    /* 添加路径 */
    if (path && path[0]) {
        if (path[0] != '/') {
            len += snprintf(url + len, url_len - len, "/");
        }
        len += snprintf(url + len, url_len - len, "%s", path);
    } else {
        len += snprintf(url + len, url_len - len, "/");
    }
    
    /* 添加查询参数 */
    if (query && query[0]) {
        len += snprintf(url + len, url_len - len, "?%s", query);
    }
    
    return (len >= (int)url_len) ? -1 : 0;
}