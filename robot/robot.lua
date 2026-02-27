local robot = {}
local log = iot.logger("robot")

local configs = require("configs")

local vm = require("vm")
local fsm = require("fsm")
local cron = require("cron")
local planner = require("planner")

local components = require("components")
local program = require("program")

robot.fsm = fsm:new()

function robot.plan(name, data)
    local ret, plan = planner.plan(name, data)
    if not ret then
        return ret, plan
    end

    -- 创建一个虚拟机并执行
    robot.vm = vm:new(plan)
    robot.vm:start()

    return true
end

function robot.init()
    log.info("init")

    -- 创建设备组件
    local ret, info = components.load()
    if not ret then
        log.error(info)
        --应该启动失败
    end

    -- 加载自定义编程
    ret, info = program.load()
    if not ret then
        log.error(info)
    end

    -- 启动计划任务

    -- 连接服务器

    -- 启动状态机
    robot.fsm:start()
end

return robot
