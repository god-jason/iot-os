local log = iot.logger("scene")

local devices = require("devices")
local cron = require("cron")

local Scene = {}
Scene.__index = Scene

function Scene:new(opts)
    local scene = setmetatable({
        name = opts.name or "-",
        ranges = opts.ranges or {}, -- 时间范围
        triggers = opts.triggers or {}, -- 触发器
        conditions = opts.conditions or {}, -- 条件
        actions = opts.actions or {} -- 响应
    }, Scene)
    return scene
end

function Scene:open()

    -- 编译时间
    for k, range in ipairs(self.ranges) do
        if not range.start or not range.finish then
            return false, "time range start and finish must be required"
        end

        local h, m = range.start:match("(%d+):(%d+)")
        if h == nil or m == nil then
            return false, "time range start must be HH:mm, example 06:00"
        end
        range._start = tonumber(h) * 60 + tonumber(m)

        h, m = range.finish:match("(%d+):(%d+)")
        if h == nil or m == nil then
            return false, "time range finish must be HH:mm, example 12:00"
        end
        range._finish = tonumber(h) * 60 + tonumber(m)
    end

    -- 编译条件
    for k, cond in ipairs(self.conditions) do
        if cond.type == "script" then
            local script = "return " .. cond.script
            local ret, info = load(script, "cond", "t", _G)
            if not ret then
                return false, info
            end
            cond._script = ret
        end
    end

    -- 编译响应
    for k, action in ipairs(self.actions) do
        if action.type == "script" then
            local script = "return function()\n" .. action.script .. "\nend"
            local ret, info = load(script, "action", "t", _G)
            if not ret then
                return false, info
            end
            -- 返回闭包
            ret, info = pcall(ret)
            if not ret then
                return ret, info
            end
            action._script = info
        end
    end

    -- 注册触发器
    local this = self
    for i, trigger in ipairs(self.triggers) do
        if trigger.type == "time" then
            -- 每日启动
            local ret, info = cron.clock(trigger.time, function()

                local tm = os.date("*t")
                if trigger.weekdays and #trigger.weekdays > 0 then
                    local has = false
                    for i, v in ipairs(trigger.weekdays) do
                        if v == tm.wday then
                            has = true
                            break
                        end
                    end
                    -- 星期不匹配
                    if not has then
                        return
                    end
                end

                this:execute()
            end)

            if not ret then
                log.error(info)
                return false, info
            else
                trigger.cron = info
            end

        elseif trigger.type == "device" then
            -- 设备变化

            local device = devices[trigger.device]
            if device then
                device:watch(function()
                    this:execute()
                end)
            else
                log.error("cannot find device", trigger.device)
                return false, "cannot find device:" .. trigger.device
            end
        end
    end

    return true
end

function Scene:close()
    -- 从计划任务注销
    for i, trigger in ipairs(self.triggers) do
        if trigger.type == "time" then
            if trigger.cron then
                cron.stop(trigger.cron)
            end
        end
    end
end

function Scene:execute()

    -- 检查时间范围
    if self.ranges and #self.ranges > 0 then
        local tm = os.date("*t")
        local hm = tm.hour * 60 + tm.min
        local ok = false
        for _, range in ipairs(self.ranges) do
            if range._start < range._finish and hm >= range._start and hm <= range._finish then
                -- 当天时间段
                ok = true
                break
            elseif range._start > range._finish and (hm >= range._finish or hm <= range._start) then
                -- 跨天时间段
                ok = true
                break
            end
        end
        if not ok then
            return false
        end
    end

    -- 检查条件
    for _, cond in ipairs(self.conditions) do
        if cond.type == "script" then
            local ok, ret = pcall(cond._script)
            if not ok then
                log.error("condition execute error", ret)
                return false
            end
            if not ret then
                return false
            end
        elseif cond.type == "device" then

            local device = devices[cond.device]
            if not device then
                return false
            end

            -- 条件对比
            local value = device:get_value(cond.key)
            if value == nil then
                log.error("device:", cond.device, " value:", cond.key, " is nil")
                return false
            end
            if type(value) ~= "number" then
                log.error("device:", cond.device, " value:", cond.key, " is not a number")
                return false
            end

            if cond.compare == ">" then
                if value <= tonumber(cond.value) then
                    return false
                end
            elseif cond.compare == ">=" then
                if value < tonumber(cond.value) then
                    return false
                end
            elseif cond.compare == "<" then
                if value >= tonumber(cond.value) then
                    return false
                end
            elseif cond.compare == "<=" then
                if value > tonumber(cond.value) then
                    return false
                end
            elseif cond.compare == "==" then
                if value ~= tonumber(cond.value) then
                    return false
                end
            elseif cond.compare == "~=" then
                if value == tonumber(cond.value) then
                    return false
                end
            end
        end
    end

    -- 执行响应
    for _, action in ipairs(self.actions) do
        if action.type == "script" then
            local ok, err = pcall(action._script)
            if not ok then
                log.error("action error:", err)
            end
        elseif action.type == "device" then
            local device = devices[action.device]
            if device then
                device:set(action.key, tonumber(action.value))
            else
                log.error("not found device:", action.device)
            end
        end
    end

end

return Scene
