--- 模块测试公共工具（单例，汇总结果共享）
if _G.__iot_test_common then
    return _G.__iot_test_common
end

local M = {}
_G.__iot_test_common = M

M.results = {}

--- 获取 C 层预注册的 Lua 模块
function M.mod(name)
    local m = package.loaded[name]
    if type(m) ~= "table" then
        m = _G[name]
    end
    if type(m) ~= "table" then
        error("module not loaded: " .. name)
    end
    _G[name] = m
    return m
end

function M.pass(name, detail)
    M.results[#M.results + 1] = { name = name, ok = true, detail = detail }
    log.info("[PASS]", name, detail or "")
end

function M.fail(name, detail)
    M.results[#M.results + 1] = { name = name, ok = false, detail = detail or "failed" }
    log.error("[FAIL]", name, detail or "")
end

function M.skip(name, detail)
    M.results[#M.results + 1] = { name = name, ok = nil, detail = detail or "skipped" }
    log.warn("[SKIP]", name, detail or "")
end

function M.assert_true(name, cond, detail)
    if cond then
        M.pass(name, detail)
        return true
    end
    M.fail(name, detail)
    return false
end

function M.wait_until(timeout_ms, interval_ms, predicate)
    local elapsed = 0
    while elapsed < timeout_ms do
        if predicate() then
            return true
        end
        iot.sleep(interval_ms)
        elapsed = elapsed + interval_ms
    end
    return false
end

function M.wait_sock_connected(sock, timeout_ms)
    local net = M.mod("net")
    return M.wait_until(timeout_ms or 15000, 50, function()
        local st = sock:state()
        if st == net.ERROR or st == net.CLOSED then
            return false
        end
        if sock:is_ssl() then
            return st == net.CONNECTED and sock:ssl_handshake_done()
        end
        return st == net.CONNECTED
    end)
end

function M.summary()
    local pass, fail, skip = 0, 0, 0
    for _, r in ipairs(M.results) do
        if r.ok == true then
            pass = pass + 1
        elseif r.ok == false then
            fail = fail + 1
        else
            skip = skip + 1
        end
    end
    log.info("[SUMMARY]", string.format("pass=%d fail=%d skip=%d total=%d", pass, fail, skip, #M.results))
    for _, r in ipairs(M.results) do
        local tag = r.ok == true and "PASS" or (r.ok == false and "FAIL" or "SKIP")
        log.info(string.format("  [%s] %s%s", tag, r.name, r.detail and (" - " .. r.detail) or ""))
    end
    return fail == 0
end

return M
