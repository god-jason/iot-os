--- uart 模块验证：打开系统串口并尝试收发
-- Windows: id=0 -> COM1, id=1 -> COM2
local T = dofile("app/tests/common.lua")

return function()
    if not uart then
        T.skip("uart.module", "uart not registered on this platform")
        return
    end

    local uart_id = 0
    local baud = 115200
    local opened = uart.setup(uart_id, baud)

    if not opened then
        T.skip("uart.setup", string.format(
            "cannot open COM%d (id=%d). Install virtual COM pair or connect hardware.",
            uart_id + 1, uart_id
        ))
        return
    end

    T.pass("uart.setup", string.format("COM%d @ %d", uart_id + 1, baud))

    local received = false
    uart.on(uart_id, function()
        local data = uart.read(uart_id, 256)
        if data and #data > 0 then
            received = true
            log.info("uart", "rx", data)
        end
    end)

    local payload = "AT\r\n"
    local sent = uart.write(uart_id, payload)
    T.assert_true("uart.write", sent and sent > 0, "sent=" .. tostring(sent))

    -- 有回环/对端时会收到数据；否则只验证写成功
    T.wait_until(2000, 100, function() return received end)
    if received then
        T.pass("uart.read", "received data")
    else
        T.skip("uart.read", "no RX within 2s (need loopback or peer on COM" .. (uart_id + 1) .. ")")
    end

    uart.close(uart_id)
    T.pass("uart.close")
end
