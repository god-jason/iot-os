local log = iot.logger("protocols")

local protocols = {}

_G.protocols = protocols

local configs = require("configs")

local types = {}

-- 注册链接
function protocols.register(name, clazz)
    protocols[name] = clazz
end

-- 创建链接
function protocols.create(link, name, opts)
    log.info("create", iot.json_encode(opts))

    local clazz = protocols[name]
    if not clazz then
        return false, "unkown protocol" .. name
    end

    return true, clazz:new(link, opts)
end

return protocols
