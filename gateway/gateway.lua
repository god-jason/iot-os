local log = iot.logger("gateway")

--- 网关管理
-- @module gateway
local gateway = {}

local configs = require("configs")
local database = require("database")
local protocols = require("protocols")
local devices = require("devices")
local links = require("links")
local boot = require("boot")

-- 打开链接
local function open_link(lnk)
    local ret, info = lnk:open()
    if not ret then
        return false, info
    end

    if not lnk.protocol or #lnk.protocol == 0 then
        return false, "no protocol"
    end

    -- 创建协议
    local ret, instanse = protocols.create(lnk, lnk.protocol, lnk.protocol_options or {})
    if not ret then
        return ret, instanse
    end

    -- 打开协议
    ret, info = instanse:open()
    if not ret then
        return false, info
    end

    lnk.protocol_instanse = instanse

    return true
end

-- 打开网关
function gateway.open()

    -- 加载连接
    local lnks = links.load()
    for i, lnk in ipairs(lnks) do
        local ret, info = open_link(lnk)
        if not ret then
            log.error("open link failed", info)
        end
    end

    -- 加载规则引擎

    return true
end

-- 关闭网关
function gateway.close()

    -- 关闭连接
    local ret, info = links.close()
    if not ret then
        return ret, info
    end

    return true
end

-- gateway.deps = {"links"}

-- 注册
boot.register("gateway", gateway)

return gateway
