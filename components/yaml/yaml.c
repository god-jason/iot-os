/**
 * @file yaml.c
 * @brief YAML与cJSON相互转换库
 *
 * 本模块提供YAML格式与cJSON对象之间的相互转换功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "yaml.h"
#include "iot_base.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ============================================
 * YAML编码：cJSON对象转换为YAML字符串
 * ============================================ */

#define YAML_OUT_SIZE 4096

/* 跳过一个空白字符 */
static const char* skip_space(const char *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

/* 编码单个cJSON项为YAML */
static void yaml_encode_item(const cJSON *item, char **out, int *left, int indent) {
    if (*left <= 0 || item == NULL) return;
    
    char indent_str[64] = {0};
    for (int i = 0; i < indent && i < 63; i++) {
        indent_str[i] = ' ';
    }
    
    int type = item->type & 0xFF;
    const cJSON *child;
    
    switch (type) {
        case cJSON_Object:
            child = item->child;
            while (child) {
                int len = snprintf(*out, *left, "%s%s: ", indent_str, child->string);
                *out += len;
                *left -= len;
                if (*left <= 0) return;
                
                int child_type = child->type & 0xFF;
                if (child_type == cJSON_Object || child_type == cJSON_Array) {
                    len = snprintf(*out, *left, "\n");
                    *out += len;
                    *left -= len;
                    if (*left <= 0) return;
                    yaml_encode_item(child, out, left, indent + 2);
                } else {
                    yaml_encode_item(child, out, left, 0);
                }
                child = child->next;
            }
            break;
            
        case cJSON_Array:
            child = item->child;
            while (child) {
                int child_type = child->type & 0xFF;
                if (child_type == cJSON_Object || child_type == cJSON_Array) {
                    int len = snprintf(*out, *left, "%s- ", indent_str);
                    *out += len;
                    *left -= len;
                    if (*left <= 0) return;
                    yaml_encode_item(child, out, left, indent + 2);
                } else {
                    int len = snprintf(*out, *left, "%s- ", indent_str);
                    *out += len;
                    *left -= len;
                    if (*left <= 0) return;
                    yaml_encode_item(child, out, left, 0);
                }
                child = child->next;
            }
            break;
            
        case cJSON_String:
        case cJSON_Raw:
            {
                const char *str = cJSON_GetStringValue(item);
                if (str == NULL) str = "";
                /* 检查是否需要引号 */
                if (strchr(str, '\n') || strchr(str, ':') || strchr(str, '#') || 
                    str[0] == ' ' || str[0] == '\0' || 
                    (str[0] == '-' && str[1] == ' ') ||
                    (str[0] == '~' && str[1] == '\0')) {
                    int len = snprintf(*out, *left, "\"%s\"\n", str);
                    *out += len;
                    *left -= len;
                } else {
                    int len = snprintf(*out, *left, "%s\n", str);
                    *out += len;
                    *left -= len;
                }
            }
            break;
            
        case cJSON_Number:
            {
                double num = cJSON_GetNumberValue(item);
                if ((int)num == num && fabs(num) < 1e10 && !isinf(num) && !isnan(num)) {
                    int len = snprintf(*out, *left, "%d\n", (int)num);
                    *out += len;
                    *left -= len;
                } else if (!isinf(num) && !isnan(num)) {
                    int len = snprintf(*out, *left, "%.10g\n", num);
                    *out += len;
                    *left -= len;
                } else {
                    int len = snprintf(*out, *left, "null\n");
                    *out += len;
                    *left -= len;
                }
            }
            break;
            
        case cJSON_True:
            {
                int len = snprintf(*out, *left, "true\n");
                *out += len;
                *left -= len;
            }
            break;
            
        case cJSON_False:
            {
                int len = snprintf(*out, *left, "false\n");
                *out += len;
                *left -= len;
            }
            break;
            
        case cJSON_NULL:
        default:
            {
                int len = snprintf(*out, *left, "~\n");
                *out += len;
                *left -= len;
            }
            break;
    }
}

/**
 * cJSON转YAML字符串
 * @param json cJSON对象
 * @param out_size 输出缓冲区大小
 * @param out 输出缓冲区
 * @return 成功返回输出的字符数，失败返回-1
 */
int yaml_encode_cjson(const cJSON *json, char *out, int out_size) {
    if (json == NULL || out == NULL || out_size <= 0) {
        return -1;
    }
    
    char *p = out;
    int left = out_size - 1;
    
    yaml_encode_item(json, &p, &left, 0);
    *p = '\0';
    
    return (int)(p - out);
}

/**
 * cJSON转YAML字符串(动态分配)
 * @param json cJSON对象
 * @return 分配的YAML字符串，需要iot_free释放;失败返回NULL
 */
char* yaml_encode_cjson_dup(const cJSON *json) {
    if (json == NULL) return NULL;
    
    char *yaml = (char*)iot_malloc(YAML_OUT_SIZE);
    if (yaml == NULL) return NULL;
    
    char *p = yaml;
    int left = YAML_OUT_SIZE - 1;
    
    yaml_encode_item(json, &p, &left, 0);
    *p = '\0';
    
    return yaml;
}

/* ============================================
 * YAML解码：YAML字符串转换为cJSON对象
 * ============================================ */

/* 解析YAML简单值 */
static cJSON* yaml_parse_value(const char **p) {
    *p = skip_space(*p);
    
    if (*p == NULL || **p == '\0' || **p == '\n') {
        return NULL;
    }
    
    /* 带引号的字符串 */
    if (**p == '"' || **p == '\'') {
        char quote = **p;
        (*p)++;
        const char *start = *p;
        while (**p && **p != quote) {
            if (**p == '\\' && *(*p + 1)) (*p)++;
            (*p)++;
        }
        size_t len = *p - start;
        char *str = (char*)iot_malloc(len + 1);
        if (str == NULL) return NULL;
        memcpy(str, start, len);
        str[len] = '\0';
        if (**p == quote) (*p)++;
        cJSON *json = cJSON_CreateString(str);
        iot_free(str);
        return json;
    }
    
    /* 数组 */
    if (**p == '[') {
        (*p)++;
        cJSON *arr = cJSON_CreateArray();
        while (**p && **p != ']') {
            *p = skip_space(*p);
            if (**p == ']' || **p == '\0') break;
            cJSON *item = yaml_parse_value(p);
            if (item) cJSON_AddItemToArray(arr, item);
            *p = skip_space(*p);
            if (**p == ',') (*p)++;
            *p = skip_space(*p);
        }
        if (**p == ']') (*p)++;
        return arr;
    }
    
    /* 对象 */
    if (**p == '{') {
        (*p)++;
        cJSON *obj = cJSON_CreateObject();
        while (**p && **p != '}') {
            *p = skip_space(*p);
            if (**p == '}' || **p == '\0') break;
            
            /* 解析key */
            const char *key_start = *p;
            while (**p && **p != ':' && !isspace(**p) && **p != '}') (*p)++;
            size_t key_len = *p - key_start;
            if (key_len == 0) {
                (*p)++;
                continue;
            }
            char *key = (char*)iot_malloc(key_len + 1);
            if (key == NULL) {
                (*p)++;
                continue;
            }
            memcpy(key, key_start, key_len);
            key[key_len] = '\0';
            
            *p = skip_space(*p);
            if (**p == ':') (*p)++;
            
            cJSON *val = yaml_parse_value(p);
            if (val) cJSON_AddItemToObject(obj, key, val);
            iot_free(key);
            
            *p = skip_space(*p);
            if (**p == ',') (*p)++;
            *p = skip_space(*p);
        }
        if (**p == '}') (*p)++;
        return obj;
    }
    
    /* 注释 */
    if (**p == '#') {
        while (**p && **p != '\n') (*p)++;
        return NULL;
    }
    
    /* null值 */
    if (**p == '~' || strncmp(*p, "null", 4) == 0 || strncmp(*p, "Null", 4) == 0 || strncmp(*p, "NULL", 4) == 0) {
        while (**p && !isspace(**p) && **p != '\n' && **p != ',') (*p)++;
        return cJSON_CreateNull();
    }
    
    /* 布尔值 true */
    if (strncmp(*p, "true", 4) == 0 && (isspace(*(*p + 4)) || !isalnum(*(*p + 4)) || *(*p + 4) == '\0')) {
        *p += 4;
        return cJSON_CreateBool(1);
    }
    
    /* 布尔值 false */
    if (strncmp(*p, "false", 5) == 0 && (isspace(*(*p + 5)) || !isalnum(*(*p + 5)) || *(*p + 5) == '\0')) {
        *p += 5;
        return cJSON_CreateBool(0);
    }
    
    /* 布尔值 yes */
    if (strncmp(*p, "yes", 3) == 0 && (isspace(*(*p + 3)) || !isalnum(*(*p + 3)) || *(*p + 3) == '\0')) {
        *p += 3;
        return cJSON_CreateBool(1);
    }
    
    /* 布尔值 no */
    if (strncmp(*p, "no", 2) == 0 && (isspace(*(*p + 2)) || !isalnum(*(*p + 2)) || *(*p + 2) == '\0')) {
        *p += 2;
        return cJSON_CreateBool(0);
    }
    
    /* 布尔值 on */
    if (strncmp(*p, "on", 2) == 0 && (isspace(*(*p + 2)) || !isalnum(*(*p + 2)) || *(*p + 2) == '\0')) {
        *p += 2;
        return cJSON_CreateBool(1);
    }
    
    /* 布尔值 off */
    if (strncmp(*p, "off", 3) == 0 && (isspace(*(*p + 3)) || !isalnum(*(*p + 3)) || *(*p + 3) == '\0')) {
        *p += 3;
        return cJSON_CreateBool(0);
    }
    
    /* 数字 */
    if (isdigit(**p) || **p == '-' || **p == '+' || **p == '.') {
        const char *start = *p;
        if (**p == '-' || **p == '+') (*p)++;
        int has_dot = 0, has_exp = 0;
        while (**p && (isdigit(**p) || **p == '.' || **p == 'e' || **p == 'E' || **p == '+' || **p == '-')) {
            if (**p == '.') has_dot = 1;
            if (**p == 'e' || **p == 'E') has_exp = 1;
            (*p)++;
        }
        size_t len = *p - start;
        if (len > 0) {
            char *num_str = (char*)iot_malloc(len + 1);
            if (num_str == NULL) return NULL;
            memcpy(num_str, start, len);
            num_str[len] = '\0';
            double num = atof(num_str);
            iot_free(num_str);
            return cJSON_CreateNumber(num);
        }
    }
    
    /* 普通字符串 */
    const char *start = *p;
    while (**p && **p != '\n' && **p != '#') (*p)++;
    size_t len = *p - start;
    while (len > 0 && isspace(*(start + len - 1))) len--;
    if (len == 0) return NULL;
    
    char *str = (char*)iot_malloc(len + 1);
    if (str == NULL) return NULL;
    memcpy(str, start, len);
    str[len] = '\0';
    cJSON *json = cJSON_CreateString(str);
    iot_free(str);
    return json;
}

/**
 * YAML字符串转换为cJSON对象
 * @param yaml YAML格式字符串
 * @return 分配的cJSON对象，需要cJSON_Delete释放;失败返回NULL
 */
cJSON* yaml_decode_cjson(const char *yaml) {
    if (yaml == NULL) return NULL;
    
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) return NULL;
    
    cJSON *current_array = NULL;
    char last_key[128] = {0};
    size_t last_key_len = 0;
    
    const char *p = yaml;
    
    while (*p) {
        p = skip_space(p);
        
        if (*p == '\0') break;
        
        /* 注释行 */
        if (*p == '#') {
            while (*p && *p != '\n') p++;
            continue;
        }
        
        /* 空行 */
        if (*p == '\n' || *p == '\r') {
            p++;
            continue;
        }
        
        /* 数组项 */
        if (*p == '-') {
            /* 检查是否是数组列表项 */
            const char *next_p = p + 1;
            next_p = skip_space(next_p);
            
            if (*next_p == '[' || *next_p == '{') {
                /* 行内数组/对象作为数组项 */
                p = next_p;
                cJSON *item = yaml_parse_value(&p);
                if (item && current_array) {
                    cJSON_AddItemToArray(current_array, item);
                } else if (item) {
                    cJSON_Delete(item);
                }
            } else if (*next_p == '\n' || *next_p == '#' || *next_p == '\0' || 
                       (isalpha(*next_p) && last_key_len == 0)) {
                /* 空数组成员 */
                if (current_array) {
                    cJSON_AddItemToArray(current_array, cJSON_CreateNull());
                }
                p = next_p;
                while (*p && *p != '\n') p++;
            } else {
                /* 普通数组项 */
                if (current_array == NULL) {
                    /* 开始新的匿名数组 */
                    current_array = cJSON_CreateArray();
                    if (last_key_len > 0) {
                        cJSON_AddItemToObject(root, last_key, current_array);
                    }
                    last_key_len = 0;
                }
                
                p = next_p;
                cJSON *item = yaml_parse_value(&p);
                if (item) {
                    cJSON_AddItemToArray(current_array, item);
                }
            }
            
            while (*p && *p != '\n') p++;
            continue;
        }
        
        /* 行内数组 */
        if (*p == '[') {
            cJSON *arr = yaml_parse_value(&p);
            if (arr) {
                if (last_key_len > 0) {
                    cJSON_AddItemToObject(root, last_key, arr);
                    last_key_len = 0;
                } else if (current_array) {
                    cJSON_AddItemToArray(current_array, arr);
                }
            }
            while (*p && *p != '\n') p++;
            continue;
        }
        
        /* key: value 格式 */
        const char *key_start = p;
        while (*p && *p != ':' && !isspace(*p) && *p != '\n') p++;
        if (*p != ':') {
            p++;
            continue;
        }
        
        size_t key_len = p - key_start;
        if (key_len >= sizeof(last_key)) key_len = sizeof(last_key) - 1;
        memcpy(last_key, key_start, key_len);
        last_key[key_len] = '\0';
        last_key_len = key_len;
        
        p++;
        p = skip_space(p);
        
        if (*p == '\n' || *p == '\r' || *p == '#' || *p == '\0') {
            /* 键后换行，可能是对象或数组 */
            if (*p == '#') {
                while (*p && *p != '\n') p++;
            }
            if (*p == '\n') {
                p++;
                p = skip_space(p);
                
                /* 下一个是数组 */
                if (*p == '-') {
                    current_array = cJSON_CreateArray();
                    cJSON_AddItemToObject(root, last_key, current_array);
                    last_key_len = 0;
                } else if (isalpha(*p) || *p == '{' || *p == '[') {
                    /* 嵌套对象 */
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddItemToObject(root, last_key, obj);
                    
                    /* 解析嵌套对象 */
                    while (*p && *p != '\n' && !(*p == '-' && isspace(*(p + 1)))) {
                        p = skip_space(p);
                        
                        /* 解析嵌套key */
                        const char *nk_start = p;
                        while (*p && *p != ':' && !isspace(*p) && *p != '\n') p++;
                        size_t nk_len = p - nk_start;
                        
                        if (nk_len > 0 && nk_len < sizeof(last_key)) {
                            char nested_key[128];
                            memcpy(nested_key, nk_start, nk_len);
                            nested_key[nk_len] = '\0';
                            
                            p = skip_space(p);
                            if (*p == ':') {
                                p++;
                                cJSON *val = yaml_parse_value(&p);
                                if (val) cJSON_AddItemToObject(obj, nested_key, val);
                            }
                        }
                        
                        p = skip_space(p);
                        if (*p == '\n') {
                            p++;
                            p = skip_space(p);
                            /* 如果下一行不是对象成员，退出 */
                            if (!isalpha(*p) && *p != '{' && *p != '[' && !(*p == '-' && isspace(*(p + 1)))) {
                                break;
                            }
                        }
                    }
                    last_key_len = 0;
                }
            }
            continue;
        }
        
        /* 多行字符串 | */
        if (*p == '|') {
            p++;
            /* 跳过初始换行 */
            if (*p == '\n') p++;
            else if (*p == '\r' && *(p + 1) == '\n') p += 2;
            
            char *multi_str = (char*)iot_malloc(1024);
            if (multi_str == NULL) {
                last_key_len = 0;
                continue;
            }
            size_t multi_len = 0;
            
            while (*p && *p != '\n' && multi_len < 1023) {
                multi_str[multi_len++] = *p++;
            }
            multi_str[multi_len] = '\0';
            
            cJSON_AddItemToObject(root, last_key, cJSON_CreateString(multi_str));
            iot_free(multi_str);
            last_key_len = 0;
            
            while (*p && *p != '\n') p++;
            continue;
        }
        
        /* 普通值 */
        cJSON *val = yaml_parse_value(&p);
        if (val && last_key_len > 0) {
            cJSON_AddItemToObject(root, last_key, val);
            last_key_len = 0;
        } else if (val) {
            cJSON_Delete(val);
        }
        
        while (*p && *p != '\n') p++;
    }
    
    /* 如果还有未处理的数组，添加到根对象 */
    /* (数组已直接在遍历中添加) */
    
    return root;
}
