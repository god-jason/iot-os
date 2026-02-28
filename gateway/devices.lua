-- local log = iot.logger("devices")
local devices = {}

_G.devices = devices

-- 注册设备
function devices.register(id, device)
    devices[id] = device
end

-- 反注册设备
function devices.unregister(id)
    devices[id] = nil
end

return devices
