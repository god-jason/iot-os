-- 主程序入口
PROJECT = "iot-noob"
VERSION = "1.0.0"

-- 引入系统适配层
require("iot")

local log = iot.logger("main")
log.info("last power reson", pm.lastReson())

-- 看门狗守护
if wdt then
    wdt.init(9000)
    sys.timerLoopStart(wdt.feed, 3000)
end

-- 日志等级改为info
log.setLevel(2)

-- 自动识别SIM2
mobile.simid(2, true)

-- 主进程
sys.taskInit(function()
    log.info("task")

    sys.wait(1000) -- 等待USB初始化完成，否则日志丢失

    -- fskv.init() -- KV 数据库

    -- 加载所有程序文件
    require("autoload").walk("/luadb/")

    -- 启动网关
    require("gateway").boot()

    log.info("exit")
end)

sys.run()
