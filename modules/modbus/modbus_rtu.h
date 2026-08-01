/**
 * @file modbus_rtu.h
 * @brief Modbus RTU 帧编解码与数据解析接口
 *
 * 提供 Modbus RTU 协议的帧构建、解析、CRC16 校验功能
 * 支持读写线圈、读写寄存器等常用功能码
 * 提供寄存器数据类型转换工具（uint32/int32/float）
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_MODBUS_RTU_H
#define IOT_MODBUS_RTU_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IOT_MODBUS_RTU_MAX_FRAME  256
#define IOT_MODBUS_RTU_MIN_FRAME  4

#define IOT_MODBUS_FC_READ_COILS              0x01
#define IOT_MODBUS_FC_READ_DISCRETE_INPUTS    0x02
#define IOT_MODBUS_FC_READ_HOLDING_REGISTERS  0x03
#define IOT_MODBUS_FC_READ_INPUT_REGISTERS    0x04
#define IOT_MODBUS_FC_WRITE_SINGLE_COIL       0x05
#define IOT_MODBUS_FC_WRITE_SINGLE_REGISTER   0x06
#define IOT_MODBUS_FC_WRITE_MULTIPLE_COILS    0x0F
#define IOT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

typedef enum {
    IOT_MODBUS_OK = 0,
    IOT_MODBUS_ERR_PARAM = -1,
    IOT_MODBUS_ERR_CRC = -2,
    IOT_MODBUS_ERR_FORMAT = -3,
    IOT_MODBUS_ERR_EXCEPTION = -4,
    IOT_MODBUS_ERR_BUF = -5,
} iot_modbus_err_t;

typedef struct {
    uint8_t addr;
    uint8_t fc;
    const uint8_t *payload;
    size_t payload_len;
    bool is_exception;
    uint8_t exception_code;
} iot_modbus_rtu_frame_t;

uint16_t iot_modbus_crc16(const uint8_t *data, size_t len);
bool iot_modbus_crc16_verify(const uint8_t *frame, size_t len);
size_t iot_modbus_crc16_append(uint8_t *out, size_t out_cap, const uint8_t *data, size_t data_len);

iot_modbus_err_t iot_modbus_rtu_build_read(uint8_t addr, uint8_t fc, uint16_t start, uint16_t qty,
                                   uint8_t *out, size_t out_cap, size_t *out_len);

iot_modbus_err_t iot_modbus_rtu_build_write_register(uint8_t addr, uint16_t reg, uint16_t value,
                                             uint8_t *out, size_t out_cap, size_t *out_len);

iot_modbus_err_t iot_modbus_rtu_build_write_registers(uint8_t addr, uint16_t start,
                                              const uint16_t *values, size_t count,
                                              uint8_t *out, size_t out_cap, size_t *out_len);

iot_modbus_err_t iot_modbus_rtu_build_write_coil(uint8_t addr, uint16_t coil, bool on,
                                         uint8_t *out, size_t out_cap, size_t *out_len);

iot_modbus_err_t iot_modbus_rtu_build_write_coils(uint8_t addr, uint16_t start,
                                            const uint8_t *bits, size_t bit_count,
                                            uint8_t *out, size_t out_cap, size_t *out_len);

iot_modbus_err_t iot_modbus_rtu_parse(const uint8_t *frame, size_t len, iot_modbus_rtu_frame_t *out);

iot_modbus_err_t iot_modbus_parse_read_bits(const uint8_t *payload, size_t payload_len,
                                    size_t bit_count, uint8_t *bits_out);

iot_modbus_err_t iot_modbus_parse_read_registers(const uint8_t *payload, size_t payload_len,
                                           uint16_t *regs_out, size_t reg_count);

iot_modbus_err_t iot_modbus_regs_to_uint32(uint16_t hi, uint16_t lo, const char *order, uint32_t *out);
iot_modbus_err_t iot_modbus_regs_to_int32(uint16_t hi, uint16_t lo, const char *order, int32_t *out);
iot_modbus_err_t iot_modbus_regs_to_float32(uint16_t hi, uint16_t lo, const char *order, float *out);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MODBUS_RTU_H */
