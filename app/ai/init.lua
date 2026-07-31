--[[
    app/ai/init.lua
    AI 智能体 Lua 封装层

    在 C 模块 (model/llm/stt/tts) 之上提供更高层的封装:
      - Agent 主循环
      - 多传感器融合
      - 特征工程
      - 一键式 AI 任务

    用法:
      local ai = require("ai")
      local agent = ai.new_agent({...})
      agent:start()
--]]

local ai = {}

---@class AiAgent
local Agent = {}
Agent.__index = Agent

-- ============================================================
-- Agent 智能体
-- ============================================================

--- 创建 AI Agent
---@param config table
---@field config.name       string    Agent 名称
---@field config.rules      table[]   规则列表
---@field config.interval_ms number  采样间隔 (ms)
---@field config.model_path string   端侧模型路径
---@field config.llm_cfg    table     LLM 配置
---@field config.tts_cfg    table     TTS 配置
---@field config.stt_cfg    table     STT 配置
function ai.new_agent(config)
    local a = setmetatable({
        name         = config.name or "agent",
        rules        = config.rules or {},
        interval_ms  = config.interval_ms or 1000,
        model_path   = config.model_path,
        llm_cfg      = config.llm_cfg,
        tts_cfg      = config.tts_cfg,
        stt_cfg      = config.stt_cfg,
        model        = nil,      -- 端侧模型
        sensors      = {},       -- 传感器表
        actions      = {},       -- 动作表
        timers       = {},       -- 定时器
        memory       = {},       -- 短期记忆
        running      = false,
    }, Agent)

    -- 加载端侧模型
    if a.model_path and pcall(require, "model") then
        local model = require("model")
        a.model = model.load(a.model_path)
        if a.model then
            print("[AI] Model loaded:", a.model_path)
        end
    end

    -- 初始化 LLM
    if a.llm_cfg and pcall(require, "llm") then
        local llm = require("llm")
        llm.create(a.llm_cfg)
        print("[AI] LLM configured:", a.llm_cfg.model or "default")
    end

    -- 初始化 TTS
    if a.tts_cfg and pcall(require, "tts") then
        local tts = require("tts")
        tts.create(a.tts_cfg)
        print("[AI] TTS configured")
    end

    return a
end

--- 注册传感器
---@param name      string   传感器名称
---@param driver    table    驱动对象 (需有 read 方法)
---@param window_size number 滑动窗口大小
function Agent:register_sensor(name, driver, window_size)
    self.sensors[name] = {
        driver      = driver,
        window      = {},
        window_size = window_size or 128,
    }
end

--- 注册动作
---@param name   string 动作名称
---@param fn     function 动作函数
function Agent:register_action(name, fn)
    self.actions[name] = fn
end

--- 规则引擎推理
---@param sensor_name string
---@param value       number
---@return string|nil action_name
---@return number|nil trigger_value
function Agent:rule_infer(sensor_name, value)
    for _, rule in ipairs(self.rules) do
        if rule.sensor == sensor_name then
            local triggered = false
            local cond = rule.condition or "gt"

            if cond == "gt" and value > rule.threshold then triggered = true
            elseif cond == "lt" and value < rule.threshold then triggered = true
            elseif cond == "eq" and value == rule.threshold then triggered = true
            elseif cond == "gte" and value >= rule.threshold then triggered = true
            elseif cond == "lte" and value <= rule.threshold then triggered = true
            elseif cond == "range" then
                if value >= rule.min and value <= rule.max then triggered = true end
            end

            if triggered then
                return rule.action, value
            end
        end
    end
    return nil
end

--- 端侧模型推理
---@param sensor_name string
---@param window      number[]
---@return table|nil
function Agent:model_infer(sensor_name, window)
    if not self.model or not pcall(require, "model") then return nil end

    local model = require("model")

    -- 提取特征
    local features = Agent.extract_features(window)

    -- 转为字节串
    local buf = ""
    for _, v in ipairs(features) do
        -- 简单打包: float -> 4 bytes (实际需要 pack 模块)
        -- 这里用简化方式: 将数据转为字符串
        buf = buf .. string.char(math.floor(v * 100) % 256)
    end

    local ok, output = pcall(model.predict, self.model, buf)
    if ok and output then
        return { sensor = sensor_name, output = output, type = "model" }
    end
    return nil
end

--- 特征提取
---@param window number[]
---@return table
function Agent.extract_features(window)
    local n = #window
    if n == 0 then return {} end

    local sum, sum_sq, max_v, min_v = 0, 0, -math.huge, math.huge
    for _, v in ipairs(window) do
        sum = sum + v
        sum_sq = sum_sq + v * v
        if v > max_v then max_v = v end
        if v < min_v then min_v = v end
    end

    local mean = sum / n
    local variance = (sum_sq / n) - (mean * mean)
    local std = math.sqrt(math.abs(variance))

    return {
        mean,                               -- 均值
        std,                                -- 标准差
        math.sqrt(sum_sq / n),              -- RMS
        max_v,                              -- 峰值
        max_v - min_v,                      -- 峰峰值
        (max_v - min_v) / (2 * std + 1e-10), -- 波峰因子
    }
end

--- 云端 LLM 推理
---@param prompt string
---@param callback function|nil
---@return string|nil
function Agent:llm_infer(prompt, callback)
    if not pcall(require, "llm") then return nil end
    local llm = require("llm")

    local ok, result, tokens = pcall(llm.ask, prompt)
    if ok and result then
        -- 存入记忆
        table.insert(self.memory, {
            role = "user", content = prompt, time = os.time()
        })
        table.insert(self.memory, {
            role = "assistant", content = result, time = os.time()
        })
        -- 限制记忆条数
        while #self.memory > 20 do
            table.remove(self.memory, 1)
        end
        return result
    end
    return nil
end

--- 语音合成并播放
---@param text string
function Agent:speak(text)
    if not pcall(require, "tts") then
        print("[AI]", text)
        return
    end
    local tts = require("tts")
    tts.speak(text)
end

--- 主循环: 感知 -> 思考 -> 行动
function Agent:start()
    if self.running then return end
    self.running = true

    print("[AI] Agent '" .. self.name .. "' started")

    -- 为每个传感器创建定时采集任务
    for name, sensor in pairs(self.sensors) do
        self.timers[name] = iotos.timer(self.interval_ms, function()
            if not self.running then return end

            -- 1. 感知: 读取传感器数据
            local ok, value = pcall(sensor.driver.read, sensor.driver)
            if not ok or not value then return end

            table.insert(sensor.window, value)
            while #sensor.window > sensor.window_size do
                table.remove(sensor.window, 1)
            end

            -- 2. 思考: 规则引擎 (快速路径)
            local action, trigger_val = self:rule_infer(name, value)
            if action then
                self:execute(name, action, trigger_val)
                return
            end

            -- 2b. 思考: 端侧模型 (窗口满时才推理)
            if self.model and #sensor.window >= sensor.window_size then
                local result = self:model_infer(name, sensor.window)
                if result then
                    self:on_model_result(name, result)
                end
            end
        end)
        self.timers[name]:start()
    end
end

--- 停止 Agent
function Agent:stop()
    self.running = false
    for _, timer in pairs(self.timers) do
        timer:stop()
    end
    self.timers = {}
    print("[AI] Agent '" .. self.name .. "' stopped")
end

--- 执行动作
---@param sensor_name string
---@param action_name string
---@param value       any
function Agent:execute(sensor_name, action_name, value)
    print(string.format("[AI:%s] SENSOR=%s ACTION=%s VALUE=%s",
        self.name, sensor_name, action_name, tostring(value)))

    -- 执行注册的动作
    local fn = self.actions[action_name]
    if fn then
        local ok, err = pcall(fn, sensor_name, value)
        if not ok then
            print("[AI] Action failed:", err)
        end
    end

    -- 发布事件
    iotos.emit("ai:action", {
        agent  = self.name,
        sensor = sensor_name,
        action = action_name,
        value  = value,
    })
end

--- 端侧模型推理结果处理
function Agent:on_model_result(sensor_name, result)
    print(string.format("[AI:%s] Model result for %s", self.name, sensor_name))
    -- 子类可覆写
end

--- 获取记忆上下文 (用于 LLM 对话)
function Agent:get_memory_context()
    local ctx = ""
    for _, m in ipairs(self.memory) do
        ctx = ctx .. string.format("[%s]: %s\n", m.role, m.content)
    end
    return ctx
end

-- ============================================================
-- 快捷工具函数
-- ============================================================

--- 文本转语音并播放
---@param text  string
---@param cfg   table|nil  TTS 配置 (首次调用必需)
function ai.say(text, cfg)
    local tts = require("tts")
    if cfg then tts.create(cfg) end
    return tts.speak(text)
end

--- 语音识别 (文件)
---@param file_path string
---@param cfg       table|nil  STT 配置
---@return string|nil text
---@return number|nil confidence
function ai.listen_file(file_path, cfg)
    local stt = require("stt")
    if cfg then stt.create(cfg) end
    return stt.recognize_file(file_path)
end

--- 询问大语言模型
---@param question string
---@param cfg       table|nil  LLM 配置
---@return string|nil answer
function ai.ask(question, cfg)
    local llm = require("llm")
    if cfg then llm.create(cfg) end
    return llm.ask(question)
end

--- 创建 LLM 聊天会话
---@param cfg table LLM 配置
---@return table session
function ai.chat_session(cfg)
    local llm = require("llm")
    llm.create(cfg)

    local history = {}
    local session = {}

    function session:send(msg)
        table.insert(history, { role = "user", content = msg })

        -- 构建 messages JSON
        local msgs = { { role = "system", content = cfg.system_prompt or "你是物联网AI助手" } }
        for _, m in ipairs(history) do
            table.insert(msgs, m)
        end

        -- 简化的 JSON 构建
        local msgs_json = "["
        for i, m in ipairs(msgs) do
            if i > 1 then msgs_json = msgs_json .. "," end
            msgs_json = msgs_json .. string.format(
                '{"role":"%s","content":"%s"}', m.role, m.content)
        end
        msgs_json = msgs_json .. "]"

        local result = llm.chat(msgs_json)
        if result and result.content then
            table.insert(history, { role = "assistant", content = result.content })
            return result.content
        end
        return nil, "chat error"
    end

    return session
end

--- 创建 AI Agent (便捷工厂)
---@param name  string
---@param rules table
---@param opts  table
---@return AiAgent
function ai.create_agent(name, rules, opts)
    opts = opts or {}
    opts.name = name
    opts.rules = rules
    return ai.new_agent(opts)
end

return ai
