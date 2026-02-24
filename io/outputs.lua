local outputs = {}
local tag = "outputs"


local configs = require "configs"
local mapping = configs.load_default("outputs", {})

local pins = {}
local named_pins = {}

function outputs.init()
    for _, pin in ipairs(mapping) do
        pin.gpio = io.gpio(pin.id)
        pins[pin.id] = pin
        if pin.name and pin.name ~= "" then
            named_pins[pin.name] = pin
        end
    end
end

function outputs.gpio(id)
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

function outputs.set(id, level)
    local gpio = outputs.gpio(id)
    if not gpio then
        return false, "找不到IO"
    end
    gpio:set(level)
    return true
end

return outputs
