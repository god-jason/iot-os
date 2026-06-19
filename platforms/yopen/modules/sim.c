/*
@module  sim
@summary SIM卡操作
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SIM
*/

/*
SIM参考示例
-- 获取SIM卡状态
local status = sim.status()
print("sim status:", status)

-- 获取ICCID
local iccid = sim.iccid()
print("iccid:", iccid)

-- 获取IMSI
local imsi = sim.imsi()
print("imsi:", imsi)

-- 获取信号强度
local rssi = sim.rssi()
print("rssi:", rssi)
*/

#include "module.h"
#include "yopen_sim.h"
#include "yopen_ril.h"

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

    /* 跳过OK/ERROR等响应行 */
    if (strncmp(line, "OK", 2) == 0 || strncmp(line, "ERROR", 5) == 0) {
        ctx->ready = 1;
        return 0;
    }

    /* 复制有效数据 */
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

    /* 等待响应 */
    int wait_count = 100;
    while (!ctx.ready && wait_count-- > 0) {
        iot_sleep(10);
    }

    return ctx.ready ? 0 : -1;
}

/**
 * @brief 获取SIM卡状态
 * @api sim.status()
 * @return int 状态值 (0=未知, 1=正常, 2=错误, 3=缺失)
 */
static int luaopen_sim_status(lua_State* L) {
    yopen_sim_status_e status = YOPEN_SIM_STATUS_UNKNOW;
    yopen_sim_get_card_status(0, &status);

    int result;
    switch (status) {
        case YOPEN_SIM_STATUS_READY:
        case YOPEN_SIM_STATUS_SIMPIN:
        case YOPEN_SIM_STATUS_PIN1_READY:
            result = 1;
            break;
        case YOPEN_SIM_STATUS_NOSIM:
            result = 3;
            break;
        default:
            result = 0;
            break;
    }

    lua_pushinteger(L, result);
    return 1;
}

/**
 * @brief 获取SIM卡ICCID
 * @api sim.iccid()
 * @return string ICCID字符串
 */
static int luaopen_sim_iccid(lua_State* L) {
    char iccid[32] = {0};
    char cmd[] = "AT+ICCID\r\n";

    if (at_cmd_sync(cmd, iccid, sizeof(iccid)) == 0) {
        /* 去掉空白字符 */
        char* p = iccid;
        char temp[32] = {0};
        int j = 0;
        for (int i = 0; iccid[i] != '\0' && j < sizeof(temp) - 1; i++) {
            if (iccid[i] >= '0' && iccid[i] <= '9') {
                temp[j++] = iccid[i];
            }
        }
        strcpy(iccid, temp);
    }

    lua_pushstring(L, iccid[0] ? iccid : "");
    return 1;
}

/**
 * @brief 获取SIM卡IMSI
 * @api sim.imsi()
 * @return string IMSI字符串
 */
static int luaopen_sim_imsi(lua_State* L) {
    char imsi[16] = {0};
    char cmd[] = "AT+CIMI\r\n";

    if (at_cmd_sync(cmd, imsi, sizeof(imsi)) == 0) {
        /* 验证IMSI格式(15位数字) */
        int len = strlen(imsi);
        if (len >= 15) {
            imsi[15] = '\0';
        }
    }

    lua_pushstring(L, imsi[0] ? imsi : "");
    return 1;
}

/**
 * @brief 获取信号强度
 * @api sim.rssi()
 * @return int 信号强度 (0-31, 99=未知)
 */
static int luaopen_sim_rssi(lua_State* L) {
    char response[64] = {0};
    char cmd[] = "AT+CSQ\r\n";

    if (at_cmd_sync(cmd, response, sizeof(response)) == 0) {
        /* 解析 +CSQ: <rssi>,<ber> */
        char* p = strstr(response, "+CSQ:");
        if (p) {
            int rssi = 99;
            sscanf(p, "+CSQ:%d", &rssi);
            lua_pushinteger(L, rssi);
            return 1;
        }
    }

    lua_pushinteger(L, 99);
    return 1;
}

/**
 * @brief 获取运营商名称
 * @api sim.operator()
 * @return string 运营商名称
 */
static int luaopen_sim_operator(lua_State* L) {
    char oper[32] = {0};
    char cmd[] = "AT+COPS?\r\n";

    if (at_cmd_sync(cmd, oper, sizeof(oper)) == 0) {
        /* 解析 +COPS: <mode>,<format>,<oper> */
        char* p = strstr(oper, "+COPS:");
        if (p) {
            /* 跳过 +COPS: 0,0," */
            p = strchr(p, '"');
            if (p) {
                p++;
                char* end = strchr(p, '"');
                if (end) {
                    int len = end - p;
                    if (len > 0 && len < sizeof(oper)) {
                        strncpy(oper, p, len);
                        oper[len] = '\0';
                    }
                }
            }
        }
    }

    lua_pushstring(L, oper[0] ? oper : "");
    return 1;
}

static const luaL_Reg luaopen_sim_funcs[] = {
    {"status",   luaopen_sim_status},
    {"iccid",    luaopen_sim_iccid},
    {"imsi",     luaopen_sim_imsi},
    {"rssi",     luaopen_sim_rssi},
    {"operator", luaopen_sim_operator},
    {NULL, NULL}
};

int luaopen_sim(lua_State* L) {
    luaL_newlib(L, luaopen_sim_funcs);

    /* 状态常量 */
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "UNKNOWN");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "READY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "ERROR");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "NOT_INSERT");

    return 1;
}