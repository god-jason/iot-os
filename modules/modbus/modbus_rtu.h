/**
 * @file modbus_rtu.h
 * @brief Modbus RTU 帧编解码与数据解析
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MODBUS_RTU_MAX_FRAME  256
#define MODBUS_RTU_MIN_FRAME  4

#define MODBUS_FC_READ_COILS              0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS    0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS  0x03
#define MODBUS_FC_READ_INPUT_REGISTERS    0x04
#define MODBUS_FC_WRITE_SINGLE_COIL       0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER   0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS    0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

typedef enum {
    MODBUS_OK = 0,
    MODBUS_ERR_PARAM = -1,
    MODBUS_ERR_CRC = -2,
    MODBUS_ERR_FORMAT = -3,
    MODBUS_ERR_EXCEPTION = -4,
    MODBUS_ERR_BUF = -5,
} modbus_err_t;

typedef struct {
    uint8_t addr;
    uint8_t fc;
    const uint8_t *payload;
    size_t payload_len;
    bool is_exception;
    uint8_t exception_code;
} modbus_rtu_frame_t;

uint16_t modbus_crc16(const uint8_t *data, size_t len);
bool modbus_crc16_verify(const uint8_t *frame, size_t len);
size_t modbus_crc16_append(uint8_t *out, size_t out_cap, const uint8_t *data, size_t data_len);

modbus_err_t modbus_rtu_build_read(uint8_t addr, uint8_t fc, uint16_t start, uint16_t qty,
                                   uint8_t *out, size_t out_cap, size_t *out_len);

modbus_err_t modbus_rtu_build_write_register(uint8_t addr, uint16_t reg, uint16_t value,
                                             uint8_t *out, size_t out_cap, size_t *out_len);

modbus_err_t modbus_rtu_build_write_registers(uint8_t addr, uint16_t start,
                                              const uint16_t *values, size_t count,
                                              uint8_t *out, size_t out_cap, size_t *out_len);

modbus_err_t modbus_rtu_build_write_coil(uint8_t addr, uint16_t coil, bool on,
                                         uint8_t *out, size_t out_cap, size_t *out_len);

modbus_err_t modbus_rtu_build_write_coils(uint8_t addr, uint16_t start,
                                            const uint8_t *bits, size_t bit_count,
                                            uint8_t *out, size_t out_cap, size_t *out_len);

modbus_err_t modbus_rtu_parse(const uint8_t *frame, size_t len, modbus_rtu_frame_t *out);

modbus_err_t modbus_parse_read_bits(const uint8_t *payload, size_t payload_len,
                                    size_t bit_count, uint8_t *bits_out);

modbus_err_t modbus_parse_read_registers(const uint8_t *payload, size_t payload_len,
                                           uint16_t *regs_out, size_t reg_count);

modbus_err_t modbus_regs_to_uint32(uint16_t hi, uint16_t lo, const char *order, uint32_t *out);
modbus_err_t modbus_regs_to_int32(uint16_t hi, uint16_t lo, const char *order, int32_t *out);
modbus_err_t modbus_regs_to_float32(uint16_t hi, uint16_t lo, const char *order, float *out);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */
