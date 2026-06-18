/**
 * @file iot_camera.c
 * @brief 相机模块Lua封装
 * @version 1.0
 * @date 2026.06.10
 * 
 * @example
 * -- Lua使用示例
 * local camera = require("camera")
 * 
 * -- 打开相机
 * camera.open()
 * 
 * -- 获取相机信息
 * local info = camera.info()
 * print("分辨率:", info.width, "x", info.height)
 * 
 * -- 方式一：拍照保存到文件
 * camera.capture("/app/photo.jpg")
 * 
 * -- 方式二：拍照获取数据
 * local data = camera.capture()
 * if data then
 *     -- 处理图片数据
 *     print("图片大小:", #data, "bytes")
 * end
 * 
 * -- 方式三：预览模式
 * camera.preview(function(data)
 *     -- 处理预览数据
 *     print("预览数据:", #data, "bytes")
 * end)
 * 
 * -- 关闭预览
 * camera.stop()
 * 
 * -- 关闭相机
 * camera.close()
 */

#include "iot_camera.h"
#include "cm_camera.h"
#include "lua.h"
#include "lauxlib.h"

/* 输出格式常量 */
#define FMT_YUV422_YUYV    0
#define FMT_YUV422_YVYU    1
#define FMT_YUV422_UYVY    2
#define FMT_YUV422_VYUY    3
#define FMT_YUV420_NV12    6
#define FMT_RAW8          10
#define FMT_RGB565        11
#define FMT_JPEG          12

/* Lua回调函数存储 */
static lua_State *camera_L = NULL;
static int camera_callback_ref = LUA_NOREF;

/**
 * @brief 预览数据回调函数
 */
static void camera_preview_callback(void *pBuf, uint32_t len)
{
    if (camera_L && camera_callback_ref != LUA_NOREF)
    {
        lua_rawgeti(camera_L, LUA_REGISTRYINDEX, camera_callback_ref);
        if (lua_isfunction(camera_L, -1))
        {
            lua_pushlstring(camera_L, (const char*)pBuf, len);
            lua_pcall(camera_L, 1, 0, 0);
        }
        lua_pop(camera_L, 1);
    }
}

/**
 * @brief 预览关闭回调函数
 */
static void camera_flush_callback(void)
{
    if (camera_L && camera_callback_ref != LUA_NOREF)
    {
        lua_rawgeti(camera_L, LUA_REGISTRYINDEX, camera_callback_ref);
        if (lua_isfunction(camera_L, -1))
        {
            lua_pushnil(camera_L);
            lua_pcall(camera_L, 1, 0, 0);
        }
        lua_pop(camera_L, 1);
    }
}

/**
 * @brief 打开相机
 * camera.open()
 */
static int l_camera_open(lua_State *L)
{
    int32_t ret = cm_camera_open();
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 关闭相机
 * camera.close()
 */
static int l_camera_close(lua_State *L)
{
    /* 清理回调 */
    if (camera_callback_ref != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, camera_callback_ref);
        camera_callback_ref = LUA_NOREF;
    }
    
    int32_t ret = cm_camera_close();
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取相机信息
 * camera.info()
 * 返回: table包含width, height
 */
static int l_camera_info(lua_State *L)
{
    uint32_t width = 0, height = 0;
    int32_t ret = cm_camera_get_info(&width, &height);
    
    if (ret == 0)
    {
        lua_newtable(L);
        
        lua_pushstring(L, "width");
        lua_pushinteger(L, width);
        lua_settable(L, -3);
        
        lua_pushstring(L, "height");
        lua_pushinteger(L, height);
        lua_settable(L, -3);
    }
    else
    {
        lua_pushnil(L);
        lua_pushstring(L, "get info failed");
        return 2;
    }
    
    return 1;
}

/**
 * @brief 拍照
 * camera.capture([filename])
 * - filename: 可选，保存到文件路径
 * 返回: 如果没有filename，返回图片数据；否则返回布尔值
 */
static int l_camera_capture(lua_State *L)
{
    const char *filename = luaL_optstring(L, 1, NULL);
    
    cm_camera_capture_cfg_t cfg = {0};
    
    if (filename)
    {
        /* 保存到文件 */
        cfg.recordFormat = CAM_OUT_FMT_JPEG;
        cfg.file_name = (uint8_t*)filename;
        cfg.image.buf_size = 0;
        
        int32_t ret = cm_camera_capture_take(&cfg);
        lua_pushboolean(L, ret == 0);
    }
    else
    {
        /* 返回数据到内存 */
        cfg.recordFormat = CAM_OUT_FMT_JPEG;
        cfg.file_name = NULL;
        cfg.image.buf_size = 0;  /* 由驱动分配内存 */
        
        int32_t ret = cm_camera_capture_take(&cfg);
        
        if (ret == 0 && cfg.image.p_buf && cfg.image.buf_size > 0)
        {
            lua_pushlstring(L, (const char*)cfg.image.p_buf, cfg.image.buf_size);
            /* 注意：根据cm_camera.h说明，使用完数据后需要free */
            free(cfg.image.p_buf);
        }
        else
        {
            lua_pushnil(L);
            lua_pushstring(L, "capture failed");
            return 2;
        }
    }
    
    return 1;
}

/**
 * @brief 开始预览
 * camera.preview(callback)
 * - callback(data): 预览数据回调函数
 */
static int l_camera_preview(lua_State *L)
{
    if (lua_isfunction(L, 1))
    {
        /* 保存回调函数引用 */
        if (camera_callback_ref != LUA_NOREF)
            luaL_unref(L, LUA_REGISTRYINDEX, camera_callback_ref);
        
        lua_pushvalue(L, 1);
        camera_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        camera_L = L;
        
        /* 配置预览 */
        cm_camera_preview_cfg_t cfg = {0};
        cfg.recordFormat = CAM_OUT_FMT_JPEG;
        cfg.enqueueCamRecordBuffer = camera_preview_callback;
        cfg.flushCamRecordBuffers = camera_flush_callback;
        
        int32_t ret = cm_camera_preview_start(&cfg);
        
        lua_pushboolean(L, ret == 0);
    }
    else
    {
        lua_pushnil(L);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }
    
    return 1;
}

/**
 * @brief 停止预览
 * camera.stop()
 */
static int l_camera_stop(lua_State *L)
{
    int32_t ret = cm_camera_preview_stop();
    
    /* 清理回调 */
    if (camera_callback_ref != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, camera_callback_ref);
        camera_callback_ref = LUA_NOREF;
    }
    
    lua_pushboolean(L, ret == 0);
    return 1;
}

/* 模块函数列表 */
static const luaL_Reg camera_lib[] = {
    {"open", l_camera_open},
    {"close", l_camera_close},
    {"info", l_camera_info},
    {"capture", l_camera_capture},
    {"preview", l_camera_preview},
    {"stop", l_camera_stop},
    {NULL, NULL}
};

/* Lua模块入口 */
LUAMOD_API int luaopen_camera(lua_State *L)
{
    /* 初始化回调引用 */
    camera_callback_ref = LUA_NOREF;
    
    /* 创建模块表 */
    luaL_newlibtable(L, camera_lib);
    luaL_setfuncs(L, camera_lib, 0);
    
    /* 添加常量 */
    lua_pushinteger(L, FMT_YUV422_YUYV);
    lua_setfield(L, -2, "FMT_YUV422_YUYV");
    
    lua_pushinteger(L, FMT_YUV422_YVYU);
    lua_setfield(L, -2, "FMT_YUV422_YVYU");
    
    lua_pushinteger(L, FMT_YUV420_NV12);
    lua_setfield(L, -2, "FMT_YUV420_NV12");
    
    lua_pushinteger(L, FMT_RGB565);
    lua_setfield(L, -2, "FMT_RGB565");
    
    lua_pushinteger(L, FMT_JPEG);
    lua_setfield(L, -2, "FMT_JPEG");
    
    return 1;
}
