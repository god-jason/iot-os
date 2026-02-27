local log = iot.logger("switch")

local Switch = {}
Switch.__index = Switch

function Switch:new(opts)
    opts = opts or {}
    local switch = setmetatable({
        pin = opts.pin,
        reverse = opts.reverse or false,
        rising = opts.rising or false,
        falling = opts.falling or false,
        debounce = opts.debounce or 50,
        level = 0,
        callback = opts.callback
    }, Switch)
    return switch
end

function Switch:init()
    local this = self

    self.gpio = iot.gpio(self.pin, {
        rising = self.rising,
        falling = self.falling,
        debounce = self.debounce,
        callback = function(id, level)
            -- 反转
            if this.reverse then
                level = level > 0 and 0 or 1
            end

            self.level = level

            log.info("switch", id, level, this.name)

            if this.disabled then
                return
            end

            -- 回调
            if type(self.callback) == "function" then
                self.callback(level)
            end

            -- 发送统一事件
            iot.emit("SWITCH", {
                pin = id,
                name = this.name,
                event = this.event,
                level = level
            })

            -- 发送特定事件
            if this.event and this.event ~= "" then
                iot.emit(this.event, level)
            end
        end
    })
end

function Switch:status()
    return self.level
end

function Switch:enable()
    log.info("enable", self.pin, self.name)
    self.disabled = false
end

function Switch:disable()
    log.info("disable", self.pin, self.name)
    self.disabled = true
end

return Switch
