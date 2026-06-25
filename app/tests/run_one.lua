--- 运行单个模块验证
--- 用法: dofile("app/tests/run_one.lua")("zlib")
return function(name)
    local T = dofile("app/tests/common.lua")

    local tests = {
        zlib = { path = "app/tests/test_zlib.lua", sync = true, need_net = false },
        net  = { path = "app/tests/test_net.lua",  sync = false, need_net = true },
        http = { path = "app/tests/test_http.lua", sync = false, need_net = true },
        mqtt = { path = "app/tests/test_mqtt.lua", sync = false, need_net = true },
        uart = { path = "app/tests/test_uart.lua", sync = false, need_net = false },
        lvgl = { path = "app/tests/test_lvgl.lua", sync = true, need_net = false },
    }

    local entry = tests[name]
    if not entry then
        error("unknown test: " .. tostring(name) .. " (zlib|net|http|mqtt|uart|lvgl)")
    end

    local timeouts = {
        zlib = 5000,
        net  = 60000,
        http = 90000,
        mqtt = 120000,
        uart = 30000,
        lvgl = 5000,
    }

    local function run_test()
        log.info("[TEST]", "start", name)
        local ok, fn_or_err = pcall(dofile, entry.path)
        if not ok then
            T.fail(name .. ".load", tostring(fn_or_err))
            return
        end
        local ok2, err = pcall(fn_or_err)
        if not ok2 then
            T.fail(name .. ".run", tostring(err))
        end
        log.info("[TEST]", "done", name)
    end

    log.info("[TEST]", "single module:", name)

    if entry.need_net then
        local net_mod = T.mod("net")
        if type(net_mod.init) ~= "function" or not net_mod.init() then
            T.fail("net.init", "network stack init failed")
            iot.setTimeout(function() T.summary() end, 1000)
            return
        end
    end

    if entry.sync then
        run_test()
    else
        iot.start(function()
            run_test()
        end)
    end

    iot.setTimeout(function()
        T.summary()
    end, timeouts[name] or 60000)
end
