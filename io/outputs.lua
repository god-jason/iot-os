local outputs = {}
local log = logging.logger("outputs")

local configs = require "configs"

local pins = {}
local named_pins = {}


-- 注册GPIO
function outputs.register(pin)
    pin.gpio = iot.gpio(pin.id)
    pins[pin.id] = pin
    if pin.name and pin.name ~= "" then
        named_pins[pin.name] = pin
    end
    return pin.gpio
end

-- 加载GPIO
function outputs.load(cfg)
    -- 关闭已经打开的GPIO
    for i, pin in pairs(pins) do
        pin.gpio:close()
    end

    -- 清空
    pins = {}
    named_pins = {}

    -- 加载配置
    local mapping = configs.load_default(cfg or "outputs", {})
    for _, pin in ipairs(mapping) do
        outputs.register(pin)
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
