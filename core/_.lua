function class(object)
    object.__index = object
end

function extend(child, parent)
    setmetatable(child, parent)
end

function new(class, object)
    return setmetatable(object or {}, class)
end

function increament_id()
    local id = 0
    return function()
        id = id + 1
        return id
    end
end

function deep_clone(obj)
    if type(obj) ~= "table" then
        return obj
    end
    local new = {}
    for k, v in pairs(obj) do
        new[k] = deep_clone(v)
    end
    return new
end

