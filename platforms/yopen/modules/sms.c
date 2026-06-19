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
#include <string.h>

/* AT命令回调上下文 */
typedef struct {
    char* buffer;
    int buffer_size;
    int ready;
} at_cmd_ctx_t;

/* AT响应回调 */
static int32_t at_response_callback(char *line, uint32_t line_len, void *user_data) {
    at_cmd_ctx_t* ctx = (at_cmd_ctx_t*)user_data;
    if (!ctx || !ctx->buffer) return -1;

    if (strncmp(line, "OK", 2) == 0 || strncmp(line, "ERROR", 5) == 0) {
        ctx->ready = 1;
        return 0;
    }

    int copy_len = line_len < (ctx->buffer_size - 1) ? line_len : (ctx->buffer_size - 1);
    if (copy_len > 0 && line[0] != '\r' && line[0] != '\n') {
        memcpy(ctx->buffer, line, copy_len);
        ctx->buffer[copy_len] = '\0';
    }

    return 0;
}

/* 同步发送AT命令 */
static int at_cmd_sync(char* cmd, char* response, int resp_size) {
    at_cmd_ctx_t ctx = {0};
    ctx.buffer = response;
    ctx.buffer_size = resp_size;
    ctx.ready = 0;

    yopen_errcode_ril_e ret = yopen_ril_send_atcmd(cmd, strlen(cmd), at_response_callback, &ctx);
    if (ret != YOPEN_RIL_SUCCESS) {
        return -1;
    }

    int wait_count = 100;
    while (!ctx.ready && wait_count-- > 0) {
        iot_sleep(10);
    }

    return ctx.ready ? 0 : -1;
}

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

    /* AT命令: AT+CMGS="phone"<CR>content<Ctrl+Z> */
    char cmd[320] = {0};
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"\r", phone);

    /* 发送AT命令进入发送模式 */
    char response[64] = {0};
    if (at_cmd_sync(cmd, response, sizeof(response)) != 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 发送短信内容(需要用0x1A Ctrl+Z结束) */
    char sms_cmd[320] = {0};
    snprintf(sms_cmd, sizeof(sms_cmd), "%s\x1A", content);

    if (at_cmd_sync(sms_cmd, response, sizeof(response)) == 0) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 接收短信
 * @api sms.receive()
 * @return table 短信列表
 */
static int luaopen_sms_receive(lua_State* L) {
    /* 读取全部短信: AT+CMGL="ALL" */
    char cmd[] = "AT+CMGL=\"ALL\"\r";
    char response[2048] = {0};

    sms_msg_t msgs[10] = {0};
    int count = 0;

    if (at_cmd_sync(cmd, response, sizeof(response)) == 0) {
        /* 解析响应 */
        char* p = response;
        char line[256] = {0};

        while (*p && count < 10) {
            /* 提取一行 */
            int i = 0;
            while (*p && *p != '\r' && *p != '\n' && i < sizeof(line) - 1) {
                line[i++] = *p++;
            }
            line[i] = '\0';

            /* 跳过空白行 */
            if (line[0] == '\0') {
                while (*p == '\r' || *p == '\n') p++;
                continue;
            }

            /* 解析 +CMGL: <index>,<status>,<oa/da>,... */
            if (strncmp(line, "+CMGL:", 6) == 0) {
                char* ptr = line;
                ptr += 6;  /* 跳过+CMGL: */

                /* 解析index */
                while (*ptr == ' ' || *ptr == '\t') ptr++;
                msgs[count].index = atoi(ptr);

                /* 跳过逗号找phone */
                ptr = strchr(ptr, ',');
                if (ptr) {
                    ptr++;  /* 跳过逗号，可能还有状态字段 */

                    /* 跳过状态字段，找phone */
                    while (*ptr && *ptr != '\"') ptr++;
                    if (*ptr == '\"') {
                        ptr++;  /* 跳过开头引号 */
                        char* end = strchr(ptr, '\"');
                        if (end) {
                            int len = end - ptr;
                            if (len > 0 && len < sizeof(msgs[count].phone)) {
                                strncpy(msgs[count].phone, ptr, len);
                                msgs[count].phone[len] = '\0';
                            }
                        }
                    }
                }
                count++;
            }

            while (*p == '\r' || *p == '\n') p++;
        }
    }

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

    char cmd[32] = {0};
    snprintf(cmd, sizeof(cmd), "AT+CMGD=%d\r", index);

    char response[64] = {0};
    int ret = at_cmd_sync(cmd, response, sizeof(response));

    lua_pushboolean(L, ret == 0);
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

    /* AT命令: AT+CMGW="phone"<CR>content<Ctrl+Z> */
    char cmd[320] = {0};
    snprintf(cmd, sizeof(cmd), "AT+CMGW=\"%s\"\r", phone);

    char response[64] = {0};
    if (at_cmd_sync(cmd, response, sizeof(response)) != 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    char sms_cmd[320] = {0};
    snprintf(sms_cmd, sizeof(sms_cmd), "%s\x1A", content);

    if (at_cmd_sync(sms_cmd, response, sizeof(response)) == 0) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
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

    char cmd[64] = {0};
    snprintf(cmd, sizeof(cmd), "AT+CSCA=\"%s\"\r", number);

    char response[64] = {0};
    int ret = at_cmd_sync(cmd, response, sizeof(response));

    lua_pushboolean(L, ret == 0);
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