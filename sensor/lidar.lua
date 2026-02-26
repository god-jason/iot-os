-- lidar.lua
-- 通用 LiDAR 库
-- 支持 UART 串口通信读取扫描数据
local Lidar = {}
Lidar.__index = Lidar

-- 构造函数
function Lidar:new(opts)
    opts = opts or {}
    local self = setmetatable({
        uart_id = opts.uart_id or 1,
        baud_rate = opts.baud_rate or 115200
    }, Lidar)

    return self
end

-- 初始化串口
function Lidar:init()
    local ret, serial = iot.uart(self.uart_id, {
        baud_rate = self.baud_rate
    })
    if not ret then
        return false, serial
    end
    self.serial = serial

    iot.start(Lidar.receive, self)

    return true
end

-- 设置扫描数据回调
function Lidar:receive()
    while true do
        local ret, len = self.serial:wait(10000)
        if ret then
            local data = self.serial:read()
            self:parse(data)
        end
    end
end

-- 解析原始数据包
function Lidar:parse(raw)
    local points = {}
    -- 这里需要根据具体雷达协议解析
    -- 示例：假设每个点占 4 字节：angle (2 bytes) + distance (2 bytes)
    local i = 1
    while i <= #raw - 3 do
        local angle = raw:byte(i) + raw:byte(i + 1) * 256
        local distance = raw:byte(i + 2) + raw:byte(i + 3) * 256
        table.insert(points, {
            angle = angle / 100,
            distance = distance / 10
        })
        i = i + 4
    end
    return points
end

return Lidar
