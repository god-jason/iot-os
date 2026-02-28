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

function merge(t1, t2)
  for k,v in pairs(t2) do
      t1[k] = v
  end
  return t1
end

