#!/usr/bin/env python3
# generate_iot_luac.py - 编译 iot.lua 生成 iot.luac 和 iot.luac.h
#
# 用法: python generate_iot_luac.py
# 需要: luac53.exe (位于 tools/lua536/)

import subprocess
import sys
import struct
from pathlib import Path

# 路径配置
SCRIPT_DIR = Path(__file__).parent.resolve()
LUA_CMD = SCRIPT_DIR / "tools" / "lua536" / "luac53.exe"
IOT_LUA = SCRIPT_DIR / "app" / "iot.lua"
IOT_LUAC = SCRIPT_DIR / "core" / "iot.luac"
IOT_LUAC_H = SCRIPT_DIR / "core" / "iot.luac.h"


def check_luac():
    if not LUA_CMD.exists():
        print(f"错误: 未找到 Lua 编译器: {LUA_CMD}")
        sys.exit(1)
    print(f"使用 Lua 编译器: {LUA_CMD}")


def check_lua_file():
    if not IOT_LUA.exists():
        print(f"错误: iot.lua 不存在: {IOT_LUA}")
        sys.exit(1)


def compile_lua():
    print("===> 编译 iot.lua ...")
    try:
        result = subprocess.run(
            [str(LUA_CMD), "-o", str(IOT_LUAC), str(IOT_LUA)],
            capture_output=True,
            text=True,
            check=True
        )
        print(f"      编译成功 -> {IOT_LUAC}")
    except subprocess.CalledProcessError as e:
        print(f"错误: 编译 iot.lua 失败")
        print(f"      stdout: {e.stdout}")
        print(f"      stderr: {e.stderr}")
        sys.exit(1)


def generate_header():
    print("===> 生成 iot.luac.h ...")
    luac_data = IOT_LUAC.read_bytes()

    # 每行 12 字节，格式: 0x00, 0x00, ...
    lines = []
    for offset in range(0, len(luac_data), 12):
        chunk = luac_data[offset:offset + 12]
        line = ", ".join(f"0x{b:02x}" for b in chunk)
        lines.append("    " + line + ",")

    if lines:
        lines[-1] = lines[-1].rstrip(",")

    header_content = (
        "#ifndef IOT_LUAC_H\n"
        "#define IOT_LUAC_H\n\n"
        "/* iot.luac - compiled Lua bytecode */\n"
        "static const unsigned char iot_luac[] = {\n"
        + "\n".join(lines)
        + "\n};\n\n"
        "static const unsigned int iot_luac_len = sizeof(iot_luac);\n\n"
        "#endif /* IOT_LUAC_H */\n"
    )

    IOT_LUAC_H.write_text(header_content, encoding="ascii")
    print(f"      生成成功 -> {IOT_LUAC_H} ({len(luac_data)} bytes)")


def main():
    print("generate_iot_luac.py")
    print("=" * 50)

    check_luac()
    check_lua_file()
    compile_lua()
    generate_header()

    print("===> 完成!")


if __name__ == "__main__":
    main()
