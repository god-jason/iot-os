local robot = {}
local log = iot.logger("robot")

local configs = require("configs")

local Fan = require("fan")
local Led = require("led")
local Relay = require("Relay")
local Servo = require("servo")
local Stepper = require("stepper")

local VM = require("vm")
local FSM = require("fsm")
local cron = require("cron")
local planner = require("planner")

-- 注册组件到全局
local components = {
    -- fan = Fan:new({})
}
_G.components = components

-- 创建组件
local function create_component(cmp)
    log.info("create component", iot.json_encode(cmp))
    if cmp.type == "fan" then
        components[cmp.name] = Fan:new(cmp)
    elseif cmp.type == "led" then
        components[cmp.name] = Led:new(cmp)
    elseif cmp.type == "relay" then
        components[cmp.name] = Relay:new(cmp)
    elseif cmp.type == "servo" then
        components[cmp.name] = Servo:new(cmp)
    elseif cmp.type == "stepper" then
        components[cmp.name] = Stepper:new(cmp)
    else
        log.error("unkown component type", cmp.type)
    end
end

-- 创建指令
local function create_instruction(name, script)
    if type(script) == "function" then
        VM.register(name, script)
        return
    end

    log.info("create instruction", name, script)

    -- 封装为闭包
    script = "return function(context, task)\n" .. script .. "\nend"

    -- local fn = load(script, "ins_" .. name, "bt", _G)
    local ret, fn = pcall(load, script, "instruction_" .. name, "bt", _G)
    if not ret then
        log.info("compile instruction error", fn)
        return
    end

    -- 返回闭包
    ret, fn = pcall(fn)
    if not ret then
        log.info("closure instruction error", fn)
        return
    end

    -- 注册到虚拟机上
    VM.register(name, fn)
end

-- 创建计划器
local function create_planner(name, script)
    if type(script) == "function" then
        planner.register(name, script)
        return
    end

    log.info("create planner", name, script)

    -- 封装为闭包
    script = "return function(data)\n" .. script .. "\nend"

    -- local fn = load(script, "planner_" .. name, "bt", _G)
    local ret, fn = pcall(load, script, "planner_" .. name, "bt", _G)
    if not ret then
        log.info("compile planner error", fn)
        return
    end

    -- 返回闭包
    ret, fn = pcall(fn)
    if not ret then
        log.info("closure planner error", fn)
        return
    end

    -- 注册到计划器上
    planner.register(name, fn)
end

function robot.plan(name, data)
    local ret, plan = planner.plan(name, data)
    if not ret then
        return ret, plan
    end

    -- 创建一个虚拟机并执行
    robot.vm = VM:new(plan)
    robot.vm:start()

    return true
end

function robot.init()
    log.info("init")

    -- 创建设备组件
    log.info("load components")
    local cms = configs.load_default("components", {})
    for k, v in ipairs(cms) do
        create_component(v)
    end
    for k, v in pairs(cms) do
        v.name = v.name or k -- key作为设备名
        create_component(v)
    end

    -- 加载自定义指令集
    log.info("load instructions")
    local ins = configs.load_default("instructions", {})
    for k, v in ipairs(ins) do
        create_instruction(v.name, v.script)
    end
    for k, v in pairs(ins) do
        create_instruction(k, v)
    end

    -- 加载自定义计划器
    log.info("load planners")
    local pls = configs.load_default("planners", {})
    for k, v in ipairs(pls) do
        create_planner(v.name, v.script)
    end
    for k, v in pairs(pls) do
        create_planner(k, v)
    end

    -- 启动计划任务
    log.info("load schedulars")

    -- 连接服务器

end

return robot
