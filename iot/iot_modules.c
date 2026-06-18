/*
@module  modules
@summary Lua核心模块注册入口
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE
*/

#include "iot_modules.h"

/* 核心基础模块 */
#include "iot_rtos.h"
#include "iot_task.h"
#include "iot_callback.h"
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
    {"callback", luaopen_callback},   /* 回调管理 */
    {"event",    luaopen_event},      /* 事件管理 */
    {"params",   luaopen_params},    /* 参数管理 */

    {"json",     luaopen_json},       /* JSON解析 */
    {"pack",     luaopen_pack},       /* 数据打包/解包 */
    {"base64",   luaopen_base64},     /* Base64编解码 */
    {"zlib",     luaopen_zlib},      /* 压缩解压 */
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
    {"u8g2",     luaopen_u8g2},      /* u8g2图形库 */
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
 */
static void register_module(lua_State* L, const char* name, lua_CFunction func)
{
    luaL_requiref(L, name, func, 1);
    lua_pop(L, 1);
}

/**
 * @brief 注册所有Lua核心模块
 * @param L Lua状态机
 */
void modules_register(lua_State* L)
{
    const luaL_Reg *lib;
    int count = 0;
    
    /* 注册核心模块 */
    for (lib = core_modules; lib->func; lib++) {
        register_module(L, lib->name, lib->func);
        count++;
    }
    
    /* 注册可选模块 */
    for (lib = optional_modules; lib->func; lib++) {
        register_module(L, lib->name, lib->func);
        count++;
    }
}
