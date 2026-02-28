function class(object)
    local clazz = {}
    clazz.__index = clazz
    return clazz
end

function extend(parent)
    local children = setmetatable({}, parent)
    children.__index = children
    return children
end

function new(clazz, object)
    --return setmetatable(object or {}, class)
    object = object or {}
    setmetatable(object, clazz)
    return object
end
