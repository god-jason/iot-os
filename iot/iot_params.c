/*
@module  params
@summary 参数列表操作
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include "iot_base.h"
#include "iot_params.h"
#include "iot_mem.h"

/* 创建参数列表 */
params_t* params_create(int initial_capacity)
{
    if (initial_capacity < 1) {
        initial_capacity = 4;
    }

    params_t* list = (params_t*)iot_malloc(sizeof(params_t));
    if (!list) {
        LOG("ERR malloc params_t");
        return NULL;
    }

    list->items = (param_t*)iot_malloc(initial_capacity * sizeof(param_t));
    if (!list->items) {
        LOG("ERR malloc items");
        iot_free(list);
        return NULL;
    }

    memset(list->items, 0, initial_capacity * sizeof(param_t));
    list->count = 0;
    list->capacity = initial_capacity;

    //LOG("create OK capacity=%d", initial_capacity);
    return list;
}

/* 销毁参数列表 */
void params_destroy(params_t* list)
{
    if (!list) {
        return;
    }

    /* 先释放每个参数中的字符串 */
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].type == PARAM_STRING && list->items[i].value.str_val) {
            iot_free(list->items[i].value.str_val);
        }
    }

    if (list->items) {
        iot_free(list->items);
    }
    iot_free(list);
}

/* 确保有足够容量 */
int params_ensure_capacity(params_t* list, int additional)
{
    if (!list) {
        return -1;
    }

    int new_capacity = list->count + additional;
    if (new_capacity <= list->capacity) {
        return 0;
    }

    /* 至少翻倍 */
    new_capacity = list->capacity * 2;
    while (new_capacity < list->count + additional) {
        new_capacity *= 2;
    }

    LOG("resize %d -> %d", list->capacity, new_capacity);

    param_t* new_items = (param_t*)iot_realloc(list->items, new_capacity * sizeof(param_t));
    if (!new_items) {
        LOG("ERR realloc");
        return -1;
    }

    /* 初始化新分配的空间 */
    memset(new_items + list->capacity, 0, (new_capacity - list->capacity) * sizeof(param_t));

    list->items = new_items;
    list->capacity = new_capacity;

    return 0;
}

/* 追加 nil */
int params_push_nil(params_t* list)
{
    if (!list) {
        return -1;
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_NIL;
    list->count++;

    return 0;
}

/* 追加 bool */
int params_push_bool(params_t* list, bool val)
{
    if (!list) {
        return -1;
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_BOOL;
    p->value.bool_val = val;
    list->count++;

    return 0;
}

/* 追加 int */
int params_push_int(params_t* list, int64_t val)
{
    if (!list) {
        return -1;
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_INT;
    p->value.int_val = val;
    list->count++;

    return 0;
}

/* 追加 number */
int params_push_number(params_t* list, double val)
{
    if (!list) {
        return -1;
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_NUMBER;
    p->value.num_val = val;
    list->count++;

    return 0;
}

/* 追加 string */
int params_push_string(params_t* list, const char* val, size_t len)
{
    if (!list) {
        return -1;
    }

    if (val == NULL) {
        return params_push_nil(list);
    }

    if (len == 0) {
        len = strlen(val);
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_STRING;
    p->value.str_val = (char*)iot_malloc(len + 1);
    if (!p->value.str_val) {
        return -1;
    }

    memcpy(p->value.str_val, val, len);
    p->value.str_val[len] = '\0';
    p->str_len = len;
    list->count++;

    return 0;
}

/* 追加 lightuserdata */
int params_push_ptr(params_t* list, void* val)
{
    if (!list) {
        return -1;
    }

    if (params_ensure_capacity(list, 1) < 0) {
        return -1;
    }

    param_t* p = &list->items[list->count];
    p->type = PARAM_LIGHTUSERDATA;
    p->value.ptr_val = val;
    list->count++;

    return 0;
}

/* 将参数列表转换为 Lua table（压入栈） */
void params_to_table(lua_State* L, params_t* params)
{
    if (!params || params->count <= 0) {
        return;
    }

    lua_createtable(L, params->count, 0);
    for (int i = 0; i < params->count; i++) {
        param_t* p = &params->items[i];
        switch (p->type) {
            case PARAM_NIL:
                lua_pushnil(L);
                break;
            case PARAM_BOOL:
                lua_pushboolean(L, p->value.bool_val);
                break;
            case PARAM_INT:
                lua_pushinteger(L, (lua_Integer)p->value.int_val);
                break;
            case PARAM_NUMBER:
                lua_pushnumber(L, (lua_Number)p->value.num_val);
                break;
            case PARAM_STRING:
                if (p->value.str_val) {
                    lua_pushlstring(L, p->value.str_val, p->str_len);
                } else {
                    lua_pushnil(L);
                }
                break;
            case PARAM_LIGHTUSERDATA:
                lua_pushlightuserdata(L, p->value.ptr_val);
                break;
        }
        lua_rawseti(L, -2, i + 1);
    }
}
