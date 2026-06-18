/*
@module  sms
@summary 短信模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     SMS
*/

/*
SMS参考示例
local sms = require("sms")

-- 设置短信中心号码（可选）
sms.set_smsc("+8613800100500")

-- 获取短信中心号码
local smsc = sms.get_smsc()
print("SMS Center:", smsc)

-- 发送文本短信（同步方式）
local ok = sms.send("13812345678", "Hello, World!")
print("Send result:", ok)

-- 发送文本短信（异步方式）
sms.send("13812345678", "Hello from Lua!", function(result)
    if result == 0 then
        print("SMS sent successfully")
    else
        print("SMS send failed:", result)
    end
end)
*/

#include "lua.h"
#include "lauxlib.h"
#include "module.h"
#include "cm_sms.h"

static int iot_sms_send(lua_State* L);
static int iot_sms_send_pdu(lua_State* L);
static int iot_sms_set_smsc(lua_State* L);
static int iot_sms_get_smsc(lua_State* L);

static const luaL_Reg sms_lib[] = {
    {"send",       iot_sms_send},
    {"send_pdu",   iot_sms_send_pdu},
    {"set_smsc",   iot_sms_set_smsc},
    {"get_smsc",   iot_sms_get_smsc},
    {NULL, NULL}
};

/**
 * @brief 短信发送回调函数
 */
static void sms_callback(int32_t result, void *user_data) {
    lua_State* L = (lua_State*)user_data;
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, (int)L);
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, result);
        lua_call(L, 1, 0);
    }
    lua_pop(L, 1);
}

/**
 * @brief 发送文本短信
 * @api sms.send(phone, text[, callback])
 * @param phone string 接收短信的电话号码
 * @param text string 短信内容
 * @param callback function 发送结果回调（可选）
 * @return bool 成功提交返回true
 * @usage
 * -- 同步方式
 * local ok = sms.send("13812345678", "Hello")
 * 
 * -- 异步方式
 * sms.send("13812345678", "Hello", function(result)
 *     print("Result:", result)
 * end)
 */
static int iot_sms_send(lua_State* L) {
    const char* phone = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);
    
    cm_msg_mode_e msg_mode = CM_MSG_MODE_UCS2;  /* 默认使用UCS2编码支持中文 */
    
    int32_t ret;
    
    if (lua_isfunction(L, 3)) {
        /* 异步方式，保存回调函数 */
        lua_pushvalue(L, 3);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        
        ret = cm_sms_send_txt_v2((char*)text, (char*)phone, msg_mode, 
                                CM_MAIN_SIM, sms_callback, (void*)ref);
                                
        if (ret != 0) {
            luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
    } else {
        /* 同步方式，不关心结果 */
        ret = cm_sms_send_txt_v2((char*)text, (char*)phone, msg_mode, 
                                CM_MAIN_SIM, NULL, NULL);
    }
    
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 发送PDU短信
 * @api sms.send_pdu(pdu_data, length)
 * @param pdu_data string PDU编码数据
 * @param length int PDU长度
 * @return bool 成功提交返回true
 */
static int iot_sms_send_pdu(lua_State* L) {
    size_t len;
    const char* pdu_data = luaL_checklstring(L, 1, &len);
    uint32_t msg_len = luaL_checkinteger(L, 2);
    
    int32_t ret;
    
    if (lua_isfunction(L, 3)) {
        /* 异步方式 */
        lua_pushvalue(L, 3);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        
        ret = cm_sms_send_pdu_v2((char*)pdu_data, (char*)&msg_len, 
                                CM_MSG_MODE_UCS2, CM_MAIN_SIM, 
                                sms_callback, (void*)ref);
                                
        if (ret != 0) {
            luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
    } else {
        /* 同步方式 */
        ret = cm_sms_send_pdu_v2((char*)pdu_data, (char*)&msg_len, 
                                CM_MSG_MODE_UCS2, CM_MAIN_SIM, 
                                NULL, NULL);
    }
    
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 设置短信中心号码
 * @api sms.set_smsc(smsc)
 * @param smsc string 短信中心号码，如"+8613800100500"
 * @return bool 成功返回true
 */
static int iot_sms_set_smsc(lua_State* L) {
    const char* smsc = luaL_checkstring(L, 1);
    
    int32_t ret = cm_sms_set_smsc((char*)smsc, CM_MAIN_SIM);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 获取短信中心号码
 * @api sms.get_smsc()
 * @return string 短信中心号码
 */
static int iot_sms_get_smsc(lua_State* L) {
    char smsc[32] = {0};
    
    int32_t ret = cm_sms_get_smsc(smsc, sizeof(smsc), CM_MAIN_SIM);
    if (ret == 0) {
        lua_pushstring(L, smsc);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

LUAMOD_API int luaopen_sms(lua_State* L) {
    luaL_newlibtable(L, sms_lib);
    luaL_setfuncs(L, sms_lib, 0);
    
    /* 编码模式常量 */
    lua_pushinteger(L, CM_MSG_MODE_GSM_7);
    lua_setfield(L, -2, "MODE_GSM_7");
    
    lua_pushinteger(L, CM_MSG_MODE_GSM_8);
    lua_setfield(L, -2, "MODE_GSM_8");
    
    lua_pushinteger(L, CM_MSG_MODE_UCS2);
    lua_setfield(L, -2, "MODE_UCS2");
    
    /* SIM卡常量 */
    lua_pushinteger(L, CM_SIM_ID_0);
    lua_setfield(L, -2, "SIM_0");
    
    lua_pushinteger(L, CM_SIM_ID_1);
    lua_setfield(L, -2, "SIM_1");
    
    return 1;
}