local components = {
    -- fan = Fan:new({})
}
-- 注册到全局
_G.components = components

local configs = require("configs")

local Fan = require("fan")
local Led = require("led")
local Relay = require("relay")
local Servo = require("servo")
local Speeder = require("speeder")
local Stepper = require("stepper")
local Switch = require("switch")

-- 创建组件
local function create_component(cmp)
    log.info("load", iot.json_encode(cmp))

    if cmp.type == "fan" then
        components[cmp.name] = Fan:new(cmp)
    elseif cmp.type == "led" then
        components[cmp.name] = Led:new(cmp)
    elseif cmp.type == "relay" then
        components[cmp.name] = Relay:new(cmp)
    elseif cmp.type == "servo" then
        components[cmp.name] = Servo:new(cmp)
    elseif cmp.type == "speeder" then
        components[cmp.name] = Speeder:new(cmp)
    elseif cmp.type == "stepper" then
        components[cmp.name] = Stepper:new(cmp)
    elseif cmp.type == "switch" then
        components[cmp.name] = Switch:new(cmp)
    else
        log.error("unkown type", cmp.type)
        return false, "unkown type" .. cmp.type
    end

    return true
end

function components.load()
    log.info("load")

    local cms = configs.load_default("components", {})
    for k, v in ipairs(cms) do        
        local ret, info = create_component(v)
        if not ret then
            return ret, info
        end
    end
    for k, v in pairs(cms) do
        v.name = v.name or k -- key作为设备名
        local ret, info = create_component(v)
        if not ret then
            return ret, info
        end
    end

    return true
end

return components
