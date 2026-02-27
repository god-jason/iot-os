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
function planner.register(job, fn)
    fns[job] = fn
end

--- 生成计划
---@param job string 计算名称
---@param data any 参数
---@return boolean 成功与否
---@return string|table 任务 VM格式
function planner.plan(job, data)
    local fn = fns[job]
    if not fn then
        return false, "找不到计划器"
    end

    local ret, res, tasks = pcall(fn, data or {})
    if not ret then
        return ret, res
    end

    return res, {
        job = job,
        tasks = tasks,
        created = os.time()
    }
end

return planner
