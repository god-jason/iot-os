local vm = require("vm")
local components = require("components")

local log = iot.logger("instruction")

vm.register("wait", function(context, task)
    -- VM中监测wait_timeout并等待
end)

vm.register("fan", function(context, task)
    components.fan.speed(task.level)
end)

vm.register("move", function(context, task)
    local rpm = components.move_speeder:calc(task.level)
    components.move.start(rpm, task.rounds)
end)


vm.register("brake", function(context, task)
    components.turn_stepper.brake()
end)

vm.register("turn_left", function(context, task)
    components.turn_stepper.start(task.rpm, task.rounds)
end)

vm.register("turn_right", function(context, task)
    components.turn_stepper.start(task.rpm, -task.rounds)

end)

