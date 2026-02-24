local IIR = {}
IIR.__index = IIR

function IIR:new(alpha, init)
    local self = setmetatable({}, IIR)
    self.alpha = alpha or 0.1
    self.y = init or 0
    return self
end

function IIR:update(x)
    self.y = self.alpha * x + (1 - self.alpha) * self.y
    return self.y
end

return IIR