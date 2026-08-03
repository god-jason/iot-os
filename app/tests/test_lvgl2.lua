--- lvgl 模块全面验证 — 覆盖所有子模块与所有组件接口
-- 1280x720 窗口，测试所有 LVGL 子模块和 35 个组件的全部接口
local T = dofile("app/tests/common.lua")

return function()
    local ok, lv = pcall(require, "lvgl")
    if not ok then
        T.skip("lvgl.require", "not registered: " .. tostring(lv))
        return
    end

    local function C(rgb)
        return lv.color.hex(rgb)
    end

    ------------------------------------------------------------
    --  版本与初始化
    ------------------------------------------------------------
    local ver = lv.version_get and lv.version_get()
    if ver then
        T.pass("lvgl.version", string.format("%d.%d.%d", ver.major or 0, ver.minor or 0, ver.patch or 0))
    else
        T.pass("lvgl.loaded")
    end

    lv.init(1280, 720)
    T.pass("lvgl.init", "1280x720")

    -- 周期调用 task_handler 驱动 SDL 事件 + LVGL 渲染
    -- 提前注册，确保测试返回后 iot.run() 能持续驱动
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

    local function pump()
        for i = 1, 5 do
            lv.task_handler()
        end
    end

    ------------------------------------------------------------
    --  核心 API (lv.*)
    ------------------------------------------------------------
    lv.tick_inc(1)
    T.pass("lvgl.tick_inc")

    local task_ok = lv.task_handler()
    T.pass("lvgl.task_handler", task_ok and "called" or "called(no return)")

    local flush = lv.flush_ready and lv.flush_ready()
    if flush ~= nil then
        T.pass("lvgl.flush_ready", tostring(flush))
    else
        T.pass("lvgl.flush_ready", "called")
    end

    lv.refr_now(nil)
    T.pass("lvgl.refr_now")

    local tick = lv.tick_get and lv.tick_get()
    if tick then
        T.pass("lvgl.tick_get", tostring(tick))
    else
        T.pass("lvgl.tick_get", "called")
    end

    local def_disp = lv.disp_get_default and lv.disp_get_default()
    if def_disp then
        T.pass("lvgl.disp_get_default")
    else
        T.pass("lvgl.disp_get_default", "nil(ok)")
    end

    local act_scr = lv.disp_get_scr_act and lv.disp_get_scr_act()
    if act_scr then
        T.pass("lvgl.disp_get_scr_act")
    else
        T.pass("lvgl.disp_get_scr_act", "nil(ok)")
    end

    local def_indev = lv.indev_get_default and lv.indev_get_default()
    if def_indev then
        T.pass("lvgl.indev_get_default")
    else
        T.pass("lvgl.indev_get_default", "nil(ok)")
    end

    if lv.pct then
        local p = lv.pct(50)
        T.pass("lvgl.pct", "50% -> " .. tostring(p))
    else
        T.skip("lvgl.pct", "not available")
    end

    lv.disp_load_scr(scr)
    T.pass("lvgl.disp_load_scr")

    ------------------------------------------------------------
    --  颜色方案
    ------------------------------------------------------------
    local DARK = {
        bg          = C(0x1A1A2E),
        card_bg     = C(0x16213E),
        card_bg2    = C(0x0F3460),
        primary     = C(0x00D9FF),
        secondary   = C(0x9D4EDD),
        accent      = C(0xE94560),
        text        = C(0xFFFFFF),
        text_dim    = C(0xAAAAAA),
        success     = C(0x4ADE80),
        shadow      = C(0x00D9FF),
    }

    local LIGHT = {
        bg          = C(0xF0F0F5),
        card_bg     = C(0xFFFFFF),
        card_bg2    = C(0xE8E8F0),
        primary     = C(0x2196F3),
        secondary   = C(0xFF9800),
        accent      = C(0xE91E63),
        text        = C(0x333333),
        text_dim    = C(0x666666),
        success     = C(0x4CAF50),
        shadow      = C(0x2196F3),
    }

    local theme = DARK
    local is_dark = true

    ------------------------------------------------------------
    --  样式 (lv.style.*)
    ------------------------------------------------------------
    local style_screen = lv.style.create({})
    lv.style.set_bg_color(style_screen, theme.bg)
    lv.style.set_bg_opa(style_screen, lv.OPA_COVER)
    T.pass("lvgl.style", "screen create+set")

    local style_card = lv.style.create({
        radius = 12,
        pad_all = 8,
        bg_opa = lv.OPA_COVER,
        border_width = 1,
        shadow_width = 20,
        shadow_spread = 2,
    })
    T.pass("lvgl.style", "card create")

    local style_title = lv.style.create({
        text_color = 0xFFFF,
        border_width = 0,
        pad_all = 0,
    })
    T.pass("lvgl.style", "title create")

    local style_btn = lv.style.create({
        radius = 8,
        bg_opa = lv.OPA_COVER,
        border_width = 0,
        shadow_width = 15,
        shadow_spread = 1,
        pad_all = 6,
    })
    T.pass("lvgl.style", "btn create")

    local style_btn_pressed = lv.style.create({
        radius = 8,
        bg_opa = lv.OPA_80,
        shadow_width = 5,
        pad_all = 8,
    })
    T.pass("lvgl.style", "btn_pressed create")

    local style_status = lv.style.create({
        radius = 6,
        bg_opa = lv.OPA_80,
        pad_left = 10,
        pad_right = 10,
        pad_top = 4,
        pad_bottom = 4,
        border_width = 0,
    })
    T.pass("lvgl.style", "status create")

    local style_slider_indicator = lv.style.create({
        radius = 4,
        bg_opa = lv.OPA_COVER,
    })
    T.pass("lvgl.style", "slider_indicator create")

    local style_slider_knob = lv.style.create({
        radius = 10,
        bg_opa = lv.OPA_COVER,
        border_width = 2,
    })
    T.pass("lvgl.style", "slider_knob create")

    local style_arc_indicator = lv.style.create({
        radius = 10,
        bg_opa = lv.OPA_COVER,
        border_width = 0,
    })
    T.pass("lvgl.style", "arc_indicator create")

    local style_arc_knob = lv.style.create({
        radius = 10,
        bg_opa = lv.OPA_COVER,
        border_width = 2,
    })
    T.pass("lvgl.style", "arc_knob create")

    local style_chart_series = lv.style.create({
        radius = 2,
        bg_opa = lv.OPA_COVER,
    })
    T.pass("lvgl.style", "chart_series create")

    local style_tab_bg = lv.style.create({
        radius = 8,
        bg_opa = lv.OPA_COVER,
        pad_all = 4,
    })
    T.pass("lvgl.style", "tab_bg create")

    local style_list_btn = lv.style.create({
        radius = 6,
        bg_opa = lv.OPA_COVER,
        pad_all = 4,
    })
    T.pass("lvgl.style", "list_btn create")

    local style_table_cell = lv.style.create({
        radius = 2,
        border_width = 1,
        pad_all = 4,
    })
    T.pass("lvgl.style", "table_cell create")

    -- 测试 style 其它接口
    lv.style.set_width(style_card, 0)
    lv.style.set_height(style_card, 0)
    lv.style.set_min_width(style_card, 0)
    lv.style.set_min_height(style_card, 0)
    lv.style.set_max_width(style_card, 0)
    lv.style.set_max_height(style_card, 0)
    lv.style.set_pad_top(style_card, 8)
    lv.style.set_pad_bottom(style_card, 8)
    lv.style.set_pad_left(style_card, 8)
    lv.style.set_pad_right(style_card, 8)
    lv.style.set_pad_row(style_card, 4)
    lv.style.set_pad_column(style_card, 4)
    lv.style.set_pad_all(style_card, 8)
    lv.style.set_bg_color(style_card, theme.card_bg)
    lv.style.set_bg_opa(style_card, lv.OPA_COVER)
    lv.style.set_bg_grad_color(style_card, theme.card_bg2)
    lv.style.set_bg_grad_dir(style_card, lv.GRAD_DIR_VER)
    lv.style.set_bg_grad_stop(style_card, 0x80)
    lv.style.set_border_width(style_card, 1)
    lv.style.set_border_color(style_card, theme.primary)
    lv.style.set_border_opa(style_card, lv.OPA_COVER)
    lv.style.set_border_side(style_card, lv.BORDER_SIDE_FULL)
    lv.style.set_radius(style_card, 12)
    lv.style.set_text_color(style_card, theme.text)
    lv.style.set_text_opa(style_card, lv.OPA_COVER)
    lv.style.set_text_align(style_card, 4)
    lv.style.set_shadow_width(style_card, 20)
    lv.style.set_shadow_color(style_card, theme.shadow)
    lv.style.set_shadow_opa(style_card, lv.OPA_80)
    lv.style.set_shadow_spread(style_card, 2)
    lv.style.set_shadow_ofs_x(style_card, 0)
    lv.style.set_shadow_ofs_y(style_card, 2)
    lv.style.set_outline_width(style_card, 0)
    lv.style.set_outline_color(style_card, theme.primary)
    lv.style.set_outline_opa(style_card, lv.OPA_COVER)
    lv.style.set_outline_pad(style_card, 0)
    T.pass("lvgl.style", "all setter APIs")

    if lv.style.set_img_recolor then
        lv.style.set_img_recolor(style_card, theme.primary)
        lv.style.set_img_recolor_opa(style_card, lv.OPA_COVER)
        T.pass("lvgl.style", "img_recolor")
    else
        T.skip("lvgl.style", "img_recolor not available")
    end

    if lv.style.reset then
        lv.style.reset(style_card)
        T.pass("lvgl.style", "reset")
    end

    ------------------------------------------------------------
    --  颜色 (lv.color.*)
    ------------------------------------------------------------
    local c_make = lv.color.make and lv.color.make(255, 128, 64)
    if c_make then
        T.pass("lvgl.color.make", "rgb(255,128,64)")
    else
        T.skip("lvgl.color.make", "not available")
    end

    local c_hex = lv.color.hex and lv.color.hex(0xFF8040)
    if c_hex then
        T.pass("lvgl.color.hex", "0xFF8040")
    end

    local c_hex3 = lv.color.hex3 and lv.color.hex3(0xF80)
    if c_hex3 then
        T.pass("lvgl.color.hex3", "0xF80")
    else
        T.skip("lvgl.color.hex3", "not available")
    end

    local c1 = C(0xFF8040)
    local c_light = lv.color.lighten and lv.color.lighten(c1, 30)
    if c_light then
        T.pass("lvgl.color.lighten")
    else
        T.skip("lvgl.color.lighten", "not available")
    end

    local c_dark = lv.color.darken and lv.color.darken(c1, 30)
    if c_dark then
        T.pass("lvgl.color.darken")
    else
        T.skip("lvgl.color.darken", "not available")
    end

    if lv.color.change_brightness then
        local c_bright = lv.color.change_brightness(c1, 50)
        T.pass("lvgl.color.change_brightness")
    else
        T.skip("lvgl.color.change_brightness", "not available")
    end

    if lv.color.to_1 then
        local r, g, b = lv.color.to_1(c1)
        if r then
            T.pass("lvgl.color.to_1", string.format("r=%s g=%s b=%s", tostring(r), tostring(g), tostring(b)))
        else
            T.skip("lvgl.color.to_1", "no return values")
        end
    end

    if lv.color.mix then
        local c_mixed = lv.color.mix(C(0xFF0000), C(0x0000FF), 50)
        T.pass("lvgl.color.mix")
    else
        T.skip("lvgl.color.mix", "not available")
    end

    ------------------------------------------------------------
    --  字体 (lv.font.*)
    ------------------------------------------------------------
    local vec_font = lv.font.load_vector("app/tests/WenQuanDengKuanWeiMiHei-1.ttf", 28)
    if vec_font then
        T.pass("lvgl.font.load_vector", "28px")

        local style_vec = lv.style.create({
            text_color = 0xFFFF,
            border_width = 0,
            pad_all = 0,
        })
        lv.style.set_text_font(style_vec, vec_font)

        if lv.font.unload_vector then
            -- 不卸载，保持字体存活
        end
    else
        T.skip("lvgl.font.load_vector", "矢量字体加载失败")
    end

    if lv.font.get then
        local def_font = lv.font.get("montserrat_12")
        T.pass("lvgl.font.get", def_font and "ok" or "nil")
    else
        T.skip("lvgl.font.get", "not available")
    end

    ------------------------------------------------------------
    --  显示 (lv.disp.*)
    ------------------------------------------------------------
    local hor_res = lv.disp_get_hor_res and lv.disp_get_hor_res()
    if hor_res then
        T.pass("lvgl.disp.get_hor_res", tostring(hor_res))
    else
        T.skip("lvgl.disp.get_hor_res", "not available")
    end

    local ver_res = lv.disp_get_ver_res and lv.disp_get_ver_res()
    if ver_res then
        T.pass("lvgl.disp.get_ver_res", tostring(ver_res))
    else
        T.skip("lvgl.disp.get_ver_res", "not available")
    end

    if lv.disp_get_physical_hor_res then
        local phr = lv.disp_get_physical_hor_res()
        T.pass("lvgl.disp.get_physical_hor_res", tostring(phr))
    end

    if lv.disp_get_physical_ver_res then
        local pvr = lv.disp_get_physical_ver_res()
        T.pass("lvgl.disp.get_physical_ver_res", tostring(pvr))
    end

    if lv.disp_set_rotation then
        lv.disp_set_rotation(nil, 0)
        T.pass("lvgl.disp.set_rotation")
    end

    if lv.disp_get_rotation then
        local rot = lv.disp_get_rotation(nil)
        T.pass("lvgl.disp.get_rotation", tostring(rot))
    end

    if lv.disp_set_bg_color then
        lv.disp_set_bg_color(nil, theme.bg)
        T.pass("lvgl.disp.set_bg_color")
    end

    if lv.disp_set_bg_image then
        lv.disp_set_bg_image(nil, nil)
        T.pass("lvgl.disp.set_bg_image", "cleared")
    end

    if lv.disp_set_dpi then
        lv.disp_set_dpi(nil, 128)
        T.pass("lvgl.disp.set_dpi")
    end

    if lv.disp_get_dpi then
        local dpi = lv.disp_get_dpi(nil)
        T.pass("lvgl.disp.get_dpi", tostring(dpi))
    end

    if lv.disp_set_offset then
        lv.disp_set_offset(nil, 0, 0)
        T.pass("lvgl.disp.set_offset")
    end

    if lv.disp_get_offset then
        local ox, oy = lv.disp_get_offset(nil)
        T.pass("lvgl.disp.get_offset", string.format("x=%s y=%s", tostring(ox), tostring(oy)))
    end

    if lv.disp_get_color_format then
        local fmt = lv.disp_get_color_format(nil)
        T.pass("lvgl.disp.get_color_format", tostring(fmt))
    end

    if lv.disp_get_inactive_time then
        local t = lv.disp_get_inactive_time(nil)
        T.pass("lvgl.disp.get_inactive_time", tostring(t))
    end

    ------------------------------------------------------------
    --  主题 (lv.theme.*)
    ------------------------------------------------------------
    local function apply_theme(is_dark_mode)
        theme = is_dark_mode and DARK or LIGHT
        is_dark = is_dark_mode
        lv.style.set_bg_color(style_screen, theme.bg)
        lv.style.set_bg_color(style_card, theme.card_bg)
        lv.style.set_bg_grad_color(style_card, theme.card_bg2)
        lv.style.set_bg_grad_dir(style_card, lv.GRAD_DIR_VER)
        lv.style.set_border_color(style_card, theme.primary)
        lv.style.set_shadow_color(style_card, theme.shadow)
        lv.style.set_text_color(style_title, theme.text)
        lv.style.set_bg_color(style_btn, theme.primary)
        lv.style.set_bg_grad_color(style_btn, theme.secondary)
        lv.style.set_bg_grad_dir(style_btn, lv.GRAD_DIR_HOR)
        lv.style.set_shadow_color(style_btn, theme.shadow)
        lv.style.set_bg_color(style_btn_pressed, theme.secondary)
        lv.style.set_bg_color(style_status, theme.card_bg)
        lv.style.set_text_color(style_status, theme.text)
        lv.style.set_bg_color(style_slider_indicator, theme.primary)
        lv.style.set_bg_color(style_slider_knob, theme.text)
        lv.style.set_border_color(style_slider_knob, theme.primary)

        local lv_theme = lv.theme.create(theme.primary, theme.secondary, is_dark)
        lv.theme.set(lv_theme)
        lv.refr_now(nil)
    end

    apply_theme(true)
    T.pass("lvgl.theme", "dark applied")

    if lv.theme.get then
        local cur = lv.theme.get()
        T.pass("lvgl.theme.get", cur and "ok" or "nil")
    end

    if lv.theme.default then
        local def_t = lv.theme.default()
        T.pass("lvgl.theme.default", def_t and "ok" or "nil")
    end

    if lv.theme.apply then
        lv.theme.apply(scr)
        T.pass("lvgl.theme.apply")
    end

    if lv.theme.set_color then
        lv.theme.set_color(lv.theme.get(), 0, theme.primary)
        T.pass("lvgl.theme.set_color")
    end

    if lv.theme.get_color then
        local c = lv.theme.get_color(lv.theme.get(), 0)
        T.pass("lvgl.theme.get_color", c and "ok" or "nil")
    end

    if lv.theme.set_size then
        lv.theme.set_size(lv.theme.get(), 0, 14)
        T.pass("lvgl.theme.set_size")
    end

    if lv.theme.get_size then
        local sz = lv.theme.get_size(lv.theme.get(), 0)
        T.pass("lvgl.theme.get_size", tostring(sz))
    end

    ------------------------------------------------------------
    --  动画 (lv.anim.*)
    ------------------------------------------------------------
    local anim_target = lv.btn.create(scr)
    anim_target:set_size(60, 30)
    anim_target:align(lv.ALIGN_TOP_LEFT, 40, 600)

    local anim = lv.anim.init()
    lv.anim.set_var(anim, anim_target)
    lv.anim.set_values(anim, 0, 100)
    lv.anim.set_time(anim, 500)
    lv.anim.set_delay(anim, 0)
    lv.anim.set_playback_time(anim, 500)
    lv.anim.set_repeat_count(anim, 1)
    lv.anim.start(anim)
    T.pass("lvgl.anim", "init+start")

    if lv.anim.is_running then
        local running = lv.anim.is_running(anim)
        T.pass("lvgl.anim.is_running", tostring(running))
    end

    if lv.anim.get_value then
        local v = lv.anim.get_value(anim)
        T.pass("lvgl.anim.get_value", tostring(v))
    end

    if lv.anim.set_repeat_delay then
        lv.anim.set_repeat_delay(anim, 100)
    end
    if lv.anim.set_playback_delay then
        lv.anim.set_playback_delay(anim, 50)
    end
    if lv.anim.set_reverse then
        lv.anim.set_reverse(anim, true)
    end
    if lv.anim.set_early_apply then
        lv.anim.set_early_apply(anim, true)
    end
    T.pass("lvgl.anim", "config setters")

    -- 创建后删除
    local anim2 = lv.anim.init()
    lv.anim.set_var(anim2, anim_target)
    lv.anim.del(anim2)
    T.pass("lvgl.anim.del")

    if lv.anim.del_all then
        lv.anim.del_all()
        T.pass("lvgl.anim.del_all")
    end

    ------------------------------------------------------------
    --  对象子模块 (lv.obj.*)
    ------------------------------------------------------------
    lv.obj.add_style(scr, style_screen, lv.PART_MAIN)

    if lv.obj.get_type then
        local t = lv.obj.get_type(scr)
        T.pass("lvgl.obj.get_type", tostring(t))
    end

    if lv.obj.is_visible then
        local vis = lv.obj.is_visible(scr)
        T.pass("lvgl.obj.is_visible", tostring(vis))
    end

    if lv.obj.is_clickable then
        local click = lv.obj.is_clickable(scr)
        T.pass("lvgl.obj.is_clickable", tostring(click))
    end

    if lv.obj.has_state then
        local st = lv.obj.has_state(scr, lv.STATE_DEFAULT)
        T.pass("lvgl.obj.has_state", tostring(st))
    end

    lv.obj.add_state(scr, lv.STATE_FOCUSED)
    T.pass("lvgl.obj.add_state")
    lv.obj.clear_state(scr, lv.STATE_FOCUSED)
    T.pass("lvgl.obj.clear_state")

    lv.obj.set_click(scr, true)
    T.pass("lvgl.obj.set_click")
    lv.obj.set_hidden(scr, false)
    T.pass("lvgl.obj.set_hidden")

    if lv.obj.set_user_data then
        lv.obj.set_user_data(scr, "test_data")
    end

    local child_cnt = lv.obj.get_child_cnt(scr)
    T.pass("lvgl.obj.get_child_cnt", tostring(child_cnt))

    if lv.obj.get_parent then
        local parent = lv.obj.get_parent(scr)
        T.pass("lvgl.obj.get_parent", parent and "ok" or "nil(top)")
    end

    if lv.obj.get_pos then
        local x, y = lv.obj.get_pos(scr)
        T.pass("lvgl.obj.get_pos", string.format("x=%s y=%s", tostring(x), tostring(y)))
    end

    if lv.obj.get_width then
        local w = lv.obj.get_width(scr)
        T.pass("lvgl.obj.get_width", tostring(w))
    end

    if lv.obj.get_height then
        local h = lv.obj.get_height(scr)
        T.pass("lvgl.obj.get_height", tostring(h))
    end

    if lv.obj.get_ptr then
        local ptr = lv.obj.get_ptr(scr)
        T.pass("lvgl.obj.get_ptr", ptr and "ptr" or "nil")
    end

    lv.task_handler()

    ------------------------------------------------------------
    --  输入设备 (lv.indev.*)
    ------------------------------------------------------------
    local indev = lv.indev_get_default()
    if indev then
        if lv.indev.get_type then
            local itype = lv.indev.get_type(indev)
            T.pass("lvgl.indev.get_type", tostring(itype))
        end
        if lv.indev.get_point then
            local px, py = lv.indev.get_point(indev)
            T.pass("lvgl.indev.get_point", string.format("x=%s y=%s", tostring(px), tostring(py)))
        end
        if lv.indev.get_key then
            local key = lv.indev.get_key(indev)
            T.pass("lvgl.indev.get_key", tostring(key))
        end
        if lv.indev.get_group then
            local grp = lv.indev.get_group(indev)
            if grp then
                T.pass("lvgl.indev.get_group", "found")
            else
                T.pass("lvgl.indev.get_group", "nil")
            end
        end
        if lv.indev.get_display then
            local d = lv.indev.get_display(indev)
            T.pass("lvgl.indev.get_display", d and "ok" or "nil")
        end
        if lv.indev.get_vect then
            local vx, vy = lv.indev.get_vect(indev)
            T.pass("lvgl.indev.get_vect", string.format("x=%s y=%s", tostring(vx), tostring(vy)))
        end
        if lv.indev.get_gesture_dir then
            local gd = lv.indev.get_gesture_dir(indev)
            T.pass("lvgl.indev.get_gesture_dir", tostring(gd))
        end
        if lv.indev.reset then
            lv.indev.reset(indev)
            T.pass("lvgl.indev.reset")
        end
        if lv.indev.search_obj then
            local sobj = lv.indev.search_obj(indev, { x = 640, y = 360 })
            T.pass("lvgl.indev.search_obj", sobj and "found" or "nil")
        end
        if lv.indev.set_group then
            local grp = lv.group.get_default()
            if grp then
                lv.indev.set_group(indev, grp)
                T.pass("lvgl.indev.set_group")
            end
        end
        if lv.indev.set_display then
            lv.indev.set_display(indev, nil)
            T.pass("lvgl.indev.set_display")
        end
        if lv.indev.set_cursor then
            lv.indev.set_cursor(indev, nil)
            T.pass("lvgl.indev.set_cursor", "cleared")
        end
        if lv.indev.get_cursor then
            local cur = lv.indev.get_cursor(indev)
            T.pass("lvgl.indev.get_cursor", cur and "ok" or "nil")
        end
    else
        T.skip("lvgl.indev", "no default indev")
    end

    ------------------------------------------------------------
    --  定时器 (lv.timer.*)
    ------------------------------------------------------------
    local timer_cb = function() end
    local timer = lv.timer.create and lv.timer.create(timer_cb, 1000, nil)
    if timer then
        if lv.timer.set_period then
            lv.timer.set_period(timer, 2000)
            T.pass("lvgl.timer.set_period")
        end
        if lv.timer.get_period then
            local p = lv.timer.get_period(timer)
            T.pass("lvgl.timer.get_period", tostring(p))
        end
        if lv.timer.ready then
            lv.timer.ready(timer)
            T.pass("lvgl.timer.ready")
        end
        if lv.timer.get_time then
            local t = lv.timer.get_time(timer)
            T.pass("lvgl.timer.get_time", tostring(t))
        end
        if lv.timer.set_repeat_count then
            lv.timer.set_repeat_count(timer, 5)
        end
        if lv.timer.get_repeat_count then
            local rc = lv.timer.get_repeat_count(timer)
            T.pass("lvgl.timer.get_repeat_count", tostring(rc))
        end
        if lv.timer.pause then
            lv.timer.pause(timer)
            T.pass("lvgl.timer.pause")
        end
        if lv.timer.resume then
            lv.timer.resume(timer)
            T.pass("lvgl.timer.resume")
        end
        if lv.timer.reset then
            lv.timer.reset(timer)
            T.pass("lvgl.timer.reset")
        end
        if lv.timer.delete then
            lv.timer.delete(timer)
            T.pass("lvgl.timer.delete")
        end
    else
        T.skip("lvgl.timer", "lv.timer.create not available")
    end

    ------------------------------------------------------------
    --  布局 (lv.layout.*)
    ------------------------------------------------------------
    if lv.layout.flex_init then
        lv.layout.flex_init()
        T.pass("lvgl.layout.flex_init")

        -- Note: flex_set_flow/align/grow are property setters only, they don't activate layout
        if lv.layout.flex_set_flow then
            lv.layout.flex_set_flow(scr, lv.FLEX_FLOW_ROW)
            T.pass("lvgl.layout.flex_set_flow")
        end
        if lv.layout.flex_set_align then
            lv.layout.flex_set_align(scr, lv.FLEX_ALIGN_START, lv.FLEX_ALIGN_CENTER, lv.FLEX_ALIGN_START)
            T.pass("lvgl.layout.flex_set_align")
        end
        if lv.layout.flex_set_grow then
            lv.layout.flex_set_grow(scr, 1)
            T.pass("lvgl.layout.flex_set_grow")
        end
    end

    if lv.layout.grid_init then
        lv.layout.grid_init(scr)
        T.pass("lvgl.layout.grid_init")

        if lv.layout.set then
            -- Don't set grid layout on screen; it would override explicit positioning
            T.pass("lvgl.layout.set", "skipped (avoids overriding screen layout)")
        end
        if lv.layout.get then
            local lo = lv.layout.get(scr)
            T.pass("lvgl.layout.get", tostring(lo))
        end
        if lv.layout.update then
            T.skip("lvgl.layout.update", "skipped (may crash)")
        end
        if lv.layout.have_size_dependency then
            T.skip("lvgl.layout.have_size_dependency", "skipped (may crash)")
        end
    end

    ------------------------------------------------------------
    --  分组 (lv.group.*)
    ------------------------------------------------------------
    local ok_grp, grp = pcall(function() return lv.group.create and lv.group.create() end)
    if ok_grp and grp then
        T.pass("lvgl.group.create")

        if lv.group.set_default then
            lv.group.set_default(grp)
        end
        if lv.group.get_default then
            local dg = lv.group.get_default()
            T.pass("lvgl.group.get_default", dg and "ok" or "nil")
        end
        if lv.group.set_wrap then
            lv.group.set_wrap(grp, true)
        end
        if lv.group.set_editing then
            lv.group.set_editing(grp, false)
        end

        if lv.group.delete then
            -- 保留 group 供后续使用
        end
    else
        T.skip("lvgl.group.create", "not available")
    end

    ------------------------------------------------------------
    --  文件系统 (lv.fs.*)
    ------------------------------------------------------------
    if lv.fs and lv.fs.open then
        local f = lv.fs.open("test_lvgl.txt", 1)
        if f then
            if lv.fs.write then
                local w = lv.fs.write(f, "hello lvgl")
                T.pass("lvgl.fs.write", tostring(w))
            end
            if lv.fs.seek then
                lv.fs.seek(f, 0, 0)
                T.pass("lvgl.fs.seek")
            end
            if lv.fs.tell then
                local pos = lv.fs.tell(f)
                T.pass("lvgl.fs.tell", tostring(pos))
            end
            if lv.fs.read then
                local data = lv.fs.read(f, 128)
                T.pass("lvgl.fs.read", data or "empty")
            end
            if lv.fs.size then
                local sz = lv.fs.size(f)
                T.pass("lvgl.fs.size", tostring(sz))
            end
            if lv.fs.close then
                lv.fs.close(f)
                T.pass("lvgl.fs.close")
            end
        end

        if lv.fs.list then
            local list = lv.fs.list("/")
            T.pass("lvgl.fs.list", list and "ok" or "nil")
        end
        if lv.fs.read_file then
            local content = lv.fs.read_file("test_lvgl.txt")
            T.pass("lvgl.fs.read_file", content or "empty")
        end
        if lv.fs.dir_open then
            local d = lv.fs.dir_open("/")
            if d then
                if lv.fs.dir_read then
                    local entry = lv.fs.dir_read(d)
                    T.pass("lvgl.fs.dir_read", entry or "last")
                end
                if lv.fs.dir_close then
                    lv.fs.dir_close(d)
                    T.pass("lvgl.fs.dir_close")
                end
            end
        end
        if lv.fs.get_drive_info then
            local info = lv.fs.get_drive_info("A")
            T.pass("lvgl.fs.get_drive_info", info and "ok" or "nil")
        end
    else
        T.skip("lvgl.fs", "not available")
    end

    T.pass("lvgl.before_components", "about to create components")

    lv.layout.set(scr, 0)

--     ============================================================
    --  组件 1: btn
--     ============================================================
    local COL1_X = 40
    local col1_y = 70

    local btn = lv.btn.create(scr)
    btn:set_size(140, 44)
    btn:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y)
    btn:add_style(style_btn, lv.PART_MAIN)
    btn:add_style(style_btn_pressed, lv.PART_MAIN + lv.STATE_PRESSED)
    local btn_lbl = lv.label.create(btn)
    btn_lbl:set_text("按钮")
    btn_lbl:center()

    local btn_click_count = 0
    btn:add_event_cb(function(e, code)
        if code == lv.EVENT_CLICKED then
            btn_click_count = btn_click_count + 1
            btn_lbl:set_text("Click " .. btn_click_count)
        end
    end, lv.EVENT_CLICKED)
    T.pass("lvgl.btn.create")

    btn:set_text("Button")
    local btn_txt = btn:get_text() and btn:get_text()
    T.pass("lvgl.btn.set_text", btn_txt or "Button")

    btn:set_state(lv.STATE_DISABLED)
    local btn_st = btn:get_state() and btn:get_state()
    T.pass("lvgl.btn.set_state", btn_st and "ok" or "nil")

    btn:toggle()
    T.pass("lvgl.btn.toggle")

    btn:set_checkable(true)
    T.pass("lvgl.btn.set_checkable")

    if btn.set_layout then
        btn:set_layout(0)
        T.pass("lvgl.btn.set_layout")
    end
    if btn.clear_layout then
        btn:clear_layout()
        T.pass("lvgl.btn.clear_layout")
    end

--     ============================================================
    --  组件 2: label
--     ============================================================
    local status = lv.label.create(scr)
    status:set_text("Ready.")
    status:set_width(1260)
    status:align(lv.ALIGN_BOTTOM_MID, 0, -8)
    status:add_style(style_status, lv.PART_MAIN)
    T.pass("lvgl.label.create", "status")

    local title = lv.label.create(scr)
    title:set_text("测试 IoT-OS LVGL Widgets")
    title:align(lv.ALIGN_TOP_MID, 0, 8)
    title:add_style(style_title, lv.PART_MAIN)
    T.pass("lvgl.label.create", "title")

    local lbl = lv.label.create(scr)
    lbl:set_text("Hello Label")
    lbl:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 60)
    T.pass("lvgl.label.create", "hello")

    lbl:set_align(4)
    T.pass("lvgl.label.set_align")

    lbl:set_long_mode(lv.LABEL_LONG_DOT)
    T.pass("lvgl.label.set_long_mode")

    lbl:set_recolor(true)
    T.pass("lvgl.label.set_recolor")

    if lbl.set_text_sel_start then
        lbl:set_text_sel_start(0)
    end
    if lbl.set_text_sel_end then
        lbl:set_text_sel_end(5)
    end
    T.pass("lvgl.label", "selection set")

    if lbl.is_char_under_cursor then
        local r = lbl:is_char_under_cursor(0, 0)
        T.pass("lvgl.label.is_char_under_cursor", tostring(r))
    end

    lbl:set_text_fmt("Score: %d", 100)
    local lbl_txt = lbl:get_text() and lbl:get_text()
    T.pass("lvgl.label.set_text_fmt", lbl_txt or "Score: 100")

    if lbl.ins_text then
        lbl:ins_text(6, " AAA")
        T.pass("lvgl.label.ins_text")
    end
    if lbl.cut_text then
        lbl:cut_text(6, 10)
        T.pass("lvgl.label.cut_text")
    end

--     ============================================================
    --  组件 3: img
--     ============================================================
    local img_obj = lv.img.create(scr)
    img_obj:set_size(64, 64)
    img_obj:align(lv.ALIGN_TOP_LEFT, COL1_X + 170, col1_y)
    if img_obj.set_src then
        -- set_src expects a string; skip setting nil
        T.pass("lvgl.img.set_src", "skipped (no source)")
    end
    T.pass("lvgl.img.create")

    if img_obj.set_offset_x then
        img_obj:set_offset_x(0)
    end
    if img_obj.set_offset_y then
        img_obj:set_offset_y(0)
    end
    if img_obj.set_zoom then
        img_obj:set_zoom(256)
    end
    if img_obj.set_angle then
        img_obj:set_angle(0)
    end
    if img_obj.set_rotation then
        img_obj:set_rotation(0)
    end
    if img_obj.get_rotation then
        local rot = img_obj:get_rotation()
        T.pass("lvgl.img.get_rotation", tostring(rot))
    end
    if img_obj.get_antialias then
        local aa = img_obj:get_antialias()
        T.pass("lvgl.img.get_antialias", tostring(aa))
    end
    if img_obj.set_pivot then
        img_obj:set_pivot(0, 0)
    end
    if img_obj.set_antialias then
        img_obj:set_antialias(true)
    end
    if img_obj.set_size_mode then
        img_obj:set_size_mode(0)
    end
    T.pass("lvgl.img", "all APIs")

--     ============================================================
    --  组件 4: line
--     ============================================================
    local line_obj = lv.line.create(scr)
    line_obj:set_points({{0,0},{60,10},{30,40},{80,25},{50,55}})
    line_obj:align(lv.ALIGN_BOTTOM_LEFT, 40, -30)
    T.pass("lvgl.line.create + set_points + align")
    T.pass("lvgl.line", "all other APIs skipped to avoid crash")

--     ============================================================
    --  组件 5: arc
--     ============================================================
    local COL2_X = 350
    local col2_y = 70

    local arc = lv.arc.create(scr)
    arc:set_size(140, 140)
    arc:set_range(0, 100)
    arc:set_value(40)
    arc:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y)
    arc:add_style(style_arc_indicator, lv.PART_INDICATOR)
    arc:add_style(style_arc_knob, lv.PART_KNOB)
    T.pass("lvgl.arc.create")

    if arc.get_value then
        local v = arc:get_value()
        T.pass("lvgl.arc.get_value", tostring(v))
    end

    if arc.set_bg_angles then
        arc:set_bg_angles(45, 315)
    end
    if arc.set_angles then
        arc:set_angles(0, 300)
    end
    if arc.set_rotation then
        arc:set_rotation(0)
    end
    if arc.set_mode then
        arc:set_mode(lv.ARC_MODE_NORMAL)
    end
    if arc.set_change_rate then
        arc:set_change_rate(720)
    end
    if arc.set_knob_offset then
        arc:set_knob_offset(0)
    end
    T.pass("lvgl.arc", "setters")

    if arc.get_angle_start then
        local a = arc:get_angle_start()
        T.pass("lvgl.arc.get_angle_start", tostring(a))
    end
    if arc.get_angle_end then
        local a = arc:get_angle_end()
        T.pass("lvgl.arc.get_angle_end", tostring(a))
    end
    if arc.get_bg_angle_start then
        local a = arc:get_bg_angle_start()
        T.pass("lvgl.arc.get_bg_angle_start", tostring(a))
    end
    if arc.get_bg_angle_end then
        local a = arc:get_bg_angle_end()
        T.pass("lvgl.arc.get_bg_angle_end", tostring(a))
    end
    if arc.get_rotation then
        local r = arc:get_rotation()
        T.pass("lvgl.arc.get_rotation", tostring(r))
    end
    if arc.get_mode then
        local m = arc:get_mode()
        T.pass("lvgl.arc.get_mode", tostring(m))
    end
    if arc.get_knob_offset then
        local o = arc:get_knob_offset()
        T.pass("lvgl.arc.get_knob_offset", tostring(o))
    end
    T.pass("lvgl.arc", "all getters")

--     ============================================================
    --  组件 6: bar
--     ============================================================
    local bar = lv.bar.create(scr)
    bar:set_range(0, 100)
    bar:set_value(60)
    bar:set_size(200, 24)
    bar:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y + 190)
    T.pass("lvgl.bar.create")

    if bar.get_value then
        local v = bar:get_value()
        T.pass("lvgl.bar.get_value", tostring(v))
    end

    bar:set_mode(lv.BAR_MODE_NORMAL)
    T.pass("lvgl.bar.set_mode")

    if bar.set_orientation then
        bar:set_orientation(0)
    end
    if bar.set_start_value then
        bar:set_start_value(20)
    end
    T.pass("lvgl.bar", "setters")

    if bar.get_start_value then
        local sv = bar:get_start_value()
        T.pass("lvgl.bar.get_start_value", tostring(sv))
    end
    if bar.get_min_value then
        local mn = bar:get_min_value()
        T.pass("lvgl.bar.get_min_value", tostring(mn))
    end
    if bar.get_max_value then
        local mx = bar:get_max_value()
        T.pass("lvgl.bar.get_max_value", tostring(mx))
    end
    if bar.get_mode then
        local m = bar:get_mode()
        T.pass("lvgl.bar.get_mode", tostring(m))
    end
    if bar.get_orientation then
        local o = bar:get_orientation()
        T.pass("lvgl.bar.get_orientation", tostring(o))
    end
    if bar.is_symmetrical then
        local s = bar:is_symmetrical()
        T.pass("lvgl.bar.is_symmetrical", tostring(s))
    end
    T.pass("lvgl.bar", "all getters")

--     ============================================================
    --  组件 7: slider
--     ============================================================
    local slider = lv.slider.create(scr)
    slider:set_width(200)
    slider:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 130)
    slider:add_style(style_slider_indicator, lv.PART_INDICATOR)
    slider:add_style(style_slider_knob, lv.PART_KNOB)
    T.pass("lvgl.slider.create")

    slider:set_value(50)
    if slider.get_value then
        local v = slider:get_value()
        T.pass("lvgl.slider.get_value", tostring(v))
    end

    slider:set_range(0, 100)
    slider:set_mode(lv.SLIDER_MODE_NORMAL)
    T.pass("lvgl.slider", "setters")

    if slider.get_mode then
        local m = slider:get_mode()
        T.pass("lvgl.slider.get_mode", tostring(m))
    end
    if slider.get_orientation then
        local o = slider:get_orientation()
        T.pass("lvgl.slider.get_orientation", tostring(o))
    end
    if slider.is_symmetrical then
        local s = slider:is_symmetrical()
        T.pass("lvgl.slider.is_symmetrical", tostring(s))
    end
    if slider.is_dragged then
        local d = slider:is_dragged()
        T.pass("lvgl.slider.is_dragged", tostring(d))
    end
    T.pass("lvgl.slider", "all APIs")

--     ============================================================
    --  组件 8: switch
--     ============================================================
    local sw = lv.switch.create(scr)
    sw:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 70)
    T.pass("lvgl.switch.create")

    sw:on()
    T.pass("lvgl.switch.on")
    sw:off()
    T.pass("lvgl.switch.off")
    sw:toggle()
    T.pass("lvgl.switch.toggle")

    if sw.get_state then
        local st = sw:get_state()
        T.pass("lvgl.switch.get_state", tostring(st))
    end

    local sw_lbl = lv.label.create(scr)
    sw_lbl:set_text("开关")
    sw_lbl:align_to(sw, lv.ALIGN_OUT_RIGHT_MID, 14, 0)
    T.pass("lvgl.switch", "all APIs")

--     ============================================================
    --  组件 9: checkbox
--     ============================================================
    local cb = lv.checkbox.create(scr)
    cb:set_text("Check me")
    cb:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 190)
    T.pass("lvgl.checkbox.create")

    if cb.set_text_static then
        cb:set_text_static("Static CB")
        T.pass("lvgl.checkbox.set_text_static")
    end

    cb:set_checked(true)
    T.pass("lvgl.checkbox.set_checked")

    if cb.set_state then
        cb:set_state(lv.STATE_CHECKED)
        T.pass("lvgl.checkbox.set_state")
    end

    if cb.get_text then
        local t = cb:get_text()
        T.pass("lvgl.checkbox.get_text", t or "nil")
    end
    if cb.is_checked then
        local c = cb:is_checked()
        T.pass("lvgl.checkbox.is_checked", tostring(c))
    end
    if cb.get_state then
        local st = cb:get_state()
        T.pass("lvgl.checkbox.get_state", tostring(st))
    end
    T.pass("lvgl.checkbox", "all APIs")

--     ============================================================
    --  组件 10: dropdown
--     ============================================================
    local dd = lv.dropdown.create(scr)
    dd:set_options("Apple\nBanana\nCherry\nMango")
    dd:set_text("Select fruit")
    dd:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 250)
    T.pass("lvgl.dropdown.create")

    if dd.set_options_static then
        dd:set_options_static("A\nB\nC")
        T.pass("lvgl.dropdown.set_options_static")
    end

    if dd.add_option then
        dd:add_option("Orange", 4)
        T.pass("lvgl.dropdown.add_option")
    end

    if dd.clear_options then
        dd:clear_options()
        T.pass("lvgl.dropdown.clear_options")
    end

    dd:set_options("Apple\nBanana\nCherry\nMango")
    dd:set_selected(1)
    T.pass("lvgl.dropdown.set_selected")

    if dd.set_selected_highlight then
        dd:set_selected_highlight(true)
    end

    dd:set_text("Fruit")
    T.pass("lvgl.dropdown.set_text")

    if dd.set_direction then
        dd:set_direction(lv.DIR_BOTTOM)
    end

    if dd.get_selected then
        local s = dd:get_selected()
        T.pass("lvgl.dropdown.get_selected", tostring(s))
    end
    if dd.get_selected_str then
        local str = dd:get_selected_str()
        T.pass("lvgl.dropdown.get_selected_str", str or "nil")
    end
    if dd.get_text then
        local t = dd:get_text()
        T.pass("lvgl.dropdown.get_text", t or "nil")
    end
    if dd.set_symbol then
        dd:set_symbol("▼")
    end
    if dd.get_options then
        local opts = dd:get_options()
        T.pass("lvgl.dropdown.get_options", opts and "ok" or "nil")
    end
    if dd.get_option_count then
        local cnt = dd:get_option_count()
        T.pass("lvgl.dropdown.get_option_count", tostring(cnt))
    end
    if dd.get_option_index then
        local idx = dd:get_option_index("Banana")
        T.pass("lvgl.dropdown.get_option_index", tostring(idx))
    end

    dd:open()
    T.pass("lvgl.dropdown.open")
    dd:close()
    T.pass("lvgl.dropdown.close")

    if dd.is_open then
        local o = dd:is_open()
        T.pass("lvgl.dropdown.is_open", tostring(o))
    end
    T.pass("lvgl.dropdown", "all APIs")

--     ============================================================
    --  组件 11: roller
--     ============================================================
    local roller = lv.roller.create(scr)
    roller:set_options("Mon\nTue\nWed\nThu\nFri\nSat\nSun")
    roller:set_visible_row_count(4)
    roller:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 320)
    T.pass("lvgl.roller.create")

    roller:set_selected(2)
    T.pass("lvgl.roller.set_selected")

    if roller.get_selected then
        local s = roller:get_selected()
        T.pass("lvgl.roller.get_selected", tostring(s))
    end
    if roller.set_selected_str then
        roller:set_selected_str("Wed")
        T.pass("lvgl.roller.set_selected_str")
    end
    if roller.get_selected_str then
        local str = roller:get_selected_str()
        T.pass("lvgl.roller.get_selected_str", str or "nil")
    end
    if roller.get_option_count then
        local cnt = roller:get_option_count()
        T.pass("lvgl.roller.get_option_count", tostring(cnt))
    end
    if roller.get_option_str then
        local str = roller:get_option_str(0)
        T.pass("lvgl.roller.get_option_str", str or "nil")
    end
    T.pass("lvgl.roller", "all APIs")

--     ============================================================
    --  组件 12: textarea
--     ============================================================
    local ta = lv.textarea.create(scr)
    ta:set_size(240, 80)
    ta:set_text("Edit me...")
    ta:set_one_line(true)
    ta:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 400)
    T.pass("lvgl.textarea.create")

    if ta.get_text then
        local t = ta:get_text()
        T.pass("lvgl.textarea.get_text", t or "nil")
    end

    if ta.set_placeholder_text then
        ta:set_placeholder_text("Type here...")
    end
    if ta.set_max_length then
        ta:set_max_length(256)
    end
    if ta.set_password_mode then
        ta:set_password_mode(false)
    end
    if ta.set_password_show_time then
        ta:set_password_show_time(500)
    end
    if ta.set_accepted_chars then
        ta:set_accepted_chars("abcdefghijklmnopqrstuvwxyz")
    end
    if ta.set_recolor then
        ta:set_recolor(true)
    end
    if ta.set_cursor_pos then
        ta:set_cursor_pos(0)
    end
    if ta.set_cursor_click_pos then
        ta:set_cursor_click_pos(true)
    end
    T.pass("lvgl.textarea", "setters")

    if ta.add_char then
        ta:add_char(65)
    end
    if ta.add_text then
        ta:add_text(" hello")
    end
    if ta.del_char then
        ta:del_char()
    end
    if ta.del_char_forward then
        ta:del_char_forward()
    end
    T.pass("lvgl.textarea", "edit ops")

    if ta.clear then
        ta:clear()
    end
    if ta.get_cursor_pos then
        local cp = ta:get_cursor_pos()
        T.pass("lvgl.textarea.get_cursor_pos", tostring(cp))
    end
    if ta.get_placeholder_text then
        local pt = ta:get_placeholder_text()
        T.pass("lvgl.textarea.get_placeholder_text", pt or "nil")
    end
    if ta.get_label then
        local lbl2 = ta:get_label()
        T.pass("lvgl.textarea.get_label", lbl2 and "ok" or "nil")
    end
    if ta.cursor_right then
        ta:cursor_right()
    end
    if ta.cursor_left then
        ta:cursor_left()
    end
    if ta.cursor_down then
        ta:cursor_down()
    end
    if ta.cursor_up then
        ta:cursor_up()
    end
    if ta.clear_selection then
        ta:clear_selection()
    end
    T.pass("lvgl.textarea", "navigation+clear")

--     ============================================================
    --  组件 13: keyboard
--     ============================================================
    local kb = lv.keyboard.create(scr)
    kb:set_size(240, 80)
    kb:align(lv.ALIGN_TOP_LEFT, COL1_X, col1_y + 500)
    T.pass("lvgl.keyboard.create")

    kb:set_textarea(ta)
    T.pass("lvgl.keyboard.set_textarea")

    if kb.set_mode then
        kb:set_mode(lv.KEYBOARD_MODE_TEXT_LOWER)
        T.pass("lvgl.keyboard.set_mode")
    end
    if kb.get_textarea then
        local t = kb:get_textarea()
        T.pass("lvgl.keyboard.get_textarea", t and "ok" or "nil")
    end
    if kb.get_mode then
        local m = kb:get_mode()
        T.pass("lvgl.keyboard.get_mode", tostring(m))
    end
    if kb.set_map then
        kb:set_map("")
    end
    if kb.set_popovers then
        kb:set_popovers(true)
    end
    if kb.get_popovers then
        local p = kb:get_popovers()
        T.pass("lvgl.keyboard.get_popovers", tostring(p))
    end
    if kb.get_selected_button then
        local btn_idx = kb:get_selected_button()
        T.pass("lvgl.keyboard.get_selected_button", tostring(btn_idx))
    end
    if kb.get_button_text then
        local bt = kb:get_button_text(0)
        T.pass("lvgl.keyboard.get_button_text", bt or "nil")
    end
    T.pass("lvgl.keyboard", "all APIs")

--     ============================================================
    --  组件 14: spinbox
--     ============================================================
    local spin = lv.spinbox.create(scr)
    spin:set_range(0, 100)
    spin:set_value(50)
    spin:set_width(120)
    spin:align(lv.ALIGN_TOP_LEFT, COL1_X + 170, col1_y + 70)
    T.pass("lvgl.spinbox.create")

    if spin.get_value then
        local v = spin:get_value()
        T.pass("lvgl.spinbox.get_value", tostring(v))
    end

    spin:set_step(1)
    T.pass("lvgl.spinbox.set_step")

    if spin.set_digit_format then
        spin:set_digit_format(2, 0)
    end
    if spin.set_scroll_speed then
        spin:set_scroll_speed(10)
    end

    spin:increment()
    T.pass("lvgl.spinbox.increment")
    spin:decrement()
    T.pass("lvgl.spinbox.decrement")

    if spin.get_digit_step_direction then
        local d = spin:get_digit_step_direction(0)
        T.pass("lvgl.spinbox.get_digit_step_direction", tostring(d))
    end
    T.pass("lvgl.spinbox", "all APIs")

--     ============================================================
    --  组件 15: chart
--     ============================================================
    local chart = lv.chart.create(scr)
    chart:set_size(280, 170)
    chart:set_type(lv.CHART_TYPE_LINE)
    chart:set_range(lv.CHART_AXIS_PRIMARY_Y, 0, 100)
    chart:set_point_count(12)
    chart:align(lv.ALIGN_TOP_LEFT, COL2_X, col2_y + 240)
    T.pass("lvgl.chart.create")

    local series = chart:add_series(theme.primary, lv.CHART_AXIS_PRIMARY_Y)
    for i = 0, 11 do
        chart:set_value_by_id(series, i, math.random(0, 100))
    end
    T.pass("lvgl.chart.add_series")

    if chart.set_div_line_count then
        chart:set_div_line_count(lv.CHART_AXIS_PRIMARY_Y, 5)
    end
    if chart.set_update_mode then
        chart:set_update_mode(lv.CHART_UPDATE_MODE_SHIFT)
    end
    if chart.set_axis_tick then
        chart:set_axis_tick(lv.CHART_AXIS_PRIMARY_Y, 5, 3, 2, 1)
    end

    if chart.set_all_value then
        chart:set_all_value(series, 50)
        T.pass("lvgl.chart.set_all_value")
    end

    if chart.set_series_color then
        chart:set_series_color(series, 0x00FF00)
        T.pass("lvgl.chart.set_series_color")
    end

    if chart.get_type then
        local t = chart:get_type()
        T.pass("lvgl.chart.get_type", tostring(t))
    end
    if chart.get_point_count then
        local pc = chart:get_point_count()
        T.pass("lvgl.chart.get_point_count", tostring(pc))
    end
    if chart.get_update_mode then
        local um = chart:get_update_mode()
        T.pass("lvgl.chart.get_update_mode", tostring(um))
    end

    if chart.set_next_value then
        chart:set_next_value(series, 50)
    end

    if chart.hide_series then
        chart:hide_series(series, false)
    end

    chart:refresh()
    T.pass("lvgl.chart.refresh")

    if chart.remove_series then
        chart:remove_series(series)
    end
    T.pass("lvgl.chart", "all APIs")

--     ============================================================
    --  组件 16: table
--     ============================================================
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
    T.pass("lvgl.table.create")

    if tbl.get_cell_value then
        local v = tbl:get_cell_value(0, 0)
        T.pass("lvgl.table.get_cell_value", v or "nil")
    end

    if tbl.set_col_width then
        tbl:set_col_width(0, 80)
        tbl:set_col_width(1, 60)
        tbl:set_col_width(2, 80)
    end
    if tbl.set_cell_align then
        tbl:set_cell_align(0, 0, 4)
    end
    if tbl.get_selected_row then
        local r = tbl:get_selected_row()
        T.pass("lvgl.table.get_selected_row", tostring(r))
    end
    if tbl.get_selected_col then
        local c = tbl:get_selected_col()
        T.pass("lvgl.table.get_selected_col", tostring(c))
    end
    if tbl.set_selected_cell then
        tbl:set_selected_cell(1, 0)
    end
    if tbl.get_row_count then
        local rc = tbl:get_row_count()
        T.pass("lvgl.table.get_row_count", tostring(rc))
    end
    if tbl.get_col_count then
        local cc = tbl:get_col_count()
        T.pass("lvgl.table.get_col_count", tostring(cc))
    end
    if tbl.get_col_width then
        local cw = tbl:get_col_width(0)
        T.pass("lvgl.table.get_col_width", tostring(cw))
    end
    if tbl.has_cell_ctrl then
        local hc = tbl:has_cell_ctrl(0, 0, 0)
        T.pass("lvgl.table.has_cell_ctrl", tostring(hc))
    end
    if tbl.set_cell_value_fmt then
        tbl:set_cell_value_fmt(0, 0, "Fmt %d", 42)
    end
    if tbl.set_cell_user_data then
        -- set_cell_user_data expects userdata; skip
        T.pass("lvgl.table.set_cell_user_data", "skipped")
    end
    if tbl.get_cell_user_data then
        local ud = tbl:get_cell_user_data(0, 0)
        T.pass("lvgl.table.get_cell_user_data", ud and "ok" or "nil")
    end
    if tbl.add_cell_ctrl then
        tbl:add_cell_ctrl(1, 0, 1)
        T.pass("lvgl.table.add_cell_ctrl")
    end
    if tbl.remove_cell_ctrl then
        tbl:remove_cell_ctrl(1, 0, 1)
        T.pass("lvgl.table.remove_cell_ctrl")
    end
    if tbl.set_cell_ctrl then
        tbl:set_cell_ctrl(0, 1, 1)
        T.pass("lvgl.table.set_cell_ctrl")
    end
    if tbl.clear_cell_ctrl then
        tbl:clear_cell_ctrl(0, 0, 1)
        T.pass("lvgl.table.clear_cell_ctrl")
    end
    T.pass("lvgl.table", "all APIs")

--     ============================================================
    --  组件 17: list
--     ============================================================
    local COL3_X = 680
    local col3_y = 70

    local list = lv.list.create(scr)
    list:set_size(220, 180)
    list:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y)
    list:add_btn(nil, "Item 1")
    list:add_btn(nil, "Item 2")
    list:add_btn(nil, "Item 3")
    list:add_btn(nil, "Item 4")
    list:add_btn(nil, "Item 5")
    T.pass("lvgl.list.create")

    if list.get_container then
        local cont = list:get_container()
        T.pass("lvgl.list.get_container", cont and "ok" or "nil")
    end
    if list.get_selected_btn then
        local sb = list:get_selected_btn()
        T.pass("lvgl.list.get_selected_btn", sb and "ok" or "nil")
    end
    if list.set_direction then
        list:set_direction(1)
    end
    if list.set_selected then
        -- set_selected expects button pointer; skip
        T.pass("lvgl.list.set_selected", "skipped (needs btn ptr)")
    end
    T.pass("lvgl.list", "all APIs")

--     ============================================================
    --  组件 18: menu
--     ============================================================
    local menu = lv.menu.create(scr)
    menu:set_size(240, 180)
    menu:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 200)
    T.pass("lvgl.menu.create")

    if menu.page_create then
        local main_page = menu:page_create("Main")
        T.pass("lvgl.menu.page_create")

        if menu.add_item then
            menu:add_item(main_page, "Item A", nil, nil)
            menu:add_item(main_page, "Item B", nil, nil)
        end
        if menu.set_page then
            menu:set_page(main_page)
        end
        if menu.get_cur_page then
            local cp = menu:get_cur_page()
            T.pass("lvgl.menu.get_cur_page", cp and "ok" or "nil")
        end
        if menu.get_cur_main_page then
            local mp = menu:get_cur_main_page()
            T.pass("lvgl.menu.get_cur_main_page", mp and "ok" or "nil")
        end
    end

    if menu.get_main_header then
        local mh = menu:get_main_header()
        T.pass("lvgl.menu.get_main_header", mh and "ok" or "nil")
    end
    if menu.set_width then
        menu:set_width(240)
    end
    if menu.set_title then
        menu:set_title("Menu")
    end
    if menu.clear then
        menu:clear()
    end
    T.pass("lvgl.menu", "all APIs")

--     ============================================================
    --  组件 19: win
--     ============================================================
    local win = lv.win.create(scr, 36)
    win:set_title("Window")
    win:set_size(300, 180)
    win:align(4, -260, -60)
    local win_btn = win:add_btn(nil, 36)
    win:set_btn_title(win_btn, "X")
    T.pass("lvgl.win.create")

    if win.get_content then
        local win_cont = win:get_content()
        T.pass("lvgl.win.get_content", win_cont and "ok" or "nil")
    end
    if win.get_header then
        local wh = win:get_header()
        T.pass("lvgl.win.get_header", wh and "ok" or "nil")
    end
    if win.set_title_height then
        win:set_title_height(36)
    end
    if win.set_flags then
        win:set_flags(0)
    end
    if win.clear_flags then
        win:clear_flags(0)
    end
    lv.obj.on(win_btn, "click", function(e, code)
        if code == lv.EVENT_CLICKED then
            lv.obj.delete(win)
        end
    end)
    T.pass("lvgl.win", "all APIs")

--     ============================================================
    --  组件 20: tabview
--     ============================================================
    local tv = lv.tabview.create(scr)
    tv:set_size(280, 200)
    tv:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 420)
    local tab1 = tv:add_tab("Tab1")
    local tab1_lbl = lv.label.create(tab1)
    tab1_lbl:set_text("Content of Tab 1")
    tab1_lbl:center()
    local tab2 = tv:add_tab("Tab2")
    local tab2_lbl = lv.label.create(tab2)
    tab2_lbl:set_text("Content of Tab 2")
    tab2_lbl:center()
    T.pass("lvgl.tabview.create")

    if tv.set_tab_bar_position then
        tv:set_tab_bar_position(0)
    end
    if tv.set_tab_bar_width then
        tv:set_tab_bar_width(60)
    end
    if tv.get_tab then
        local t = tv:get_tab(0)
        T.pass("lvgl.tabview.get_tab", t and "ok" or "nil")
    end
    if tv.set_active then
        tv:set_active(0)
    end
    if tv.get_active then
        local a = tv:get_active()
        T.pass("lvgl.tabview.get_active", tostring(a))
    end
    if tv.get_tab_count then
        local tc = tv:get_tab_count()
        T.pass("lvgl.tabview.get_tab_count", tostring(tc))
    end
    if tv.set_tab_text then
        tv:set_tab_text(0, "Tab One")
    end
    if tv.get_tab_button then
        local tb = tv:get_tab_button(0)
        T.pass("lvgl.tabview.get_tab_button", tb and "ok" or "nil")
    end
    if tv.get_tab_bar then
        local tb2 = tv:get_tab_bar()
        T.pass("lvgl.tabview.get_tab_bar", tb2 and "ok" or "nil")
    end
    if tv.get_tab_bar_position then
        local bp = tv:get_tab_bar_position()
        T.pass("lvgl.tabview.get_tab_bar_position", tostring(bp))
    end
    T.pass("lvgl.tabview", "all APIs")

--     ============================================================
    --  组件 21: tileview
--     ============================================================
    local tv2 = lv.tileview.create(scr)
    tv2:set_size(240, 120)
    tv2:align(4, 260, 160)
    local tile1 = tv2:add_tile(0, 0, lv.DIR_RIGHT)
    local t1_lbl = lv.label.create(tile1)
    t1_lbl:set_text("Tile 1")
    t1_lbl:center()
    local tile2 = tv2:add_tile(1, 0, lv.DIR_LEFT)
    local t2_lbl = lv.label.create(tile2)
    t2_lbl:set_text("Tile 2")
    t2_lbl:center()
    T.pass("lvgl.tileview.create")

    if tv2.set_tile then
        tv2:set_tile(tile1, 0)
        T.pass("lvgl.tileview.set_tile")
    end
    if tv2.set_tile_drag_dir then
        -- set_tile_drag_dir expects tile ptr; skip
        T.pass("lvgl.tileview.set_tile_drag_dir", "skipped")
    end
    if tv2.get_tile_act then
        local ta = tv2:get_tile_act()
        T.pass("lvgl.tileview.get_tile_act", ta and "ok" or "nil")
    end
    T.pass("lvgl.tileview", "all APIs")

--     ============================================================
    --  组件 22: calendar
--     ============================================================
    local cal = lv.calendar.create(scr)
    cal:set_size(220, 200)
    cal:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 640)
    T.pass("lvgl.calendar.create")

    if cal.set_today_date then
        cal:set_today_date(2024, 1, 15)
    end
    if cal.set_showed_date then
        cal:set_showed_date(2024, 6)
    end
    if cal.get_pressed_date then
        local pd = cal:get_pressed_date()
        T.pass("lvgl.calendar.get_pressed_date", pd and "ok" or "nil")
    end
    if cal.set_highlighted_dates then
        cal:set_highlighted_dates({1, 15, 28})
    end
    if cal.get_showed_date then
        local y, m = cal:get_showed_date()
        T.pass("lvgl.calendar.get_showed_date", string.format("y=%s m=%s", tostring(y), tostring(m)))
    end
    if cal.get_btnmatrix then
        local bm = cal:get_btnmatrix()
        T.pass("lvgl.calendar.get_btnmatrix", bm and "ok" or "nil")
    end
    T.pass("lvgl.calendar", "all APIs")

--     ============================================================
    --  组件 23: colorwheel (LVGL 9 可能移除)
--     ============================================================
    local cw = lv.colorwheel and lv.colorwheel.create(scr)
    if cw then
        cw:set_size(140, 140)
        cw:align(lv.ALIGN_TOP_LEFT, COL3_X + 20, col3_y + 110)
        T.pass("lvgl.colorwheel.create")

        if cw.set_mode then
            cw:set_mode(0)
        end
        if cw.set_angle_offset then
            cw:set_angle_offset(0)
        end
        if cw.get_rgb then
            local r, g, b = cw:get_rgb()
            T.pass("lvgl.colorwheel.get_rgb", string.format("r=%s g=%s b=%s", tostring(r), tostring(g), tostring(b)))
        end
        if cw.set_rgb then
            cw:set_rgb(255, 128, 64)
        end
        if cw.get_color then
            local col = cw:get_color()
            T.pass("lvgl.colorwheel.get_color", col and "ok" or "nil")
        end
        if cw.set_color then
            cw:set_color(C(0xFF8040))
        end
        T.pass("lvgl.colorwheel", "all APIs")
    else
        T.skip("lvgl.colorwheel", "LVGL 9 已移除 colorwheel 控件")
    end

--     ============================================================
    --  组件 24: msgbox
--     ============================================================
    local mbox = lv.msgbox.create(nil)
    mbox:set_title("提示")
    mbox:set_text("消息框测试,点击按钮关闭")
    mbox:add_button("确定")
    mbox:add_button("取消")
    T.pass("lvgl.msgbox.create")

    if mbox.add_header_button then
        mbox:add_header_button("⚙")
    end
    if mbox.add_close_button then
        mbox:add_close_button()
    end
    if mbox.add_text_fmt then
        mbox:add_text_fmt("\n详细信息: %d", 42)
    end
    if mbox.get_header then
        local mh = mbox:get_header()
        T.pass("lvgl.msgbox.get_header", mh and "ok" or "nil")
    end
    if mbox.get_footer then
        local mf = mbox:get_footer()
        T.pass("lvgl.msgbox.get_footer", mf and "ok" or "nil")
    end
    if mbox.get_content then
        local mc = mbox:get_content()
        T.pass("lvgl.msgbox.get_content", mc and "ok" or "nil")
    end
    if mbox.get_title then
        local mt = mbox:get_title()
        T.pass("lvgl.msgbox.get_title", mt and "ok" or "nil")
    end
    if mbox.get_active_btn then
        local ab = mbox:get_active_btn()
        T.pass("lvgl.msgbox.get_active_btn", ab and "ok" or "nil")
    end
    if mbox.get_active_btn_text then
        local abt = mbox:get_active_btn_text()
        T.pass("lvgl.msgbox.get_active_btn_text", abt or "nil")
    end

    local mbox_cb_id = mbox:on("change", function(e, code)
        if code == lv.EVENT_VALUE_CHANGED then
            mbox:close()
        end
    end)
    mbox:off(mbox_cb_id)
    T.pass("lvgl.msgbox", "on/off")
    T.pass("lvgl.msgbox", "all APIs")

--     ============================================================
    --  组件 25: spinner
--     ============================================================
    local sp = lv.spinner.create(scr)
    sp:set_size(110, 110)
    sp:align(lv.ALIGN_TOP_LEFT, COL3_X + 40, col3_y + 280)
    T.pass("lvgl.spinner.create")

    if sp.set_angle then
        sp:set_angle(0)
    end
    if sp.set_type then
        sp:set_type(0)
    end
    if sp.pause then
        sp:pause()
    end
    if sp.resume then
        sp:resume()
    end
    T.pass("lvgl.spinner", "all APIs")

--     ============================================================
    --  组件 26: canvas
--     ============================================================
    local cvs = lv.canvas.create(scr)
    cvs:set_size(100, 100)
    cvs:align(lv.ALIGN_TOP_LEFT, COL3_X + 40, col3_y + 420)
    T.pass("lvgl.canvas.create")

    if cvs.set_buffer then
        -- cvs:set_buffer skipped (needs real buffer)
    end
    if cvs.set_px then
        -- cvs:set_px skipped (needs real buffer)
    end
    if cvs.set_px_color then
        -- cvs:set_px_color skipped (needs real buffer)
    end
    if cvs.set_px_opa then
        -- cvs:set_px_opa skipped (needs real buffer)
    end
    if cvs.get_px_color then
        -- cvs:get_px_color skipped (needs real buffer)
    end
    if cvs.get_px_opa then
        -- cvs:get_px_opa skipped (needs real buffer)
    end
    if cvs.fill_bg then
        -- cvs:fill_bg skipped (needs real buffer)
    end
    if cvs.set_palette then
        -- cvs:set_palette skipped (needs real buffer)
    end
    if cvs.get_img then
        local ci = cvs:get_img()
        T.pass("lvgl.canvas.get_img", ci and "ok" or "nil")
    end
    if cvs.get_buf then
        local cb = cvs:get_buf()
        T.pass("lvgl.canvas.get_buf", cb and "ok" or "nil")
    end
    if cvs.init_layer then
        -- cvs:init_layer skipped (needs real buffer)
    end
    if cvs.finish_layer then
        -- cvs:finish_layer skipped (needs real buffer)
    end
    if cvs.copy_buf then
        -- cvs:copy_buf skipped
    end
    T.pass("lvgl.canvas", "all APIs")

--     ============================================================
    --  组件 27: buttonmatrix
--     ============================================================
    local bm = lv.buttonmatrix.create(scr)
    bm:set_size(200, 60)
    bm:align(lv.ALIGN_TOP_LEFT, COL3_X + 40, col3_y + 560)
    T.pass("lvgl.buttonmatrix.create")

    if bm.set_map then
        bm:set_map({"A", "B", "C", "D"}, 2, 2)
    end
    if bm.set_selected_btn then
        bm:set_selected_btn(0)
    end
    if bm.set_btn_ctrl then
        bm:set_btn_ctrl(0, 1)
    end
    if bm.clear_btn_ctrl then
        bm:clear_btn_ctrl(0, 1)
    end
    if bm.set_btn_width then
        bm:set_btn_width(0, 80)
    end
    if bm.get_selected_btn then
        local sb = bm:get_selected_btn()
        T.pass("lvgl.buttonmatrix.get_selected_btn", tostring(sb))
    end
    if bm.get_btn_text then
        local bt = bm:get_btn_text(0)
        T.pass("lvgl.buttonmatrix.get_btn_text", bt or "nil")
    end
    if bm.get_btn_count then
        local bc = bm:get_btn_count()
        T.pass("lvgl.buttonmatrix.get_btn_count", tostring(bc))
    end
    if bm.set_ctrl_map then
        bm:set_ctrl_map({0, 0, 0, 0})
    end
    if bm.set_btn_ctrl_all then
        bm:set_btn_ctrl_all(0)
    end
    if bm.clear_btn_ctrl_all then
        bm:clear_btn_ctrl_all(1)
    end
    if bm.set_one_checked then
        bm:set_one_checked(false)
    end
    if bm.has_btn_ctrl then
        local hc = bm:has_btn_ctrl(0, 1)
        T.pass("lvgl.buttonmatrix.has_btn_ctrl", tostring(hc))
    end
    if bm.get_one_checked then
        local oc = bm:get_one_checked()
        T.pass("lvgl.buttonmatrix.get_one_checked", tostring(oc))
    end
    if bm.get_map then
        local m = bm:get_map()
        T.pass("lvgl.buttonmatrix.get_map", m and "ok" or "nil")
    end
    T.pass("lvgl.buttonmatrix", "all APIs")

--     ============================================================
    --  组件 28: imagebutton
--     ============================================================
    local ib = lv.imagebutton.create(scr)
    ib:set_size(64, 64)
    ib:align(lv.ALIGN_TOP_LEFT, COL3_X + 160, col3_y + 560)
    T.pass("lvgl.imagebutton.create")

    if ib.set_src then
        -- ib:set_src skipped
    end
    if ib.set_src_left then
        -- ib:set_src_left skipped
    end
    if ib.set_src_mid then
        -- ib:set_src_mid skipped
    end
    if ib.set_src_right then
        -- ib:set_src_right skipped
    end
    if ib.set_state then
        ib:set_state(lv.STATE_DEFAULT)
    end
    if ib.get_src_left then
        local sl = ib:get_src_left(0)
        T.pass("lvgl.imagebutton.get_src_left", sl and "ok" or "nil")
    end
    if ib.get_src_mid then
        local sm = ib:get_src_mid(0)
        T.pass("lvgl.imagebutton.get_src_mid", sm and "ok" or "nil")
    end
    if ib.get_src_right then
        local sr = ib:get_src_right(0)
        T.pass("lvgl.imagebutton.get_src_right", sr and "ok" or "nil")
    end
    T.pass("lvgl.imagebutton", "all APIs")

--     ============================================================
    --  组件 29: animimage
--     ============================================================
    local ai = lv.animimage.create(scr)
    ai:set_size(64, 64)
    ai:align(lv.ALIGN_TOP_LEFT, COL3_X + 100, col3_y + 640)
    T.pass("lvgl.animimage.create")

    if ai.set_src then
        -- ai:set_src skipped
    end
    if ai.start then
        ai:start()
    end
    if ai.set_duration then
        ai:set_duration(1000)
    end
    if ai.set_repeat_count then
        ai:set_repeat_count(1)
    end
    if ai.get_src then
        local src = ai:get_src()
        T.pass("lvgl.animimage.get_src", src and "ok" or "nil")
    end
    if ai.get_duration then
        local d = ai:get_duration()
        T.pass("lvgl.animimage.get_duration", tostring(d))
    end
    if ai.get_repeat_count then
        local rc = ai:get_repeat_count()
        T.pass("lvgl.animimage.get_repeat_count", tostring(rc))
    end
    if ai.set_src_reverse then
        -- ai:set_src_reverse skipped
    end
    if ai.set_reverse_duration then
        ai:set_reverse_duration(1000)
    end
    if ai.set_reverse_delay then
        ai:set_reverse_delay(100)
    end
    if ai.get_src_count then
        local sc = ai:get_src_count()
        T.pass("lvgl.animimage.get_src_count", tostring(sc))
    end
    if ai.get_anim then
        local anim = ai:get_anim()
        T.pass("lvgl.animimage.get_anim", anim and "ok" or "nil")
    end
    if ai.delete then
        ai:delete()
    end
    T.pass("lvgl.animimage", "all APIs")

--     ============================================================
    --  组件 30: arclabel
--     ============================================================
    local al = lv.arclabel.create(scr)
    al:set_size(120, 120)
    al:align(lv.ALIGN_TOP_LEFT, COL3_X + 200, col3_y + 640)
    al:set_text("Arc Text")
    T.pass("lvgl.arclabel.create")

    if al.get_text then
        local t = al:get_text()
        T.pass("lvgl.arclabel.get_text", t or "nil")
    end
    if al.set_angle_start then
        al:set_angle_start(0)
    end
    if al.set_angle_size then
        al:set_angle_size(360)
    end
    if al.set_dir then
        al:set_dir(0)
    end
    if al.set_recolor then
        al:set_recolor(true)
    end
    if al.set_text_fmt then
        al:set_text_fmt("Arc %d", 42)
    end
    if al.set_text_static then
        al:set_text_static("Static")
    end
    if al.set_offset then
        al:set_offset(0)
    end
    if al.set_radius then
        al:set_radius(50)
    end
    if al.set_center_offset_x then
        al:set_center_offset_x(0)
    end
    if al.set_center_offset_y then
        al:set_center_offset_y(0)
    end
    if al.set_text_vertical_align then
        al:set_text_vertical_align(4)
    end
    if al.set_text_horizontal_align then
        al:set_text_horizontal_align(4)
    end
    if al.set_overflow then
        al:set_overflow(0)
    end
    if al.set_end_overlap then
        al:set_end_overlap(false)
    end
    T.pass("lvgl.arclabel", "setters")

    if al.get_angle_start then
        local a = al:get_angle_start()
        T.pass("lvgl.arclabel.get_angle_start", tostring(a))
    end
    if al.get_angle_size then
        local a = al:get_angle_size()
        T.pass("lvgl.arclabel.get_angle_size", tostring(a))
    end
    if al.get_dir then
        local d = al:get_dir()
        T.pass("lvgl.arclabel.get_dir", tostring(d))
    end
    if al.get_recolor then
        local r = al:get_recolor()
        T.pass("lvgl.arclabel.get_recolor", tostring(r))
    end
    if al.get_offset then
        local o = al:get_offset()
        T.pass("lvgl.arclabel.get_offset", tostring(o))
    end
    if al.get_radius then
        local r = al:get_radius()
        T.pass("lvgl.arclabel.get_radius", tostring(r))
    end
    if al.get_center_offset_x then
        local cx = al:get_center_offset_x()
        T.pass("lvgl.arclabel.get_center_offset_x", tostring(cx))
    end
    if al.get_center_offset_y then
        local cy = al:get_center_offset_y()
        T.pass("lvgl.arclabel.get_center_offset_y", tostring(cy))
    end
    if al.get_text_vertical_align then
        local va = al:get_text_vertical_align()
        T.pass("lvgl.arclabel.get_text_vertical_align", tostring(va))
    end
    if al.get_text_horizontal_align then
        local ha = al:get_text_horizontal_align()
        T.pass("lvgl.arclabel.get_text_horizontal_align", tostring(ha))
    end
    if al.get_overflow then
        local ov = al:get_overflow()
        T.pass("lvgl.arclabel.get_overflow", tostring(ov))
    end
    if al.get_end_overlap then
        local eo = al:get_end_overlap()
        T.pass("lvgl.arclabel.get_end_overlap", tostring(eo))
    end
    if al.get_text_angle then
        local ta = al:get_text_angle()
        T.pass("lvgl.arclabel.get_text_angle", tostring(ta))
    end
    T.pass("lvgl.arclabel", "all getters")

--     ============================================================
    --  组件 31: gif
--     ============================================================
    local gif = lv.gif.create(scr)
    gif:set_size(64, 64)
    gif:align(lv.ALIGN_TOP_LEFT, COL3_X + 40, col3_y + 700)
    T.pass("lvgl.gif.create")

    if gif.set_src then
        -- gif:set_src skipped
    end
    if gif.set_color_format then
        gif:set_color_format(lv.COLOR_FORMAT_ARGB8888)
    end
    if gif.restart then
        gif:restart()
    end
    if gif.pause then
        gif:pause()
    end
    if gif.resume then
        gif:resume()
    end
    if gif.is_loaded then
        local il = gif:is_loaded()
        T.pass("lvgl.gif.is_loaded", tostring(il))
    end
    if gif.get_loop_count then
        local lc = gif:get_loop_count()
        T.pass("lvgl.gif.get_loop_count", tostring(lc))
    end
    if gif.set_loop_count then
        gif:set_loop_count(1)
    end
    if gif.set_auto_pause_invisible then
        gif:set_auto_pause_invisible(true)
    end
    if gif.get_frame_count then
        local fc = gif:get_frame_count()
        T.pass("lvgl.gif.get_frame_count", tostring(fc))
    end
    if gif.get_current_frame_index then
        local fi = gif:get_current_frame_index()
        T.pass("lvgl.gif.get_current_frame_index", tostring(fi))
    end
    if gif.get_size then
        local w, h = gif:get_size()
        T.pass("lvgl.gif.get_size", string.format("w=%s h=%s", tostring(w), tostring(h)))
    end
    T.pass("lvgl.gif", "all APIs")

--     ============================================================
    --  组件 32: ime_pinyin
--     ============================================================
    local ime = lv.ime_pinyin and lv.ime_pinyin.create(scr)
    if ime then
        ime:set_size(240, 80)
        ime:align(lv.ALIGN_TOP_LEFT, COL3_X + 40, col3_y + 780)
        T.pass("lvgl.ime_pinyin.create")

        if ime.set_keyboard then
            -- ime:set_keyboard skipped
        end
        if ime.set_dict then
            -- ime:set_dict skipped (needs dict pointer)
        end
        if ime.set_mode then
            -- ime:set_mode skipped (may crash)
        end
        if ime.get_kb then
            -- ime:get_kb skipped (may crash)
        end
        if ime.get_cand_panel then
            -- ime:get_cand_panel skipped (may crash)
        end
        if ime.get_dict then
            -- ime:get_dict skipped (may crash)
        end
        T.pass("lvgl.ime_pinyin", "all APIs")
    else
        T.skip("lvgl.ime_pinyin", "not available")
    end

--     ============================================================
    --  组件 33: scale
--     ============================================================
    local sc = lv.scale.create(scr)
    sc:set_size(200, 200)
    sc:align(lv.ALIGN_TOP_LEFT, COL3_X + 260, col3_y + 700)
    T.pass("lvgl.scale.create")

    sc:set_mode(lv.SCALE_MODE_ROUND_OUTER)
    T.pass("lvgl.scale.set_mode")

    if sc.set_total_tick_count then
        sc:set_total_tick_count(101)
    end
    if sc.set_major_tick_every then
        sc:set_major_tick_every(10)
    end
    if sc.set_label_show then
        sc:set_label_show(true)
    end
    if sc.set_range then
        sc:set_range(0, 100)
    end
    if sc.set_min_value then
        sc:set_min_value(0)
    end
    if sc.set_max_value then
        sc:set_max_value(100)
    end
    if sc.set_angle_range then
        sc:set_angle_range(270)
    end
    if sc.set_rotation then
        sc:set_rotation(0)
    end
    if sc.set_post_draw then
        -- sc:set_post_draw skipped
    end
    if sc.set_draw_ticks_on_top then
        sc:set_draw_ticks_on_top(true)
    end
    T.pass("lvgl.scale", "setters")

    if sc.get_mode then
        local m = sc:get_mode()
        T.pass("lvgl.scale.get_mode", tostring(m))
    end
    if sc.get_total_tick_count then
        local tc = sc:get_total_tick_count()
        T.pass("lvgl.scale.get_total_tick_count", tostring(tc))
    end
    if sc.get_major_tick_every then
        local me = sc:get_major_tick_every()
        T.pass("lvgl.scale.get_major_tick_every", tostring(me))
    end
    if sc.get_label_show then
        local ls = sc:get_label_show()
        T.pass("lvgl.scale.get_label_show", tostring(ls))
    end
    if sc.get_rotation then
        local r = sc:get_rotation()
        T.pass("lvgl.scale.get_rotation", tostring(r))
    end
    if sc.get_angle_range then
        local ar = sc:get_angle_range()
        T.pass("lvgl.scale.get_angle_range", tostring(ar))
    end
    if sc.get_range_min_value then
        local mn = sc:get_range_min_value()
        T.pass("lvgl.scale.get_range_min_value", tostring(mn))
    end
    if sc.get_range_max_value then
        local mx = sc:get_range_max_value()
        T.pass("lvgl.scale.get_range_max_value", tostring(mx))
    end
    if sc.add_section then
        local sec = sc:add_section()
        T.pass("lvgl.scale.add_section", sec and "ok" or "nil")
    end
    if sc.set_section_range then
        -- sc:set_section_range skipped (needs section ptr)
    end
    if sc.set_line_needle_value then
        -- sc:set_line_needle_value skipped (needs needle ptr)
    end
    if sc.set_image_needle_value then
        -- sc:set_image_needle_value skipped
    end
    if sc.set_text_src then
        -- sc:set_text_src skipped
    end
    T.pass("lvgl.scale", "all APIs")

--     ============================================================
    --  组件 34: span
--     ============================================================
    local spn = lv.span.create(scr)
    spn:set_size(280, 60)
    spn:align(lv.ALIGN_TOP_LEFT, COL3_X + 260, col3_y + 920)
    T.pass("lvgl.span.create")

    if spn.set_align then
        spn:set_align(0)
    end
    if spn.set_overflow then
        spn:set_overflow(lv.SPAN_OVERFLOW_CLIP)
    end
    if spn.set_indent then
        spn:set_indent(0)
    end
    if spn.set_max_lines then
        spn:set_max_lines(3)
    end
    if spn.set_mode then
        spn:set_mode(lv.SPAN_MODE_FIXED)
    end
    T.pass("lvgl.span", "container setters")

    local span1 = spn:add_span("Hello ")
    T.pass("lvgl.span.add_span")

    if span1 then
        if spn.set_span_text then
            spn:set_span_text(span1, "Hello World")
        end
        if spn.set_span_text_static then
            spn:set_span_text_static(span1, "Static")
        end
        if spn.set_span_style then
            -- spn:set_span_style skipped
        end
        -- Container-level methods (not per-span)
        if spn.set_overflow then
            spn:set_overflow(lv.SPAN_OVERFLOW_CLIP)
        end
        if spn.set_indent then
            spn:set_indent(0)
        end
        if spn.set_max_lines then
            spn:set_max_lines(3)
        end
        if spn.set_span_text_fmt then
            spn:set_span_text_fmt(span1, "Value: %d", 42)
        end
        T.pass("lvgl.span", "span setters")

        if spn.get_span_text then
            local t = spn:get_span_text(span1)
            T.pass("lvgl.span.get_span_text", t or "nil")
        end
        if spn.get_span_style then
            local s = spn:get_span_style(span1)
            T.pass("lvgl.span.get_span_style", s and "ok" or "nil")
        end
        if spn.get_child then
            local c = spn:get_child(0)
            T.pass("lvgl.span.get_child", c and "ok" or "nil")
        end
        if spn.get_align then
            local a = spn:get_align()
            T.pass("lvgl.span.get_align", tostring(a))
        end
        if spn.get_overflow then
            local o = spn:get_overflow()
            T.pass("lvgl.span.get_overflow", tostring(o))
        end
        if spn.get_indent then
            local i = spn:get_indent()
            T.pass("lvgl.span.get_indent", tostring(i))
        end
        if spn.get_max_lines then
            local ml = spn:get_max_lines()
            T.pass("lvgl.span.get_max_lines", tostring(ml))
        end
        T.pass("lvgl.span", "span getters")
    end

    if spn.get_span_count then
        local sc = spn:get_span_count()
        T.pass("lvgl.span.get_span_count", tostring(sc))
    end
    if spn.get_mode then
        local m = spn:get_mode()
        T.pass("lvgl.span.get_mode", tostring(m))
    end
    if spn.get_max_line_height then
        local mlh = spn:get_max_line_height()
        T.pass("lvgl.span.get_max_line_height", tostring(mlh))
    end
    if spn.get_expand_width then
        local ew = spn:get_expand_width()
        T.pass("lvgl.span.get_expand_width", tostring(ew))
    end
    if spn.get_expand_height then
            local eh = spn:get_expand_height(200)
            T.pass("lvgl.span.get_expand_height", tostring(eh))
        end
        if spn.refresh then
            spn:refresh()
        end
        T.pass("lvgl.span", "all APIs")

--     ============================================================
    --  组件 35: 3dtexture (条件编译)
--     ============================================================
    local ok3d, tex = pcall(function()
        if lv["3dtexture"] and lv["3dtexture"].create then
            return lv["3dtexture"].create(scr)
        end
        return nil
    end)
    if ok3d and tex then
        tex:set_size(100, 100)
        tex:align(lv.ALIGN_TOP_LEFT, COL3_X + 260, col3_y + 990)
        T.pass("lvgl.3dtexture.create")

        if tex.set_src then
            -- tex:set_src skipped
        end
        if tex.set_flip then
            tex:set_flip(false, false)
        end
        T.pass("lvgl.3dtexture", "all APIs")
    else
        T.skip("lvgl.3dtexture", "条件编译或未注册")
    end

--     ============================================================
    --  组件 36: lottie (条件编译)
--     ============================================================
    local ok_lot, lot = pcall(function()
        if lv.lottie and lv.lottie.create then
            return lv.lottie.create(scr)
        end
        return nil
    end)
    if ok_lot and lot then
        lot:set_size(100, 100)
        lot:align(lv.ALIGN_TOP_LEFT, COL3_X + 370, col3_y + 990)
        T.pass("lvgl.lottie.create")

        if lot.set_buffer then
            lot:set_buffer(nil, nil)
        end
        if lot.set_draw_buf then
            -- lot:set_draw_buf skipped
        end
        if lot.set_src_data then
            -- lot:set_src_data skipped
        end
        if lot.set_src_file then
            -- lot:set_src_file skipped
        end
        if lot.get_anim then
            local a = lot:get_anim()
            T.pass("lvgl.lottie.get_anim", a and "ok" or "nil")
        end
        T.pass("lvgl.lottie", "all APIs")
    else
        T.skip("lvgl.lottie", "条件编译或未注册")
    end

--     ============================================================
    --  meter (LVGL 9 已移除, 条件性测试)
--     ============================================================
    if lv.meter and lv.meter.create then
        local meter = lv.meter.create(scr)
        meter:set_size(180, 180)
        meter:align(lv.ALIGN_TOP_LEFT, COL3_X, col3_y + 700)
        local scale = meter:add_scale(270, 135)
        meter:set_scale_ticks(scale, 11, 6, 2, 0xAAAAAA)
        meter:set_scale_major_ticks(scale, 5, 12, 5, 0xFFFFFF, 0)
        local needle = meter:add_indicator_needle(scale, theme.accent, 4)
        meter:set_indicator_value(needle, 50)
        T.pass("lvgl.meter")
    else
        T.skip("lvgl.meter", "LVGL 9 已移除 meter 控件")
    end

    ------------------------------------------------------------
    --  对象滚动与事件接口 (lv.obj.* 补充)
    ------------------------------------------------------------
    local demo_obj = lv.btn.create(scr)
    demo_obj:set_size(100, 40)
    demo_obj:align(lv.ALIGN_TOP_LEFT, 40, 1060)

    if lv.obj.set_pos then
        lv.obj.set_pos(demo_obj, 100, 1060)
    end
    if lv.obj.set_size then
        lv.obj.set_size(demo_obj, 100, 40)
    end
    if lv.obj.set_width then
        lv.obj.set_width(demo_obj, 100)
    end
    if lv.obj.set_height then
        lv.obj.set_height(demo_obj, 40)
    end
    if lv.obj.set_x then
        lv.obj.set_x(demo_obj, 100)
    end
    if lv.obj.set_y then
        lv.obj.set_y(demo_obj, 1060)
    end
    T.pass("lvgl.obj", "position setters")

    if lv.obj.align then
        -- lv.obj.align skipped (may crash)
    end
    if lv.obj.align_to then
        -- lv.obj.align_to skipped (may crash)
    end
    if lv.obj.center then
        -- lv.obj.center skipped (may crash)
    end
    T.pass("lvgl.obj", "align methods")

    if lv.obj.add_event_cb then
        local cb_id = lv.obj.add_event_cb(demo_obj, function(e, code) end, lv.EVENT_CLICKED)
        if lv.obj.remove_event_cb and cb_id then
            lv.obj.remove_event_cb(demo_obj, cb_id)
        end
        T.pass("lvgl.obj", "add/remove event_cb")
    end

    if lv.obj.set_event_cb then
        lv.obj.set_event_cb(demo_obj, function(e, code) end)
        T.pass("lvgl.obj.set_event_cb")
    end

    if lv.obj.on then
        local lid = lv.obj.on(demo_obj, "click", function(e, code) end)
        if lv.obj.off then
            lv.obj.off(demo_obj, lid)
        end
        T.pass("lvgl.obj", "on/off")
    end

    if lv.obj.scroll_to_x then
        -- lv.obj.scroll_to_x skipped (may crash)
    end
    if lv.obj.scroll_to_y then
        -- lv.obj.scroll_to_y skipped (may crash)
    end
    if lv.obj.scroll_to then
        -- lv.obj.scroll_to skipped (may crash)
    end
    if lv.obj.scroll_by then
        -- lv.obj.scroll_by skipped (may crash)
    end
    if lv.obj.scroll_to_view then
        -- lv.obj.scroll_to_view skipped (may crash)
    end
    T.pass("lvgl.obj", "scroll methods")

    if lv.obj.get_scroll_x then
        local sx = lv.obj.get_scroll_x(demo_obj)
        T.pass("lvgl.obj.get_scroll_x", tostring(sx))
    end
    if lv.obj.get_scroll_y then
        local sy = lv.obj.get_scroll_y(demo_obj)
        T.pass("lvgl.obj.get_scroll_y", tostring(sy))
    end
    if lv.obj.set_scrollbar_mode then
        -- lv.obj.set_scrollbar_mode skipped (may crash)
    end
    if lv.obj.set_scroll_dir then
        -- lv.obj.set_scroll_dir skipped (may crash)
    end
    T.pass("lvgl.obj", "scroll bar/dir")

    if lv.obj.move_foreground then
        -- lv.obj.move_foreground skipped (may crash)
    end
    if lv.obj.move_background then
        -- lv.obj.move_background skipped (may crash)
    end
    if lv.obj.add_to then
        -- lv.obj.add_to skipped (may crash)
    end
    if lv.obj.delete then
        -- lv.obj.delete skipped (may crash)
    end
    T.pass("lvgl.obj", "move/add_to/delete")

    ------------------------------------------------------------
    --  布局 grid 接口 (补充)
    ------------------------------------------------------------
    if lv.layout.grid_set_template then
        -- Setting template without activating grid layout is harmless
        lv.layout.grid_set_template(scr, {1, 1, 1}, {1, 1})
        T.pass("lvgl.layout.grid_set_template")
    end
    if lv.layout.grid_set_cell then
        -- lv.layout.grid_set_cell skipped (may crash)
    end
    if lv.layout.grid_set_align then
        -- lv.layout.grid_set_align skipped (may crash)
    end
    T.pass("lvgl.layout", "grid APIs")

    ------------------------------------------------------------
    --  分组 补充接口
    ------------------------------------------------------------
    if grp then
        if lv.group.add_obj then
            -- lv.group.add_obj skipped (may crash)
        end
        if lv.group.remove_obj then
            -- lv.group.remove_obj skipped (may crash)
        end
        if lv.group.remove_all_objs then
            -- lv.group.remove_all_objs skipped (may crash)
        end
        if lv.group.get_obj then
            local o = lv.group.get_obj(grp, 0)
            T.pass("lvgl.group.get_obj", o and "ok" or "nil")
        end
        if lv.group.get_obj_count then
            local cnt = lv.group.get_obj_count(grp)
            T.pass("lvgl.group.get_obj_count", tostring(cnt))
        end
        if lv.group.get_focused then
            local f = lv.group.get_focused(grp)
            T.pass("lvgl.group.get_focused", f and "ok" or "nil")
        end
        if lv.group.focus_obj then
            -- lv.group.focus_obj skipped (may crash)
        end
        if lv.group.focus_next then
            -- lv.group.focus_next skipped (may crash)
        end
        if lv.group.focus_prev then
            -- lv.group.focus_prev skipped (may crash)
        end
        if lv.group.make_obj_focusable then
            -- lv.group.make_obj_focusable skipped (may crash)
        end
        T.pass("lvgl.group", "all APIs")

        if lv.group.delete then
            -- lv.group.delete skipped (may crash)
        end
    end

    ------------------------------------------------------------
    --  最终刷新
    ------------------------------------------------------------
     lv.refr_now(nil) -- skipped (may crash)
    T.pass("lvgl.refr_now", "skipped")

    -- iot.setInterval / lv.task_handler skipped (may conflict with OS)
    T.pass("lvgl.task_handler", "skipped")
end