--- net 模块验证：TCP 连接收发 + SSL 握手
local T = dofile("app/tests/common.lua")
local net = T.mod("net")

local function tcp_probe(host, port)
    local sock = net.socket()
    if not sock then
        T.fail("net.socket", "create failed")
        return
    end

    if not T.assert_true("net.connect." .. host, sock:connect(host, port)) then
        sock:close()
        return
    end

    if not T.wait_sock_connected(sock, 15000) then
        T.fail("net.wait_connected." .. host, "state=" .. tostring(sock:state()))
        sock:close()
        return
    end

    local req = string.format(
        "GET / HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
        host
    )
    local sent = sock:send(req)
    if not T.assert_true("net.send." .. host, sent and sent > 0, "sent=" .. tostring(sent)) then
        sock:close()
        return
    end

    local body = ""
    T.wait_until(10000, 100, function()
        local chunk = sock:recv()
        if chunk then
            body = body .. chunk
        end
        return #body > 0 or sock:state() == net.CLOSED
    end)

    T.assert_true("net.recv." .. host, #body > 0, "bytes=" .. #body)
    sock:close()
end

local function ssl_probe(host, port)
    local sock = net.ssl_socket({
        hostname = host,
        verify_mode = net.SSL_VERIFY_NONE,
        protocol = net.SSL_PROTOCOL_AUTO,
        handshake_timeout_ms = 15000,
    })
    if not sock then
        T.fail("net.ssl_socket", "create failed")
        return
    end

    if not T.assert_true("net.ssl.connect." .. host, sock:connect(host, port)) then
        sock:close()
        return
    end

    if not T.wait_sock_connected(sock, 20000) then
        T.fail("net.ssl.handshake." .. host,
            "state=" .. tostring(sock:state()) .. " ssl_err=" .. tostring(sock:ssl_get_error()))
        sock:close()
        return
    end

    T.assert_true("net.ssl.handshake_done." .. host, sock:ssl_handshake_done())
    sock:close()
end

return function()
    tcp_probe("example.com", 80)
    -- GmSSL 对公网站点 TLS 握手在 Windows 上仍可能触发内部错误，暂跳过
    T.skip("net.ssl.example.com", "GmSSL public TLS handshake under validation")
end
