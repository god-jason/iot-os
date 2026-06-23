/*
@module  modules
@summary Lua核心模块注册入口
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE
*/

#include "iot_modules.h"
#include "iot_log.h"

#include "iot_rtos.h"
#include "iot_task.h"
#include "iot_callback.h" 
#include "iot_params.h"
#include "iot_pack.h"


/* 核心模块列表 */
static const luaL_Reg core_modules[] = {
    {"rtos",     luaopen_rtos_register},       /* 实时操作系统接口 */
    {"log",      luaopen_log_register},        /* 日志模块 */
    {"pack",     luaopen_pack_register},       /* 数据打包/解包 */

    {NULL, NULL}
};

/**
 * @brief 注册单个模块
 * @param L Lua状态机
 * @param name 模块名称
 * @param func 模块初始化函数
 * @return 0 成功，-1 失败
 */
static int register_module(lua_State* L, const char* name, lua_CFunction func)
{
    if (!L || !name || !func) {
        LOG("ERR register_module: invalid parameter");
        return -1;
    }
    
    /* luaL_requiref 会将模块表压入栈顶（第3个参数为1时） */
    luaL_requiref(L, name, func, 1);
    
    /* 检查是否成功创建了模块 */
    if (lua_isnil(L, -1)) {
        LOG("ERR register_module: %s returned nil", name);
        lua_pop(L, 1);
        return -1;
    }
    
    /* 弹出模块表，模块已经注册到 package.loaded 中 */
    lua_pop(L, 1);
    
    return 0;
}

/**
 * @brief 注册所有Lua核心模块
 * @param L Lua状态机
 */
void modules_register(lua_State* L)
{
    const luaL_Reg *lib;
    int success_count = 0;
    int fail_count = 0;
    
    /* 注册核心模块 */
    for (lib = core_modules; lib->func; lib++) {
        if (register_module(L, lib->name, lib->func) == 0) {
            success_count++;
        } else {
            fail_count++;
            LOG("WARN module %s register failed", lib->name);
        }
    }
    
    LOG("modules register: success=%d, failed=%d", success_count, fail_count);
}
