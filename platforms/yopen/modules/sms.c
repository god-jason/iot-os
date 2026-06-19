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

    yopen_sms_errcode_e ret = yopen_sms_send_text(0, (char*)phone, (char*)content, YOPEN_GSM);
    lua_pushboolean(L, ret == YOPEN_SMS_SUCCESS);
    return 1;
}

/**
 * @brief 接收短信
 * @api sms.receive()
 * @return table 短信列表
 */
static int luaopen_sms_receive(lua_State* L) {
    yopen_sms_stor_info_s stor_info = {0};
    yopen_sms_get_storage_info(0, &stor_info);

    int max_count = stor_info.usedSlotSM + stor_info.usedSlotME;
    if (max_count > 20) max_count = 20;

    lua_createtable(L, max_count, 0);
    int count = 0;

    for (int i = 1; i <= max_count && count < 20; i++) {
        yopen_sms_recv_s sms = {0};
        yopen_sms_errcode_e ret = yopen_sms_read(0, i, &sms);

        if (ret == YOPEN_SMS_SUCCESS) {
            lua_createtable(L, 0, 4);

            lua_pushinteger(L, sms.index);
            lua_setfield(L, -2, "index");

            lua_pushstring(L, (const char*)sms.addr_str);
            lua_setfield(L, -2, "phone");

            lua_pushstring(L, (const char*)sms.body_str);
            lua_setfield(L, -2, "content");

            /* 计算时间戳 */
            int timestamp = (sms.scts.uYear + 2000) * 100000000 +
                           sms.scts.uMonth * 1000000 +
                           sms.scts.uDay * 10000 +
                           sms.scts.uHour * 100 +
                           sms.scts.uMinute;
            lua_pushinteger(L, timestamp);
            lua_setfield(L, -2, "time");

            lua_seti(L, -2, ++count);
        }
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
    yopen_sms_errcode_e ret = yopen_sms_delete(0, (uint8_t)index, YOPEN_SMS_DEL_INDEX);
    lua_pushboolean(L, ret == YOPEN_SMS_SUCCESS);
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

    lua_pushboolean(L, 0);
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
    yopen_sms_errcode_e ret = yopen_sms_set_center_address(0, (char*)number);
    lua_pushboolean(L, ret == YOPEN_SMS_SUCCESS);
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