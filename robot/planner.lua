local planner = {}
local log = iot.logger("planner")

local fns = {}

--[[
计划器参考
function(params)
    if not ok then
        return false, "未准备好"
    end
    return true, {{type:move}}
end
]] --

-- 注册计划器
function planner.register(name, fn)
    fns[name] = fn
end

--- 生成计划
---@param name string 计算名称
---@param ... any 参数列表
---@return boolean 成功与否
---@return string|table 任务列表
function planner.plan(name, ...)
    local fn = fns[name]
    if not fn then
        return false, "找不到计划器"
    end

    local ret, res, tasks = pcall(fn, ...)
    if not ret then
        return ret, res
    end

    return res, tasks
end

return planner
