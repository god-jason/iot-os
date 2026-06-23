# IoT-OS 模块编译配置说明

## 概述

IoT-OS 支持通过宏定义控制各个模块的编译，可以根据需要选择性编译不同的模块。

## 模块列表

### Core 模块（默认编译）

| 模块 | 宏定义 | 说明 |
|------|--------|------|
| rtos | - | 实时操作系统接口 |
| log | - | 日志模块 |
| pack | - | 数据打包/解包 |
| wdt | `IOT_ENABLE_LUA_WDT` | 看门狗模块 |

### Modules 模块（可选）

| 模块 | 宏定义 | 配置选项 | 说明 |
|------|--------|----------|------|
| crypto | `IOT_ENABLE_MODULE_CRYPTO` | `module_crypto` | 加密模块 |
| fs | `IOT_ENABLE_MODULE_FS` | `module_fs` | 文件系统模块 |
| http | `IOT_ENABLE_MODULE_HTTP` | `module_http` | HTTP 模块 |
| mqtt | `IOT_ENABLE_MODULE_MQTT` | `module_mqtt` | MQTT 模块 |
| net | `IOT_ENABLE_MODULE_NET` | `module_net` | 网络模块 |
| zlib | `IOT_ENABLE_MODULE_ZLIB` | `module_zlib` | ZLIB 压缩模块 |

### Vendor 模块（可选）

| 模块 | 宏定义 | 配置选项 | 说明 |
|------|--------|----------|------|
| cjson | `IOT_ENABLE_VENDOR_CJSON` | `vendor_cjson` | CJSON 模块 |
| gmssl | `IOT_ENABLE_VENDOR_GMSSL` | `vendor_gmssl` | GMSSL 模块 |
| sqlite3 | `IOT_ENABLE_VENDOR_SQLITE3` | `vendor_sqlite3` | SQLite3 模块 |

## XMake 配置

### 命令行配置

```bash
# 启用 Lua WDT 模块（默认启用）
xmake config --lua_wdt=y

# 启用所有 Modules 模块
xmake config --module_crypto=y --module_fs=y --module_http=y --module_mqtt=y --module_net=y --module_zlib=y

# 启用所有 Vendor 模块
xmake config --vendor_cjson=y --vendor_gmssl=y --vendor_sqlite3=y

# 禁用 Lua WDT 模块
xmake config --lua_wdt=n
```

### 配置文件

在项目根目录创建 `xmake.lua` 配置文件：

```lua
-- 模块配置示例
option("lua_wdt")
    set_default(true)
    set_showmenu(true)
    set_description("Enable Lua WDT module")

option("module_crypto")
    set_default(false)
    set_showmenu(true)
    set_description("Enable crypto module")

option("module_fs")
    set_default(false)
    set_showmenu(true)
    set_description("Enable fs module")

option("module_http")
    set_default(false)
    set_showmenu(true)
    set_description("Enable http module")

option("module_mqtt")
    set_default(false)
    set_showmenu(true)
    set_description("Enable mqtt module")

option("module_net")
    set_default(false)
    set_showmenu(true)
    set_description("Enable net module")

option("module_zlib")
    set_default(false)
    set_showmenu(true)
    set_description("Enable zlib module")

option("vendor_cjson")
    set_default(false)
    set_showmenu(true)
    set_description("Enable cjson vendor module")

option("vendor_gmssl")
    set_default(false)
    set_showmenu(true)
    set_description("Enable gmssl vendor module")

option("vendor_sqlite3")
    set_default(false)
    set_showmenu(true)
    set_description("Enable sqlite3 vendor module")
```

## 使用示例

### 最小配置（仅 Core 模块）

```bash
xmake config --lua_wdt=y
xmake build
```

### 完整配置（所有模块）

```bash
xmake config --lua_wdt=y \
             --module_crypto=y \
             --module_fs=y \
             --module_http=y \
             --module_mqtt=y \
             --module_net=y \
             --module_zlib=y \
             --vendor_cjson=y \
             --vendor_gmssl=y \
             --vendor_sqlite3=y
xmake build
```

### Web 应用配置

```bash
xmake config --lua_wdt=y \
             --module_http=y \
             --module_net=y \
             --vendor_cjson=y
xmake build
```

### 数据库应用配置

```bash
xmake config --lua_wdt=y \
             --module_fs=y \
             --vendor_sqlite3=y
xmake build
```

## Lua 使用示例

### WDT 模块

```lua
-- 初始化看门狗，超时时间 5000ms
wdt.init(5000)

-- 定期喂狗
while true do
    -- 执行任务
    do_something()
    
    -- 喂狗
    wdt.feed()
    
    -- 延迟
    rtos.sleep(1000)
end
```

### Crypto 模块

```lua
-- 需要启用: xmake config --module_crypto=y
local crypto = require("crypto")

-- MD5 哈希
local hash = crypto.md5("hello")
print(hash)
```

### FS 模块

```lua
-- 需要启用: xmake config --module_fs=y
local fs = require("fs")

-- 列出文件
local files = fs.listdir("/sdcard")
for i, file in ipairs(files) do
    print(file)
end
```

### HTTP 模块

```lua
-- 需要启用: xmake config --module_http=y
local http = require("http")

-- GET 请求
local response = http.get("http://example.com/api")
print(response.body)
```

### CJSON 模块

```lua
-- 需要启用: xmake config --vendor_cjson=y
local cjson = require("cjson")

-- 编码 JSON
local data = {name = "test", value = 123}
local json_str = cjson.encode(data)
print(json_str)

-- 解码 JSON
local obj = cjson.decode(json_str)
print(obj.name)
```

### SQLite3 模块

```lua
-- 需要启用: xmake config --vendor_sqlite3=y
local sqlite3 = require("sqlite3")

-- 打开数据库
local db = sqlite3.open("test.db")

-- 执行 SQL
db:exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT)")

-- 插入数据
local stmt = db:prepare("INSERT INTO users (name) VALUES (?)")
stmt:bind(1, "Alice")
stmt:step()
stmt:finalize()

-- 查询数据
for row in db:nrows("SELECT * FROM users") do
    print(row.id, row.name)
end

-- 关闭数据库
db:close()
```

## 注意事项

1. **依赖关系**：某些模块可能有依赖关系，需要按顺序启用
2. **内存占用**：启用更多模块会增加内存占用
3. **编译时间**：启用更多模块会增加编译时间
4. **默认配置**：Lua WDT 模块默认启用，其他模块默认禁用

## 故障排查

### 模块未找到

如果 Lua 脚本中出现 `module 'xxx' not found` 错误：

1. 检查是否启用了对应的编译选项
2. 重新编译项目：`xmake rebuild`
3. 检查模块名称是否正确

### 编译错误

如果出现编译错误：

1. 检查依赖模块是否已启用
2. 检查 xmake.lua 配置是否正确
3. 清理构建缓存：`xmake clean && xmake build`