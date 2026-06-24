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
#include "iot_log.h"
#include "iot.h"
#include "lualib.h"
#include "iot_rtos.h"
#include "iot_modules.h"
#include "iot.luac.h"
#include "zlib.h"

#define LUA_STACK_SIZE  (16 * 1024)
#define MAIN_LUA_PATH   "app/main.lua"
#define IOT_LUA_PATH    "app/iot.lua"
#define APP_ZIP_PATH    "app.zip"
#define APP_TAR_PATH    "app.tar.gz"
#define APP_DIR         "app"

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
    
    /* 构建 .luac 文件路径 */
    snprintf(luac_path, sizeof(luac_path), "%sc", lua_path);
    
    /* 1. 优先尝试加载 .luac 文件 */
    if (iot_fs_access(luac_path, 0)) {
        LOG_DEBUG("%s", luac_path);
        if (luaL_dofile(L, luac_path) == LUA_OK) {
            LOG_DEBUG("OK %s", luac_path);
            return true;
        } else {
            LOG_ERROR("Load failed: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    
    /* 2. 如果 .luac 加载失败，尝试加载原来的 .lua 文件 */
    if (iot_fs_access(lua_path, 0)) {
        LOG_DEBUG("%s", lua_path);
        if (luaL_dofile(L, lua_path) == LUA_OK) {
            LOG_DEBUG("OK %s", lua_path);
            return true;
        } else {
            LOG_ERROR("Load failed: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    
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
        /* 获取文件名 */
        const char* name = iot_fs_dirent_name(file_data);

        /* 跳过无效或空名称 */
        if (!name || name[0] == '\0') {
            continue;
        }

        /* 跳过 "." 和 ".." */
        if (name[0] == '.' &&
            (name[1] == '\0' ||
             (name[1] == '.' && name[2] == '\0'))) {
            continue;
        }

        /* 构建完整路径 */
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, name);

        /* 判断是否为目录 */
        if (iot_fs_dirent_is_dir(file_data)) {
            /* 目录：递归删除 */
            delete_directory_recursive(full_path);
        } else {
            /* 文件：直接删除 */
            iot_fs_remove(full_path);
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
        LOG_INFO("Found app.zip, extracting...");
        clear_app_dir();
        int ret = zip_decompress_file(APP_ZIP_PATH, APP_DIR);
        if (ret == ZIP_OK) {
            LOG_INFO("zip decompress success");
        } else {
            LOG_ERROR("zip decompress failed ret=%d", ret);
        }
    } else if (iot_fs_access(APP_TAR_PATH, 0)) {
        LOG_INFO("Found app.tar.gz, extracting...");
        clear_app_dir();
        int ret = tar_decompress_file(APP_TAR_PATH, APP_DIR);
        if (ret == TAR_OK) {
            LOG_INFO("tar decompress success");
        } else {
            LOG_ERROR("tar decompress failed ret=%d", ret);
        }
    }
}

/**
 * @brief Lua Task 主函数
 */
void iot_lua_task(void* argument)
{
    (void)argument;

    LOG_INFO("starting");

    /* 检查并解压升级包 */
    check_and_extract_app_zip();

    /* 初始化消息队列 */
    if (!iot_rtos_msg_init()) {
        LOG_ERROR("msg queue init failed");
        return;
    }
    LOG_DEBUG("msg queue OK");

    /* 创建 Lua 虚拟机 */
    g_lua_state = luaL_newstate();
    if (g_lua_state == NULL) {
        LOG_ERROR("lua state create failed");
        return;
    }
    LOG_DEBUG("lua state OK");

    /* 打开标准库 */
    luaL_openlibs(g_lua_state);
    LOG_DEBUG("libs OK");

    /* 设置 Lua 状态机，用于闭包调用和消息发布 */
    iot_set_lua_state(g_lua_state);

    /* 注册所有模块 */
    modules_register(g_lua_state);
    LOG_DEBUG("modules OK");

    /* 优先从文件系统加载iot.lua/iot.luac，否则使用默认版本 */
    LOG_DEBUG("load iot.lua");
    if (!iot_load_lua_file(g_lua_state, IOT_LUA_PATH)) {
        LOG_INFO("use default iot.luac");
        /* 尝试加载默认的iot.luac */
        if (luaL_loadbuffer(g_lua_state, (const char*)iot_luac, iot_luac_len, "iot.luac") != LUA_OK) {
            LOG_ERROR("load default failed: %s", lua_tostring(g_lua_state, -1));
            lua_pop(g_lua_state, 1);
        } else {
            if (lua_pcall(g_lua_state, 0, 0, 0) != LUA_OK) {
                LOG_ERROR("exec default failed: %s", lua_tostring(g_lua_state, -1));
                lua_pop(g_lua_state, 1);
            } else {
                LOG_DEBUG("default OK");
            }
        }
    }

    /* 加载 main.lua/main.luac */
    if(!iot_load_lua_file(g_lua_state, MAIN_LUA_PATH))
    {
        LOG_ERROR("load main failed");
        return;
    }

    /* 运行 iot.run() */
    LOG_DEBUG("run iot.run()");
    if (luaL_loadstring(g_lua_state, "iot.run()") != LUA_OK) {
        LOG_ERROR("load iot.run() failed: %s", lua_tostring(g_lua_state, -1));
        return;
    }
    
    /* 执行 iot.run() */
    if (lua_pcall(g_lua_state, 0, 0, 0) != LUA_OK) {
        LOG_ERROR("exec iot.run() failed: %s", lua_tostring(g_lua_state, -1));
        return;
    }

    LOG_DEBUG("exiting");
    lua_close(g_lua_state);
    g_lua_state = NULL;
}

/**
 * @brief 启动IoT Lua应用
 */
bool iot_start(void)
{
    LOG_INFO("start");

    iot_task_t tid = iot_task_create("lua", iot_lua_task, NULL, LUA_STACK_SIZE, IOT_OS_PRIO_NORMAL);
    return (tid != NULL);
}
