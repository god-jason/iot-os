local log = iot.logger("scenes")

local scenes = {}

-- 注册到全局
_G.scenes = scenes

local boot = require("boot")
local database = require("database")

local Scene = require("scene")

-- 创建场景
function scenes.create(scene)
    log.info("create", iot.json_encode(scene))

    local s = Scene:new(scene)
    scenes[scene.id] = s

    local ret, info = s:open()
    if not ret then
        return false, info
    end

    return true, s
end

-- 加载场景
function scenes.open()
    local ss = database.find("scene")
    for i, s in ipairs(ss) do
        local ret, info = scenes.create(s)
        if not ret then
            log.error("scene:", s.id, " open error:", info)
        end
    end

    return true
end

-- 关闭场景
function scenes.close()
    for i, s in pairs(scenes) do
        if s.__index == Scene then
            s:close()
        end
    end
end

scenes.deps = {"gateway"}

boot.register("scenes", scenes)

return scenes
