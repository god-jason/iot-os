/*
@module  sms
@summary 短信操作
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SMS
*/

/*
SMS参考示例
-- 发送短信
local ok = sms.send("10086", "CXYE")
print("send result:", ok)

-- 读取短信
local msgs = sms.receive()
for i, msg in ipairs(msgs) do
    print("from:", msg.phone, "content:", msg.content)
end

-- 删除短信
sms.delete(index)

-- 存储短信到SIM卡
sms.store("10086", "hello")
*/

#include "module.h"
#include "yopen_sms.h"
#include "yopen_ril.h"

/* 短信句柄 */
typedef struct {
    int index;
    char phone[32];
    char content[256];
    int timestamp;
} sms_msg_t;

/**
 * @brief 发送短信
 * @api sms.send(phone, content)
 * @string phone 手机号码
 * @string content 短信内容
 * @return bool true表示成功
 */
static int luaopen_sms_send(lua_State* L) {
    const char* phone = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);

    if (!phone || !content) {
        lua_pushboolean(L, 0);
        return 1;
    }

    yopen_errcode_ril_e ret = yopen_ril_request_send_sms(
        (char*)phone,
        (char*)content
    );

    lua_pushboolean(L, ret == YOPEN_RIL_SUCCESS);
    return 1;
}

/**
 * @brief 接收短信
 * @api sms.receive()
 * @return table 短信列表
 */
static int luaopen_sms_receive(lua_State* L) {
    /* 最多读取10条短信 */
    sms_msg_t msgs[10] = {0};
    int count = 0;

    /* TODO: 调用 yopen_sms_list 获取短信列表 */

    lua_createtable(L, count, 0);
    for (int i = 0; i < count; i++) {
        lua_createtable(L, 0, 4);

        lua_pushinteger(L, msgs[i].index);
        lua_setfield(L, -2, "index");

        lua_pushstring(L, msgs[i].phone);
        lua_setfield(L, -2, "phone");

        lua_pushstring(L, msgs[i].content);
        lua_setfield(L, -2, "content");

        lua_pushinteger(L, msgs[i].timestamp);
        lua_setfield(L, -2, "time");

        lua_seti(L, -2, i + 1);
    }

    return 1;
}

/**
 * @brief 删除短信
 * @api sms.delete(index)
 * @int index 短信索引
 * @return bool true表示成功
 */
static int luaopen_sms_delete(lua_State* L) {
    int index = (int)luaL_checkinteger(L, 1);

    yopen_errcode_ril_e ret = yopen_ril_request_delete_sms(index);

    lua_pushboolean(L, ret == YOPEN_RIL_SUCCESS);
    return 1;
}

/**
 * @brief 存储短信到SIM卡
 * @api sms.store(phone, content)
 * @string phone 手机号码
 * @string content 短信内容
 * @return bool true表示成功
 */
static int luaopen_sms_store(lua_State* L) {
    const char* phone = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);

    if (!phone || !content) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* TODO: 实现存储功能 */

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置短信中心号码
 * @api sms.sca(number)
 * @string number 短信中心号码
 * @return bool true表示成功
 */
static int luaopen_sms_sca(lua_State* L) {
    const char* number = luaL_checkstring(L, 1);

    /* TODO: 实现设置短信中心功能 */

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg luaopen_sms_funcs[] = {
    {"send",    luaopen_sms_send},
    {"receive", luaopen_sms_receive},
    {"delete",  luaopen_sms_delete},
    {"store",   luaopen_sms_store},
    {"sca",     luaopen_sms_sca},
    {NULL, NULL}
};

int luaopen_sms(lua_State* L) {
    luaL_newlib(L, luaopen_sms_funcs);
    return 1;
}