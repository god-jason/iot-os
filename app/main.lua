--- 模块封装验证入口
--- 单测: echo zlib > app/tests/.target  再 xmake run
--- 全测: 删除 app/tests/.target
log.level(log.INFO)
log.info("iot-os", "module verification")

local target
local f = io.open("app/tests/.target", "r")
if f then
    target = f:read("*l")
    f:close()
    if target then
        target = target:match("^%s*(.-)%s*$")
    end
end

-- 避免测试超时退出程序
-- iot.setTimeout(function ()
--     print("timeout!")
--     os.exit(0)
-- end, 15000)

local ok, err = pcall(function()
    if target and target ~= "" then
        dofile("app/tests/run_one.lua")(target)
    else
        dofile("app/tests/run_all.lua")()
    end
end)
if not ok then
    log.error("tests", err)
end
