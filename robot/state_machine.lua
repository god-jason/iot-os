local tag = "fsm"

local StateMachine = {}
StateMachine.__index = StateMachine

-- 创建状态机
function StateMachine:new(opts)
    opts = opts or {}
    return setmetatable({
        name = opts.name or "FSM",
        tick = opts.tick or 1000,
        state = nil,
        states = {}
    }, StateMachine)
end

-- 注册状态
-- @param string name 名称
-- @param table state 状态 (需要有三个回调函数 enter, tick, leave)
function StateMachine:register(name, state)
    self.states[name] = state
end

-- 克隆状态机
function StateMachine:clone()
    return setmetatable({
        name = self.name,
        tick = self.tick,
        state = nil,
        states = self.states
    }, StateMachine)
end

-- 启动状态机
function StateMachine:start(name)
    -- 修改状态
    local ret, info = self:set(name)
    if not ret then
        return ret, info
    end

    -- 启动进程
    iot.start(function()

        self.running = true
        while self.running do
            iot.sleep(self.tick)

            -- 执行状态任务
            if self.state then
                if self.state.tick then
                    self.state.tick(self)
                end
            else
                log.error(tag, self.name, "未设置状态")
            end
        end

        -- 清理状态机

    end)

    return true
end

-- 修改状态
function StateMachine:set(name)

    -- 上一个状态离开
    local state = self.state
    if state then
        self.state = nil
        if state.leave then
            state.leave(self)
        end
    end

    -- 加载新状态
    local state = self.states[name]
    if not state then
        return false, "未知状态" .. name
    end

    -- 新状态进入
    if state.enter then
        state.enter(self)
    end

    self.state = state
    self.state_name = name

    return true
end

-- 停止状态机
function StateMachine:stop()
    self.running = false

    -- 状态离开
    local state = self.state
    if state then
        self.state = nil
        if state.leave then
            state.leave(self)
        end
    end

end

return StateMachine
