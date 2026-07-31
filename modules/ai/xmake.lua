--[[
    modules/ai/xmake.lua
    AI 模块构建配置

    模块依赖:
      - model:  需要 iotfs (文件读取), 按平台需要 tflite-micro 或 onnxruntime
      - llm:    需要 http 模块 (网络请求), json (cjson)
      - stt:    需要 http 模块, http_form (multipart), 平台音频采集 HAL
      - tts:    需要 http 模块, 平台音频播放 HAL

    编译选项:
      HAS_TFLITE_MICRO   - ESP32 平台启用 TFLite Micro
      HAS_ONNX_RUNTIME   - Linux/Windows 启用 ONNX Runtime
      HAS_EDGE_TTS       - 启用免费 Edge TTS (默认开启)

    注: 如果未链接推理库 (tflite/onnx)，model 模块可正常编译，
       但会在运行时返回 MODEL_BACKEND_NONE。
--]]

add_rules("mode.debug", "mode.release")

target("ai")
    set_kind("static")
    --add_deps("http")
    --add_deps("base")

    -- AI 模块源文件
    add_files("ai_module.c", "model.c", "llm.c", "stt.c", "tts.c")

    -- 头文件
    add_includedirs(".", {public = true})

    -- 依赖 HTTP 模块头文件
    add_includedirs("../http", {public = true})

    -- 依赖基础模块
    add_includedirs("../../core", {public = true})

    -- 依赖 net 模块 (http_client.h -> net.h)
    add_includedirs("../net", {public = true})

    -- 平台判定: ESP32 系列 -> TFLite Micro
    if is_plat("cross") then
        add_defines("PLATFORM_ESP32")
        add_defines("HAS_TFLITE_MICRO")
        -- 需要 tflite-micro 的库/头路径
        -- add_includedirs("$(SDK_DIR)/components/tflite-micro")
        -- add_links("tflite-micro")
    end

    -- 平台判定: Linux
    if is_plat("linux") then
        add_defines("PLATFORM_LINUX", "HAS_ONNX_RUNTIME")
        -- 系统包: libonnxruntime-dev
        -- add_links("onnxruntime")
    end

    -- 平台判定: Windows
    if is_plat("windows") then
        add_defines("PLATFORM_WINDOWS", "HAS_ONNX_RUNTIME")
        -- vcpkg install onnxruntime
        -- add_links("onnxruntime")
    end
