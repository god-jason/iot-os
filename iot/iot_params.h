/**
 * @file iot_params.h
 * @brief 参数列表操作
 *
 * 本模块提供参数列表的创建、添加、获取等功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_PARAMS_HEADER
#define IOT_PARAMS_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "lua.h"

/* 参数类型枚举 */
typedef enum {
    PARAM_NIL = 0,
    PARAM_BOOL,
    PARAM_INT,
    PARAM_NUMBER,
    PARAM_STRING,
    PARAM_LIGHTUSERDATA
} param_type_t;

/* 参数结构体 */
typedef struct {
    param_type_t type;
    union {
        bool bool_val;
        int64_t int_val;
        double num_val;
        char* str_val;
        void* ptr_val;
    } value;
    size_t str_len;  /* 字符串长度（仅对STRING有效） */
} param_t;

/* 参数列表（动态长度） */
typedef struct {
    param_t* items;
    int count;
    int capacity;
} params_t;

/* 创建参数列表 */
params_t* params_create(int initial_capacity);

/* 销毁参数列表 */
void params_destroy(params_t* list);

/* 确保有足够容量 */
int params_ensure_capacity(params_t* list, int additional);

/* 追加 nil */
int params_push_nil(params_t* list);

/* 追加 bool */
int params_push_bool(params_t* list, bool val);

/* 追加 int */
int params_push_int(params_t* list, int64_t val);

/* 追加 number */
int params_push_number(params_t* list, double val);

/* 追加 string */
int params_push_string(params_t* list, const char* val, size_t len);

/* 追加 lightuserdata */
int params_push_ptr(params_t* list, void* val);

/* 将参数列表转换为 Lua table（压入栈） */
void params_to_table(lua_State* L, params_t* params);

#endif /* IOT_PARAMS_HEADER */
