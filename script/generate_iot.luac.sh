#!/bin/bash

# generate_iot.luac.sh - 编译 iot.lua 为 iot.luac 并生成 script/iot.luac.h

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IOTOS_SCRIPT_DIR="$SCRIPT_DIR/script"
LUA_CMD="$SCRIPT_DIR/tools/lua536/luac53.exe"
IOT_LUA="$IOTOS_SCRIPT_DIR/iot.lua"
IOT_LUAC="$IOTOS_SCRIPT_DIR/iot.luac"
IOT_LUAC_H="$IOTOS_SCRIPT_DIR/iot.luac.h"

resolve_python() {
	if [ -n "${PYTHON}" ] && "${PYTHON}" --version >/dev/null 2>&1; then
		return 0
	fi
	if command -v py >/dev/null 2>&1; then
		PYTHON="$(py -3 -c 'import sys; print(sys.executable)' 2>/dev/null)"
	fi
	if [ -z "${PYTHON}" ] && command -v python3 >/dev/null 2>&1; then
		PYTHON="$(command -v python3)"
	fi
	if [ -z "${PYTHON}" ] && command -v python >/dev/null 2>&1; then
		PYTHON="$(command -v python)"
	fi
}

if [ ! -x "$LUA_CMD" ] && [ ! -f "$LUA_CMD" ]; then
	echo "错误: 未找到 Lua 5.3.6 编译器: $LUA_CMD"
	exit 1
fi

if [ ! -f "$IOT_LUA" ]; then
	echo "错误: iot.lua 不存在: $IOT_LUA"
	exit 1
fi

resolve_python
if [ -z "${PYTHON}" ]; then
	echo "错误: 未找到 Python，无法生成 iot.luac.h"
	exit 1
fi

echo "===> 编译 iot.lua ..."
cd "$IOTOS_SCRIPT_DIR" || exit 1
if ! "$LUA_CMD" -o iot.luac iot.lua; then
	echo "错误: 编译 iot.lua 失败"
	exit 1
fi

echo "===> 生成 iot.luac.h ..."
"${PYTHON}" - "$IOT_LUAC" "$IOT_LUAC_H" <<'PY'
import sys
from pathlib import Path

luac = Path(sys.argv[1]).read_bytes()
header = Path(sys.argv[2])
lines = []
for offset in range(0, len(luac), 12):
    chunk = luac[offset:offset + 12]
    line = ", ".join(f"0x{b:02x}" for b in chunk)
    lines.append("    " + line + ",")
if lines:
    lines[-1] = lines[-1].rstrip(",")

header.write_text(
    "#ifndef IOT_LUAC_H\n"
    "#define IOT_LUAC_H\n\n"
    "/* iot.luac - compiled Lua bytecode */\n"
    "static const unsigned char iot_luac[] = {\n"
    + "\n".join(lines)
    + "\n};\n\n"
    "static const unsigned int iot_luac_len = sizeof(iot_luac);\n\n"
    "#endif /* IOT_LUAC_H */\n",
    encoding="ascii",
    newline="\n",
)
print(f"generated {header} ({len(luac)} bytes)")
PY

echo "===> 完成: $IOT_LUAC_H"
