local MA = {}
MA.__index = MA

function MA:new(size)
    local self = setmetatable({}, MA)
    self.size = size or 5
    self.buffer = {}
    return self
end

function MA:update(val)
    table.insert(self.buffer, val)
    if #self.buffer > self.size then
        table.remove(self.buffer, 1)
    end
    local sum = 0
    for i=1,#self.buffer do sum = sum + self.buffer[i] end
    return sum / #self.buffer
end

return MA