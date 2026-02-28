local utils = require("utils")
local log = iot.logger("watcher")

local Watcher = {}
Watcher.__index = Watcher

function Watcher:new()
    return setmetatable({
        inc = utils.increment(),
        watchers = {}
    }, Watcher)
end

function Watcher:watch(cb)
    local id = self.inc()
    self.watchers[id] = cb
    return id
end

function Watcher:unwatch(id)
    self.watchers[id] = nil
end

function Watcher:clear()
    self.watchers = {}
end

function Watcher:execute(...)
    for i, cb in pairs(self.watchers) do
        if cb then
            local ok, err = pcall(cb, ...)
            if not ok then
                log.error("callback error:", err)
            end
        end
    end
end

return Watcher
