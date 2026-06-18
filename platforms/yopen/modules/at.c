/*
@module  at
@summary AT指令
@version 1.0
@date    2026.06.19
@author  TRAE
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

#include "module.h"
#include "yopen_ril.h"

#define AT_URC_MAX 16
#define AT_RSP_MAX 512

typedef struct {
    const char* prefix;
    void* callback_ud;
} at_urc_entry_t;

static at_urc_entry_t at_urc_table[AT_URC_MAX] = {0};
static int at_urc_count = 0;
static int at_init_flag = 0;

/* AT命令响应缓冲 */
static char g_at_rsp[AT_RSP_MAX] = {0};
static int g_at_rsp_len = 0;

/**
 * @brief URC主动上报回调
 */
static int32_t at_urc_callback(char *line, uint32_t line_len) {
    if (!line || line_len == 0) {
        return 0;
    }

    /* 遍历URC表，查找匹配的prefix */
    for (int i = 0; i < at_urc_count; i++) {
        if (at_urc_table[i].prefix) {
            size_t prefix_len = strlen(at_urc_table[i].prefix);
            if (prefix_len > 0 && strncmp(line, at_urc_table[i].prefix, prefix_len) == 0) {
                if (at_urc_table[i].callback_ud) {
                    /* TODO: 调用Lua回调 */
                }
            }
        }
    }

    return 0;
}

/**
 * @brief AT命令同步响应回调
 */
static int32_t at_sync_callback(char *line, uint32_t line_len, void *user_data) {
    if (!line || line_len == 0) {
        return 0;
    }

    /* 累积响应数据 */
    if (g_at_rsp_len + line_len < AT_RSP_MAX - 1) {
        memcpy(g_at_rsp + g_at_rsp_len, line, line_len);
        g_at_rsp_len += line_len;
        g_at_rsp[g_at_rsp_len] = '\0';
    }

    return 0;
}

/**
 * @brief 初始化AT模块
 */
static int at_init(void) {
    if (at_init_flag) {
        return 0;
    }

    if (yopen_ril_init(at_urc_callback) == YOPEN_RIL_SUCCESS) {
        at_init_flag = 1;
        return 0;
    }
    return -1;
}

/**
 * @brief 同步发送AT指令
 * @api at.send(cmd, timeout)
 * @string cmd AT指令
 * @int timeout 超时时间(毫秒)
 * @return string 响应内容
 */
static int luaopen_at_send(lua_State* L) {
    size_t cmd_len;
    const char* cmd = luaL_checklstring(L, 1, &cmd_len);
    uint32_t timeout = (uint32_t)luaL_optinteger(L, 2, 10000);  /* 默认10s */

    /* 确保AT模块已初始化 */
    if (!at_init_flag) {
        at_init();
    }

    /* 清空响应缓冲 */
    memset(g_at_rsp, 0, sizeof(g_at_rsp));
    g_at_rsp_len = 0;

    /* 构建完整的AT命令 (需要\r\n结尾) */
    char at_cmd[256];
    if (cmd_len > sizeof(at_cmd) - 3) {
        lua_pushnil(L);
        lua_pushstring(L, "command too long");
        return 2;
    }

    memcpy(at_cmd, cmd, cmd_len);
    at_cmd[cmd_len] = '\r';
    at_cmd[cmd_len + 1] = '\n';
    at_cmd[cmd_len + 2] = '\0';

    /* 发送AT命令 */
    yopen_errcode_ril_e ret = yopen_ril_send_atcmd(
        at_cmd,
        (uint32_t)(cmd_len + 2),
        at_sync_callback,
        NULL
    );

    if (ret == YOPEN_RIL_SUCCESS && g_at_rsp_len > 0) {
        lua_pushstring(L, g_at_rsp);
        return 1;
    }

    lua_pushnil(L);
    lua_pushstring(L, "at send failed");
    return 2;
}

/**
 * @brief 注册URC回调
 * @api at.on(prefix, callback)
 * @string prefix URC前缀，如 "+CMT"
 * @func callback 回调函数
 * @return bool true表示成功
 */
static int luaopen_at_on(lua_State* L) {
    const char* prefix = luaL_checkstring(L, 1);

    if (at_urc_count >= AT_URC_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!at_init_flag) {
        at_init();
    }

    /* 注册URC */
    at_urc_table[at_urc_count].prefix = prefix;
    at_urc_table[at_urc_count].callback_ud = NULL;  /* TODO: 保存Lua回调 */
    at_urc_count++;

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 取消注册URC回调
 * @api at.off(prefix)
 * @string prefix URC前缀
 * @return bool true表示成功
 */
static int luaopen_at_off(lua_State* L) {
    const char* prefix = luaL_checkstring(L, 1);

    for (int i = 0; i < at_urc_count; i++) {
        if (at_urc_table[i].prefix && strcmp(at_urc_table[i].prefix, prefix) == 0) {
            /* 移除该URC */
            for (int j = i; j < at_urc_count - 1; j++) {
                at_urc_table[j] = at_urc_table[j + 1];
            }
            at_urc_count--;
            lua_pushboolean(L, 1);
            return 1;
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

static const luaL_Reg luaopen_at_funcs[] = {
    {"send", luaopen_at_send},
    {"on",   luaopen_at_on},
    {"off",  luaopen_at_off},
    {NULL, NULL}
};

int luaopen_at(lua_State* L) {
    luaL_newlib(L, luaopen_at_funcs);
    return 1;
}