/*
@module iot
@summary IoT核心调度
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include <stdio.h>
#include <string.h>
#include "iot_task.h"
#include "iot_base.h"
#include "platform.h"
#include "iot_rtos.h"
#include "iot_modules.h"
#include "iot.luac.h"
#include "zlib.h"

#define LUA_STACK_SIZE  (16 * 1024)
#define MAIN_LUA_PATH   "/app/main.lua"
#define IOT_LUA_PATH    "/app/iot.lua"
#define APP_ZIP_PATH    "/app.zip"
#define APP_TAR_PATH    "/app.tar.gz"
#define APP_DIR         "/app"

static lua_State* g_lua_state = NULL;

/**
 * @brief 加载 Lua 文件，优先尝试加载 .luac 字节码文件
 * @param L Lua 状态机
 * @param lua_path Lua 源文件路径（例如"app/iot.lua"）
 * @return 成功返回 true，失败返回 false
 */
static bool iot_load_lua_file(lua_State* L, const char* lua_path)
{
    char luac_path[256];
    int lua_path_len = strlen(lua_path);
    
    /* 构建 .luac 文件路径 */
    snprintf(luac_path, sizeof(luac_path), "%sc", lua_path);
    
    /* 1. 优先尝试加载 .luac 文件 */
    if (iot_fs_access(luac_path, 0)) {
        LOG("%s", luac_path);
        if (luaL_dofile(L, luac_path) == LUA_OK) {
            LOG("OK %s", luac_path);
            return true;
        } else {
            LOG("ERR %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    
    /* 2. 如果 .luac 加载失败，尝试加载原来的 .lua 文件 */
    if (iot_fs_access(lua_path, 0)) {
        LOG("%s", lua_path);
        if (luaL_dofile(L, lua_path) == LUA_OK) {
            LOG("OK %s", lua_path);
            return true;
        } else {
            LOG("ERR %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    
    LOG("ERR both failed");
    return false;
}

/**
 * @brief 递归删除目录下的所有文件和子目录
 * @param dir_path 目录路径
 */
static void delete_directory_recursive(const char *dir_path)
{
    iot_fs_dirent_t file_data;
    iot_fs_dir_t find_fd;
    char full_path[256];

    /* 打开目录遍历 */
    find_fd = iot_fs_opendir(dir_path);
    if (find_fd == 0) {
        /* 目录为空或无法遍历 */
        return;
    }

    while (iot_fs_readdir(find_fd, &file_data) == 0) {
        /* 跳过 "." 和 ".." */
        if (file_data.file_name[0] == '.' &&
            (file_data.file_name[1] == '\0' ||
             (file_data.file_name[1] == '.' && file_data.file_name[2] == '\0'))) {
            continue;
        }

        /* 构建完整路径 */
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, file_data.file_name);

        if (file_data.file_attr == 1) {
            /* 文件：直接删除 */
            iot_fs_remove(full_path);
        } else {
            /* 目录：递归删除 */
            delete_directory_recursive(full_path);
        }
    }

    /* 关闭遍历句柄 */
    iot_fs_closedir(find_fd);

    /* 删除空目录 */
    iot_fs_remove(dir_path);
}

/**
 * @brief 清空app目录
 */
static void clear_app_dir(void)
{
    /* 递归删除/app下所有文件和目录 */
    delete_directory_recursive(APP_DIR);
    /* 重新创建/app目录 */
    iot_fs_mkdir(APP_DIR, 0);
}

/**
 * @brief 检查并解压升级包
 */
static void check_and_extract_app_zip(void)
{
    /* 检查app.zip 是否存在 */
    if (iot_fs_access(APP_ZIP_PATH, 0)) {
        LOG("app.zip exists, clear app dir and extract");
        clear_app_dir();
        int ret = zip_decompress_file(APP_ZIP_PATH, APP_DIR);
        if (ret == ZIP_OK) {
            LOG("zip decompress success");
        } else {
            LOG("zip decompress failed ret=%d", ret);
        }
    } else if (iot_fs_access(APP_TAR_PATH, 0)) {
        LOG("app.tar.gz exists, clear app dir and extract");
        clear_app_dir();
        int ret = tar_decompress_file(APP_TAR_PATH, APP_DIR);
        if (ret == TAR_OK) {
            LOG("tar decompress success");
        } else {
            LOG("tar decompress failed ret=%d", ret);
        }
    }
}

/**
 * @brief Lua Task 主函数
 */
static void iot_lua_task(void* argument)
{
    (void)argument;

    LOG("starting");

    /* 检查并解压升级包 */
    check_and_extract_app_zip();

    /* 初始化消息队列 */
    if (!iot_rtos_msg_init()) {
        LOG("ERR msg queue init");
        return;
    }
    LOG("msg queue OK");

    /* 初始化事件监听 */
    iot_event_init();
    LOG("event init OK");

    /* 创建 Lua 虚拟机 */
    g_lua_state = luaL_newstate();
    if (g_lua_state == NULL) {
        LOG("ERR lua state create");
        return;
    }
    LOG("lua state OK");

    /* 打开标准库 */
    luaL_openlibs(g_lua_state);
    LOG("libs OK");

    /* 设置 Lua 状态机，用于闭包调用和消息发布 */
    iot_set_lua_state(g_lua_state);

    /* 注册所有模块 */
    modules_register(g_lua_state);
    LOG("modules OK");

    /* 优先从文件系统加载iot.lua/iot.luac，否则使用默认版本 */
    LOG("load iot.lua");
    if (!iot_load_lua_file(g_lua_state, IOT_LUA_PATH)) {
        LOG("use default iot.luac");
        /* 尝试加载默认的iot.luac */
        if (luaL_loadbuffer(g_lua_state, (const char*)iot_luac, iot_luac_len, "iot.luac") != LUA_OK) {
            LOG("ERR load default: %s", lua_tostring(g_lua_state, -1));
            lua_pop(g_lua_state, 1);
        } else {
            if (lua_pcall(g_lua_state, 0, 0, 0) != LUA_OK) {
                LOG("ERR exec default: %s", lua_tostring(g_lua_state, -1));
                lua_pop(g_lua_state, 1);
            } else {
                LOG("default OK");
            }
        }
    }

    /* 加载 main.lua/main.luac */
    if(!iot_load_lua_file(g_lua_state, MAIN_LUA_PATH))
    {
        LOG("ERR load main");
    }

    /* 运行 iot.run() */
    LOG("run iot.run()");
    if (luaL_loadstring(g_lua_state, "iot.run()") != LUA_OK) {
        LOG("ERR load iot.run(): %s", lua_tostring(g_lua_state, -1));
        return;
    }
    
    /* 执行 iot.run() */
    if (lua_pcall(g_lua_state, 0, 0, 0) != LUA_OK) {
        LOG("ERR exec iot.run(): %s", lua_tostring(g_lua_state, -1));
        return;
    }

    LOG("exiting");
    lua_close(g_lua_state);
    g_lua_state = NULL;

    iot_task_exit();
}

/**
 * @brief 启动IoT Lua应用
 */
bool iot_start(void)
{
    LOG("start");

    iot_task_t tid = iot_task_create("lua", iot_lua_task, NULL, LUA_STACK_SIZE, IOT_OS_PRIO_NORMAL);
    return (tid != NULL);
}
