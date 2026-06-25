--- modbus RTU 模块验证
local T = dofile("app/tests/common.lua")
local modbus = T.mod("modbus")

return function()
    local pdu = string.char(0x01, modbus.FC_READ_HOLDING, 0x00, 0x00, 0x00, 0x02)
    local frame = modbus.append_crc(pdu)

    T.assert_true("modbus.verify", modbus.verify(frame))
    T.assert_true("modbus.crc16", modbus.crc16(pdu) == 0x0BC4)

    local req = modbus.build_read_holding(1, 0, 2)
    T.assert_true("modbus.build_read_holding", req ~= nil)
    T.assert_true("modbus.build_verify", modbus.verify(req))

    local parsed = modbus.parse(req)
    T.assert_true("modbus.parse", parsed ~= nil)
    T.assert_true("modbus.parse.addr", parsed.addr == 1)
    T.assert_true("modbus.parse.fc", parsed.fc == modbus.FC_READ_HOLDING)

    local resp_pdu = string.char(0x01, modbus.FC_READ_HOLDING, 0x04, 0x01, 0x2C, 0x00, 0x64)
    local resp = modbus.append_crc(resp_pdu)
    local rsp = modbus.parse(resp)
    T.assert_true("modbus.parse_resp", rsp ~= nil)

    local regs = modbus.decode_registers(rsp.payload, 2)
    T.assert_true("modbus.decode_registers", regs ~= nil)
    T.assert_true("modbus.reg0", regs[1] == 0x012C)
    T.assert_true("modbus.reg1", regs[2] == 0x0064)

    local wreq = modbus.build_write_register(1, 10, 1234)
    T.assert_true("modbus.build_write_register", wreq ~= nil and modbus.verify(wreq))

    local bits = modbus.decode_bits(string.char(0x01, 0x05), 8)
    T.assert_true("modbus.decode_bits", bits ~= nil)
    T.assert_true("modbus.bit0", bits[1] == 1)
    T.assert_true("modbus.bit2", bits[3] == 1)
end
