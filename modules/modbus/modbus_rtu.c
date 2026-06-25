/**
 * @file modbus_rtu.c
 * @brief Modbus RTU 帧编解码与数据解析
 */

#include "modbus_rtu.h"
#include <string.h>

static modbus_err_t modbus_rtu_finalize(uint8_t *out, size_t out_cap, size_t data_len, size_t *out_len)
{
    uint16_t crc;

    if (!out || !out_len || data_len + 2 > out_cap || data_len + 2 > MODBUS_RTU_MAX_FRAME) {
        return MODBUS_ERR_BUF;
    }

    crc = modbus_crc16(out, data_len);
    out[data_len] = (uint8_t)(crc & 0xFF);
    out[data_len + 1] = (uint8_t)((crc >> 8) & 0xFF);
    *out_len = data_len + 2;
    return MODBUS_OK;
}

uint16_t modbus_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    size_t i;
    int b;

    if (!data) {
        return crc;
    }

    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (b = 0; b < 8; b++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

bool modbus_crc16_verify(const uint8_t *frame, size_t len)
{
    uint16_t crc;
    uint16_t recv;

    if (!frame || len < MODBUS_RTU_MIN_FRAME) {
        return false;
    }

    crc = modbus_crc16(frame, len - 2);
    recv = (uint16_t)frame[len - 2] | ((uint16_t)frame[len - 1] << 8);
    return crc == recv;
}

size_t modbus_crc16_append(uint8_t *out, size_t out_cap, const uint8_t *data, size_t data_len)
{
    uint16_t crc;

    if (!out || !data || data_len + 2 > out_cap) {
        return 0;
    }

    memcpy(out, data, data_len);
    crc = modbus_crc16(out, data_len);
    out[data_len] = (uint8_t)(crc & 0xFF);
    out[data_len + 1] = (uint8_t)((crc >> 8) & 0xFF);
    return data_len + 2;
}

modbus_err_t modbus_rtu_build_read(uint8_t addr, uint8_t fc, uint16_t start, uint16_t qty,
                                   uint8_t *out, size_t out_cap, size_t *out_len)
{
    if (!out || !out_len || addr == 0 || qty == 0) {
        return MODBUS_ERR_PARAM;
    }

    if (fc != MODBUS_FC_READ_COILS &&
        fc != MODBUS_FC_READ_DISCRETE_INPUTS &&
        fc != MODBUS_FC_READ_HOLDING_REGISTERS &&
        fc != MODBUS_FC_READ_INPUT_REGISTERS) {
        return MODBUS_ERR_PARAM;
    }

    if (out_cap < 8) {
        return MODBUS_ERR_BUF;
    }

    out[0] = addr;
    out[1] = fc;
    out[2] = (uint8_t)((start >> 8) & 0xFF);
    out[3] = (uint8_t)(start & 0xFF);
    out[4] = (uint8_t)((qty >> 8) & 0xFF);
    out[5] = (uint8_t)(qty & 0xFF);

    return modbus_rtu_finalize(out, out_cap, 6, out_len);
}

modbus_err_t modbus_rtu_build_write_register(uint8_t addr, uint16_t reg, uint16_t value,
                                             uint8_t *out, size_t out_cap, size_t *out_len)
{
    if (!out || !out_len || addr == 0) {
        return MODBUS_ERR_PARAM;
    }

    if (out_cap < 8) {
        return MODBUS_ERR_BUF;
    }

    out[0] = addr;
    out[1] = MODBUS_FC_WRITE_SINGLE_REGISTER;
    out[2] = (uint8_t)((reg >> 8) & 0xFF);
    out[3] = (uint8_t)(reg & 0xFF);
    out[4] = (uint8_t)((value >> 8) & 0xFF);
    out[5] = (uint8_t)(value & 0xFF);

    return modbus_rtu_finalize(out, out_cap, 6, out_len);
}

modbus_err_t modbus_rtu_build_write_registers(uint8_t addr, uint16_t start,
                                              const uint16_t *values, size_t count,
                                              uint8_t *out, size_t out_cap, size_t *out_len)
{
    size_t byte_count;
    size_t i;
    size_t pos;

    if (!out || !out_len || !values || addr == 0 || count == 0 || count > 123) {
        return MODBUS_ERR_PARAM;
    }

    byte_count = count * 2;
    if (7 + byte_count + 2 > out_cap || 7 + byte_count + 2 > MODBUS_RTU_MAX_FRAME) {
        return MODBUS_ERR_BUF;
    }

    out[0] = addr;
    out[1] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    out[2] = (uint8_t)((start >> 8) & 0xFF);
    out[3] = (uint8_t)(start & 0xFF);
    out[4] = (uint8_t)((count >> 8) & 0xFF);
    out[5] = (uint8_t)(count & 0xFF);
    out[6] = (uint8_t)byte_count;

    pos = 7;
    for (i = 0; i < count; i++) {
        out[pos++] = (uint8_t)((values[i] >> 8) & 0xFF);
        out[pos++] = (uint8_t)(values[i] & 0xFF);
    }

    return modbus_rtu_finalize(out, out_cap, pos, out_len);
}

modbus_err_t modbus_rtu_build_write_coil(uint8_t addr, uint16_t coil, bool on,
                                         uint8_t *out, size_t out_cap, size_t *out_len)
{
    uint16_t value = on ? 0xFF00 : 0x0000;

    if (!out || !out_len || addr == 0) {
        return MODBUS_ERR_PARAM;
    }

    if (out_cap < 8) {
        return MODBUS_ERR_BUF;
    }

    out[0] = addr;
    out[1] = MODBUS_FC_WRITE_SINGLE_COIL;
    out[2] = (uint8_t)((coil >> 8) & 0xFF);
    out[3] = (uint8_t)(coil & 0xFF);
    out[4] = (uint8_t)((value >> 8) & 0xFF);
    out[5] = (uint8_t)(value & 0xFF);

    return modbus_rtu_finalize(out, out_cap, 6, out_len);
}

modbus_err_t modbus_rtu_build_write_coils(uint8_t addr, uint16_t start,
                                          const uint8_t *bits, size_t bit_count,
                                          uint8_t *out, size_t out_cap, size_t *out_len)
{
    size_t byte_count;
    size_t i;
    size_t pos;

    if (!out || !out_len || !bits || addr == 0 || bit_count == 0 || bit_count > 1968) {
        return MODBUS_ERR_PARAM;
    }

    byte_count = (bit_count + 7) / 8;
    if (7 + byte_count + 2 > out_cap || 7 + byte_count + 2 > MODBUS_RTU_MAX_FRAME) {
        return MODBUS_ERR_BUF;
    }

    out[0] = addr;
    out[1] = MODBUS_FC_WRITE_MULTIPLE_COILS;
    out[2] = (uint8_t)((start >> 8) & 0xFF);
    out[3] = (uint8_t)(start & 0xFF);
    out[4] = (uint8_t)((bit_count >> 8) & 0xFF);
    out[5] = (uint8_t)(bit_count & 0xFF);
    out[6] = (uint8_t)byte_count;

    pos = 7;
    for (i = 0; i < byte_count; i++) {
        out[pos++] = bits[i];
    }

    return modbus_rtu_finalize(out, out_cap, pos, out_len);
}

modbus_err_t modbus_rtu_parse(const uint8_t *frame, size_t len, modbus_rtu_frame_t *out)
{
    if (!frame || !out || len < MODBUS_RTU_MIN_FRAME) {
        return MODBUS_ERR_PARAM;
    }

    if (!modbus_crc16_verify(frame, len)) {
        return MODBUS_ERR_CRC;
    }

    memset(out, 0, sizeof(*out));
    out->addr = frame[0];
    out->fc = frame[1];

    if (out->fc & 0x80) {
        if (len < 5) {
            return MODBUS_ERR_FORMAT;
        }
        out->is_exception = true;
        out->exception_code = frame[2];
        out->payload = frame + 2;
        out->payload_len = 1;
        return MODBUS_ERR_EXCEPTION;
    }

    if (len <= 4) {
        return MODBUS_ERR_FORMAT;
    }

    out->payload = frame + 2;
    out->payload_len = len - 4;
    return MODBUS_OK;
}

modbus_err_t modbus_parse_read_bits(const uint8_t *payload, size_t payload_len,
                                    size_t bit_count, uint8_t *bits_out)
{
    size_t byte_count;
    size_t i;

    if (!payload || !bits_out || bit_count == 0) {
        return MODBUS_ERR_PARAM;
    }

    if (payload_len < 1) {
        return MODBUS_ERR_FORMAT;
    }

    byte_count = payload[0];
    if (payload_len < 1 + byte_count) {
        return MODBUS_ERR_FORMAT;
    }

    for (i = 0; i < bit_count; i++) {
        size_t src_byte = i / 8;
        size_t src_bit = i % 8;
        if (src_byte >= byte_count) {
            bits_out[i] = 0;
        } else {
            bits_out[i] = (payload[1 + src_byte] >> src_bit) & 0x01;
        }
    }

    return MODBUS_OK;
}

modbus_err_t modbus_parse_read_registers(const uint8_t *payload, size_t payload_len,
                                         uint16_t *regs_out, size_t reg_count)
{
    size_t byte_count;
    size_t i;
    size_t pos;

    if (!payload || !regs_out || reg_count == 0) {
        return MODBUS_ERR_PARAM;
    }

    if (payload_len < 1) {
        return MODBUS_ERR_FORMAT;
    }

    byte_count = payload[0];
    if (byte_count != reg_count * 2 || payload_len < 1 + byte_count) {
        return MODBUS_ERR_FORMAT;
    }

    pos = 1;
    for (i = 0; i < reg_count; i++) {
        regs_out[i] = ((uint16_t)payload[pos] << 8) | payload[pos + 1];
        pos += 2;
    }

    return MODBUS_OK;
}

static uint16_t modbus_swap16(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}

modbus_err_t modbus_regs_to_uint32(uint16_t hi, uint16_t lo, const char *order, uint32_t *out)
{
    uint16_t w0 = hi;
    uint16_t w1 = lo;
    uint32_t value;

    if (!out) {
        return MODBUS_ERR_PARAM;
    }

    if (order == NULL || order[0] == '\0' || strcmp(order, "ABCD") == 0) {
        value = ((uint32_t)w0 << 16) | w1;
    } else if (strcmp(order, "CDAB") == 0) {
        value = ((uint32_t)w1 << 16) | w0;
    } else if (strcmp(order, "BADC") == 0) {
        value = ((uint32_t)modbus_swap16(w0) << 16) | modbus_swap16(w1);
    } else if (strcmp(order, "DCBA") == 0) {
        value = ((uint32_t)modbus_swap16(w1) << 16) | modbus_swap16(w0);
    } else {
        return MODBUS_ERR_PARAM;
    }

    *out = value;
    return MODBUS_OK;
}

modbus_err_t modbus_regs_to_int32(uint16_t hi, uint16_t lo, const char *order, int32_t *out)
{
    uint32_t u;
    modbus_err_t err = modbus_regs_to_uint32(hi, lo, order, &u);

    if (err != MODBUS_OK) {
        return err;
    }

    *out = (int32_t)u;
    return MODBUS_OK;
}

modbus_err_t modbus_regs_to_float32(uint16_t hi, uint16_t lo, const char *order, float *out)
{
    uint32_t u;
    modbus_err_t err = modbus_regs_to_uint32(hi, lo, order, &u);
    union {
        uint32_t u32;
        float f;
    } conv;

    if (err != MODBUS_OK || !out) {
        return err != MODBUS_OK ? err : MODBUS_ERR_PARAM;
    }

    conv.u32 = u;
    *out = conv.f;
    return MODBUS_OK;
}
