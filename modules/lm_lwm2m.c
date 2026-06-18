/*
@module  lwm2m
@summary LwM2M物联网协议模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     LWM2M
*/

/*
LwM2M参考示例
local lwm2m = require("lwm2m")

-- 创建LwM2M实例
local client = lwm2m.create({
    platform = lwm2m.ONENET,      -- 平台类型
    host = "183.230.40.39:5683", -- 服务器地址
    auth_code = "your_auth_code"  -- 认证码
})

-- 添加object
client:add_obj(3200, {1, 0, 0, 1, 0})  -- 添加5个实例，其中0和3可用

-- 设置资源列表
client:discover(3200, {5500, 5501, 5750})

-- 注册事件回调
client:on("event", function(event, data)
    print("LwM2M event:", event)
    if event == lwm2m.EVENT_REG_SUCCESS then
        print("注册成功")
        -- 上报数据
        client:pack(3200, 0, 5500, lwm2m.TYPE_INTEGER, "25")
        client:notify()
    end
end)

-- 登录平台
client:open(30, 86400)  -- 超时30s，生命周期86400s

-- 平台下发写指令回调
client:on("write", function(mid, objid, insid, resid, data)
    print("write:", objid, insid, resid, data)
    client:write_rsp(mid, 2)  -- 响应成功
end)

-- 平台下发执行指令回调
client:on("execute", function(mid, objid, insid, resid, data)
    print("execute:", objid, insid, resid, data)
    client:execute_rsp(mid, 2)  -- 响应成功
end)
*/

#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "cm_lwm2m.h"

#define LWM2M_HANDLE_METATABLE "lwm2m_handle_mt"

static int mid_counter = 0;

static int lwm2m_create(lua_State* L);
static int lwm2m_delete(lua_State* L);
static int lwm2m_add_obj(lua_State* L);
static int lwm2m_del_obj(lua_State* L);
static int lwm2m_discover(lua_State* L);
static int lwm2m_open(lua_State* L);
static int lwm2m_update(lua_State* L);
static int lwm2m_close(lua_State* L);
static int lwm2m_pack(lua_State* L);
static int lwm2m_notify(lua_State* L);
static int lwm2m_read_rsp(lua_State* L);
static int lwm2m_write_rsp(lua_State* L);
static int lwm2m_execute_rsp(lua_State* L);
static int lwm2m_observe_rsp(lua_State* L);
static int lwm2m_param_rsp(lua_State* L);
static int lwm2m_on(lua_State* L);

static const luaL_Reg lwm2m_lib[] = {
    {"create", lwm2m_create},
    {NULL, NULL}
};

static const luaL_Reg lwm2m_methods[] = {
    {"delete",      lwm2m_delete},
    {"add_obj",     lwm2m_add_obj},
    {"del_obj",     lwm2m_del_obj},
    {"discover",    lwm2m_discover},
    {"open",        lwm2m_open},
    {"update",      lwm2m_update},
    {"close",       lwm2m_close},
    {"pack",        lwm2m_pack},
    {"notify",      lwm2m_notify},
    {"read_rsp",    lwm2m_read_rsp},
    {"write_rsp",   lwm2m_write_rsp},
    {"execute_rsp", lwm2m_execute_rsp},
    {"observe_rsp", lwm2m_observe_rsp},
    {"param_rsp",   lwm2m_param_rsp},
    {"on",          lwm2m_on},
    {NULL, NULL}
};

typedef struct {
    cm_lwm2m_handle_t handle;
    lua_State* L;
    int event_cb_ref;
    int read_cb_ref;
    int write_cb_ref;
    int execute_cb_ref;
    int observe_cb_ref;
} lwm2m_client_t;

static void lwm2m_event_callback(int32_t event, cm_lwm2m_cb_param_t param) {
    lwm2m_client_t* client = (lwm2m_client_t*)param.cb_param;
    if (!client || !client->L || client->event_cb_ref == LUA_NOREF) return;
    
    lua_State* L = client->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, client->event_cb_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_pushinteger(L, event);
    lua_pushlightuserdata(L, client);
    
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        LOG("lwm2m event callback error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static void lwm2m_read_callback(int32_t mid, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param) {
    lwm2m_client_t* client = (lwm2m_client_t*)param.cb_param;
    if (!client || !client->L || client->read_cb_ref == LUA_NOREF) return;
    
    lua_State* L = client->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, client->read_cb_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_pushinteger(L, mid);
    lua_pushinteger(L, objid);
    lua_pushinteger(L, insid);
    lua_pushinteger(L, resid);
    lua_pushlightuserdata(L, client);
    
    if (lua_pcall(L, 5, 0, 0) != LUA_OK) {
        LOG("lwm2m read callback error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static void lwm2m_write_callback(int32_t mid, int32_t objid, int32_t insid, int32_t resid, int32_t type, int32_t is_over, char *data, int32_t len, cm_lwm2m_cb_param_t param) {
    lwm2m_client_t* client = (lwm2m_client_t*)param.cb_param;
    if (!client || !client->L || client->write_cb_ref == LUA_NOREF) return;
    
    lua_State* L = client->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, client->write_cb_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_pushinteger(L, mid);
    lua_pushinteger(L, objid);
    lua_pushinteger(L, insid);
    lua_pushinteger(L, resid);
    lua_pushinteger(L, type);
    lua_pushinteger(L, is_over);
    lua_pushlstring(L, data, len);
    lua_pushlightuserdata(L, client);
    
    if (lua_pcall(L, 8, 0, 0) != LUA_OK) {
        LOG("lwm2m write callback error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static void lwm2m_execute_callback(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *data, int32_t len, cm_lwm2m_cb_param_t param) {
    lwm2m_client_t* client = (lwm2m_client_t*)param.cb_param;
    if (!client || !client->L || client->execute_cb_ref == LUA_NOREF) return;
    
    lua_State* L = client->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, client->execute_cb_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_pushinteger(L, mid);
    lua_pushinteger(L, objid);
    lua_pushinteger(L, insid);
    lua_pushinteger(L, resid);
    lua_pushlstring(L, data, len);
    lua_pushlightuserdata(L, client);
    
    if (lua_pcall(L, 6, 0, 0) != LUA_OK) {
        LOG("lwm2m execute callback error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

static void lwm2m_observe_callback(int32_t mid, int32_t observe, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param) {
    lwm2m_client_t* client = (lwm2m_client_t*)param.cb_param;
    if (!client || !client->L || client->observe_cb_ref == LUA_NOREF) return;
    
    lua_State* L = client->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, client->observe_cb_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_pushinteger(L, mid);
    lua_pushinteger(L, observe);
    lua_pushinteger(L, objid);
    lua_pushinteger(L, insid);
    lua_pushinteger(L, resid);
    lua_pushlightuserdata(L, client);
    
    if (lua_pcall(L, 6, 0, 0) != LUA_OK) {
        LOG("lwm2m observe callback error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

/**
 * @brief 创建LwM2M客户端
 * @api lwm2m.create(config)
 * @param config table 配置参数
 * @return userdata LwM2M客户端对象
 * @usage
 * local client = lwm2m.create({
 *     platform = lwm2m.ONENET,
 *     host = "183.230.40.39:5683",
 *     auth_code = "your_auth_code"
 * })
 */
static int lwm2m_create(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    
    lwm2m_client_t* client = (lwm2m_client_t*)cm_malloc(sizeof(lwm2m_client_t));
    if (!client) {
        lua_pushnil(L);
        return 1;
    }
    
    memset(client, 0, sizeof(lwm2m_client_t));
    client->L = L;
    client->event_cb_ref = LUA_NOREF;
    client->read_cb_ref = LUA_NOREF;
    client->write_cb_ref = LUA_NOREF;
    client->execute_cb_ref = LUA_NOREF;
    client->observe_cb_ref = LUA_NOREF;
    
    cm_lwm2m_cfg_t cfg;
    memset(&cfg, 0, sizeof(cm_lwm2m_cfg_t));
    
    lua_getfield(L, 1, "platform");
    cfg.platform = lua_isnumber(L, -1) ? (cm_lwm2m_platform_e)lua_tointeger(L, -1) : CM_LWM2M_ONENET;
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "host");
    if (lua_isstring(L, -1)) {
        cfg.host = (char*)cm_malloc(strlen(lua_tostring(L, -1)) + 1);
        strcpy((char*)cfg.host, lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "auth_code");
    if (lua_isstring(L, -1)) {
        cfg.auth_code = (char*)cm_malloc(strlen(lua_tostring(L, -1)) + 1);
        strcpy((char*)cfg.auth_code, lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "psk");
    if (lua_isstring(L, -1)) {
        cfg.psk = (char*)cm_malloc(strlen(lua_tostring(L, -1)) + 1);
        strcpy((char*)cfg.psk, lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "pskid");
    if (lua_isstring(L, -1)) {
        cfg.pskid = (char*)cm_malloc(strlen(lua_tostring(L, -1)) + 1);
        strcpy((char*)cfg.pskid, lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "pattern");
    cfg.endpoint.pattern = lua_isnumber(L, -1) ? lua_tointeger(L, -1) : 2;
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "auto_update");
    cfg.auto_update = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : 0;
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "flag");
    cfg.flag = lua_isnumber(L, -1) ? lua_tointeger(L, -1) : 0;
    lua_pop(L, 1);
    
    cfg.cb.onEvent = lwm2m_event_callback;
    cfg.cb.onRead = lwm2m_read_callback;
    cfg.cb.onWrite = lwm2m_write_callback;
    cfg.cb.onExec = lwm2m_execute_callback;
    cfg.cb.onObserve = lwm2m_observe_callback;
    cfg.cb_param = client;
    
    int32_t ret = cm_lwm2m_create(&cfg, &client->handle);
    
    if (cfg.host) cm_free(cfg.host);
    if (cfg.auth_code) cm_free(cfg.auth_code);
    if (cfg.psk) cm_free(cfg.psk);
    if (cfg.pskid) cm_free(cfg.pskid);
    
    if (ret != 0) {
        cm_free(client);
        lua_pushnil(L);
        lua_pushinteger(L, ret);
        return 2;
    }
    
    luaL_getmetatable(L, LWM2M_HANDLE_METATABLE);
    lua_setmetatable(L, -2);
    
    lua_pushlightuserdata(L, client);
    return 1;
}

static lwm2m_client_t* lwm2m_check_client(lua_State* L) {
    void* ud = luaL_checkudata(L, 1, LWM2M_HANDLE_METATABLE);
    luaL_argcheck(L, ud != NULL, 1, "lwm2m client expected");
    return (lwm2m_client_t*)ud;
}

/**
 * @brief 删除LwM2M客户端
 * @api client:delete()
 * @return bool 成功返回true
 */
static int lwm2m_delete(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    
    cm_lwm2m_delete(client->handle);
    
    if (client->event_cb_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, client->event_cb_ref);
    }
    if (client->read_cb_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, client->read_cb_ref);
    }
    if (client->write_cb_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, client->write_cb_ref);
    }
    if (client->execute_cb_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, client->execute_cb_ref);
    }
    if (client->observe_cb_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, client->observe_cb_ref);
    }
    
    cm_free(client);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 添加object
 * @api client:add_obj(objid, instances)
 * @param objid int Object ID
 * @param instances table 实例数组，1表示可用，0表示不可用
 * @return bool 成功返回true
 */
static int lwm2m_add_obj(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t objid = luaL_checkinteger(L, 2);
    
    luaL_checktype(L, 3, LUA_TTABLE);
    int inscount = lua_rawlen(L, 3);
    uint8_t* instances = (uint8_t*)cm_malloc(inscount);
    if (!instances) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    for (int i = 0; i < inscount; i++) {
        lua_rawgeti(L, 3, i + 1);
        instances[i] = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    
    int32_t ret = cm_lwm2m_add_obj(client->handle, objid, instances, inscount, 0, 0);
    cm_free(instances);
    
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 删除object
 * @api client:del_obj(objid)
 * @param objid int Object ID
 * @return bool 成功返回true
 */
static int lwm2m_del_obj(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t objid = luaL_checkinteger(L, 2);
    
    int32_t ret = cm_lwm2m_del_obj(client->handle, objid);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 设置资源列表
 * @api client:discover(objid, resources)
 * @param objid int Object ID
 * @param resources table 资源ID数组
 * @return bool 成功返回true
 */
static int lwm2m_discover(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t objid = luaL_checkinteger(L, 2);
    
    luaL_checktype(L, 3, LUA_TTABLE);
    int rescount = lua_rawlen(L, 3);
    int32_t* resources = (int32_t*)cm_malloc(rescount * sizeof(int32_t));
    if (!resources) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    for (int i = 0; i < rescount; i++) {
        lua_rawgeti(L, 3, i + 1);
        resources[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    
    int32_t ret = cm_lwm2m_discover(client->handle, objid, resources, rescount);
    cm_free(resources);
    
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 登录平台
 * @api client:open(timeout, lifetime)
 * @param timeout int 超时时间(秒)
 * @param lifetime int 生命周期(秒)
 * @return bool 成功返回true
 */
static int lwm2m_open(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    uint32_t timeout = luaL_checkinteger(L, 2);
    uint32_t lifetime = luaL_checkinteger(L, 3);
    
    int32_t ret = cm_lwm2m_open(client->handle, timeout, lifetime);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 更新生命周期
 * @api client:update(lifetime, update_object)
 * @param lifetime int 生命周期(秒)
 * @param update_object bool 是否更新object
 * @return bool 成功返回true
 */
static int lwm2m_update(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    uint32_t lifetime = luaL_checkinteger(L, 2);
    bool update_object = lua_isboolean(L, 3) ? lua_toboolean(L, 3) : false;
    
    int32_t ret = cm_lwm2m_update(client->handle, lifetime, update_object);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 退出平台
 * @api client:close()
 * @return bool 成功返回true
 */
static int lwm2m_close(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    
    int32_t ret = cm_lwm2m_close(client->handle);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 组包数据
 * @api client:pack(objid, insid, resid, type, value)
 * @param objid int Object ID
 * @param insid int Instance ID
 * @param resid int Resource ID
 * @param type int 数据类型
 * @param value string 数据值
 * @return bool 成功返回true
 */
static int lwm2m_pack(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t objid = luaL_checkinteger(L, 2);
    int32_t insid = luaL_checkinteger(L, 3);
    int32_t resid = luaL_checkinteger(L, 4);
    int32_t type = luaL_checkinteger(L, 5);
    
    size_t len;
    const char* value = luaL_checklstring(L, 6, &len);
    
    int32_t content_type = lua_isnumber(L, 7) ? lua_tointeger(L, 7) : 0;
    
    int32_t ret = cm_lwm2m_notify_packing(client->handle, objid, insid, resid, type, (char*)value, len, content_type);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 上报数据
 * @api client:notify([mid])
 * @param mid int 消息ID，默认自动生成
 * @return bool 成功返回true
 */
static int lwm2m_notify(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : mid_counter++;
    
    int32_t ret = cm_lwm2m_notify(client->handle, mid);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 响应读操作
 * @api client:read_rsp(mid, result, objid, insid, resid, type, value)
 * @param mid int 消息ID
 * @param result int 结果码
 * @param objid int Object ID
 * @param insid int Instance ID
 * @param resid int Resource ID
 * @param type int 数据类型
 * @param value string 数据值
 * @return bool 成功返回true
 */
static int lwm2m_read_rsp(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = luaL_checkinteger(L, 2);
    int32_t result = luaL_checkinteger(L, 3);
    int32_t objid = luaL_checkinteger(L, 4);
    int32_t insid = luaL_checkinteger(L, 5);
    int32_t resid = luaL_checkinteger(L, 6);
    int32_t type = luaL_checkinteger(L, 7);
    
    size_t len;
    const char* value = luaL_checklstring(L, 8, &len);
    
    int32_t ret = cm_lwm2m_read_rsp(client->handle, mid, result, objid, insid, resid, type, (char*)value, len);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 响应写操作
 * @api client:write_rsp(mid, result)
 * @param mid int 消息ID
 * @param result int 结果码
 * @return bool 成功返回true
 */
static int lwm2m_write_rsp(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = luaL_checkinteger(L, 2);
    int32_t result = luaL_checkinteger(L, 3);
    
    int32_t ret = cm_lwm2m_write_rsp(client->handle, mid, result);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 响应执行操作
 * @api client:execute_rsp(mid, result)
 * @param mid int 消息ID
 * @param result int 结果码
 * @return bool 成功返回true
 */
static int lwm2m_execute_rsp(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = luaL_checkinteger(L, 2);
    int32_t result = luaL_checkinteger(L, 3);
    
    int32_t ret = cm_lwm2m_execute_rsp(client->handle, mid, result);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 响应观测操作
 * @api client:observe_rsp(mid, result)
 * @param mid int 消息ID
 * @param result int 结果码
 * @return bool 成功返回true
 */
static int lwm2m_observe_rsp(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = luaL_checkinteger(L, 2);
    int32_t result = luaL_checkinteger(L, 3);
    
    int32_t ret = cm_lwm2m_observe_rsp(client->handle, mid, result);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 响应参数操作
 * @api client:param_rsp(mid, result)
 * @param mid int 消息ID
 * @param result int 结果码
 * @return bool 成功返回true
 */
static int lwm2m_param_rsp(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    int32_t mid = luaL_checkinteger(L, 2);
    int32_t result = luaL_checkinteger(L, 3);
    
    int32_t ret = cm_lwm2m_param_rsp(client->handle, mid, result);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 注册事件回调
 * @api client:on(event, callback)
 * @param event string 事件类型: "event", "read", "write", "execute", "observe"
 * @param callback function 回调函数
 */
static int lwm2m_on(lua_State* L) {
    lwm2m_client_t* client = lwm2m_check_client(L);
    const char* event = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    
    if (strcmp(event, "event") == 0) {
        if (client->event_cb_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, client->event_cb_ref);
        }
        lua_pushvalue(L, 3);
        client->event_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (strcmp(event, "read") == 0) {
        if (client->read_cb_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, client->read_cb_ref);
        }
        lua_pushvalue(L, 3);
        client->read_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (strcmp(event, "write") == 0) {
        if (client->write_cb_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, client->write_cb_ref);
        }
        lua_pushvalue(L, 3);
        client->write_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (strcmp(event, "execute") == 0) {
        if (client->execute_cb_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, client->execute_cb_ref);
        }
        lua_pushvalue(L, 3);
        client->execute_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (strcmp(event, "observe") == 0) {
        if (client->observe_cb_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, client->observe_cb_ref);
        }
        lua_pushvalue(L, 3);
        client->observe_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    
    return 0;
}

LUAMOD_API int luaopen_lwm2m(lua_State* L) {
    luaL_newmetatable(L, LWM2M_HANDLE_METATABLE);
    
    lua_pushstring(L, "__index");
    lua_newtable(L);
    luaL_setfuncs(L, lwm2m_methods, 0);
    lua_settable(L, -3);
    
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, lwm2m_delete);
    lua_settable(L, -3);
    
    luaL_newlibtable(L, lwm2m_lib);
    luaL_setfuncs(L, lwm2m_lib, 0);
    
    /* 平台类型常量 */
    lua_pushinteger(L, CM_LWM2M_ONENET);
    lua_setfield(L, -2, "ONENET");
    
    lua_pushinteger(L, CM_LWM2M_CTWING);
    lua_setfield(L, -2, "CTWING");
    
    lua_pushinteger(L, CM_LWM2M_DMP);
    lua_setfield(L, -2, "DMP");
    
    lua_pushinteger(L, CM_LWM2M_HUAWEIYUN);
    lua_setfield(L, -2, "HUAWEIYUN");
    
    lua_pushinteger(L, CM_LWM2M_OTHER);
    lua_setfield(L, -2, "OTHER");
    
    /* 事件类型常量 */
    lua_pushinteger(L, CM_LWM2M_EVENT_CONNECT_SUCCESS);
    lua_setfield(L, -2, "EVENT_CONNECT_SUCCESS");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_CONNECT_FAILED);
    lua_setfield(L, -2, "EVENT_CONNECT_FAILED");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_REG_SUCCESS);
    lua_setfield(L, -2, "EVENT_REG_SUCCESS");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_REG_FAILED);
    lua_setfield(L, -2, "EVENT_REG_FAILED");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_UPDATE_SUCCESS);
    lua_setfield(L, -2, "EVENT_UPDATE_SUCCESS");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_UPDATE_FAILED);
    lua_setfield(L, -2, "EVENT_UPDATE_FAILED");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_NOTIFY_SUCCESS);
    lua_setfield(L, -2, "EVENT_NOTIFY_SUCCESS");
    
    lua_pushinteger(L, CM_LWM2M_EVENT_NOTIFY_FAILED);
    lua_setfield(L, -2, "EVENT_NOTIFY_FAILED");
    
    /* 数据类型常量 */
    lua_pushinteger(L, 1);  /* string */
    lua_setfield(L, -2, "TYPE_STRING");
    
    lua_pushinteger(L, 2);  /* opaque */
    lua_setfield(L, -2, "TYPE_OPAQUE");
    
    lua_pushinteger(L, 3);  /* integer */
    lua_setfield(L, -2, "TYPE_INTEGER");
    
    lua_pushinteger(L, 4);  /* float */
    lua_setfield(L, -2, "TYPE_FLOAT");
    
    lua_pushinteger(L, 5);  /* bool */
    lua_setfield(L, -2, "TYPE_BOOL");
    
    lua_pushinteger(L, 6);  /* hex str */
    lua_setfield(L, -2, "TYPE_HEX");
    
    /* 结果码常量 */
    lua_pushinteger(L, CM_LWM2M_RESULT_205_CONTENT);
    lua_setfield(L, -2, "RESULT_CONTENT");
    
    lua_pushinteger(L, CM_LWM2M_RESULT_204_CHANGED);
    lua_setfield(L, -2, "RESULT_CHANGED");
    
    lua_pushinteger(L, CM_LWM2M_RESULT_400_BADREQUEST);
    lua_setfield(L, -2, "RESULT_BADREQUEST");
    
    lua_pushinteger(L, CM_LWM2M_RESULT_404_NOTFOUND);
    lua_setfield(L, -2, "RESULT_NOTFOUND");
    
    return 1;
}