--- lvgl 模块验证 — 大窗口综合组件测试
-- 通过 lvgl.init(w, h) 创建大窗口，覆盖所有 LVGL 控件
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

    -- 初始化大窗口 1280x720
    lv.init(1280, 720)
    T.pass("lvgl.init", "1280x720")

    -- 获取活动屏幕
    local scr = lv.scr_act()
    if not scr then
        T.fail("lvgl.scr_act", "no active screen")
        return
    end
    T.pass("lvgl.scr_act")

    -- 底部状态栏（先创建，供回调使用）
    local status = lv.label.create(scr)
    status:set_text("Ready.")
    status:set_width(1280)
    status:align(lv.ALIGN_BOTTOM_MID, 0, -10)
    T.pass("lvgl.label", "status")

    -- 标题
    local title = lv.label.create(scr)
    title:set_text("IoT-OS LVGL All Widgets Test (1280x720)")
    title:align(lv.ALIGN_TOP_MID, 0, 12)
    T.pass("lvgl.label", "title")

    -- ==================== 第1列（基础控件） x=50 ====================
    local COL1_X = 50
    local col1_y = 70

    -- ---- btn ----
    local btn = lv.btn.create(scr)
    btn:set_size(140, 44)
    btn:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y)
    local btn_lbl = lv.label.create(btn)
    btn_lbl:set_text("Button")
    btn_lbl:center()
    local btn_click_count = 0
    btn:add_event_cb(function(e, code)
        if code == lv.EVENT_CLICKED then
            btn_click_count = btn_click_count + 1
            btn_lbl:set_text("Btn " .. btn_click_count)
            status:set_text("Btn clicked: " .. btn_click_count)
        end
    end, lv.EVENT_CLICKED)
    T.pass("lvgl.btn")

    -- ---- switch ----
    local sw = lv.switch.create(scr)
    sw:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 70)
    local sw_lbl = lv.label.create(scr)
    sw_lbl:set_text("Switch: OFF")
    sw_lbl:align_to(sw, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    sw:add_event_cb(function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local on = sw:has_state(lv.STATE_CHECKED)
            sw_lbl:set_text(on and "Switch: ON" or "Switch: OFF")
            status:set_text("Switch: " .. (on and "ON" or "OFF"))
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.switch")

    -- ---- slider ----
    local slider = lv.slider.create(scr)
    slider:set_width(200)
    slider:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 130)
    local sl_lbl = lv.label.create(scr)
    sl_lbl:set_text("Slider: 0")
    sl_lbl:align_to(slider, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    slider:add_event_cb(function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local v = slider:get_value()
            sl_lbl:set_text("Slider: " .. v)
            status:set_text("Slider: " .. v)
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.slider")

    -- ---- checkbox ----
    local cb = lv.checkbox.create(scr)
    cb:set_text("Check me")
    cb:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 190)
    cb:add_event_cb(function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local on = cb:is_checked()
            status:set_text("Checkbox: " .. (on and "checked" or "unchecked"))
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.checkbox")

    -- ---- dropdown ----
    local dd = lv.dropdown.create(scr)
    dd:set_options("Apple\nBanana\nCherry\nMango")
    dd:set_text("Select fruit")
    dd:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 250)
    local dd_lbl = lv.label.create(scr)
    dd_lbl:set_text("Fruit: Apple")
    dd_lbl:align_to(dd, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    dd:add_event_cb(function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local str = dd:get_selected_str()
            dd_lbl:set_text("Fruit: " .. (str or "?"))
            status:set_text("Dropdown: " .. (str or "?"))
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.dropdown")

    -- ---- roller ----
    local roller = lv.roller.create(scr)
    roller:set_options("Mon\nTue\nWed\nThu\nFri\nSat\nSun")
    roller:set_visible_row_count(4)
    roller:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 320)
    T.pass("lvgl.roller")

    -- ---- spinbox ----
    local spin = lv.spinbox.create(scr)
    spin:set_range(0, 100)
    spin:set_value(50)
    spin:set_width(120)
    spin:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 490)
    T.pass("lvgl.spinbox")

    -- ==================== 第2列（数据控件） x=370 ====================
    local COL2_X = 370
    local col2_y = 70

    -- ---- arc ----
    local arc = lv.arc.create(scr)
    arc:set_size(140, 140)
    arc:set_range(0, 100)
    arc:set_value(40)
    arc:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y)
    local arc_lbl = lv.label.create(scr)
    arc_lbl:set_text("Arc: 40")
    arc_lbl:align_to(arc, lv.ALIGN_OUT_BOTTOM_MID, 0, 8)
    arc:add_event_cb(function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local v = arc:get_value()
            arc_lbl:set_text("Arc: " .. v)
            status:set_text("Arc: " .. v)
        end
    end, lv.EVENT_VALUE_CHANGED)
    T.pass("lvgl.arc")

    -- ---- bar ----
    local bar = lv.bar.create(scr)
    bar:set_range(0, 100)
    bar:set_value(60)
    bar:set_size(200, 24)
    bar:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y + 190)
    T.pass("lvgl.bar")

    -- ---- chart ----
    local chart = lv.chart.create(scr)
    chart:set_size(280, 170)
    chart:set_type(lv.CHART_TYPE_LINE)
    chart:set_range(lv.CHART_AXIS_PRIMARY_Y, 0, 100)
    chart:set_point_count(12)
    chart:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y + 240)
    local series = chart:add_series(lv.COLOR_RED, lv.CHART_AXIS_PRIMARY_Y)
    for i = 0, 11 do
        chart:set_value_by_id(series, i, math.random(0, 100))
    end
    T.pass("lvgl.chart")

    -- ---- table ----
    local tbl = lv.table.create(scr)
    tbl:set_size(280, 140)
    tbl:set_row_cnt(4)
    tbl:set_col_cnt(3)
    tbl:set_cell_value(0, 0, "Name")
    tbl:set_cell_value(0, 1, "Age")
    tbl:set_cell_value(0, 2, "City")
    tbl:set_cell_value(1, 0, "Tom")
    tbl:set_cell_value(1, 1, "25")
    tbl:set_cell_value(1, 2, "Beijing")
    tbl:set_cell_value(2, 0, "Lucy")
    tbl:set_cell_value(2, 1, "30")
    tbl:set_cell_value(2, 2, "Shanghai")
    tbl:set_cell_value(3, 0, "Jack")
    tbl:set_cell_value(3, 1, "28")
    tbl:set_cell_value(3, 2, "Shenzhen")
    tbl:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y + 440)
    T.pass("lvgl.table")

    -- ==================== 第3列（高级控件） x=700 ====================
    local COL3_X = 700
    local col3_y = 70

    -- ---- meter ----
    local meter = lv.meter.create(scr)
    meter:set_size(180, 180)
    meter:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y)
    local scale = meter:add_scale(270, 135)
    meter:set_scale_ticks(scale, 11, 6, 2, 0x000000)
    meter:set_scale_major_ticks(scale, 5, 12, 5, 0x000000, 0)
    local needle = meter:add_indicator_needle(scale, lv.COLOR_RED, 4)
    meter:set_indicator_value(needle, 50)
    T.pass("lvgl.meter")

    -- ---- tabview ----
    local tv = lv.tabview.create(scr)
    tv:set_size(280, 200)
    tv:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 210)
    local tab1 = tv:add_tab("Tab1")
    local tab1_lbl = lv.label.create(tab1)
    tab1_lbl:set_text("Content of Tab 1")
    tab1_lbl:center()
    local tab2 = tv:add_tab("Tab2")
    local tab2_lbl = lv.label.create(tab2)
    tab2_lbl:set_text("Content of Tab 2")
    tab2_lbl:center()
    T.pass("lvgl.tabview")

    -- ---- list ----
    local list = lv.list.create(scr)
    list:set_size(220, 180)
    list:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 440)
    list:add_btn(nil, "Item 1")
    list:add_btn(nil, "Item 2")
    list:add_btn(nil, "Item 3")
    list:add_btn(nil, "Item 4")
    list:add_btn(nil, "Item 5")
    T.pass("lvgl.list")

    -- ==================== 第4列（文本与杂项） x=1020 ====================
    local COL4_X = 1020
    local col4_y = 70

    -- ---- textarea ----
    local ta = lv.textarea.create(scr)
    ta:set_size(220, 80)
    ta:set_text("Edit me...")
    ta:set_one_line(true)
    ta:align(lv.ALIGN_TOP_LEFT, COL4_X, col4_y)
    T.pass("lvgl.textarea")

    -- ---- colorwheel ----
    local cw = lv.colorwheel.create(scr)
    cw:set_size(140, 140)
    cw:align(lv.ALIGN_TOP_LEFT, COL4_X + 20, col4_y + 110)
    T.pass("lvgl.colorwheel")

    -- ---- spinner ----
    local sp = lv.spinner.create(scr)
    sp:set_size(110, 110)
    sp:align(lv.ALIGN_TOP_LEFT, COL4_X + 40, col4_y + 280)
    T.pass("lvgl.spinner")

    -- ---- calendar ----
    local cal = lv.calendar.create(scr)
    cal:set_size(220, 200)
    cal:align(lv.ALIGN_TOP_LEFT, COL4_X, col4_y + 420)
    T.pass("lvgl.calendar")

    -- 立即刷新
    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    -- 周期调用 task_handler 驱动 SDL 事件 + LVGL 渲染
    local timer_id = iot.setInterval(function()
        lv.task_handler()
    end, 10)
    T.pass("lvgl.task_handler", "interval started")
end
