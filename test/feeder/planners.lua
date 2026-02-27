local planner = require("planner")
local log = iot.logger("planner")

planner.register("move_forward", function(data)
    return true, {{
        type = "turn_left",
        rounds = 10,
        wait_timeout = 1000
    }, {
        type = "brake"
    }}
end)
