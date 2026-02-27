local robot = require("robot")
local components = require("components")

robot.fsm:register("idle", {
    enter = function()
    end,
    leave = function()
    end,
    tick = function()
    end
})

robot.fsm:register("charge", {
    enter = function()
        robot.plan("chage")
    end,
    leave = function()
        components.relay_charge:off()
    end,
    tick = function()

    end
})
