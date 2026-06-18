--- IOT核心调度框架
--[[
@module iot
@summary 核心逻辑，整合C层消息队列和Lua层调度
@version 2.0
@date    2026.06.08
@usage

-- 定时器
local id = iot.setTimeout(function()
    print("timeout!")
end, 1000)

local id = iot.setInterval(function()
    print("interval!")
end, 1000)

iot.clearTimeout(id)

-- 协程任务
iot.start(function()
    iot.sleep(1000)
    print("task run!")
end)

-- 消息订阅
local cancel = iot.on("NET_READY", function(data)
    print("network ready:", data)
end)

-- 发布消息
iot.emit("MY_EVENT", "hello")

-- 单次订阅
iot.once("INIT_DONE", function()
    print("init done!")
end)

-- 等待消息
local ok, data = iot.wait("NET_READY", 5000)

-- 日志器
local log = iot.logger("TAG")
log.info("message")

-- 安全调用
local ok, result = iot.call(some_function, arg1, arg2)
]]

local iot = {}
_G.iot = iot

local table = _G.table
local unpack = table.unpack
local coroutine = _G.coroutine

--======================================== 内部常量 ========================================--

local MSG_TIMEOUT = 1
local MSG_PUBLISH = 2
local MSG_CALL = 3

local TASK_TIMER_ID_MAX = 0x1FFFFF
local MSG_TIMER_ID_MAX = 0x7FFFFF

--======================================== 内部变量 ========================================--

local timerId = 0
local timerPool = {}       -- {id -> {co, callback, args, loop, tid}}
local rtosTimerIdMap = {} -- {rtos_tid -> id}
local subscribers = {}     -- {topic -> {callback -> true}}
local messageQueue = {}    -- {{topic, ...}}

--======================================== 内部工具 ========================================--

local function coresume(co, ...)
    local ok, err = coroutine.resume(co, ...)
    if not ok then
        log.error("[iot] coroutine error:", debug.traceback(co, err))
    end
end

local function checkTask()
    local co, ismain = coroutine.running()
    if ismain then
        error("attempt to yield from outside a coroutine")
    end
    return co
end

local function genTimerId()
    while true do
        timerId = timerId + 1
        if timerId >= MSG_TIMER_ID_MAX then timerId = TASK_TIMER_ID_MAX end
        if not timerPool[timerId] then return timerId end
    end
end

local function removeTimer(id)
    local entry = timerPool[id]
    if entry then
        if entry.tid then
            rtos.timer_stop(entry.tid)
            rtos.timer_delete(entry.tid)
            rtosTimerIdMap[entry.tid] = nil
        end
        timerPool[id] = nil
    end
end

--======================================== 定时器 ========================================--

--- 创建定时器
local function createTimer(func, timeout, loop, ...)
    if timeout < 1 then timeout = 1 end
    local id = genTimerId()
    local tid = rtos.timer_create(timeout, loop and 1 or 0)
    timerPool[id] = { callback = iot.call, args = {...}, loop = loop, tid = tid }
    rtosTimerIdMap[tid] = id
    return id
end

--- 设置超时定时器
-- @param func 回调函数
-- @param timeout 超时时间(毫秒)
-- @return integer 定时器ID
function iot.setTimeout(func, timeout, ...)
    return createTimer(func, timeout, false, ...)
end

--- 设置循环定时器
-- @param func 回调函数
-- @param interval 间隔时间(毫秒)
-- @return integer 定时器ID
function iot.setInterval(func, interval, ...)
    if interval < 10 then interval = 10 end
    return createTimer(func, interval, true, ...)
end

--- 清空定时器
function iot.clearTimeout(id)
    if id then removeTimer(id) end
end

--- 清空循环定时器
function iot.clearInterval(id)
    if id then removeTimer(id) end
end

--- 判断定时器是否激活
function iot.isActive(id)
    return id and timerPool[id] ~= nil
end

--- 停止所有定时器
-- @param callback 回调函数（可选）
function iot.clearAll(callback)
    if callback then
        for id, entry in pairs(timerPool) do
            if entry.callback == callback then removeTimer(id) end
        end
    else
        for id in pairs(timerPool) do removeTimer(id) end
    end
end

--======================================== 协程 ========================================--

--- 创建协程任务
function iot.start(func, ...)
    local co = coroutine.create(function(...) iot.call(func, ...) end)
    coresume(co, ...)
    return co
end

--- 协程休眠
function iot.sleep(ms)
    ms = ms or 1
    local co = checkTask()
    local id = genTimerId()
    local tid = rtos.timer_create(ms, 0)
    timerPool[id] = { co = co, tid = tid }
    rtosTimerIdMap[tid] = id
    local msg = {coroutine.yield()}
    removeTimer(id)
    if #msg ~= 0 then return unpack(msg) end
end

--======================================== 消息订阅 ========================================--

--- 订阅消息
-- @param topic 消息主题
-- @param callback 回调函数
-- @return function 取消订阅函数
function iot.on(topic, callback)
    if not subscribers[topic] then subscribers[topic] = {} end
    subscribers[topic][callback] = true
    return function() iot.off(topic, callback) end
end

--- 单次订阅
function iot.once(topic, callback)
    local cancel
    local fn = function(...)
        callback(...)
        if cancel then cancel() end
    end
    cancel = iot.on(topic, fn)
    return cancel
end

--- 取消订阅
function iot.off(topic, callback)
    if subscribers[topic] then
        subscribers[topic][callback] = nil
        for _ in pairs(subscribers[topic]) do return end
        subscribers[topic] = nil
    end
end

--- 订阅多个主题
function iot.onAll(topics, callback)
    local cancels = {}
    for _, topic in ipairs(topics) do
        cancels[#cancels + 1] = iot.on(topic, callback)
    end
    return function()
        for _, cancel in ipairs(cancels) do cancel() end
    end
end

--- 等待消息
-- @param topic 消息主题
-- @param ms 超时时间(毫秒，可选)
-- @return boolean 成功/超时
function iot.wait(topic, ms)
    local co = checkTask()
    if not subscribers[topic] then subscribers[topic] = {} end
    subscribers[topic][co] = true

    local id
    if ms then
        id = genTimerId()
        local tid = rtos.timer_create(ms, 0)
        timerPool[id] = { co = co, tid = tid }
        rtosTimerIdMap[tid] = id
    end

    local msg = {coroutine.yield()}

    iot.off(topic, co)
    if id then removeTimer(id) end

    if msg[1] ~= nil then return false end
    return true, unpack(msg, 2)
end

--- 发布消息
function iot.emit(topic, ...)
    messageQueue[#messageQueue + 1] = {topic, ...}
end

--======================================== 消息分发 ========================================--

local function dispatch()
    while #messageQueue > 0 do
        local msg = table.remove(messageQueue, 1)
        local topic = msg[1]
        local subs = subscribers[topic]
        if subs then
            local list = {}
            for cb in pairs(subs) do list[#list + 1] = cb end
            for _, cb in ipairs(list) do
                if type(cb) == "function" then
                    cb(unpack(msg, 2))
                elseif type(cb) == "thread" then
                    coresume(cb, unpack(msg))
                end
            end
        end
    end
end

local function processCmsg(msg)
    if not msg then return end

    if msg.type == MSG_TIMEOUT then
        local id = rtosTimerIdMap[msg.id]
        if id then
            local entry = timerPool[id]
            if entry then
                if entry.co then
                    coresume(entry.co)
                elseif entry.callback then
                    if entry.args[1] then
                        entry.callback(unpack(entry.args))
                    else
                        entry.callback()
                    end
                    if not entry.loop then removeTimer(id) end
                end
            end
        end

    elseif msg.type == MSG_PUBLISH then
        if msg.params then
            iot.emit(msg.data, unpack(msg.params))
        else
            iot.emit(msg.data)
        end

    elseif msg.type == MSG_CALL then
        if msg.data then
            local func = debug.getuservalue(msg.data)
            if type(func) == "function" then
                if msg.params then
                    func(unpack(msg.params))
                else
                    func()
                end
            end
        end
    end
end

--======================================== 主循环 ========================================--

--- 运行主调度循环
function iot.run()
    while true do
        dispatch()
        local msg = rtos.recv(10)
        if msg then
            processCmsg(msg)
            dispatch()
        end
    end
end

--- 安全运行一次调度
function iot.safeRun()
    dispatch()
    local msg = rtos.recv(0)
    if msg then
        processCmsg(msg)
        dispatch()
    end
end

--======================================== 工具函数 ========================================--

--- 添加异常调用栈
function iot.traceback(err)
    if DEBUG then return debug.traceback(err, 2) end
    log.error(debug.traceback())
    return err
end

--- 安全调用
function iot.call(func, ...)
    local res = {xpcall(func, iot.traceback, ...)}
    if not res[1] then
        log.error(res[2])
        iot.emit("error", res[2])
        return false, res[2]
    end
    return unpack(res, 2)
end

--- 扩展安全调用
function iot.xcall(func, ...)
    local res = {xpcall(func, iot.traceback, ...)}
    if not res[1] then
        log.error(res[2])
        iot.emit("error", res[2])
        return false, res[2]
    end
    if res[2] == false then log.warn(res[3]) end
    return unpack(res, 2)
end

--- 创建日志器
function iot.logger(tag)
    return {
        debug = function(...) log.debug(tag, ...) end,
        trace = function(...) log.trace(tag, ...) end,
        info = function(...) log.info(tag, ...) end,
        warn = function(...) log.warn(tag, ...) end,
        error = function(...) log.error(tag, ...) end,
        fatal = function(...) log.error(tag, ...); pm.reboot() end
    }
end

--======================================== 向后兼容 ========================================--

-- 别名
iot.subscribe = iot.on
iot.unsubscribe = iot.off

return iot
