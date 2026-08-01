"""批量给 AI 模块的类型和函数名添加 iot_ 前缀"""
import re
import os

AI_DIR = r"d:\WORK\iot-os\modules\ai"

files = ["model.h", "model.c", "llm.h", "llm.c", "stt.h", "stt.c", "tts.h", "tts.c", "ai_module.c"]

def rename_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    original = content

    # =========== model 相关 ===========
    if "model" in os.path.basename(filepath) or "ai_module" in os.path.basename(filepath):
        # 类型 (typedef/struct/enum 后的名称) —— 不碰 model_handle (Lua metatable 字符串)
        content = content.replace("model_dtype_t", "iot_model_dtype_t")
        content = content.replace("model_backend_t", "iot_model_backend_t")
        content = content.replace("model_precision_t", "iot_model_precision_t")
        content = content.replace("model_tensor_info_t", "iot_model_tensor_info_t")
        content = content.replace("model_config_t", "iot_model_config_t")
        content = content.replace("model_callback_t", "iot_model_callback_t")
        content = content.replace("tf_backend_ctx_t", "iot_tf_backend_ctx_t")
        content = content.replace("model_t ", "iot_model_t ")
        content = content.replace("model_t*", "iot_model_t*")
        content = content.replace("(model_t*)", "(iot_model_t*)")
        content = content.replace("(model_t**)", "(iot_model_t**)")
        # 函数
        funcs = [
            "model_load_from_file", "model_load_from_memory", "model_free",
            "model_set_input", "model_get_input_count", "model_get_input_info",
            "model_get_output_count", "model_get_output_info", "model_get_output",
            "model_invoke", "model_clone", "model_get_config", "model_get_info_json",
            "model_backend_name", "model_get_available_backend",
        ]
        for func in funcs:
            content = content.replace(func, "iot_" + func)
        # 枚举值
        content = content.replace("MODEL_DTYPE_", "IOT_MODEL_DTYPE_")
        content = content.replace("MODEL_BACKEND_", "IOT_MODEL_BACKEND_")
        content = content.replace("MODEL_PRECISION_", "IOT_MODEL_PRECISION_")
        content = content.replace("MODEL_MAX_DIMS", "IOT_MODEL_MAX_DIMS")
        content = content.replace("MODEL_MAX_NAME", "IOT_MODEL_MAX_NAME")

    # =========== llm 相关 ===========
    if "llm" in os.path.basename(filepath) or "ai_module" in os.path.basename(filepath):
        content = content.replace("llm_message_role_t", "iot_llm_message_role_t")
        content = content.replace("llm_message_t", "iot_llm_message_t")
        content = content.replace("llm_tool_t", "iot_llm_tool_t")
        content = content.replace("llm_response_t", "iot_llm_response_t")
        content = content.replace("llm_config_t", "iot_llm_config_t")
        content = content.replace("llm_client_t", "iot_llm_client_t")
        content = content.replace("llm_conv_t", "iot_llm_conv_t")
        content = content.replace("llm_config_t*", "iot_llm_config_t*")
        content = content.replace("llm_response_t*", "iot_llm_response_t*")
        content = content.replace("llm_client_t*", "iot_llm_client_t*")
        content = content.replace("LLM_ROLE_", "IOT_LLM_ROLE_")
        funcs = [
            "llm_chunk_to_str", "llm_client_create", "llm_client_free",
            "llm_chat", "llm_ask", "llm_chat_stream", "llm_response_free",
            "llm_client_set_config", "llm_conv_new", "llm_conv_add",
            "llm_conv_free", "llm_conv_chat", "llm_conv_to_json", "llm_json_sprintf",
        ]
        for func in funcs:
            content = content.replace(func, "iot_" + func)

    # =========== stt 相关 ===========
    if "stt" in os.path.basename(filepath) or "ai_module" in os.path.basename(filepath):
        content = content.replace("stt_provider_t", "iot_stt_provider_t")
        content = content.replace("stt_audio_format_t", "iot_stt_audio_format_t")
        content = content.replace("stt_config_t", "iot_stt_config_t")
        content = content.replace("stt_result_t", "iot_stt_result_t")
        content = content.replace("stt_callback_t", "iot_stt_callback_t")
        content = content.replace("stt_vad_callback_t", "iot_stt_vad_callback_t")
        content = content.replace("stt_audio_callback_t", "iot_stt_audio_callback_t")
        content = content.replace("stt_impl_t", "iot_stt_impl_t")
        content = content.replace("stt_t ", "iot_stt_t ")
        content = content.replace("stt_t*", "iot_stt_t*")
        content = content.replace("(stt_t*)", "(iot_stt_t*)")
        content = content.replace("STT_PROVIDER_", "IOT_STT_PROVIDER_")
        content = content.replace("STT_AUDIO_", "IOT_STT_AUDIO_")
        content = content.replace("STT_JSON_BUF_SIZE", "IOT_STT_JSON_BUF_SIZE")
        content = content.replace("STT_RESP_BUF_SIZE", "IOT_STT_RESP_BUF_SIZE")
        content = content.replace("STT_AUDIO_BUF_SIZE", "IOT_STT_AUDIO_BUF_SIZE")
        content = content.replace("STT_VAD_", "IOT_STT_VAD_")
        funcs = [
            "stt_create", "stt_set_config", "stt_free", "stt_provider_name",
            "stt_recognize_file", "stt_recognize_buffer",
            "stt_start_record_and_recognize", "stt_stop_recording", "stt_is_recording",
            "stt_set_vad_callback", "stt_set_audio_callback",
            "stt_set_wake_word", "stt_start_continuous_listen", "stt_stop_continuous_listen",
            "stt_result_free", "stt_pcm_to_wav", "stt_vad_detect",
            "stt_openai_recognize", "stt_baidu_recognize", "stt_iflytek_recognize",
            "stt_do_recognize", "stt_read_file", "stt_get_api_url",
        ]
        for func in funcs:
            content = content.replace(func, "iot_" + func)

    # =========== tts 相关 ===========
    if "tts" in os.path.basename(filepath) or "ai_module" in os.path.basename(filepath):
        content = content.replace("tts_provider_t", "iot_tts_provider_t")
        content = content.replace("tts_audio_format_t", "iot_tts_audio_format_t")
        content = content.replace("tts_voice_t", "iot_tts_voice_t")
        content = content.replace("tts_config_t", "iot_tts_config_t")
        content = content.replace("tts_result_t", "iot_tts_result_t")
        content = content.replace("tts_callback_t", "iot_tts_callback_t")
        content = content.replace("tts_t ", "iot_tts_t ")
        content = content.replace("tts_t*", "iot_tts_t*")
        content = content.replace("(tts_t*)", "(iot_tts_t*)")
        content = content.replace("TTS_PROVIDER_", "IOT_TTS_PROVIDER_")
        content = content.replace("TTS_FORMAT_", "IOT_TTS_FORMAT_")
        content = content.replace("TTS_VOICE_", "IOT_TTS_VOICE_")
        content = content.replace("TTS_JSON_BUF_SIZE", "IOT_TTS_JSON_BUF_SIZE")
        content = content.replace("TTS_AUDIO_MAX_SIZE", "IOT_TTS_AUDIO_MAX_SIZE")
        funcs = [
            "tts_create", "tts_set_config", "tts_free", "tts_provider_name",
            "tts_synthesize", "tts_synthesize_ssml", "tts_synthesize_to_file",
            "tts_synthesize_stream", "tts_synthesize_async",
            "tts_speak", "tts_stop_speaking", "tts_is_speaking",
            "tts_play_audio", "tts_result_free",
            "tts_voice_id_for_provider", "tts_text_to_ssml",
            "tts_openai_synthesize", "tts_edge_synthesize", "tts_do_synthesize",
            "tts_get_api_url",
        ]
        for func in funcs:
            content = content.replace(func, "iot_" + func)

    if content != original:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"  [UPDATED] {os.path.basename(filepath)}")
    else:
        print(f"  [UNCHANGED] {os.path.basename(filepath)}")

if __name__ == "__main__":
    for fname in files:
        fpath = os.path.join(AI_DIR, fname)
        if os.path.exists(fpath):
            rename_file(fpath)
        else:
            print(f"  [MISSING] {fname}")
    print("\nDone.")
