/**
 * @file modbus_module.c
 * @brief Modbus RTU Lua 模块
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "iot.h"
#include "modbus_rtu.h"

static const char *modbus_err_str(modbus_err_t err)
{
    switch (err) {
        case MODBUS_OK:            return "ok";
        case MODBUS_ERR_PARAM:     return "invalid parameter";
        case MODBUS_ERR_CRC:       return "crc error";
        case MODBUS_ERR_FORMAT:    return "format error";
        case MODBUS_ERR_EXCEPTION: return "modbus exception";
        case MODBUS_ERR_BUF:       return "buffer too small";
        default:                   return "unknown error";
    }
}

static int modbus_push_err(lua_State *L, modbus_err_t err)
{
    lua_pushnil(L);
    lua_pushstring(L, modbus_err_str(err));
    return 2;
}

static int modbus_push_frame(lua_State *L, const uint8_t *frame, size_t len)
{
    lua_pushlstring(L, (const char *)frame, len);
    return 1;
}

static int modbus_build_ok(lua_State *L, modbus_err_t err, const uint8_t *frame, size_t len)
{
    if (err != MODBUS_OK) {
        return modbus_push_err(L, err);
    }
    return modbus_push_frame(L, frame, len);
}

/**
 * @brief modbus.crc16(data)
 */
static int luaopen_modbus_crc16(lua_State *L)
{
    size_t len = 0;
    const char *data = luaL_checklstring(L, 1, &len);
    uint16_t crc = modbus_crc16((const uint8_t *)data, len);
    lua_pushinteger(L, crc);
    return 1;
}

/**
 * @brief modbus.append_crc(data)
 */
static int luaopen_modbus_append_crc(lua_State *L)
{
    size_t len = 0;
    const char *data = luaL_checklstring(L, 1, &len);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len;

    out_len = modbus_crc16_append(buf, sizeof(buf), (const uint8_t *)data, len);
    if (out_len == 0) {
        return modbus_push_err(L, MODBUS_ERR_BUF);
    }

    return modbus_push_frame(L, buf, out_len);
}

/**
 * @brief modbus.verify(frame)
 */
static int luaopen_modbus_verify(lua_State *L)
{
    size_t len = 0;
    const char *frame = luaL_checklstring(L, 1, &len);
    lua_pushboolean(L, modbus_crc16_verify((const uint8_t *)frame, len));
    return 1;
}

/**
 * @brief modbus.build_read(addr, fc, start, qty)
 */
static int luaopen_modbus_build_read(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint8_t fc = (uint8_t)luaL_checkinteger(L, 2);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 3);
    uint16_t qty = (uint16_t)luaL_checkinteger(L, 4);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err;

    err = modbus_rtu_build_read(addr, fc, start, qty, buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

/**
 * @brief modbus.build_write_register(addr, reg, value)
 */
static int luaopen_modbus_build_write_register(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t reg = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t value = (uint16_t)luaL_checkinteger(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err;

    err = modbus_rtu_build_write_register(addr, reg, value, buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static int modbus_read_uint16_array(lua_State *L, int idx, uint16_t *values, size_t max_count, size_t *count)
{
    size_t i;
    luaL_checktype(L, idx, LUA_TTABLE);
    *count = (size_t)lua_rawlen(L, idx);
    if (*count == 0 || *count > max_count) {
        return -1;
    }

    for (i = 0; i < *count; i++) {
        lua_rawgeti(L, idx, (int)(i + 1));
        if (!lua_isinteger(L, -1)) {
            lua_pop(L, 1);
            return -1;
        }
        values[i] = (uint16_t)lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    return 0;
}

static int modbus_read_bool_array(lua_State *L, int idx, uint8_t *bits, size_t max_bits, size_t *bit_count)
{
    size_t i;
    size_t byte_count;
    luaL_checktype(L, idx, LUA_TTABLE);
    *bit_count = (size_t)lua_rawlen(L, idx);
    if (*bit_count == 0 || *bit_count > max_bits) {
        return -1;
    }

    byte_count = (*bit_count + 7) / 8;
    memset(bits, 0, byte_count);

    for (i = 0; i < *bit_count; i++) {
        lua_rawgeti(L, idx, (int)(i + 1));
        if (lua_toboolean(L, -1)) {
            bits[i / 8] |= (uint8_t)(1U << (i % 8));
        }
        lua_pop(L, 1);
    }
    return 0;
}

/**
 * @brief modbus.build_write_registers(addr, start, values)
 */
static int luaopen_modbus_build_write_registers(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t values[123];
    size_t count = 0;
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err;

    if (modbus_read_uint16_array(L, 3, values, 123, &count) != 0) {
        return modbus_push_err(L, MODBUS_ERR_PARAM);
    }

    err = modbus_rtu_build_write_registers(addr, start, values, count, buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

/**
 * @brief modbus.build_write_coil(addr, coil, on)
 */
static int luaopen_modbus_build_write_coil(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t coil = (uint16_t)luaL_checkinteger(L, 2);
    bool on = lua_toboolean(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err;

    err = modbus_rtu_build_write_coil(addr, coil, on, buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

/**
 * @brief modbus.build_write_coils(addr, start, bits)
 */
static int luaopen_modbus_build_write_coils(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint8_t bits[246];
    size_t bit_count = 0;
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err;

    if (modbus_read_bool_array(L, 3, bits, 1968, &bit_count) != 0) {
        return modbus_push_err(L, MODBUS_ERR_PARAM);
    }

    err = modbus_rtu_build_write_coils(addr, start, bits, bit_count, buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static void modbus_push_frame_table(lua_State *L, const modbus_rtu_frame_t *frame, modbus_err_t err)
{
    lua_createtable(L, 0, 6);
    lua_pushinteger(L, frame->addr);
    lua_setfield(L, -2, "addr");
    lua_pushinteger(L, frame->fc & 0x7F);
    lua_setfield(L, -2, "fc");

    if (frame->is_exception) {
        lua_pushboolean(L, 1);
        lua_setfield(L, -2, "exception");
        lua_pushinteger(L, frame->exception_code);
        lua_setfield(L, -2, "code");
    } else if (frame->payload_len > 0) {
        lua_pushlstring(L, (const char *)frame->payload, frame->payload_len);
        lua_setfield(L, -2, "payload");
        if (frame->fc == MODBUS_FC_READ_COILS ||
            frame->fc == MODBUS_FC_READ_DISCRETE_INPUTS ||
            frame->fc == MODBUS_FC_READ_HOLDING_REGISTERS ||
            frame->fc == MODBUS_FC_READ_INPUT_REGISTERS) {
            lua_pushinteger(L, frame->payload[0]);
            lua_setfield(L, -2, "byte_count");
        }
    }

    if (err == MODBUS_ERR_EXCEPTION) {
        lua_pushstring(L, modbus_err_str(err));
        lua_setfield(L, -2, "error");
    }
}

/**
 * @brief modbus.parse(frame)
 */
static int luaopen_modbus_parse(lua_State *L)
{
    size_t len = 0;
    const char *frame = luaL_checklstring(L, 1, &len);
    modbus_rtu_frame_t parsed;
    modbus_err_t err;

    err = modbus_rtu_parse((const uint8_t *)frame, len, &parsed);
    if (err == MODBUS_ERR_CRC || err == MODBUS_ERR_FORMAT || err == MODBUS_ERR_PARAM) {
        return modbus_push_err(L, err);
    }

    modbus_push_frame_table(L, &parsed, err);
    if (err == MODBUS_ERR_EXCEPTION) {
        lua_pushstring(L, modbus_err_str(err));
        return 2;
    }
    return 1;
}

/**
 * @brief modbus.decode_bits(payload, count)
 */
static int luaopen_modbus_decode_bits(lua_State *L)
{
    size_t len = 0;
    const char *payload = luaL_checklstring(L, 1, &len);
    size_t count = (size_t)luaL_checkinteger(L, 2);
    uint8_t *bits = NULL;
    size_t i;
    modbus_err_t err;

    if (count == 0) {
        return modbus_push_err(L, MODBUS_ERR_PARAM);
    }

    bits = (uint8_t *)iot_malloc(count);
    if (!bits) {
        return modbus_push_err(L, MODBUS_ERR_BUF);
    }

    err = modbus_parse_read_bits((const uint8_t *)payload, len, count, bits);
    if (err != MODBUS_OK) {
        iot_free(bits);
        return modbus_push_err(L, err);
    }

    lua_createtable(L, (int)count, 0);
    for (i = 0; i < count; i++) {
        lua_pushinteger(L, bits[i]);
        lua_rawseti(L, -2, (int)(i + 1));
    }
    iot_free(bits);
    return 1;
}

/**
 * @brief modbus.decode_registers(payload, count [, byte_order])
 * byte_order: "be"(default) or "le"
 */
static int luaopen_modbus_decode_registers(lua_State *L)
{
    size_t len = 0;
    const char *payload = luaL_checklstring(L, 1, &len);
    size_t count = (size_t)luaL_checkinteger(L, 2);
    const char *order = luaL_optstring(L, 3, "be");
    uint16_t *regs = NULL;
    size_t i;
    modbus_err_t err;

    if (count == 0) {
        return modbus_push_err(L, MODBUS_ERR_PARAM);
    }

    regs = (uint16_t *)iot_malloc(count * sizeof(uint16_t));
    if (!regs) {
        return modbus_push_err(L, MODBUS_ERR_BUF);
    }

    err = modbus_parse_read_registers((const uint8_t *)payload, len, regs, count);
    if (err != MODBUS_OK) {
        iot_free(regs);
        return modbus_push_err(L, err);
    }

    if (strcmp(order, "le") == 0) {
        for (i = 0; i < count; i++) {
            regs[i] = (uint16_t)((regs[i] >> 8) | (regs[i] << 8));
        }
    } else if (strcmp(order, "be") != 0) {
        iot_free(regs);
        return modbus_push_err(L, MODBUS_ERR_PARAM);
    }

    lua_createtable(L, (int)count, 0);
    for (i = 0; i < count; i++) {
        lua_pushinteger(L, regs[i]);
        lua_rawseti(L, -2, (int)(i + 1));
    }
    iot_free(regs);
    return 1;
}

/**
 * @brief modbus.to_uint32(reg_hi, reg_lo [, order])
 */
static int luaopen_modbus_to_uint32(lua_State *L)
{
    uint16_t hi = (uint16_t)luaL_checkinteger(L, 1);
    uint16_t lo = (uint16_t)luaL_checkinteger(L, 2);
    const char *order = luaL_optstring(L, 3, "ABCD");
    uint32_t value;
    modbus_err_t err = modbus_regs_to_uint32(hi, lo, order, &value);

    if (err != MODBUS_OK) {
        return modbus_push_err(L, err);
    }

    lua_pushinteger(L, (lua_Integer)value);
    return 1;
}

/**
 * @brief modbus.to_int32(reg_hi, reg_lo [, order])
 */
static int luaopen_modbus_to_int32(lua_State *L)
{
    uint16_t hi = (uint16_t)luaL_checkinteger(L, 1);
    uint16_t lo = (uint16_t)luaL_checkinteger(L, 2);
    const char *order = luaL_optstring(L, 3, "ABCD");
    int32_t value;
    modbus_err_t err = modbus_regs_to_int32(hi, lo, order, &value);

    if (err != MODBUS_OK) {
        return modbus_push_err(L, err);
    }

    lua_pushinteger(L, value);
    return 1;
}

/**
 * @brief modbus.to_float(reg_hi, reg_lo [, order])
 */
static int luaopen_modbus_to_float(lua_State *L)
{
    uint16_t hi = (uint16_t)luaL_checkinteger(L, 1);
    uint16_t lo = (uint16_t)luaL_checkinteger(L, 2);
    const char *order = luaL_optstring(L, 3, "ABCD");
    float value;
    modbus_err_t err = modbus_regs_to_float32(hi, lo, order, &value);

    if (err != MODBUS_OK) {
        return modbus_push_err(L, err);
    }

    lua_pushnumber(L, (lua_Number)value);
    return 1;
}

static int luaopen_modbus_build_read_holding(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t qty = (uint16_t)luaL_checkinteger(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err = modbus_rtu_build_read(addr, MODBUS_FC_READ_HOLDING_REGISTERS, start, qty,
                                             buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static int luaopen_modbus_build_read_input(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t qty = (uint16_t)luaL_checkinteger(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err = modbus_rtu_build_read(addr, MODBUS_FC_READ_INPUT_REGISTERS, start, qty,
                                             buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static int luaopen_modbus_build_read_coils(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t qty = (uint16_t)luaL_checkinteger(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err = modbus_rtu_build_read(addr, MODBUS_FC_READ_COILS, start, qty,
                                             buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static int luaopen_modbus_build_read_discrete(lua_State *L)
{
    uint8_t addr = (uint8_t)luaL_checkinteger(L, 1);
    uint16_t start = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t qty = (uint16_t)luaL_checkinteger(L, 3);
    uint8_t buf[MODBUS_RTU_MAX_FRAME];
    size_t out_len = 0;
    modbus_err_t err = modbus_rtu_build_read(addr, MODBUS_FC_READ_DISCRETE_INPUTS, start, qty,
                                             buf, sizeof(buf), &out_len);
    return modbus_build_ok(L, err, buf, out_len);
}

static const luaL_Reg modbus_lib[] = {
    { "crc16",                  luaopen_modbus_crc16 },
    { "append_crc",             luaopen_modbus_append_crc },
    { "verify",                 luaopen_modbus_verify },
    { "build_read",             luaopen_modbus_build_read },
    { "build_read_holding",     luaopen_modbus_build_read_holding },
    { "build_read_input",       luaopen_modbus_build_read_input },
    { "build_read_coils",       luaopen_modbus_build_read_coils },
    { "build_read_discrete",    luaopen_modbus_build_read_discrete },
    { "build_write_register",   luaopen_modbus_build_write_register },
    { "build_write_registers",  luaopen_modbus_build_write_registers },
    { "build_write_coil",       luaopen_modbus_build_write_coil },
    { "build_write_coils",      luaopen_modbus_build_write_coils },
    { "parse",                  luaopen_modbus_parse },
    { "decode_bits",            luaopen_modbus_decode_bits },
    { "decode_registers",       luaopen_modbus_decode_registers },
    { "to_uint32",              luaopen_modbus_to_uint32 },
    { "to_int32",               luaopen_modbus_to_int32 },
    { "to_float",               luaopen_modbus_to_float },
    { NULL, NULL }
};

static void modbus_register_constants(lua_State *L)
{
    lua_pushinteger(L, MODBUS_FC_READ_COILS);               lua_setfield(L, -2, "FC_READ_COILS");
    lua_pushinteger(L, MODBUS_FC_READ_DISCRETE_INPUTS);     lua_setfield(L, -2, "FC_READ_DISCRETE");
    lua_pushinteger(L, MODBUS_FC_READ_HOLDING_REGISTERS);   lua_setfield(L, -2, "FC_READ_HOLDING");
    lua_pushinteger(L, MODBUS_FC_READ_INPUT_REGISTERS);     lua_setfield(L, -2, "FC_READ_INPUT");
    lua_pushinteger(L, MODBUS_FC_WRITE_SINGLE_COIL);        lua_setfield(L, -2, "FC_WRITE_COIL");
    lua_pushinteger(L, MODBUS_FC_WRITE_SINGLE_REGISTER);    lua_setfield(L, -2, "FC_WRITE_REGISTER");
    lua_pushinteger(L, MODBUS_FC_WRITE_MULTIPLE_COILS);     lua_setfield(L, -2, "FC_WRITE_COILS");
    lua_pushinteger(L, MODBUS_FC_WRITE_MULTIPLE_REGISTERS); lua_setfield(L, -2, "FC_WRITE_REGISTERS");
}

LUAMOD_API int luaopen_modbus_register(lua_State *L)
{
    luaL_newlib(L, modbus_lib);
    modbus_register_constants(L);
    return 1;
}
