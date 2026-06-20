/*
@module  modules
@summary Lua核心模块注册入口
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE
*/

#include "iot_modules.h"
#include "iot_log.h"

/* 核心基础模块 */
#include "iot_rtos.h"
#include "iot_task.h"
#include "iot_callback.h"  /* 回调函数管理 - 不作为Lua模块注册 */
#include "iot_event.h"
#include "iot_params.h"

/* 数据处理模块 */
#include "iot_json.h"
#include "iot_pack.h"
#include "iot_base64.h"
#include "iot_zlib.h"
#include "iot_yaml.h"

#ifdef ENABLE_SQLITE3
#include "iot_sqlite3.h"
#endif

#ifdef ENABLE_FONTS
#include "iot_font.h"
#include "iot_u8g2.h"
#endif

#ifdef ENABLE_GMSSL
#include "iot_gmssl.h"
#endif

#ifdef ENABLE_JPEG
#include "iot_jpeg.h"
#endif

/* 核心模块列表 */
static const luaL_Reg core_modules[] = {
    {"rtos",     luaopen_rtos},       /* 实时操作系统接口 */
    {"task",     luaopen_task},       /* 任务管理 */
    {"event",    luaopen_event},      /* 事件管理 */
    {"params",   luaopen_params},    /* 参数管理 */
    {"log",      luaopen_log},        /* 日志模块 */

    {"json",     luaopen_json},       /* JSON解析 */
    {"pack",     luaopen_pack},       /* 数据打包/解包 */
    {"base64",   luaopen_base64},     /* Base64编解码 */
    {"zlib",     luaopen_zlib},       /* 压缩解压 */
    {"yaml",     luaopen_yaml},       /* YAML解析 */

    {NULL, NULL}
};

/* 可选核心模块列表 */
static const luaL_Reg optional_modules[] = {
#ifdef ENABLE_SQLITE3
    {"sqlite3",  luaopen_sqlite3},    /* SQLite3数据库 */
#endif

#ifdef ENABLE_FONTS
    {"font",     luaopen_font},       /* 字体渲染 */
    {"u8g2",     luaopen_u8g2},       /* u8g2图形库 */
#endif

#ifdef ENABLE_GMSSL
    {"gmssl",    luaopen_gmssl},      /* 国密算法库 */
#endif

#ifdef ENABLE_JPEG
    {"jpeg",     luaopen_jpeg},       /* JPEG编解码 */
#endif

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
    
    /* 注册可选模块 */
    for (lib = optional_modules; lib->func; lib++) {
        if (register_module(L, lib->name, lib->func) == 0) {
            success_count++;
        } else {
            fail_count++;
            LOG("WARN module %s register failed", lib->name);
        }
    }
    
    LOG("modules register: success=%d, failed=%d", success_count, fail_count);
}
