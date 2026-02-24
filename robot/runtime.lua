local tag = "runtime"

local commands = {}

-- 自增ID
local increament = 0
local function increament_id()
    increament = increament + 1
    return increament
end

-- 定义实例
local Runtime = {}
Runtime.__index = Runtime

-- 注册指令
function Runtime.register(cmd, handler)
    commands[cmd] = handler
end

-- 创建实例
function Runtime:new(opts)
    opts = opts or {}
    return setmetatable({
        id = increament_id(),
        job = opts.job or "-",
        tasks = opts.tasks or {},
        on_finish = opts.on_finish,
        on_error = opts.on_error,
        current = 1
    }, Runtime)
end

-- 复制实例
function Runtime:clone()
    return setmetatable({
        id = increament_id(),
        job = self.job,
        tasks = self.tasks,
        on_finish = self.on_finish,
        on_error = self.on_error,
        current = 1
    }, Runtime)
end

-- 暂停
function Runtime:pause()
    self.paused = true
    iot.emit("runtime_" .. self.id .. "_break")
end

-- 停止
function Runtime:stop()
    self.stoped = true
    iot.emit("runtime_" .. self.id .. "_break")
end

-- 执行
function Runtime:execute(cursor)
    cursor = cursor or 1 -- 默认从头开始
    log.info(tag, "execute", cursor, json.encode(self.tasks))

    -- 从起始任务执行
    for i = cursor, #self.tasks, 1 do
        local task = self.tasks[i]
        log.info(tag, "task", i, iot.json_encode(task))
        self.current = i

        local fn = commands[task.type]
        if type(fn) == "function" then
            -- fn(task)
            local ret, info = pcall(fn, self, task)
            if not ret then
                log.error(tag, info)
                -- 上报错误
                if not self.on_error then
                    self.on_error(info)
                end

                -- TODO 设备状态
                return
            end
        else
            log.info(tag, "unkown command", task.type)
        end

        -- 任务等待
        if task.wait_timeout ~= nil and task.wait_timeout > 0 then
            local ret, info = iot.wait("runtime_" .. self.id .. "_break", task.wait_timeout)
            if not ret then
                -- 超时，任务正常执行
            else
                -- 被中断
            end
        end

        -- 任务暂停
        if self.paused then
            -- log.info(tag, "pause")
            return
        end

        if self.stoped then
            -- log.info(tag, "stop")
            self.job = "none"
            return
        end
    end

    -- 任务结束
    self.job = "none"
    self.stoped = true

    log.info(tag, "execute finished")

    if self.on_finish ~= nil then
        self.on_finish()
    end
end

-- 恢复
function Runtime:resume()
    self.paused = false
    iot.start(Runtime.execute, self, self.current)
end

-- 启动
function Runtime:start()
    iot.start(Runtime.execute, self)
end

return Runtime
