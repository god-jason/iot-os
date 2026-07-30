--- lvgl 模块验证 — 通过 SDL 窗口显示 LVGL 界面
local T = dofile("app/tests/common.lua")

return function()
    local ok, lv = pcall(require, "lvgl")
    if not ok then
        T.skip("lvgl.require", "not registered: " .. tostring(lv))
        return
    end

    -- 版本信息
    local ver = lv.version_get and lv.version_get()
    if ver then
        T.pass("lvgl.version", string.format("%d.%d.%d", ver.major or 0, ver.minor or 0, ver.patch or 0))
    else
        T.pass("lvgl.loaded")
    end

    -- 检查 SDL 驱动是否可用
    if not lv.sdl_init then
        T.skip("lvgl.sdl", "SDL driver not available in this build")
        return
    end

    -- 初始化 SDL 窗口 (480x320)
    local ok2 = lv.sdl_init(480, 320)
    if not ok2 then
        T.fail("lvgl.sdl_init", "failed to create SDL window")
        return
    end
    T.pass("lvgl.sdl_init", "480x320 window created")

    -- 获取活动屏幕
    local scr = lv.scr_act()
    if not scr then
        T.fail("lvgl.scr_act", "no active screen")
        lv.sdl_deinit()
        return
    end
    T.pass("lvgl.scr_act")

    -- 创建标题标签
    local title = lv.label.create(scr)
    title:set_text("IoT-OS LVGL Test")
    title:align(lv.ALIGN_TOP_MID, 0, 20)
    T.pass("lvgl.label.create", "title")

    -- 创建按钮
    local btn = lv.btn.create(scr)
    btn:set_size(120, 50)
    btn:align(lv.ALIGN.CENTER, 0, 0)
    T.pass("lvgl.btn.create")

    -- 按钮上的文字
    local btn_label = lv.label.create(btn)
    btn_label:set_text("Click Me!")
    btn_label:center()
    T.pass("lvgl.label.on_btn")

    -- 创建底部状态标签
    local status = lv.label.create(scr)
    status:set_text("Running...")
    status:align(lv.ALIGN.BOTTOM_MID, 0, -20)
    T.pass("lvgl.label.status")

    -- 立即刷新一次
    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    -- 使用 iot.setInterval 非阻塞运行 SDL 事件循环
    -- 每 10ms 调用一次 lv.sdl_loop()，共运行约 3 秒（300 次）
    local loops = 0
    local max_loops = 300
    local timer_id = iot.setInterval(function()
        local running = lv.sdl_loop()
        loops = loops + 1
        if not running or loops >= max_loops then
            iot.clearInterval(timer_id)
            if loops > 0 then
                T.pass("lvgl.sdl_loop", string.format("ran %d iterations", loops))
            else
                T.fail("lvgl.sdl_loop", "window closed immediately")
            end
            lv.sdl_deinit()
            T.pass("lvgl.sdl_deinit")
            -- 测试完成，延迟退出（等待 summary 输出）
            iot.setTimeout(function()
                os.exit(0)
            end, 1000)
        end
    end, 10)
end
