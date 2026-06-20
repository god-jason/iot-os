/**
 * @file url.c
 * @brief URL 解析和操作库实现
 *
 * 提供 URL 解析、编码、解码和构建功能
 */

#include "url.h"
#include "platform.h"
#include "iot_log.h"

#include <string.h>
#include <ctype.h>

/*===========================================================
 * 内部宏定义
 *===========================================================*/

#define IS_UNRESERVED(c) \
    (((c) >= 'A' && (c) <= 'Z') || \
     ((c) >= 'a' && (c) <= 'z') || \
     ((c) >= '0' && (c) <= '9') || \
     (c) == '-' || (c) == '_' || (c) == '.' || (c) == '~')

#define IS_SUBCOMPONENT(c) \
    (((c) >= 'A' && (c) <= 'Z') || \
     ((c) >= 'a' && (c) <= 'z') || \
     ((c) >= '0' && (c) <= '9') || \
     (c) == '-' || (c) == '_' || (c) == '.' || (c) == '~' || \
     (c) == ':' || (c) == '@')

#define IS_QUERY(c) \
    (((c) >= 'A' && (c) <= 'Z') || \
     ((c) >= 'a' && (c) <= 'z') || \
     ((c) >= '0' && (c) <= '9') || \
     (c) == '-' || (c) == '_' || (c) == '.' || (c) == '~' || \
     (c) == ':' || (c) == '@' || (c) == '/' || (c) == '?' || \
     (c) == '!' || (c) == '*' || (c) == '\'' || \
     (c) == '(' || (c) == ')' || (c) == ';')

/*===========================================================
 * 内部函数声明
 *===========================================================*/

static char* url_strdup(const char* s);
static char* url_strndup(const char* s, size_t n);
static const char* url_skip_scheme(const char* url);
static int url_parse_authority(iot_url_t* url, const char* str, const char* end);
static void url_free_components(iot_url_t* url);

/*===========================================================
 * 字符串处理辅助函数
 *===========================================================*/

static char* url_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = (char*)iot_malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

static char* url_strndup(const char* s, size_t n) {
    if (!s) return NULL;
    /* 找到实际字符串长度 */
    size_t len = 0;
    while (len < n && s[len] != '\0') {
        len++;
    }
    char* copy = (char*)iot_malloc(len + 1);
    if (copy) {
        memcpy(copy, s, len);
        copy[len] = '\0';
    }
    return copy;
}

/*===========================================================
 * URL 解析实现
 *===========================================================*/

static const char* url_skip_scheme(const char* url) {
    const char* p = url;
    
    /* 方案必须以字母开头 */
    if (!isalpha((unsigned char)*p)) {
        return url;
    }
    
    /* 跳过方案前缀 */
    while (*p && (isalnum((unsigned char)*p) || *p == '+' || *p == '-' || *p == '.')) {
        p++;
    }
    
    /* 检查冒号 */
    if (*p == ':') {
        return p + 1;
    }
    
    return url;
}

/* 跳过空白字符 */
static const char* url_skip_ws(const char* p, const char* end) {
    while (p < end && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

/* 查找字符 (带边界) */
static const char* url_find_char(const char* p, const char* end, char c) {
    while (p < end && *p != c) {
        p++;
    }
    return p;
}

static int url_parse_authority(iot_url_t* url, const char* str, const char* end) {
    const char* p = str;
    const char* host_start = p;
    
    /* 检查用户信息 (@ 之前) */
    const char* at = url_find_char(p, end, '@');
    if (at < end) {
        /* 存在用户信息 */
        const char* colon = url_find_char(p, at, ':');
        if (colon < at) {
            url->username = url_strndup(p, colon - p);
            url->password = url_strndup(colon + 1, at - colon - 1);
        } else {
            url->username = url_strndup(p, at - p);
        }
        host_start = at + 1;
    }
    
    /* 解析主机和端口 */
    const char* host_end = host_start;
    
    /* 检查是否为 IP 字面量或主机结束 */
    if (*host_start == '[') {
        /* IPv6 地址 */
        const char* bracket = url_find_char(host_start + 1, end, ']');
        if (bracket < end) {
            host_end = bracket + 1;
            /* 提取 IPv6 地址 */
            url->host = url_strndup(host_start + 1, bracket - host_start - 1);
        }
    }
    
    if (!url->host) {
        /* 查找冒号或路径结束 */
        const char* colon_or_slash = host_start;
        while (host_end < end && *host_end != ':' && *host_end != '/') {
            host_end++;
        }
        if (host_end > host_start) {
            url->host = url_strndup(host_start, host_end - host_start);
        }
    }
    
    /* 解析端口 */
    if (host_end < end && *host_end == ':') {
        const char* port_start = host_end + 1;
        const char* port_end = url_find_char(port_start, end, '/');
        if (port_end > port_start) {
            int port = 0;
            for (const char* pp = port_start; pp < port_end; pp++) {
                if (*pp >= '0' && *pp <= '9') {
                    port = port * 10 + (*pp - '0');
                } else {
                    port = 0;
                    break;
                }
            }
            url->port = port;
        }
    }
    
    return 0;
}

static void url_free_components(iot_url_t* url) {
    if (url->scheme) iot_free(url->scheme);
    if (url->host) iot_free(url->host);
    if (url->path) iot_free(url->path);
    if (url->query) iot_free(url->query);
    if (url->fragment) iot_free(url->fragment);
    if (url->username) iot_free(url->username);
    if (url->password) iot_free(url->password);
    memset(url, 0, sizeof(iot_url_t));
}

iot_url_t* iot_url_parse(const char* url_str) {
    if (!url_str || *url_str == '\0') {
        return NULL;
    }
    
    iot_url_t* url = (iot_url_t*)iot_calloc(1, sizeof(iot_url_t));
    if (!url) {
        return NULL;
    }
    
    const char* p = url_str;
    const char* end = p + strlen(p);
    
    /* 跳过前导空白 */
    p = url_skip_ws(p, end);
    
    /* 解析方案 */
    const char* scheme_end = url_skip_scheme(p);
    if (scheme_end > p) {
        /* 发现有效方案 */
        url->scheme = url_strndup(p, scheme_end - p - 1);
        for (char* pp = url->scheme; *pp; pp++) {
            *pp = tolower((unsigned char)*pp);
        }
        p = scheme_end;
    }
    
    /* 跳过 "//" 如果存在 */
    if (p < end && p[0] == '/' && p + 1 < end && p[1] == '/') {
        p += 2;
        
        /* 解析权限信息 (user:pass@host:port) */
        const char* auth_end = url_find_char(p, end, '/');
        if (auth_end == end) {
            /* 剩余部分都是权限信息，没有路径 */
            url_parse_authority(url, p, end);
            /* 路径为空 */
            url->path = url_strdup("/");
        } else {
            url_parse_authority(url, p, auth_end);
            p = auth_end;
        }
    } else if (p < end && *p == '/') {
        /* 绝对路径，不带权限 */
        /* 什么都不做，继续解析路径 */
    }
    
    /* 解析路径 */
    if (p < end && *p == '/') {
        const char* path_end = url_find_char(p, end, '?');
        const char* frag_or_query = url_find_char(p, end, '#');
        
        const char* path_end_real = path_end < end ? path_end : end;
        if (frag_or_query < path_end_real) {
            path_end_real = frag_or_query;
        }
        
        if (path_end_real > p) {
            url->path = url_strndup(p, path_end_real - p);
        } else {
            url->path = url_strdup("/");
        }
        p = path_end_real;
    } else if (p < end && *p != '?' && *p != '#') {
        /* 相对路径 */
        const char* frag_or_query = url_find_char(p, end, '#');
        const char* query = url_find_char(p, end, '?');
        const char* path_end_real = frag_or_query < end ? frag_or_query : end;
        if (query < path_end_real) {
            path_end_real = query;
        }
        if (path_end_real > p) {
            url->path = url_strndup(p, path_end_real - p);
        }
    }
    
    /* 解析查询字符串 */
    if (p < end && *p == '?') {
        p++;
        const char* query_end = url_find_char(p, end, '#');
        if (query_end > p) {
            url->query = url_strndup(p, query_end - p);
        }
        p = query_end;
    }
    
    /* 解析片段 */
    if (p < end && *p == '#') {
        p++;
        if (p < end) {
            url->fragment = url_strdup(p);
        }
    }
    
    /* 如果没有指定端口，使用默认端口 */
    if (url->port == 0 && url->scheme) {
        url->port = iot_url_default_port(url->scheme);
    }
    
    return url;
}

void iot_url_free(iot_url_t* url) {
    if (!url) return;
    url_free_components(url);
    iot_free(url);
}

iot_url_t* iot_url_copy(const iot_url_t* url) {
    if (!url) return NULL;
    
    iot_url_t* copy = (iot_url_t*)iot_calloc(1, sizeof(iot_url_t));
    if (!copy) return NULL;
    
    if (url->scheme) copy->scheme = url_strdup(url->scheme);
    if (url->host) copy->host = url_strdup(url->host);
    if (url->path) copy->path = url_strdup(url->path);
    if (url->query) copy->query = url_strdup(url->query);
    if (url->fragment) copy->fragment = url_strdup(url->fragment);
    if (url->username) copy->username = url_strdup(url->username);
    if (url->password) copy->password = url_strdup(url->password);
    copy->port = url->port;
    
    return copy;
}

/*===========================================================
 * URL 构建实现
 *===========================================================*/

char* iot_url_build(const iot_url_t* url) {
    if (!url) return NULL;
    
    /* 估算需要的缓冲区大小 */
    size_t buf_size = 256;
    if (url->scheme) buf_size += strlen(url->scheme);
    if (url->host) buf_size += strlen(url->host);
    if (url->path) buf_size += strlen(url->path);
    if (url->query) buf_size += strlen(url->query);
    if (url->fragment) buf_size += strlen(url->fragment);
    if (url->username) buf_size += strlen(url->username);
    if (url->password) buf_size += strlen(url->password);
    
    char* buf = (char*)iot_malloc(buf_size);
    if (!buf) return NULL;
    
    char* p = buf;
    char* end = buf + buf_size;
    
    /* 添加方案 */
    if (url->scheme && url->scheme[0]) {
        p += snprintf(p, end - p, "%s:", url->scheme);
    }
    
    /* 添加权限信息 (//user:pass@host:port) */
    if (url->host && url->host[0]) {
        p += snprintf(p, end - p, "//");
        
        /* 用户信息 */
        if (url->username && url->username[0]) {
            p += snprintf(p, end - p, "%s", url->username);
            if (url->password && url->password[0]) {
                p += snprintf(p, end - p, ":%s", url->password);
            }
            p += snprintf(p, end - p, "@");
        }
        
        /* 主机名 */
        p += snprintf(p, end - p, "%s", url->host);
        
        /* 端口 */
        if (url->port > 0) {
            int default_port = iot_url_default_port(url->scheme);
            if (url->port != default_port) {
                p += snprintf(p, end - p, ":%d", url->port);
            }
        }
    }
    
    /* 路径 */
    if (url->path && url->path[0]) {
        if (url->path[0] != '/' && url->host && url->host[0]) {
            /* 需要添加前导斜杠 */
            p += snprintf(p, end - p, "/%s", url->path);
        } else {
            p += snprintf(p, end - p, "%s", url->path);
        }
    } else if (url->host && url->host[0]) {
        p += snprintf(p, end - p, "/");
    }
    
    /* 查询字符串 */
    if (url->query && url->query[0]) {
        p += snprintf(p, end - p, "?%s", url->query);
    }
    
    /* 片段 */
    if (url->fragment && url->fragment[0]) {
        p += snprintf(p, end - p, "#%s", url->fragment);
    }
    
    return buf;
}

int iot_url_default_port(const char* scheme) {
    if (!scheme) return 0;
    
    if (strcmp(scheme, "http") == 0) return 80;
    if (strcmp(scheme, "https") == 0) return 443;
    if (strcmp(scheme, "ftp") == 0) return 21;
    if (strcmp(scheme, "ftps") == 0) return 990;
    if (strcmp(scheme, "ssh") == 0) return 22;
    if (strcmp(scheme, "telnet") == 0) return 23;
    if (strcmp(scheme, "ws") == 0) return 80;
    if (strcmp(scheme, "wss") == 0) return 443;
    if (strcmp(scheme, "mqtt") == 0) return 1883;
    if (strcmp(scheme, "mqtts") == 0) return 8883;
    
    return 0;
}

bool iot_url_is_absolute(const iot_url_t* url) {
    if (!url) return false;
    return (url->scheme && url->scheme[0] != '\0');
}

/*===========================================================
 * URL 编码/解码实现
 *===========================================================*/

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static char int_to_hex(int v, int uppercase) {
    if (uppercase) {
        return (v < 10) ? ('0' + v) : ('A' + v - 10);
    } else {
        return (v < 10) ? ('0' + v) : ('a' + v - 10);
    }
}

char* iot_url_encode(const char* str, int len, const iot_url_encode_opt_t* opt) {
    if (!str) return NULL;
    
    iot_url_encode_opt_t default_opt = URL_ENCODE_DEFAULT;
    if (!opt) opt = &default_opt;
    
    if (len < 0) len = strlen(str);
    
    /* 估算输出缓冲区大小 (最坏情况: 每个字符变成 %XX) */
    char* out = (char*)iot_malloc(len * 3 + 1);
    if (!out) return NULL;
    
    char* dst = out;
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)str[i];
        
        if (opt->encode_space_as_plus && c == ' ') {
            *dst++ = '+';
        } else if (IS_UNRESERVED(c)) {
            *dst++ = c;
        } else {
            *dst++ = '%';
            *dst++ = int_to_hex((c >> 4) & 0x0F, opt->encode_uppercase_hex);
            *dst++ = int_to_hex(c & 0x0F, opt->encode_uppercase_hex);
        }
    }
    *dst = '\0';
    
    return out;
}

char* iot_url_decode(const char* str, int len, const iot_url_encode_opt_t* opt) {
    if (!str) return NULL;
    (void)opt; /* 解码选项暂未使用 */
    
    if (len < 0) len = strlen(str);
    
    char* out = (char*)iot_malloc(len + 1);
    if (!out) return NULL;
    
    char* dst = out;
    for (int i = 0; i < len; i++) {
        if (str[i] == '+' && opt && opt->encode_space_as_plus) {
            *dst++ = ' ';
        } else if (str[i] == '%' && i + 2 < len) {
            int hi = hex_to_int(str[i + 1]);
            int lo = hex_to_int(str[i + 2]);
            if (hi >= 0 && lo >= 0) {
                *dst++ = (char)((hi << 4) | lo);
                i += 2;
            } else {
                *dst++ = str[i];
            }
        } else {
            *dst++ = str[i];
        }
    }
    *dst = '\0';
    
    return out;
}

char* iot_url_encode_query(const char* str) {
    iot_url_encode_opt_t opt = {1, 1}; /* space as +, uppercase hex */
    return iot_url_encode(str, -1, &opt);
}

char* iot_url_decode_query(const char* str) {
    iot_url_encode_opt_t opt = {1, 1}; /* space as +, uppercase hex */
    return iot_url_decode(str, -1, &opt);
}
