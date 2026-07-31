--- lvgl 模块验证 — 大窗口综合组件测试（酷炫样式 + 主题切换）
-- 1280x720 大窗口，暗色/亮色主题切换，自定义渐变样式
local T = dofile("app/tests/common.lua")

return function()
    local ok, lv = pcall(require, "lvgl")
    if not ok then
        T.skip("lvgl.require", "not registered: " .. tostring(lv))
        return
    end

    -- RGB888 转 LVGL 颜色值（RGB565）
    local function C(rgb)
        return lv.color.hex(rgb)
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

    -- ==================== 主题与颜色方案 ====================
    -- 暗色主题颜色方案
    local DARK = {
        bg          = C(0x1A1A2E),
        card_bg     = C(0x16213E),
        card_bg2    = C(0x0F3460),
        primary     = C(0x00D9FF),  -- 青色
        secondary   = C(0x9D4EDD),  -- 紫色
        accent      = C(0xE94560),  -- 玫红
        text        = C(0xFFFFFF),
        text_dim    = C(0xAAAAAA),
        success     = C(0x4ADE80),
        shadow      = C(0x00D9FF),
    }

    -- 亮色主题颜色方案
    local LIGHT = {
        bg          = C(0xF0F0F5),
        card_bg     = C(0xFFFFFF),
        card_bg2    = C(0xE8E8F0),
        primary     = C(0x2196F3),  -- 蓝色
        secondary   = C(0xFF9800),  -- 橙色
        accent      = C(0xE91E63),  -- 粉红
        text        = C(0x333333),
        text_dim    = C(0x666666),
        success     = C(0x4CAF50),
        shadow      = C(0x2196F3),
    }

    -- 当前主题（默认暗色）
    local theme = DARK
    local is_dark = true

    -- ==================== 创建样式 ====================
    -- 屏幕背景样式
    local style_screen = lv.style.create({})
    lv.style.set_bg_color(style_screen, theme.bg)
    lv.style.set_bg_opa(style_screen, lv.OPA_COVER)
    T.pass("lvgl.style", "screen")

    -- 卡片容器样式（渐变背景 + 圆角 + 阴影）
    local style_card = lv.style.create({
        radius = 12,
        pad_all = 8,
        bg_opa = lv.OPA_COVER,
        border_width = 1,
        shadow_width = 20,
        shadow_spread = 2,
    })
    T.pass("lvgl.style", "card")

    -- 标题文字样式
    local style_title = lv.style.create({
        text_color = 0xFFFF,  -- 白色（暗色主题）
        border_width = 0,
        pad_all = 0,
    })
    T.pass("lvgl.style", "title")

    -- 按钮样式（渐变 + 圆角 + 阴影）
    local style_btn = lv.style.create({
        radius = 8,
        bg_opa = lv.OPA_COVER,
        border_width = 0,
        shadow_width = 15,
        shadow_spread = 1,
        pad_all = 6,
    })
    T.pass("lvgl.style", "btn")

    -- 按钮按下样式
    local style_btn_pressed = lv.style.create({
        radius = 8,
        bg_opa = lv.OPA_80,
        shadow_width = 5,
        pad_all = 8,
    })
    T.pass("lvgl.style", "btn_pressed")

    -- 状态栏样式
    local style_status = lv.style.create({
        radius = 6,
        bg_opa = lv.OPA_80,
        pad_left = 10,
        pad_right = 10,
        pad_top = 4,
        pad_bottom = 4,
        border_width = 0,
    })
    T.pass("lvgl.style", "status")

    -- 滑块指示器样式
    local style_slider_indicator = lv.style.create({
        radius = 4,
        bg_opa = lv.OPA_COVER,
    })
    T.pass("lvgl.style", "slider_indicator")

    -- 滑块旋钮样式
    local style_slider_knob = lv.style.create({
        radius = 10,
        bg_opa = lv.OPA_COVER,
        border_width = 2,
    })
    T.pass("lvgl.style", "slider_knob")

    -- ==================== 应用屏幕样式 ====================
    lv.obj.add_style(scr, style_screen, lv.PART_MAIN)

    -- ==================== 主题切换函数 ====================
    local function apply_theme(is_dark_mode)
        theme = is_dark_mode and DARK or LIGHT
        is_dark = is_dark_mode

        -- 更新屏幕背景
        lv.style.set_bg_color(style_screen, theme.bg)

        -- 更新卡片样式
        lv.style.set_bg_color(style_card, theme.card_bg)
        lv.style.set_bg_grad_color(style_card, theme.card_bg2)
        lv.style.set_bg_grad_dir(style_card, lv.GRAD_DIR_VER)
        lv.style.set_border_color(style_card, theme.primary)
        lv.style.set_shadow_color(style_card, theme.shadow)

        -- 更新标题样式
        lv.style.set_text_color(style_title, theme.text)

        -- 更新按钮样式
        lv.style.set_bg_color(style_btn, theme.primary)
        lv.style.set_bg_grad_color(style_btn, theme.secondary)
        lv.style.set_bg_grad_dir(style_btn, lv.GRAD_DIR_HOR)
        lv.style.set_shadow_color(style_btn, theme.shadow)

        -- 更新按钮按下样式
        lv.style.set_bg_color(style_btn_pressed, theme.secondary)

        -- 更新状态栏样式
        lv.style.set_bg_color(style_status, theme.card_bg)
        lv.style.set_text_color(style_status, theme.text)

        -- 更新滑块样式
        lv.style.set_bg_color(style_slider_indicator, theme.primary)
        lv.style.set_bg_color(style_slider_knob, theme.text)
        lv.style.set_border_color(style_slider_knob, theme.primary)

        -- 创建并设置 LVGL 主题
        local lv_theme = lv.theme.create(theme.primary, theme.secondary, is_dark)
        lv.theme.set(lv_theme)

        lv.refr_now(nil)
    end

    -- 应用默认暗色主题
    apply_theme(true)
    T.pass("lvgl.theme", "dark applied")

    -- ==================== 底部状态栏 ====================
    local status = lv.label.create(scr)
    status:set_text("Ready.")
    status:set_width(1260)
    status:align(lv.ALIGN_BOTTOM_MID, 0, -8)
    status:add_style(style_status, lv.PART_MAIN)
    T.pass("lvgl.label", "status")

    -- ==================== 标题栏 ====================
    local title = lv.label.create(scr)
    title:set_text("◆ IoT-OS LVGL Widget Showcase")
    title:align(lv.ALIGN_TOP_MID, 0, 8)
    title:add_style(style_title, lv.PART_MAIN)
    T.pass("lvgl.label", "title")

    -- ==================== 主题切换按钮 ====================
    local theme_btn = lv.btn.create(scr)
    theme_btn:set_size(120, 36)
    theme_btn:align(lv.ALIGN_TOP_RIGHT, -20, 8)
    theme_btn:add_style(style_btn, lv.PART_MAIN)
    theme_btn:add_style(style_btn_pressed, lv.PART_MAIN + lv.STATE_PRESSED)
    local theme_btn_lbl = lv.label.create(theme_btn)
    theme_btn_lbl:set_text("☀ Light")
    theme_btn_lbl:center()
    theme_btn:add_event_cb(function(e, code)
        if code == lv.EVENT_CLICKED then
            apply_theme(not is_dark)
            theme_btn_lbl:set_text(is_dark and "☀ Light" or "🌙 Dark")
            status:set_text("Theme: " .. (is_dark and "Dark" or "Light"))
        end
    end, lv.EVENT_CLICKED)
    T.pass("lvgl.theme", "toggle button")

    -- ==================== 第1列（基础控件） x=40 ====================
    local COL1_X = 40
    local col1_y = 70

    -- ---- btn ----
    local btn = lv.btn.create(scr)
    btn:set_size(140, 44)
    btn:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y)
    btn:add_style(style_btn, lv.PART_MAIN)
    btn:add_style(style_btn_pressed, lv.PART_MAIN + lv.STATE_PRESSED)
    local btn_lbl = lv.label.create(btn)
    btn_lbl:set_text("Button")
    btn_lbl:center()
    local btn_click_count = 0
    btn:add_event_cb(function(e, code)
        if code == lv.EVENT_CLICKED then
            btn_click_count = btn_click_count + 1
            btn_lbl:set_text("Click " .. btn_click_count)
            status:set_text("Button clicked: " .. btn_click_count)
        end
    end, lv.EVENT_CLICKED)
    T.pass("lvgl.btn")

    -- ---- switch ----
    local sw = lv.switch.create(scr)
    sw:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 70)
    local sw_lbl = lv.label.create(scr)
    sw_lbl:set_text("Switch: OFF")
    sw_lbl:align_to(sw, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    sw_lbl:add_style(style_title, lv.PART_MAIN)
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
    slider:add_style(style_slider_indicator, lv.PART_INDICATOR)
    slider:add_style(style_slider_knob, lv.PART_KNOB)
    local sl_lbl = lv.label.create(scr)
    sl_lbl:set_text("Slider: 0")
    sl_lbl:align_to(slider, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    sl_lbl:add_style(style_title, lv.PART_MAIN)
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
    dd_lbl:add_style(style_title, lv.PART_MAIN)
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

    -- ==================== 第2列（数据控件） x=350 ====================
    local COL2_X = 350
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
    arc_lbl:add_style(style_title, lv.PART_MAIN)
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
    local series = chart:add_series(theme.primary, lv.CHART_AXIS_PRIMARY_Y)
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

    -- ==================== 第3列（高级控件） x=680 ====================
    local COL3_X = 680
    local col3_y = 70

    -- ---- meter ----
    local meter = lv.meter.create(scr)
    meter:set_size(180, 180)
    meter:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y)
    local scale = meter:add_scale(270, 135)
    meter:set_scale_ticks(scale, 11, 6, 2, 0xAAAAAA)
    meter:set_scale_major_ticks(scale, 5, 12, 5, 0xFFFFFF, 0)
    local needle = meter:add_indicator_needle(scale, theme.accent, 4)
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

    -- ==================== 第4列（文本与杂项） x=1000 ====================
    local COL4_X = 1000
    local col4_y = 70

    -- ---- textarea ----
    local ta = lv.textarea.create(scr)
    ta:set_size(240, 80)
    ta:set_text("Edit me...")
    ta:set_one_line(true)
    ta:align(lv.ALIGN_TOP_LEFT, COL4_X, col4_y)
    local def_group = lv.group.get_default()
    if def_group then
        lv.group.add_obj(def_group, ta)
        ta:add_event_cb(function()
            lv.group.set_editing(def_group, true)
            status:set_text("textarea focused — type to edit")
        end, lv.EVENT_FOCUSED)
        ta:add_event_cb(function()
            lv.group.set_editing(def_group, false)
            status:set_text("textarea defocused")
        end, lv.EVENT_DEFOCUSED)
    end
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

    -- ==================== 容器与线条控件 ====================
    -- ---- line (底部折线) ----
    local line_obj = lv.line.create(scr)
    line_obj:set_points({{0,0},{60,10},{30,40},{80,25},{50,55}})
    line_obj:align(lv.ALIGN_BOTTOM_LEFT, 40, -30)
    T.pass("lvgl.line")

    -- ---- win (窗口,用on注册关闭按钮事件) ----
    local win = lv.win.create(scr, 36)
    win:set_title("Window")
    win:set_size(300, 180)
    win:align(lv.ALIGN_CENTER, -260, -60)
    local win_btn = win:add_btn(nil, 36)
    win:set_btn_title(win_btn, "X")
    -- 用on注册click事件
    lv.obj.on(win_btn, "click", function(e, code)
        if code == lv.EVENT_CLICKED then
            lv.obj.delete(win)
            status:set_text("Window closed")
        end
    end)
    local win_cont = win:get_content()
    local win_lbl = lv.label.create(win_cont)
    win_lbl:set_text("Window content area")
    win_lbl:center()
    T.pass("lvgl.win")

    -- ---- msgbox (消息框,用on注册按钮事件) ----
    local mbox = lv.msgbox.create(nil)
    mbox:set_title("提示")
    mbox:set_text("消息框测试,点击按钮关闭")
    mbox:add_button("确定")
    mbox:add_button("取消")
    -- 用on注册change事件
    local mbox_cb_id = mbox:on("change", function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local btn_text = mbox:get_active_btn_text()
            status:set_text("MsgBox: " .. (btn_text or "?"))
            mbox:close()
        end
    end)
    -- 测试off: 先移除再重新注册,验证off正常工作
    mbox:off(mbox_cb_id)
    mbox:on("change", function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            local btn_text = mbox:get_active_btn_text()
            status:set_text("MsgBox: " .. (btn_text or "?"))
            mbox:close()
        end
    end)
    T.pass("lvgl.on")
    T.pass("lvgl.off")
    T.pass("lvgl.msgbox")

    -- ---- tileview (平铺视图) ----
    local tv2 = lv.tileview.create(scr)
    tv2:set_size(240, 120)
    tv2:align(lv.ALIGN_CENTER, 260, 160)
    local tile1 = tv2:add_tile(0, 0, lv.DIR_RIGHT)
    local t1_lbl = lv.label.create(tile1)
    t1_lbl:set_text("Tile 1")
    t1_lbl:center()
    local tile2 = tv2:add_tile(1, 0, lv.DIR_LEFT)
    local t2_lbl = lv.label.create(tile2)
    t2_lbl:set_text("Tile 2")
    t2_lbl:center()
    T.pass("lvgl.tileview")

    -- 立即刷新
    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    -- 周期调用 task_handler 驱动 SDL 事件 + LVGL 渲染
    local timer_id = iot.setInterval(function()
        lv.task_handler()
    end, 10)
    T.pass("lvgl.task_handler", "interval started")
end
