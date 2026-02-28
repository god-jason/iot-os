local log = iot.logger("links")

local links = {}

_G.links = links

local configs = require("configs")

local types = {}

-- 注册链接
function links.register(name, clazz)
    types[name] = clazz
end

-- 创建链接
function links.create(opts)
    log.info("create", iot.json_encode(opts))

    local clazz = types[opts.type]
    if not clazz then
        return false, "unkown type" .. opts.type
    end

    -- 注册到全局
    local link = clazz:new(opts)
    if opts.id and #opts.id > 0 then
        links[opts.id] = link
    end
    if opts.name and #opts.name > 0 then
        links[opts.name] = link
    end

    return true, link
end

-- 加载链接
function links.load()
    log.info("load")
    local lnks = {}

    local cms = configs.load_default("links", {})
    for k, v in ipairs(cms) do
        local ret, info = links.create(v)
        if not ret then
            log.error(info)
        else
            table.insert(lnks, info)
        end
    end
    for k, v in pairs(cms) do
        v.name = v.name or k -- key作为设备名
        local ret, info = links.create(v)
        if not ret then
            log.error(info)
        else
            table.insert(lnks, info)
        end
    end

    return lnks
end

return links
