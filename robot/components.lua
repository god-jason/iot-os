local log = iot.logger("components")

local components = {
    -- fan = Fan:new({})
}

-- 注册到全局
_G.components = components

local settings = require("settings")
local boot = require("boot")

local types = {}

-- 注册组件
function components.register(name, clazz)
    types[name] = clazz
end

-- 创建组件
function components.create(cmp)
    log.info("create", iot.json_encode(cmp))

    local fn = types[cmp.type]
    if not fn then
        return false, "unkown type" .. cmp.type
    end

    components[cmp.name] = fn:new(cmp)
    return true
end

-- 加载组件
function components.open()
    log.info("load")

    local cms = settings.components
    for k, v in ipairs(cms) do
        local ret, info = components.create(v)
        if not ret then
            return ret, info
        end
    end

    return true
end


components.deps = {"settings"}

boot.register("components", components)

settings.register("components", {})

return components
