/*
@module  ntp
@summary NTP网络时间同步
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     NTP
*/

/*
NTP参考示例
-- 使用默认配置同步时间
ntp.sync(function(ok, time)
    if ok then
        print("NTP同步成功, 时间:", time)
    else
        print("NTP同步失败")
    end
end)

-- 使用自定义配置
ntp.set({
    server = "ntp1.aliyun.com",
    port = 123,
    timeout = 6000,
    dns = 1,
    sync_rtc = true
})
ntp.sync(function(ok, time)
    if ok then
        print("NTP同步成功, 时间:", time)
        -- 获取RTC时间验证
        local r = rtc.get_time()
        print("RTC时间:", r)
    else
        print("NTP同步失败")
    end
end)

-- 仅获取时间字符串,不同步到RTC
ntp.set({sync_rtc = false})
ntp.sync(function(ok, time_str)
    if ok then
        print("网络时间:", time_str)
    end
end)
*/


#include "lua.h"
#include "module.h"
#include "cm_ntp.h"
#include "iot_callback.h"
#include "iot_rtos.h"

/* NTP回调相关 */
static void* g_ntp_callback_ud = NULL;

/* NTP事件回调 */
static void ntp_event_cb(cm_ntp_event_e event, void *event_param, void *cb_param) {
    if (!g_ntp_callback_ud) {
        return;
    }

    params_t* params = params_create(2);
    if (!params) {
        return;
    }

    /* 推送参数: ok, time */
    if (event == CM_NTP_EVENT_SYNC_OK || event == CM_NTP_EVENT_SETTIME_FAIL) {
        params_push_bool(params, 1);
        params_push_string(params, (const char *)event_param,
            event_param ? strlen((const char *)event_param) : 0);
    } else {
        params_push_bool(params, 0);
        params_push_nil(params);
    }

    /* 调用回调 */
    iot_rtos_call(g_ntp_callback_ud, params);
}

/**
 * @brief 设置NTP配置
 * @api ntp.set(cfg)
 * @table cfg 配置表
 * @string cfg.server NTP服务器地址
 * @int cfg.port 端口号,默认123
 * @int cfg.timeout 超时时间(毫秒),范围1000~10000,默认3000
 * @int cfg.dns DNS优先级,0使用全局优先级,1:v4优先,2:v6优先
 * @bool cfg.sync_rtc 是否同步到RTC,默认true
 * @return bool 成功返回true,失败返回false
 * @usage
ntp.set({server="ntp1.aliyun.com", port=123, timeout=6000, sync_rtc=true})
*/
static int iot_ntp_set(lua_State *L) {
    if (!lua_istable(L, 1)) {
        return luaL_argerror(L, 1, "table expected");
    }

    const char* server = NULL;
    uint16_t port = 123;

    /* server */
    lua_getfield(L, 1, "server");
    if (!lua_isnil(L, -1)) {
        server = lua_tostring(L, -1);
        cm_ntp_set_cfg(CM_NTP_CFG_SERVER, (void *)server);
    }
    lua_pop(L, 1);

    /* port */
    lua_getfield(L, 1, "port");
    if (!lua_isnil(L, -1)) {
        port = lua_tointeger(L, -1);
        cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port);
    }
    lua_pop(L, 1);

    /* timeout */
    lua_getfield(L, 1, "timeout");
    if (!lua_isnil(L, -1)) {
        uint32_t timeout = lua_tointeger(L, -1);
        cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout);
    }
    lua_pop(L, 1);

    /* dns */
    lua_getfield(L, 1, "dns");
    if (!lua_isnil(L, -1)) {
        uint32_t dns = lua_tointeger(L, -1);
        cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns);
    }
    lua_pop(L, 1);

    /* sync_rtc */
    lua_getfield(L, 1, "sync_rtc");
    if (!lua_isnil(L, -1)) {
        int sync = lua_toboolean(L, -1);
        cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &sync);
    }
    lua_pop(L, 1);

    LOG("set server=%s port=%d", server ? server : "default", port);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 执行NTP时间同步
 * @api ntp.sync(callback)
 * @function callback 回调函数,回调格式 function(ok, time_str)
 * @return bool 成功返回true,失败返回false
 * @usage
ntp.sync(function(ok, time)
    if ok then print("时间:", time) end
end)
*/
static int iot_ntp_sync(lua_State *L) {
    if (lua_isfunction(L, 1)) {
        /* 释放旧的回调 */
        if (g_ntp_callback_ud) {
            iot_callback_free(g_ntp_callback_ud);
            g_ntp_callback_ud = NULL;
        }

        /* 保存新的回调 */
        g_ntp_callback_ud = iot_callback_save(L, 1);

        /* 设置回调 */
        cm_ntp_set_cfg(CM_NTP_CFG_CB, ntp_event_cb);
        cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, NULL);
    }

    LOG("sync");
    int ret = cm_ntp_sync();
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg ntp_lib[] = {
    { "config", iot_ntp_set },
    { "sync",  iot_ntp_sync },
    {NULL, NULL}
};

LUAMOD_API int luaopen_ntp(lua_State *L) {
    luaL_newlibtable(L, ntp_lib);
    luaL_setfuncs(L, ntp_lib, 0);
    return 1;
}
