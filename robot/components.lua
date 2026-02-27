local components = {
    -- fan = Fan:new({})
}
-- 注册到全局
_G.components = components

local configs = require("configs")

local types = {}

-- 注册组件
function components.register(name, fn)
    types[name] = fn
end

-- 创建组件
function components.create(cmp)
    log.info("load", iot.json_encode(cmp))

    local fn = types[cmp.type]
    if not fn then
        return false, "unkown type" .. cmp.type
    end

    components[cmp.name] = fn:new(cmp)
    return true
end


-- 加载组件
function components.load()
    log.info("load")

    local cms = configs.load_default("components", {})
    for k, v in ipairs(cms) do
        local ret, info = components.create(v)
        if not ret then
            return ret, info
        end
    end
    for k, v in pairs(cms) do
        v.name = v.name or k -- key作为设备名
        local ret, info = components.create(v)
        if not ret then
            return ret, info
        end
    end

    return true
end

return components
