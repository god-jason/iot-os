--- LVGL 全组件 Lua 绑定测试
-- 测试所有封装的 LVGL 组件，包括基础控件、高级控件和新组件
-- 运行方式: xmake run (需先配置 app/tests/.target 或直接运行)
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

    -- 初始化窗口
    lv.init(1024, 768)
    T.pass("lvgl.init", "1024x768")

    -- 周期调用 task_handler
    iot.setInterval(function()
        lv.task_handler()
    end, 10)

    -- 获取活动屏幕
    local scr = lv.scr_act()
    if not scr then
        T.fail("lvgl.scr_act", "no active screen")
        return
    end
    T.pass("lvgl.scr_act")

    -- 颜色辅助函数
    local function C(rgb)
        return lv.color.hex(rgb)
    end

    -- ==================== 基础控件测试 ====================

    -- ---- btn ----
    do
        local btn = lv.btn.create(scr)
        btn:set_size(100, 40)
        btn:set_text("Button")
        btn:align(lv.ALIGN_TOP_LEFT, 10, 10)
        local text = btn:get_text()
        btn:set_state(lv.STATE_DEFAULT)
        T.pass("lvgl.btn", "create/set_text/get_text/set_state")
    end

    -- ---- label ----
    do
        local lbl = lv.label.create(scr)
        lbl:set_text("Hello LVGL")
        lbl:align(lv.ALIGN_TOP_LEFT, 10, 60)
        local text = lbl:get_text()
        lbl:set_long_mode(lv.LABEL_LONG_SCROLL_CIRCULAR)
        lbl:set_recolor(true)
        lbl:set_text_sel_start(0)
        lbl:set_text_sel_end(5)
        lbl:set_text_fmt("Value: %d", 42)
        lbl:ins_text(0, ">>")
        lbl:cut_text(0, 2)
        T.pass("lvgl.label", "create/set_text/get_text/set_long_mode/set_recolor/set_text_sel")
    end

    -- ---- img ----
    do
        local img = lv.img.create(scr)
        img:set_size(60, 60)
        img:align(lv.ALIGN_TOP_LEFT, 10, 110)
        img:set_offset_x(0)
        img:set_offset_y(0)
        img:set_angle(0)
        img:set_scale(256)
        img:set_rotation(0)
        img:set_antialias(true)
        img:set_size_mode(lv.IMG_SIZE_MODE_REAL)
        local pivot = img:get_pivot()
        local antialias = img:get_antialias()
        T.pass("lvgl.img", "create/set_offset/set_angle/set_scale/set_rotation")
    end

    -- ---- line ----
    do
        local line = lv.line.create(scr)
        line:set_points({{0,0},{50,10},{100,40}})
        line:set_y_invert(false)
        line:align(lv.ALIGN_TOP_LEFT, 10, 180)
        local cnt = line:get_point_count()
        local y_inv = line:get_y_invert()
        T.pass("lvgl.line", "create/set_points/set_y_invert/get_point_count")
    end

    -- ---- arc ----
    do
        local arc = lv.arc.create(scr)
        arc:set_size(100, 100)
        arc:set_range(0, 100)
        arc:set_value(50)
        arc:set_rotation(90)
        arc:set_mode(lv.ARC_MODE_NORMAL)
        arc:set_change_rate(5)
        arc:set_knob_offset(0)
        arc:align(lv.ALIGN_TOP_LEFT, 10, 210)
        local v = arc:get_value()
        local start = arc:get_angle_start()
        local end_a = arc:get_angle_end()
        local bg_start = arc:get_bg_angle_start()
        local bg_end = arc:get_bg_angle_end()
        local rot = arc:get_rotation()
        local mode = arc:get_mode()
        T.pass("lvgl.arc", "create/set_range/set_value/set_rotation/set_mode")
    end

    -- ---- bar ----
    do
        local bar = lv.bar.create(scr)
        bar:set_size(150, 20)
        bar:set_range(0, 100)
        bar:set_value(60)
        bar:set_start_value(20)
        bar:set_mode(lv.BAR_MODE_NORMAL)
        bar:set_orientation(lv.ORIENTATION_HORIZONTAL)
        bar:align(lv.ALIGN_TOP_LEFT, 10, 320)
        local v = bar:get_value()
        local min = bar:get_min_value()
        local max = bar:get_max_value()
        local mode = bar:get_mode()
        local orient = bar:get_orientation()
        local sym = bar:is_symmetrical()
        T.pass("lvgl.bar", "create/set_range/set_value/set_mode/set_orientation")
    end

    -- ---- slider ----
    do
        local slider = lv.slider.create(scr)
        slider:set_size(150, 20)
        slider:set_range(0, 100)
        slider:set_value(50)
        slider:set_start_value(20)
        slider:set_mode(lv.SLIDER_MODE_NORMAL)
        slider:set_orientation(lv.ORIENTATION_HORIZONTAL)
        slider:align(lv.ALIGN_TOP_LEFT, 10, 360)
        local v = slider:get_value()
        local left = slider:get_left_value()
        local min = slider:get_min_value()
        local max = slider:get_max_value()
        local mode = slider:get_mode()
        local orient = slider:get_orientation()
        local sym = slider:is_symmetrical()
        T.pass("lvgl.slider", "create/set_range/set_value/set_mode/set_orientation")
    end

    -- ---- checkbox ----
    do
        local cb = lv.checkbox.create(scr)
        cb:set_text("Checkbox")
        cb:align(lv.ALIGN_TOP_LEFT, 10, 400)
        local text = cb:get_text()
        T.pass("lvgl.checkbox", "create/set_text/get_text")
    end

    -- ---- switch ----
    do
        local sw = lv.switch.create(scr)
        sw:align(lv.ALIGN_TOP_LEFT, 10, 440)
        T.pass("lvgl.switch", "create")
    end

    -- ---- led ----
    do
        local led = lv.led.create(scr)
        led:align(lv.ALIGN_TOP_LEFT, 10, 480)
        led:set_color(C(0x00FF00))
        led:set_brightness(128)
        led:on()
        led:off()
        led:toggle()
        local bright = led:get_brightness()
        T.pass("lvgl.led", "create/set_color/set_brightness/on/off/toggle")
    end

    -- ---- dropdown ----
    do
        local dd = lv.dropdown.create(scr)
        dd:set_options("Option1\nOption2\nOption3")
        dd:set_selected(0)
        dd:set_text("Select")
        dd:set_symbol(lv.SYMBOL_DOWN)
        dd:set_selected_highlight(true)
        dd:align(lv.ALIGN_TOP_LEFT, 10, 520)
        local sel = dd:get_selected()
        local text = dd:get_text()
        local opts = dd:get_options()
        local cnt = dd:get_option_count()
        local idx = dd:get_option_index("Option2")
        dd:open()
        dd:close()
        local open = dd:is_open()
        T.pass("lvgl.dropdown", "create/set_options/set_selected/set_symbol/open/close")
    end

    -- ---- roller ----
    do
        local roller = lv.roller.create(scr)
        roller:set_options("Item1\nItem2\nItem3\nItem4")
        roller:set_selected(0)
        roller:set_visible_row_count(3)
        roller:set_selected_str("Item2")
        roller:align(lv.ALIGN_TOP_LEFT, 10, 580)
        local sel = roller:get_selected()
        local opts = roller:get_options()
        local cnt = roller:get_option_count()
        T.pass("lvgl.roller", "create/set_options/set_selected/set_visible_row_count")
    end

    -- ---- textarea ----
    do
        local ta = lv.textarea.create(scr)
        ta:set_size(200, 60)
        ta:set_text("Hello")
        ta:set_placeholder_text("Type here...")
        ta:set_one_line(true)
        ta:set_password_mode(false)
        ta:set_max_length(100)
        ta:set_accepted_chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
        ta:set_cursor_pos(0)
        ta:set_cursor_click_pos(true)
        ta:align(lv.ALIGN_TOP_LEFT, 10, 630)
        local text = ta:get_text()
        local ph = ta:get_placeholder_text()
        local label = ta:get_label()
        local pos = ta:get_cursor_pos()
        ta:add_char(string.byte('A'))
        ta:add_text(" World")
        ta:cursor_right()
        ta:cursor_left()
        ta:cursor_down()
        ta:cursor_up()
        ta:clear_selection()
        T.pass("lvgl.textarea", "create/set_text/set_placeholder_text/set_password_mode")
    end

    -- ---- spinbox ----
    do
        local spin = lv.spinbox.create(scr)
        spin:set_range(0, 100)
        spin:set_value(50)
        spin:set_width(120)
        spin:align(lv.ALIGN_TOP_LEFT, 10, 700)
        T.pass("lvgl.spinbox", "create/set_range/set_value")
    end

    -- ---- keyboard ----
    do
        local kb = lv.keyboard.create(scr)
        kb:set_mode(lv.KEYBOARD_MODE_TEXT_LOWER)
        kb:set_popovers(true)
        kb:align(lv.ALIGN_BOTTOM_MID, 0, -10)
        local mode = kb:get_mode()
        local pop = kb:get_popovers()
        T.pass("lvgl.keyboard", "create/set_mode/set_popovers")
    end

    lv.task_handler()

    -- ==================== 高级控件测试 ====================

    -- ---- chart ----
    do
        local chart = lv.chart.create(scr)
        chart:set_size(250, 150)
        chart:set_type(lv.CHART_TYPE_LINE)
        chart:set_point_count(10)
        chart:set_range(lv.CHART_AXIS_PRIMARY_Y, 0, 100)
        chart:set_div_line_count(3, 5)
        chart:set_update_mode(lv.CHART_UPDATE_MODE_SHIFT)
        chart:align(lv.ALIGN_TOP_LEFT, 220, 10)
        local series = chart:add_series(C(0xFF0000), lv.CHART_AXIS_PRIMARY_Y)
        chart:set_all_value(series, 50)
        chart:set_value_by_id(series, 0, 75)
        chart:set_next_value(series, 80)
        chart:refresh()
        local typ = chart:get_type()
        local pc = chart:get_point_count()
        local um = chart:get_update_mode()
        chart:hide_series(series, false)
        T.pass("lvgl.chart", "create/set_type/add_series/set_value/refresh")
    end

    -- ---- table ----
    do
        local tbl = lv.table.create(scr)
        tbl:set_size(250, 120)
        tbl:set_row_cnt(3)
        tbl:set_col_cnt(4)
        tbl:set_cell_value(0, 0, "Header1")
        tbl:set_cell_value(0, 1, "Header2")
        tbl:set_col_width(0, 60)
        tbl:set_cell_ctrl(0, 0, lv.TABLE_CELL_CTRL_MERGE_RIGHT)
        tbl:set_selected_cell(1, 1)
        tbl:align(lv.ALIGN_TOP_LEFT, 220, 180)
        local v = tbl:get_cell_value(0, 0)
        local rows = tbl:get_row_count()
        local cols = tbl:get_col_count()
        local w = tbl:get_col_width(0)
        local has = tbl:has_cell_ctrl(0, 0, lv.TABLE_CELL_CTRL_MERGE_RIGHT)
        tbl:clear_cell_ctrl(0, 0, lv.TABLE_CELL_CTRL_MERGE_RIGHT)
        T.pass("lvgl.table", "create/set_row_cnt/set_cell_value/set_col_width")
    end

    -- ---- tabview ----
    do
        local tv = lv.tabview.create(scr)
        tv:set_size(250, 160)
        tv:align(lv.ALIGN_TOP_LEFT, 220, 320)
        local tab1 = tv:add_tab("Tab1")
        local lbl1 = lv.label.create(tab1)
        lbl1:set_text("Tab 1 Content")
        lbl1:center()
        local tab2 = tv:add_tab("Tab2")
        local lbl2 = lv.label.create(tab2)
        lbl2:set_text("Tab 2 Content")
        lbl2:center()
        T.pass("lvgl.tabview", "create/add_tab")
    end

    -- ---- tileview ----
    do
        local tv = lv.tileview.create(scr)
        tv:set_size(250, 120)
        tv:align(lv.ALIGN_TOP_LEFT, 220, 500)
        local tile1 = tv:add_tile(0, 0, lv.DIR_RIGHT)
        local lbl1 = lv.label.create(tile1)
        lbl1:set_text("Tile 1")
        lbl1:center()
        T.pass("lvgl.tileview", "create/add_tile")
    end

    -- ---- win ----
    do
        local win = lv.win.create(scr, 30)
        win:set_title("Window Title")
        win:set_size(250, 140)
        win:align(lv.ALIGN_TOP_LEFT, 220, 640)
        local btn = win:add_btn(nil, 24)
        win:set_btn_title(btn, "X")
        local content = win:get_content()
        local lbl = lv.label.create(content)
        lbl:set_text("Window Content")
        lbl:center()
        T.pass("lvgl.win", "create/set_title/add_btn/get_content")
    end

    -- ---- list ----
    do
        local list = lv.list.create(scr)
        list:set_size(180, 150)
        list:align(lv.ALIGN_TOP_LEFT, 500, 10)
        list:add_text("Section")
        list:add_btn(nil, "Item 1")
        list:add_btn(nil, "Item 2")
        list:add_btn(nil, "Item 3")
        list:set_button_text(list, 1, "Updated Item")
        local text = list:get_button_text(list, 1)
        T.pass("lvgl.list", "create/add_text/add_btn/get_button_text")
    end

    -- ---- menu ----
    do
        local menu = lv.menu.create(scr)
        menu:set_size(180, 150)
        menu:align(lv.ALIGN_TOP_LEFT, 500, 180)
        local page = lv.menu.page_create(menu, "Menu")
        local cont = lv.menu.cont_create(page)
        local lbl = lv.label.create(cont)
        lbl:set_text("Menu Item")
        lv.menu.set_page(menu, page)
        T.pass("lvgl.menu", "create/page_create/cont_create/set_page")
    end

    -- ---- msgbox ----
    do
        local mbox = lv.msgbox.create(nil)
        mbox:set_title("Message")
        mbox:set_text("This is a message box")
        mbox:add_button("OK")
        mbox:add_button("Cancel")
        mbox:set_size(250, 100)
        mbox:close()
        T.pass("lvgl.msgbox", "create/set_title/set_text/add_button")
    end

    -- ---- calendar ----
    do
        local cal = lv.calendar.create(scr)
        cal:set_size(200, 180)
        cal:align(lv.ALIGN_TOP_LEFT, 500, 350)
        local dt = cal:get_showed_date()
        T.pass("lvgl.calendar", "create/get_showed_date")
    end

    -- ---- canvas ----
    do
        local canvas = lv.canvas.create(scr)
        canvas:set_size(80, 80)
        canvas:align(lv.ALIGN_TOP_LEFT, 500, 550)
        local buf = lv.malloc(80 * 80 * 4)
        if buf then
            canvas:set_buffer(buf, 80, 80, lv.COLOR_FORMAT_ARGB8888)
            canvas:fill_bg(C(0x0000FF), lv.OPA_COVER)
            local img = canvas:get_img()
            canvas:set_px(10, 10, C(0xFF0000), lv.OPA_COVER)
            T.pass("lvgl.canvas", "create/set_buffer/fill_bg/set_px")
        else
            T.skip("lvgl.canvas", "buffer alloc failed")
        end
    end

    -- ---- colorwheel ----
    do
        local cw = lv.colorwheel.create(scr)
        if cw then
            cw:set_size(100, 100)
            cw:align(lv.ALIGN_TOP_LEFT, 500, 650)
            T.pass("lvgl.colorwheel", "create")
        else
            T.skip("lvgl.colorwheel", "LVGL 9 已移除 colorwheel 控件")
        end
    end

    -- ---- spinner ----
    do
        local sp = lv.spinner.create(scr)
        sp:set_size(60, 60)
        sp:align(lv.ALIGN_TOP_LEFT, 620, 650)
        T.pass("lvgl.spinner", "create")
    end

    lv.task_handler()

    -- ==================== 新组件测试 ====================

    -- ---- scale ----
    do
        local scale = lv.scale.create(scr)
        scale:set_size(150, 150)
        scale:set_range(0, 100)
        scale:set_total_tick_count(11)
        scale:set_major_tick_every(5)
        scale:set_label_show(true)
        scale:align(lv.ALIGN_TOP_LEFT, 720, 10)
        T.pass("lvgl.scale", "create/set_range/set_total_tick_count")
    end

    -- ---- span ----
    do
        local span = lv.span.create(scr)
        span:set_size(200, 60)
        span:align(lv.ALIGN_TOP_LEFT, 720, 180)
        local sp = span:add_span()
        span:set_span_text(sp, "Span Text")
        T.pass("lvgl.span", "create/add_span/set_span_text")
    end

    -- ---- buttonmatrix ----
    do
        local bm = lv.buttonmatrix.create(scr)
        bm:set_map({"Btn1", "Btn2", "Btn3", ""})
        bm:set_size(200, 80)
        bm:align(lv.ALIGN_TOP_LEFT, 720, 270)
        local btn_cnt = bm:get_btn_count()
        T.pass("lvgl.buttonmatrix", "create/set_map/get_btn_count")
    end

    -- ---- imagebutton ----
    do
        local ib = lv.imagebutton.create(scr)
        ib:set_size(60, 60)
        ib:align(lv.ALIGN_TOP_LEFT, 720, 370)
        T.pass("lvgl.imagebutton", "create/set_size")
    end

    -- ---- animimage ----
    do
        local ai = lv.animimage.create(scr)
        ai:set_size(60, 60)
        ai:align(lv.ALIGN_TOP_LEFT, 720, 450)
        ai:set_duration(1000)
        ai:set_repeat_count(0)
        T.pass("lvgl.animimage", "create/set_duration/set_repeat_count")
    end

    -- ---- arclabel ----
    do
        local al = lv.arclabel.create(scr)
        al:set_text("Arc Label")
        al:set_size(100, 100)
        al:align(lv.ALIGN_TOP_LEFT, 720, 530)
        local text = al:get_text()
        T.pass("lvgl.arclabel", "create/set_text/get_text")
    end

    -- ---- gif (仅测试创建，需要gif文件才能显示) ----
    do
        local gif = lv.gif.create(scr)
        gif:set_size(60, 60)
        gif:align(lv.ALIGN_TOP_LEFT, 720, 650)
        -- 设置GIF颜色格式，不实际加载文件
        gif:set_color_format(lv.COLOR_FORMAT_ARGB8888)
        T.pass("lvgl.gif", "create/set_color_format")
    end

    -- ---- ime_pinyin (仅测试创建，需要关联键盘) ----
    do
        local ime = lv.ime_pinyin.create(scr)
        ime:set_mode(0) -- K26 mode
        ime:align(lv.ALIGN_TOP_LEFT, 800, 650)
        local kb = ime:get_kb()
        local panel = ime:get_cand_panel()
        T.pass("lvgl.ime_pinyin", "create/set_mode/get_kb/get_cand_panel")
    end

    -- ==================== 基础功能测试 ====================

    -- ---- style ----
    do
        local s = lv.style.create({
            radius = 8,
            pad_all = 4,
            bg_opa = lv.OPA_COVER,
            border_width = 1,
        })
        T.pass("lvgl.style", "create")
    end

    -- ---- group ----
    do
        local g = lv.group.create()
        lv.group.set_default(g)
        local dg = lv.group.get_default()
        T.pass("lvgl.group", "create/set_default/get_default")
    end

    -- ---- layout ----
    do
        local cont = lv.obj.create(scr)
        cont:set_size(180, 60)
        cont:align(lv.ALIGN_TOP_LEFT, 900, 10)
        lv.layout.set_flex(cont, lv.FLEX_FLOW_ROW, 4, 4, 4, 4)
        local btn1 = lv.btn.create(cont)
        btn1:set_size(40, 30)
        local btn2 = lv.btn.create(cont)
        btn2:set_size(40, 30)
        lv.layout.set_grid(cont, {"100px", "auto"}, {"40px", "auto"})
        T.pass("lvgl.layout", "set_flex/set_grid")
    end

    -- ---- timer ----
    do
        local timer = lv.timer.create(function()
            -- timer callback
        end, 1000)
        lv.timer.set_repeat_count(timer, 1)
        lv.timer.resume(timer)
        T.pass("lvgl.timer", "create/set_repeat_count/resume")
    end

    -- ---- anim ----
    do
        local a = lv.anim.create()
        lv.anim.set_time(a, 500)
        lv.anim.set_playback_time(a, 500)
        lv.anim.set_repeat_count(a, 1)
        lv.anim.set_repeat_delay(a, 0)
        lv.anim.set_playback_delay(a, 0)
        lv.anim.set_reverse(a, false)
        lv.anim.set_early_apply(a, false)
        T.pass("lvgl.anim", "create/set_time/set_playback_time/set_repeat_count")
    end

    -- ---- disp ----
    do
        local dpi = lv.disp.get_dpi()
        local offset = lv.disp.get_offset()
        local fmt = lv.disp.get_color_format()
        lv.disp.set_dpi(96)
        lv.disp.set_offset(0, 0)
        T.pass("lvgl.disp", "get_dpi/get_offset/get_color_format")
    end

    -- ==================== 对象基础功能测试 ====================

    -- ---- obj ----
    do
        local obj = lv.obj.create(scr)
        obj:set_size(100, 40)
        obj:set_pos(10, 10)
        local pos = obj:get_pos()
        local size = obj:get_size()
        obj:add_flag(lv.OBJ_FLAG_CLICKABLE)
        obj:clear_flag(lv.OBJ_FLAG_CLICKABLE)
        obj:set_style_prop("bg_color", C(0x333333), 0)
        T.pass("lvgl.obj", "create/set_size/set_pos/add_flag")
    end

    -- ---- obj event system ----
    do
        local obj = lv.obj.create(scr)
        obj:set_size(100, 40)
        local cb_id = obj:on("click", function(e, code)
            -- event callback
        end)
        obj:off(cb_id)
        T.pass("lvgl.obj.on/off", "event system")
    end

    -- 最终刷新
    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    T.pass("lvgl.all", "全部组件测试完成")
end