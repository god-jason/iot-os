function class(object)
    object.__index = object
    return object
end

function extend(child, parent)
    child.__index = child
    setmetatable(child, {
        __index = parent
    })
end

function new(class, object)
    --return setmetatable(object or {}, class)
    object = object or {}
    setmetatable(object, class)
    return object
end

function increment_id()
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

