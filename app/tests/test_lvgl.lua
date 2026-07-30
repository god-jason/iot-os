--- lvgl 模块验证 — 通过 SDL 窗口显示 LVGL 界面
-- require("lvgl") 时自动创建 SDL 窗口，通过 iot.setInterval 周期调用 lvgl.task_handler() 驱动
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

    -- 获取活动屏幕
    local scr = lv.scr_act()
    if not scr then
        T.fail("lvgl.scr_act", "no active screen")
        return
    end
    T.pass("lvgl.scr_act")

    -- 标题
    local title = lv.label.create(scr)
    title:set_text("IoT-OS LVGL Widgets Test")
    title:align(lv.ALIGN_TOP_MID, 0, 10)
    T.pass("lvgl.label", "title")

    -- 底部状态（先创建，供回调使用）
    local status = lv.label.create(scr)
    status:set_text("Click buttons or drag sliders...")
    status:align(lv.ALIGN_BOTTOM_MID, 0, -10)
    T.pass("lvgl.label", "status")

    -- ---- btn ----
    local btn = lv.btn.create(scr)
    btn:set_size(100, 40)
    btn:align(lv.ALIGN_TOP_LEFT, 20, 50)
    local btn_lbl = lv.label.create(btn)
    btn_lbl:set_text("Button")
    btn_lbl:center()
    local btn_click_count = 0
    btn:add_event_cb(function(e, code, target, cur_target)
        if code == lv.EVENT_CLICKED then
            btn_click_count = btn_click_count + 1
            btn_lbl:set_text("Btn " .. btn_click_count)
            status:set_text("Btn clicked: " .. btn_click_count)
        end
    end, lv.EVENT_CLICKED)
    T.pass("lvgl.btn")

    -- ---- switch ----
    local sw = lv.switch.create(scr)
    sw:align(lv.ALIGN_TOP_LEFT, 20, 110)
    local sw_lbl = lv.label.create(scr)
    sw_lbl:set_text("Switch: OFF")
    sw_lbl:align_to(sw, lv.ALIGN_OUT_RIGHT_MID, 10, 0)
    sw:add_event_cb(function(e, code, target_ptr, cur_target_ptr)
        if code == lv.EVENT_VALUE_CHANGED then
            local on = sw:has_state(lv.STATE_CHECKED)
            sw_lbl:set_text(on and "Switch: ON" or "Switch: OFF")
            status:set_text("Switch: " .. (on and "ON" or "OFF"))
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.switch")

    -- ---- slider ----
    local slider = lv.slider.create(scr)
    slider:set_width(160)
    slider:align(lv.ALIGN_TOP_LEFT, 20, 170)
    local sl_lbl = lv.label.create(scr)
    sl_lbl:set_text("Slider: 0")
    sl_lbl:align_to(slider, lv.ALIGN_OUT_RIGHT_MID, 10, 0)
    slider:add_event_cb(function(e, code, target_ptr, cur_target_ptr)
        if code == lv.EVENT_VALUE_CHANGED then
            local v = slider:get_value()
            sl_lbl:set_text("Slider: " .. v)
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.slider")

    -- ---- 第二列 ----
    local btn2 = lv.btn.create(scr)
    btn2:set_size(100, 40)
    btn2:align(lv.ALIGN_TOP_LEFT, 260, 50)
    local btn2_lbl = lv.label.create(btn2)
    btn2_lbl:set_text("Toggle")
    btn2_lbl:center()
    local btn2_on = false
    btn2:add_event_cb(function(e, code, target_ptr, cur_target_ptr)
        if code == lv.EVENT_CLICKED then
            btn2_on = not btn2_on
            if btn2_on then
                btn2_lbl:set_text("Toggled")
                btn2:add_state(lv.STATE_CHECKED)
            else
                btn2_lbl:set_text("Toggle")
                btn2:clear_state(lv.STATE_CHECKED)
            end
            status:set_text("Toggle: " .. (btn2_on and "ON" or "OFF"))
        end
    end, lv.EVENT_CLICKED)

    local sw2 = lv.switch.create(scr)
    sw2:align(lv.ALIGN_TOP_LEFT, 260, 110)
    local sw2_lbl = lv.label.create(scr)
    sw2_lbl:set_text("SW2: OFF")
    sw2_lbl:align_to(sw2, lv.ALIGN_OUT_RIGHT_MID, 10, 0)
    sw2:add_event_cb(function(e, code, target_ptr, cur_target_ptr)
        if code == lv.EVENT_VALUE_CHANGED then
            local on = sw2:has_state(lv.STATE_CHECKED)
            sw2_lbl:set_text(on and "SW2: ON" or "SW2: OFF")
        end
    end, lv.EVENT_VALUE_CHANGED)

    local slider2 = lv.slider.create(scr)
    slider2:set_width(160)
    slider2:align(lv.ALIGN_TOP_LEFT, 260, 170)
    local sl2_lbl = lv.label.create(scr)
    sl2_lbl:set_text("SL2: 0")
    sl2_lbl:align_to(slider2, lv.ALIGN_OUT_RIGHT_MID, 10, 0)
    slider2:add_event_cb(function(e, code, target_ptr, cur_target_ptr)
        if code == lv.EVENT_VALUE_CHANGED then
            local v = slider2:get_value()
            sl2_lbl:set_text("SL2: " .. v)
        end
    end, lv.EVENT_VALUE_CHANGED)

    -- 立即刷新
    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    -- 周期调用 task_handler 驱动 SDL 事件 + LVGL 渲染
    local timer_id = iot.setInterval(function()
        lv.task_handler()
    end, 10)
    T.pass("lvgl.task_handler", "interval started")

    -- iot.setTimeout(function()
    --     iot.clearInterval(timer_id)
    --     T.pass("lvgl.timeout", "30s elapsed")
    --     os.exit(0)
    -- end, 5000)
end
