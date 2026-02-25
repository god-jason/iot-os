
local logging = {
    debug = log.debug,
    trace = log.debug,
    info = log.info,
    warn = log.warn,
    error = log.error,
    fatal = log.error
}

function logging.logger(tag)
    return {
        debug = function(...)
            log.debug(tag, ...)
        end,
        trace = function(...)
            log.debug(tag, ...)
        end,
        info = function(...)
            log.info(tag, ...)
        end,
        warn = function(...)
            log.warn(tag, ...)
        end,
        error = function(...)
            log.error(tag, ...)
        end,
        fatal = function(...)
            log.error(tag, ...)
            rtos.reboot()
        end
    }
end

function logging.level(level)
    if level ~= nil then
        return log.setLevel(level)
    else
        return log.getLevel()
    end
end

return logging
