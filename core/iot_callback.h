/**
 * @file iot_callback.h
 * @brief 回调函数管理模块
 *
 * 本模块提供Lua回调函数的注册和管理功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_CALLBACK_H
#define IOT_CALLBACK_H

#include "lua.h"
#include "iot_params.h"

/**
 * @brief 保存Lua回调函数到注册表
 * @param L Lua状态机
 * @param idx 回调函数在栈上的位置
 * @return userdata指针，用于后续调用和释放
 */
void* iot_callback_save(lua_State* L, int idx);

/**
 * @brief 释放保存的回调函数
 * @param ud userdata指针
 */
void iot_callback_free(void* ud);

/**
 * @brief 调用保存的回调函数
 * @param ud userdata指针
 * @param params 参数列表
 */
void iot_callback_call(void* ud, params_t* params);

/**
 * @brief 从栈上获取参数并调用回调函数
 * @param L Lua状态机
 * @param ud userdata指针
 * @param nargs 参数数量（负数表示从栈顶开始的位置）
 */
void iot_callback_call_from_stack(lua_State* L, void* ud, int nargs);

#endif /* IOT_CALLBACK_H */