--- lvgl 模块验证（低优先级，当前可能未注册）
local T = dofile("app/tests/common.lua")

return function()
    local ok, lv = pcall(require, "lvgl")
    if not ok then
        T.skip("lvgl.require", "not registered: " .. tostring(lv))
        return
    end

    local ver = lv.version_get and lv.version_get()
    if ver then
        T.pass("lvgl.version", string.format("%d.%d.%d", ver.major or 0, ver.minor or 0, ver.patch or 0))
    else
        T.pass("lvgl.loaded")
    end

    -- 无 display driver 时无法真正显示，仅验证 API 可调用
    if lv.tick_inc then
        lv.tick_inc(5)
        T.pass("lvgl.tick_inc")
    end
    if lv.task_handler then
        lv.task_handler()
        T.pass("lvgl.task_handler")
    end
end
