/*
@module  fota
@summary 固件升级
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     FOTA
*/

/*
FOTA参考示例
-- 设置FOTA升级URL
local ret = fota.set_url("http://example.com/firmware.bin")
print("set_url ret:", ret)

-- 设置重启时间(分钟)
fota.set_reboot_time({
    first_timeout = 5,
    second_timeout = 10,
    third_timeout = 20,
    fourth_timeout = 30,
    fifth_timeout = 40,
    sixth_timeout = 50,
    constant_timeout = 120
})

-- 获取FOTA配置
local cfg = fota.get_config()
print("fota mode:", cfg.fota_mode)
print("url:", cfg.url)

-- 注册回调
fota.on_result(function(error)
    print("fota error:", error)
    if error == 0 then
        print("fota success")
    end
end)

-- 执行升级
local ret = fota.upgrade()
print("upgrade ret:", ret)
*/


#include "lua.h"
#include "module.h"
#include "cm_fota.h"
#include "iot_callback.h"

/* FOTA回调函数 */
static void* g_fota_result_callback_ud = NULL;

static void fota_result_callback_wrapper(cm_fota_error_e error) {
    if (!g_fota_result_callback_ud) {
        return;
    }

    params_t* params = params_create(1);
    if (params) {
        params_push_int(params, error);
        iot_rtos_call(g_fota_result_callback_ud, params);
    }
}

/**
 * @brief 设置FOTA服务器URL
 * @api fota.set_url(url)
 * @string url 服务器URL
 * @return int 0成功,其他失败
 * @usage
fota.set_url("http://example.com/firmware.bin")
*/
static int iot_fota_set_url(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    LOG_INFO("set_url url=%s", url);
    int ret = cm_fota_set_url((char*)url);
    if (ret != 0) {
        LOG_INFO("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 设置FOTA失败重启时间
 * @api fota.set_reboot_time(time)
 * @table time 重启时间配置
 * @return int 0成功,其他失败
 * @usage
fota.set_reboot_time({
    first_timeout = 5,
    second_timeout = 10,
    third_timeout = 20
})
*/
static int iot_fota_set_reboot_time(lua_State* L) {
    cm_fota_reboot_time time;
    memset(&time, 0, sizeof(time));

    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "first_timeout");
        time.first_timeout = lua_isnil(L, -1) ? 5 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "second_timeout");
        time.second_timeout = lua_isnil(L, -1) ? 10 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "third_timeout");
        time.third_timeout = lua_isnil(L, -1) ? 20 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "fourth_timeout");
        time.fourth_timeout = lua_isnil(L, -1) ? 30 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "fifth_timeout");
        time.fifth_timeout = lua_isnil(L, -1) ? 40 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "sixth_timeout");
        time.sixth_timeout = lua_isnil(L, -1) ? 50 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 1, "constant_timeout");
        time.constant_timeout = lua_isnil(L, -1) ? 120 : lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    LOG_INFO("set_reboot_time first=%d second=%d third=%d", time.first_timeout, time.second_timeout, time.third_timeout);
    int ret = cm_fota_set_reboot_time(&time);
    if (ret != 0) {
        LOG_INFO("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 获取FOTA配置
 * @api fota.get_config()
 * @return table FOTA配置信息
 * @usage
local cfg = fota.get_config()
print("mode:", cfg.fota_mode, "url:", cfg.url)
*/
static int iot_fota_get_config(lua_State* L) {
    cm_fota_info_t info;
    memset(&info, 0, sizeof(info));

    cm_fota_read_config(&info);

    lua_createtable(L, 0, 4);
    lua_pushinteger(L, info.fixed_info.fota_mode);
    lua_setfield(L, -2, "fota_mode");
    lua_pushstring(L, info.fixed_info.url);
    lua_setfield(L, -2, "url");
    lua_pushinteger(L, info.fixed_info.stage);
    lua_setfield(L, -2, "stage");

    return 1;
}

/**
 * @brief 注册FOTA结果回调
 * @api fota.on_result(callback)
 * @func callback 回调函数
 * @return nil
 * @usage
fota.on_result(function(error)
    print("fota error:", error)
end)
*/
static int iot_fota_on_result(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        return 0;
    }

    /* 释放旧的回调 */
    if (g_fota_result_callback_ud) {
        iot_callback_free(g_fota_result_callback_ud);
        g_fota_result_callback_ud = NULL;
    }

    /* 保存新的回调 */
    g_fota_result_callback_ud = iot_callback_save(L, 1);

    cm_fota_res_callback_register(fota_result_callback_wrapper);
    return 0;
}

/**
 * @brief 执行FOTA升级
 * @api fota.upgrade()
 * @return int 0成功,其他失败
 * @usage
local ret = fota.upgrade()
*/
static int iot_fota_upgrade(lua_State* L) {
    LOG_INFO("upgrade");
    int ret = cm_fota_exec_upgrade();
    if (ret != 0) {
        LOG_INFO("ERR ret=%d", ret);
    }
    lua_pushinteger(L, ret);
    return 1;
}

static const luaL_Reg fota_lib[] = {
    { "set_url",       iot_fota_set_url },
    { "set_reboot_time", iot_fota_set_reboot_time },
    { "get_config",    iot_fota_get_config },
    { "on_result",     iot_fota_on_result },
    { "upgrade",       iot_fota_upgrade },
    {NULL, NULL}
};

LUAMOD_API int luaopen_fota(lua_State* L) {
    luaL_newlibtable(L, fota_lib);
    luaL_setfuncs(L, fota_lib, 0);
    return 1;
}
