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

local protocol_instanses = {}

-- 打开链接
local function open_link_protocol(lnk)
    if not lnk.protocol or #lnk.protocol == 0 then
        return false, "no protocol"
    end

    -- 创建协议
    local ret, instanse = protocols.create(lnk, lnk.protocol, lnk.protocol_options or {})
    if not ret then
        return ret, instanse
    end

    -- 打开协议
    local ret, info = instanse:open()
    if not ret then
        return false, info
    end

    -- 保存
    -- lnk.protocol_instanse = instanse
    protocol_instanses[lnk.id] = instanse

    return true
end

local function close_link_protocol(link_id)
    local instanse = protocol_instanses[link_id]
    if instanse then
        pcall(instanse.close, instanse)
    end
end

-- 打开网关
function gateway.open()

    -- 创建协议
    for i, s in pairs(links) do
        if type(s) == "table" then
            local ret, info = open_link_protocol(s)
            if not ret then
                log.error("open link protocol failed", info)
            end
        end
    end

    -- 加载规则引擎

    return true
end

-- 关闭网关
function gateway.close()

    -- 关闭协议
    for k, instanse in pairs(protocol_instanses) do
        pcall(instanse.close, instanse)
    end
    protocol_instanses = {}

    return true
end

gateway.deps = {"links"}

-- 注册
boot.register("gateway", gateway)

return gateway
