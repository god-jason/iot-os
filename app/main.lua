
log.info("test")

local tab = {
    a = 1,
    b = 2,
    c = 3.14,
    d = "hello world",
    e = true,
    f = nil,
    g = {
        a = 1,
        b = 2,
        c = 3.14,
        d = "hello world",
        e = true,
        f = nil,
    }
}


local t1 = os.time()
for i = 1, 1000000 do
    json.encode(tab)
end
log.info("test json", os.date("%c", os.time()), os.time() - t1)

iot.setInterval(function()
    log.info("test", os.date("%c", os.time()), json.encode({a = 1, b = 2}))
end, 1000)

iot.start(function ()
    while true do
        log.info("test sleep")
        iot.sleep(2000)    
    end
end)