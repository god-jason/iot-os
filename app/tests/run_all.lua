--- 运行全部模块验证
local T = dofile("app/tests/common.lua")
local M = T

local tests = {
    { name = "zlib", path = "app/tests/test_zlib.lua", sync = true },
    { name = "modbus", path = "app/tests/test_modbus.lua", sync = true },
    { name = "net", path = "app/tests/test_net.lua", sync = false },
    { name = "http", path = "app/tests/test_http.lua", sync = false },
    { name = "mqtt", path = "app/tests/test_mqtt.lua", sync = false },
    { name = "uart", path = "app/tests/test_uart.lua", sync = false },
    { name = "lvgl", path = "app/tests/test_lvgl.lua", sync = true },
}

local function run_one(entry)
    log.info("[TEST]", "start", entry.name)
    local ok, fn_or_err = pcall(dofile, entry.path)
    if not ok then
        T.fail(entry.name .. ".load", tostring(fn_or_err))
        return
    end
    local ok2, err = pcall(fn_or_err)
    if not ok2 then
        T.fail(entry.name .. ".run", tostring(err))
    end
    log.info("[TEST]", "done", entry.name)
end

local function run_all()
    log.info("[TEST]", "module verification start")

    local net_mod = M.mod("net")
    if type(net_mod.init) ~= "function" or not net_mod.init() then
        T.fail("net.init", "network stack init failed")
    end

    for _, entry in ipairs(tests) do
        if entry.sync then
            run_one(entry)
        end
    end

    iot.start(function()
        run_one({ name = "net", path = "app/tests/test_net.lua" })
        run_one({ name = "http", path = "app/tests/test_http.lua" })
        run_one({ name = "mqtt", path = "app/tests/test_mqtt.lua" })
        run_one({ name = "uart", path = "app/tests/test_uart.lua" })
    end)

    iot.setTimeout(function()
        T.summary()
    end, 120000)
end

return run_all
