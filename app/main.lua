
iot.setInterval(function()
    log.info("test", os.date("%c", os.time()), json.encode({a = 1, b = 2}))
end, 1000)
