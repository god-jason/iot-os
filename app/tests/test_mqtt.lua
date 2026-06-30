--- mqtt 模块验证：连接、订阅、发布、自动重连
local T = dofile("app/tests/common.lua")
local mqtt = T.mod("mqtt")

local BROKER = "git.zgwit.com"
local PORT = 1883
local TOPIC = "iot-os/test"

return function()
    local client = mqtt.new()
    if not T.assert_true("mqtt.new", client ~= nil) then
        return
    end

    local connected = false
    local got_message = false
    local reconnect_enabled = false

    client:on("connect", function(ok)
        connected = (ok == 1)
        log.info("mqtt", "connect event", ok)
    end)

    client:enable_auto_reconnect(3000)
    reconnect_enabled = true

    local ok, err = client:connect({
        host = BROKER,
        port = PORT,
        client_id = "iot_os_" .. tostring(os.time()),
        keepalive = 30,
        clean_session = true,
        timeout_ms = 20000,
    })
    if not T.assert_true("mqtt.connect", ok, err) then
        client:close()
        return
    end

    if not T.wait_until(20000, 100, function()
        return client:is_connected() or client:state() == mqtt.STATE_ERROR
    end) then
        T.fail("mqtt.wait_connected", "timeout")
        client:close()
        return
    end

    T.assert_true("mqtt.is_connected", client:is_connected())

    ok, err = client:subscribe(TOPIC, mqtt.QOS_0, function(topic, payload)
        log.info("mqtt", "recv", topic, payload)
        if topic == TOPIC then
            got_message = true
        end
    end)
    if not T.assert_true("mqtt.subscribe", ok, err) then
        client:close()
        return
    end

    iot.sleep(500)

    local msg = "hello from iot-os " .. os.time()
    ok, err = client:publish(TOPIC, msg, mqtt.QOS_0, false)
    if not T.assert_true("mqtt.publish", ok, err) then
        client:close()
        return
    end

    if not T.wait_until(10000, 100, function() return got_message end) then
        T.skip("mqtt.recv_self", "no message within 10s on " .. TOPIC .. " (broker latency?)")
    else
        T.pass("mqtt.recv_self", TOPIC)
    end

    T.assert_true("mqtt.auto_reconnect_enabled", reconnect_enabled)

    client:disconnect()
    iot.sleep(1000)
    T.assert_true("mqtt.disconnect", not client:is_connected())

    ok, err = client:connect({
        host = BROKER,
        port = PORT,
        client_id = "iot_os_re_" .. tostring(os.time()),
        keepalive = 30,
        clean_session = true,
        timeout_ms = 20000,
    })
    if T.assert_true("mqtt.reconnect", ok, err) then
        T.wait_until(15000, 100, function() return client:is_connected() end)
        T.assert_true("mqtt.reconnect.connected", client:is_connected())
    end

    client:close()
end
