/*
@module  at
@summary AT指令
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     AT
*/

/*
AT参考示例
-- 同步发送AT指令
local rsp = at.send("AT+CFUN=1", 10)
if rsp then
    print("response:", rsp)
end

-- 注册URC回调
at.on("+CMT", function(urc)
    print("received urc:", urc)
end)

-- 取消注册URC回调
at.off("+CMT")
*/


#include "lua.h"
#include "module.h"
#include "iot_rtos.h"
#include "cm_virt_at.h"

#define AT_URC_MAX 16
typedef struct {
    const char* prefix;
    void* callback_ud;
} at_urc_entry_t;

static at_urc_entry_t at_urc_table[AT_URC_MAX] = {0};
static int at_urc_count = 0;

static void at_urc_callback(uint8_t* urc, int32_t urc_len) {
    for (int i = 0; i < at_urc_count; i++) {
        if (at_urc_table[i].prefix && strncmp((char*)urc, at_urc_table[i].prefix, strlen(at_urc_table[i].prefix)) == 0) {
            if (at_urc_table[i].callback_ud) {
                params_t* params = params_create(1);
                if (!params) {
                    LOG("ERR params_create failed");
                    continue;
                }
                params_push_string(params, (const char*)urc, urc_len);
                iot_rtos_call(at_urc_table[i].callback_ud, params);
            }
        }
    }
}

static void at_async_callback(cm_virt_at_param_t* param) {
    if (!param->user_param) return;
    
    void* ud = (void*)param->user_param;
    params_t* params = params_create(1);
    if (!params) {
        LOG("ERR params_create failed");
        return;
    }
    
    if (param->rsp) {
        params_push_string(params, (const char*)param->rsp, param->rsp_len);
    } else {
        params_push_nil(params);
    }
    
    iot_rtos_call(ud, params);
}

/**
 * @brief 同步发送AT指令
 * @api at.send(cmd, timeout)
 * @string cmd AT指令
 * @int timeout 超时时间(毫秒)
 * @return string 响应内容
 * @usage
local rsp = at.send("AT+CFUN=1", 10)
*/
static int iot_at_send(lua_State* L) {
    size_t cmd_len;
    const char* cmd = luaL_checklstring(L, 1, &cmd_len);
    uint32_t timeout = (uint32_t)luaL_optinteger(L, 2, 10);
    
    char at_cmd[256];
    snprintf(at_cmd, sizeof(at_cmd), "%.*s\r\n", (int)cmd_len, cmd);
    
    uint8_t rsp[512];
    int32_t rsp_len = 0;
    
    if (cm_virt_at_send_sync((const uint8_t*)at_cmd, rsp, &rsp_len, timeout) == 0) {
        lua_pushlstring(L, (char*)rsp, rsp_len);
        return 1;
    }
    
    lua_pushnil(L);
    return 1;
}

/**
 * @brief 异步发送AT指令
 * @api at.sendAsync(cmd, callback)
 * @string cmd AT指令
 * @function callback 回调函数
 * @return int 0成功, 其他失败
 * @usage
at.sendAsync("AT+COPS=?", function(rsp)
    print("response:", rsp)
end)
*/
static int iot_at_sendAsync(lua_State* L) {
    size_t cmd_len;
    const char* cmd = luaL_checklstring(L, 1, &cmd_len);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    char at_cmd[256];
    snprintf(at_cmd, sizeof(at_cmd), "%.*s\r\n", (int)cmd_len, cmd);
    
    /* 保存回调函数 */
    void* ud = lua_newuserdata(L, 1);
    lua_pushvalue(L, 2);
    lua_setuservalue(L, -2);

    lua_pushlightuserdata(L, ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pop(L, 1);
    
    if (cm_virt_at_send_async((const uint8_t*)at_cmd, at_async_callback, ud) == 0) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushlightuserdata(L, ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 注册URC回调
 * @api at.on(prefix, callback)
 * @string prefix URC前缀
 * @function callback 回调函数
 * @return int 0成功, 其他失败
 * @usage
at.on("+CMT", function(urc)
    print("received urc:", urc)
end)
*/
static int iot_at_on(lua_State* L) {
    size_t prefix_len;
    const char* prefix = luaL_checklstring(L, 1, &prefix_len);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    if (at_urc_count >= AT_URC_MAX) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    /* 创建userdata保存回调 */
    void* ud = lua_newuserdata(L, 1);
    lua_pushvalue(L, 2);
    lua_setuservalue(L, -2);

    lua_pushlightuserdata(L, ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pop(L, 1);
    
    for (int i = 0; i < at_urc_count; i++) {
        if (at_urc_table[i].prefix && strcmp(at_urc_table[i].prefix, prefix) == 0) {
            if (at_urc_table[i].callback_ud) {
                lua_pushlightuserdata(L, at_urc_table[i].callback_ud);
                lua_pushnil(L);
                lua_settable(L, LUA_REGISTRYINDEX);
            }
            at_urc_table[i].callback_ud = ud;
            lua_pushinteger(L, 0);
            return 1;
        }
    }
    
    at_urc_table[at_urc_count].prefix = prefix;
    at_urc_table[at_urc_count].callback_ud = ud;
    
    if (cm_virt_at_urc_cb_reg((const uint8_t*)prefix, at_urc_callback) == 0) {
        at_urc_count++;
        lua_pushinteger(L, 0);
    } else {
        lua_pushlightuserdata(L, at_urc_table[at_urc_count].callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        at_urc_table[at_urc_count].prefix = NULL;
        at_urc_table[at_urc_count].callback_ud = NULL;
        lua_pushinteger(L, -1);
    }
    return 1;
}

/**
 * @brief 取消注册URC回调
 * @api at.off(prefix)
 * @string prefix URC前缀
 * @return int 0成功, 其他失败
 * @usage
at.off("+CMT")
*/
static int iot_at_off(lua_State* L) {
    const char* prefix = luaL_checkstring(L, 1);
    
    for (int i = 0; i < at_urc_count; i++) {
        if (at_urc_table[i].prefix && strcmp(at_urc_table[i].prefix, prefix) == 0) {
            if (cm_virt_at_urc_cb_dereg((const uint8_t*)prefix, at_urc_callback) == 0) {
                if (at_urc_table[i].callback_ud) {
                    lua_pushlightuserdata(L, at_urc_table[i].callback_ud);
                    lua_pushnil(L);
                    lua_settable(L, LUA_REGISTRYINDEX);
                }
                at_urc_table[i].prefix = NULL;
                at_urc_table[i].callback_ud = NULL;
                
                for (int j = i; j < at_urc_count - 1; j++) {
                    at_urc_table[j] = at_urc_table[j + 1];
                }
                at_urc_count--;
                at_urc_table[at_urc_count].prefix = NULL;
                at_urc_table[at_urc_count].callback_ud = NULL;
                
                lua_pushinteger(L, 0);
                return 1;
            }
        }
    }
    
    lua_pushinteger(L, -1);
    return 1;
}

static const luaL_Reg at_lib[] = {
    { "send",    iot_at_send },
    { "sendAsync", iot_at_sendAsync },
    { "on",      iot_at_on },
    { "off",     iot_at_off },
    {NULL, NULL}
};

LUAMOD_API int luaopen_at(lua_State* L) {
    luaL_newlibtable(L, at_lib);
    luaL_setfuncs(L, at_lib, 0);
    return 1;
}
