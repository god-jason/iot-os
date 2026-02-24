local inputs = {}
local tag = "inputs"

local configs = require "configs"

--[[
{{
    id = 1,
    name = "",
    event = "KEY_START",
    rising = true,
    falling = true,
    debounce = 0
}}
]]

local mapping = configs.load_default("inputs", {})

local pins = {}
local named_pins = {}

local function on_input(id, level)
    local pin = pins[id]
    if not pin then
        log.warn(tag, "未知IO", id, level)
        return
    end

    log.info(tag, "input", id, level, pin.name)

    -- 发送统一事件
    iot.emit("INPUT", id, level, pin.name)

    -- 发送特定事件
    if pin.event and pin.event ~= "" then
        iot.emit(pin.event, level)
    end
end

function inputs.init()
    for _, pin in ipairs(mapping) do
        pin.gpio = io.gpio(pin.id, {
            rising = pin.rising,
            falling = pin.falling,
            debounce = pin.debounce,
            callback = on_input -- 输入回调
        })
        pins[pin.id] = pin
        if pin.name and pin.name ~= "" then
            named_pins[pin.name] = pin
        end
    end
end

function inputs.gpio(id)
    local pin

    if type(id) == "number" then
        pin = pins[id]
    elseif type(id) == "string" then
        pin = named_pins[id]
    end

    if not pin then
        return nil
    end
    return pin.gpio
end

function inputs.get(id)
    local gpio = inputs.gpio(id)
    if not gpio then
        return nil
    end
    return gpio:get()
end

return inputs
