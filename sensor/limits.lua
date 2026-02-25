local limits = {}
local log = require("logging").logger("limits")

local configs = require "configs"

local pins = {}
local named_pins = {}

local function on_input(id, level)
    local pin = pins[id]
    if not pin then
        log.warn("未知IO", id, level)
        return
    end

    pin.level = level
    log.info("limit", id, level, pin.name)

    -- 发送统一事件
    iot.emit("LIMIT", id, level, pin.name)

    -- 发送特定事件
    if pin.event and pin.event ~= "" then
        iot.emit(pin.event, level)
    end
end

-- 注册GPIO
function limits.register(pin)
    pin.gpio = iot.gpio(pin.id, {
        rising = pin.rising or false,
        falling = pin.falling or false,
        debounce = pin.debounce or 50,
        level = pin.level or 0, -- 默认是低电平
        callback = on_input -- 输入回调
    })
    pins[pin.id] = pin
    if pin.name and pin.name ~= "" then
        named_pins[pin.name] = pin
    end
    return pin.gpio
end

-- 加载GPIO
function limits.load(cfg)
    -- 关闭已经打开的GPIO
    for i, pin in pairs(pins) do
        pin.gpio:close()
    end

    -- 清空
    pins = {}
    named_pins = {}

    -- 加载配置
    local mapping = configs.load_default(cfg or "limits", {})
    for _, pin in ipairs(mapping) do
        limits.register(pin)
    end
end

-- 获取GPIO实例
function limits.gpio(id)
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

-- 获取GPIO状态
function limits.get(id)
    local gpio = limits.gpio(id)
    if not gpio then
        return nil
    end
    return gpio:get()
end

return limits
