# IoT-OS 物联网操作系统

基于 Lua 的轻量级物联网操作系统，支持 Windows、Linux、ESP32、移动模组(ML307N)等多平台运行。

## 目录

- [项目简介](#项目简介)
- [快速开始](#快速开始)
- [项目结构](#项目结构)
- [编译说明](#编译说明)
- [调试方法](#调试方法)
- [Lua API 参考](#lua-api-参考)
- [平台适配](#平台适配)

---

## 项目简介

IoT-OS 是一个面向物联网场景的 Lua 运行时环境，提供：

- **跨平台支持**: Windows、Linux、ESP32、ML307N 等
- **丰富模块**: 文件系统、网络(MQTT/HTTP)、加密、压缩、GUI(LVGL)
- **驱动支持**: 传感器、执行器、通信模块等
- **统一接口**: 平台抽象层，代码可移植

---

## 快速开始

### 1. 环境要求

| 平台 | 编译工具 | 其他依赖 |
|------|----------|----------|
| Windows | xmake, MSVC | - |
| Linux | xmake, GCC | - |
| ESP32 | xmake, esp-idf | ESP32 SDK |
| ML307N | ARM GCC | 模组 SDK |

### 2. 安装编译工具

```bash
# 安装 xmake
winget install xmake

# 或使用脚本安装
curl -fsSL https://xmake.io/shget.text | bash
```

### 3. 编译运行

```bash
# Windows 平台
cd platforms/windows
xmake build
xmake run

# Linux 平台
cd platforms/linux
xmake build
xmake run
```

### 4. 编写应用

在 `app/main.lua` 中编写业务逻辑：

```lua
-- 日志输出
log.info("Application started")

-- 定时器使用
local timer = rtos.timer_create(1000, 1)  -- 1秒周期定时器

-- 消息循环
while true do
    local id, data, params = rtos.recv(1000)
    if id == "timeout" then
        log.debug("Timer tick")
    end
end
```

---

## 项目结构

```
iot-os/
├── app/                    # 应用代码
│   ├── main.lua           # 主入口
│   └── iot.lua            # IoT 核心脚本
├── core/                   # 核心模块 (C)
│   ├── iot_log.c/h       # 日志系统
│   ├── iot_queue.c/h     # 消息队列
│   ├── iot_callback.c/h  # 回调管理
│   ├── iot_params.c/h    # 参数列表
│   ├── iot_rtos.c/h      # RTOS 抽象
│   ├── iot_task.c/h      # 任务管理
│   ├── iot_wdt.c/h       # 看门狗
│   ├── iot_modules.c/h   # 模块注册
│   └── iot_pack.c/h      # 数据打包
├── modules/                # Lua 扩展模块
│   ├── fs/               # 文件系统
│   ├── net/              # Socket 网络
│   ├── http/             # HTTP 客户端/服务端
│   ├── mqtt/             # MQTT 客户端
│   ├── crypto/           # 加密算法
│   ├── zlib/             # 压缩解压
│   └── lvgl/             # GUI 界面
├── drivers/                # 硬件驱动
│   ├── gpio/             # GPIO
│   ├── uart/             # 串口
│   ├── i2c/              # I2C
│   ├── spi/              # SPI
│   └── sensors/          # 传感器驱动
├── platforms/              # 平台适配层
│   ├── windows/          # Windows 平台
│   ├── linux/            # Linux 平台
│   ├── esp32/            # ESP32 平台
│   └── ml307n/           # 移动模组平台
└── vendor/               # 第三方库
    ├── lua/              # Lua 运行时
    ├── gmssl/            # 国密算法
    └── cjson/            # JSON 解析
```

### 核心模块说明

| 目录 | 说明 |
|------|------|
| `app/` | 应用层 Lua 代码 |
| `core/` | C 语言核心功能（日志、队列、定时器） |
| `modules/` | Lua 扩展模块（文件系统、网络协议） |
| `drivers/` | 硬件驱动 |
| `platforms/` | 平台适配代码 |
| `vendor/` | 第三方依赖 |

---

## 编译说明

### 使用 xmake 编译

```bash
# 编译所有模块
xmake build

# 清理并重新编译
xmake clean
xmake build

# 查看编译目标
xmake show

# 切换目标平台
xmake f -p windows
xmake f -p linux
xmake f -p esp32
```

### 各平台编译

#### Windows

```bash
cd platforms/windows
xmake build
xmake run
```

#### Linux

```bash
cd platforms/linux
xmake build
./build/linux/x86_64/release/iot
```

#### ESP32

```bash
cd platforms/esp32
xmake build -b esp32c3
xmake flash
```

#### ML307N

需要使用模组厂商提供的编译工具链，参考 `platforms/ml307n/README.md`。

### 编译配置

编辑 `xmake.lua` 或使用命令配置：

```bash
# 设置交叉编译工具链
xmake f -p cross -a arm64 -t gcc

# 设置编译模式
xmake f -m release

# 查看当前配置
xmake f --show
```

---

## 调试方法

### 1. 日志调试

系统内置分级日志，通过 `log` 模块控制：

```lua
-- 设置日志级别
log.level(log.DEBUG)

-- 输出日志
log.trace("Trace message")
log.debug("Debug message")
log.info("Info message")
log.warn("Warning message")
log.error("Error message")

-- 多参数
log.debug("Value:", value, "Status:", status)
```

### 2. C 代码调试

在 `iot_log.h` 中定义了日志宏：

```c
#include "iot_log.h"

LOG_DEBUG("Variable value: %d", value);
LOG_INFO("Function called");
LOG_ERROR("Failed with error: %d", err);
```

### 3. Windows 平台调试

```bash
# 使用 VSCode 调试
code .

# 在 launch.json 中配置
{
    "name": "IoT-OS Launch",
    "type": "cppvsdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/windows/x64/release/iot.exe",
    "args": [],
    "cwd": "${workspaceFolder}/build/windows/x64/release"
}
```

### 4. GDB 调试 (Linux/ESP32)

```bash
# Linux
gdb ./build/linux/x86_64/release/iot

# ESP32
xtensa-esp32-elf-gdb ./build/esp32/release/iot.elf

# 常用命令
break main
run
next
step
print variable
backtrace
```

### 5. 串口调试

Windows 平台使用 COM 口输出日志：

```bash
# 查看可用串口
mode

# 连接串口 (115200 波特率)
xmake run --port=COM3 --baud=115200
```

### 6. ASSERT 断言

核心代码中使用断言检测错误：

```c
#include "iot_base.h"

ASSERT(ptr != NULL, "Pointer is NULL");
ASSERT(count > 0, "Count must be positive");
```

---

## Lua API 参考

### 模块列表

| 模块 | 说明 | 平台 |
|------|------|------|
| `rtos` | 定时器、消息队列 | All |
| `log` | 分级日志输出 | All |
| `pack` | 二进制打包/解包 | All |
| `wdt` | 看门狗定时器 | All |
| `json` | JSON 编解码 | All |
| `fs` | 文件系统 | All |
| `net` | Socket 网络 | All |
| `http` | HTTP 客户端/服务端 | All |
| `mqtt` | MQTT 客户端 | All |
| `crypto` | 加密算法 | All |
| `zlib` | 压缩/解压 | All |
| `uart` | 串口通信 | All |
| `lvgl` | GUI 界面 | All |

---

### rtos - 实时操作系统

定时器和消息队列管理。

#### 常量

```lua
rtos.INFINITE  -- 永久等待 (-1)
```

#### 函数

##### rtos.timer_create(timeout_ms, [periodic])

创建定时器。

```lua
local timer = rtos.timer_create(1000)      -- 单次定时器
local timer = rtos.timer_create(1000, 1)   -- 周期定时器
```

##### rtos.timer_stop(timer_id)

停止定时器。

```lua
rtos.timer_stop(timer)
```

##### rtos.timer_delete(timer_id)

删除定时器。

```lua
rtos.timer_delete(timer)
```

##### rtos.recv(timeout)

接收消息。

```lua
while true do
    local id, data, params = rtos.recv(1000)
    if id == "timeout" then
        -- 定时器超时
    elseif id == "publish" then
        -- 事件消息
    elseif id == "call" then
        -- 方法调用
    end
end
```

---

### log - 日志模块

分级日志输出。

#### 常量

```lua
log.TRACE  -- 跟踪 (0)
log.DEBUG  -- 调试 (1)
log.INFO   -- 信息 (2)
log.WARN   -- 警告 (3)
log.ERROR  -- 错误 (4)
```

#### 函数

##### log.level([level])

设置/获取日志级别。

```lua
log.level(log.DEBUG)  -- 设置
local lvl = log.level()  -- 获取
```

##### log.debug/info/warn/error(...)

输出各级别日志。

```lua
log.debug("Value:", value)
log.info("Connected")
log.warn("Retry count:", count)
log.error("Failed:", err)
```

---

### pack - 数据打包

二进制数据打包/解包。

#### 常量

```lua
pack.BE  -- 大端序 (0)
pack.LE  -- 小端序 (1)
```

#### 函数

##### pack.pack(format, ...)

打包数据。

```lua
local data = pack.pack("I B B", 12345, 1, 2)  -- uint32 + 2个uint8
local data = pack.pack("s", "hello")           -- 字符串
```

##### pack.unpack(data, format, [offset])

解包数据。

```lua
local value, a, b = pack.unpack(data, "I B B")
local str = pack.unpack(data, "s", 10)
```

---

### wdt - 看门狗

系统异常检测。

#### 函数

##### wdt.init(timeout_ms)

初始化看门狗。

```lua
wdt.init(5000)  -- 5秒超时
```

##### wdt.feed()

喂狗。

```lua
while true do
    wdt.feed()
    sys.wait(1000)
end
```

---

### json - JSON 解析

#### 函数

##### json.decode(str)

解码 JSON。

```lua
local obj = json.decode('{"name":"test","value":123}')
```

##### json.encode(data)

编码为 JSON。

```lua
local str = json.encode({name = "test", value = 123})
```

---

### fs - 文件系统

#### 文件操作

##### fs.open(path, mode)

打开文件。

```lua
local f = fs.open("/test.txt", "w")  -- 写入
local f = fs.open("/test.txt", "r")  -- 读取
```

##### File:write/read/close

文件读写。

```lua
f:write("hello")
local data = f:read(100)
f:close()
```

#### 目录操作

##### fs.mkdir/mkdir/remove

目录管理。

```lua
fs.mkdir("/sdcard")
fs.remove("/tmp/cache")
```

##### fs.lsdir(path)

列出目录。

```lua
for _, name in ipairs(fs.lsdir("/")) do
    print(name)
end
```

---

### net - 网络

Socket 编程。

#### Socket 创建

```lua
local sock = net.socket()
sock:connect("example.com", 80)
sock:send("GET / HTTP/1.0\r\n\r\n")
local data = sock:recv(1024)
sock:close()
```

#### DNS 解析

```lua
local ip = net.dns_resolve("example.com")
```

---

### http - HTTP

HTTP 客户端/服务端。

#### 请求

```lua
-- GET 请求
local code, headers, body = http.request("GET", "http://example.com/api")

-- POST 请求
local code, headers, body = http.request("POST", "http://example.com/api", {
    body = "data=test"
})
```

---

### mqtt - MQTT

MQTT 客户端。

#### 创建客户端

```lua
local m = mqtt.client("device001", {
    host = "broker.example.com",
    port = 1883,
    username = "user",
    password = "pass"
})
```

#### 订阅/发布

```lua
m:connect()
m:subscribe("topic/test")
m:publish("topic/test", "hello")

m:on("message", function(topic, data)
    print("Received:", topic, data)
end)
```

---

### crypto - 加密

#### 哈希算法

```lua
local md5 = crypto.md5("hello")
local sha256 = crypto.sha256("hello")
```

#### HMAC

```lua
local hmac = crypto.hmac_sha256("hello", "secret")
```

#### AES 加解密

```lua
local cipher = crypto.aes_encrypt("hello", "1234567890123456", "cbc")
local plain = crypto.aes_decrypt(cipher, "1234567890123456", "cbc")
```

---

### zlib - 压缩

#### deflate 压缩

```lua
local compressed = zlib.deflate.compress("hello world")
local original = zlib.deflate.decompress(compressed)
```

#### gzip/zip/tar

```lua
zlib.gzip.decompress_file("data.gz", "output")
zlib.zip.decompress_file("app.zip", "/app")
zlib.tar.decompress_file("app.tar.gz", "/app")
```

---

### uart - 串口

#### 配置

```lua
uart.setup(0, 115200)  -- 串口0, 115200波特率
```

#### 读写

```lua
uart.write(0, "AT\r\n")
local data = uart.read(0, 100)
```

#### 回调

```lua
uart.on(0, function(data)
    print("UART received:", data)
end)
```

---

## 平台适配

详细平台接口说明请参考 [platforms/README.md](platforms/README.md)。

### Windows

- 支持完整功能
- GPIO/I2C/SPI 为模拟实现
- 适合开发和测试

### Linux

- 支持完整功能
- GPIO 需要 `/sys/class/gpio` 支持
- 适合树莓派等嵌入式 Linux

### ESP32

- 支持 WiFi、Bluetooth
- GPIO/I2C/SPI 硬件支持
- 适合低功耗设备

### ML307N

- 移动网络支持 (4G Cat.1)
- 丰富的外设接口
- 适合物联网网关

---

## 许可证

MIT License

## 贡献指南

欢迎提交 Issue 和 Pull Request！
