local log = iot.logger("relay")

local Relay = {}
Relay.__index = Relay

require("components").register("relay", Relay)

function Relay:new(opts)
    opts = opts or {}
    local relay = setmetatable({
        pin = opts.pin,
        reverse = opts.reverse or false,
        gpio = iot.gpio(opts.pin)
    }, Relay)
    return relay
end

function Relay:on()
    log.info("on")
    self.gpio:set(self.reverse and 0 or 1)
end

function Relay:off()
    log.info("off")
    self.gpio:set(self.reverse and 1 or 0)
end

function Relay:set(onOff)
    log.info("set", onOff)
    if self.reverse then
        self.gpio:set(onOff and 0 or 1)
    else
        self.gpio:set(onOff and 1 or 0)
    end
end

function Relay:toggle()
    log.info("toggle")
    self.gpio:toggle()
end

return Relay
