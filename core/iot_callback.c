/*
@module  callback
@summary 回调函数管理模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include "iot_base.h"
#include "iot_callback.h"
#include "iot_rtos.h"

/**
 * @brief 保存Lua回调函数到注册表
 * @param L Lua状态机
 * @param idx 回调函数在栈上的位置
 * @return userdata指针，用于后续调用和释放
 *
 * @note 返回的 userdata 指针在调用 iot_callback_free 之前始终有效
 */
void* iot_callback_save(lua_State* L, int idx) {
    /* 创建userdata存储回调函数 */
    void* ud = lua_newuserdata(L, sizeof(void*));
    if (!ud) {
        return NULL;
    }
    
    /* 复制回调函数到uservalue */
    lua_pushvalue(L, idx);
    lua_setuservalue(L, -2);
    
    /* 保存到注册表防止GC */
    lua_pushlightuserdata(L, ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    /* 移除栈上的userdata（已在注册表中保存引用） */
    lua_pop(L, 1);
    
    return ud;
}

/**
 * @brief 释放保存的回调函数
 * @param ud userdata指针
 *
 * @note 释放后不能再使用该 userdata 调用回调
 */
void iot_callback_free(void* ud) {
    if (!ud) {
        return;
    }
    
    lua_State* L = iot_get_lua_state();
    if (!L) {
        return;
    }
    
    /* 从注册表中删除引用 */
    lua_pushlightuserdata(L, ud);
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

/**
 * @brief 调用保存的回调函数
 * @param ud userdata指针
 * @param params 参数列表
 */
void iot_callback_call(void* ud, params_t* params) {
    if (!ud) {
        if (params) {
            params_destroy(params);
        }
        return;
    }
    
    iot_rtos_call(ud, params);
}

/**
 * @brief 从栈上获取参数并调用回调函数
 * @param L Lua状态机
 * @param ud userdata指针
 * @param nargs 参数数量（负数表示从栈顶开始的位置）
 */
void iot_callback_call_from_stack(lua_State* L, void* ud, int nargs) {
    if (!L || !ud) return;
    
    params_t* params = params_create(nargs > 0 ? nargs : -nargs);
    if (!params) {
        return;
    }
    
    int count = (nargs > 0) ? nargs : -nargs;
    for (int i = 0; i < count; i++) {
        int idx = (nargs > 0) ? (1 + i) : (nargs + i);
        switch (lua_type(L, idx)) {
            case LUA_TBOOLEAN:
                params_push_int(params, lua_toboolean(L, idx));
                break;
            case LUA_TNUMBER:
                params_push_int(params, (int)lua_tointeger(L, idx));
                break;
            case LUA_TSTRING: {
                size_t len;
                const char* str = lua_tolstring(L, idx, &len);
                params_push_string(params, str, len);
                break;
            }
            default:
                params_push_nil(params);
                break;
        }
    }
    
    iot_rtos_call(ud, params);
}