
function Cemtrace_isMyType(filename)
    --cemtracer_core_provisioning.log.0.gz
    filestr = string.match(filename, "^cemtracer_*")
    if (filestr ~= nil) then 
        return true
    end    
    filestr = string.match(filename, "^cemtracer_core_provisioning.log*")
    if (filestr ~= nil) then 
        return true
    end
    filestr = string.match(filename, "^cemtracer.log*")
    if (filestr ~= nil) then 
        return true
    end
    filestr = string.match(filename, "^cemtracer_application_provisioning.log*")
    if (filestr ~= nil) then 
        return true
    end
    filestr = string.match(filename, "^cemtracer_health_services.log*")
    if (filestr ~= nil) then 
        return true
    end
    filestr = string.match(filename, "^cemtracer_sysapi.log*")
    if (filestr ~= nil) then 
        return true
    end
    
    return false
end

function Cemtrace_getTypeName()
    return "Cemtrace"
end

function Cemtrace_getLineTime(lineStr)
    local entry = DateEntry()
    dateStr = string.match(lineStr, "^%d%d%s%a+%s%d%d%d%d%s%d%d:%d%d:%d%d%s+")
    if (dateStr == nil) then 
        entry.exist = false
        return entry
    end
    day, month, year, hour, minute, second = string.match(lineStr, "^(%d%d)%s(%a+)%s(%d%d%d%d)%s(%d%d):(%d%d):(%d%d)%s%s")
    print(day, month, year, hour, minute, second)

    monthTable = {Jan=1, Feb=2, Mar=3, Apr=4, May=5, Jun=6, Jul=7, Aug=8, Sept=9, Oct=10, Nov=11, Dec=12}

    entry.exist = true
    entry.year = tonumber(year)
    entry.month = monthTable[month]
    entry.day = tonumber(day)
    entry.hour = tonumber(hour)
    entry.minute = tonumber(minute)
    entry.second = tonumber(second)
    entry.mSecond = 0
    return entry
end

function Cemtrace_getElement(lineStr)

end
