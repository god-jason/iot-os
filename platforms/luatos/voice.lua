local voice = {}

local log = require("logging").logger("voice")

local pa, pp

function voice.init(opts)
    log.info("init")
    opts = opts or {}

    -- 默认参数
    local pa_pin = opts.pa_pin or 21 -- 喇叭pa功放脚
    local power_pin = opts.power_pin or 20 -- es8311电源脚
    local i2c_id = opts.i2c_id or 0 -- i2c_id 0
    local i2s_id = opts.i2s_id or 0 -- i2s_id 0
    local voice_vol = opts.voice_vol or 60 -- 喇叭音量
    local mic_vol = opts.mic_vol or 80 -- 麦克风音量

    pa = pa_pin
    pp = power_pin

    gpio.setup(pa_pin, 1, gpio.PULLUP) -- 设置功放PA脚
    gpio.setup(power_pin, 1, gpio.PULLUP) -- 设置ES83111电源脚

    i2c.setup(i2c_id, i2c.FAST)
    i2s.setup(i2s_id, 0, 16000, 16, i2s.MONO_R, i2s.MODE_LSB, 16)

    audio.config(0, pa_pin, 1, 3, 100, power_pin, 1, 100)
    audio.setBus(0, audio.BUS_I2S, {
        chip = "es8311",
        i2cid = i2c_id,
        i2sid = i2s_id
    }) -- 通道0的硬件输出通道设置为I2S

    audio.vol(0, voice_vol)
    audio.micVol(0, mic_vol)

    -- audio.tts(0, "初始化完成")
end

function voice.tts(text)
    log.info("tts", text)
    audio.tts(0, text)
end

function voice.play(path)
    log.info("play", path)
    audio.play(0, path)
end

function voice.stop()
    log.info("stop")
    audio.playStop(0)
end

function voice.pause()
    log.info("pause")
    audio.pause(0, true)
end

function voice.resume()
    log.info("resume")
    audio.pause(0, false)
end


function voice.is_end()
    audio.isEnd(0)
end

function voice.close()
    audio.stop(0)

    -- 关闭使用
    gpio.set(pa, 0)
    gpio.set(pp, 0)
end

return voice
