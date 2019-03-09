
--c4_safe_ktrace.log.1.gz
function SafeKtrace_isMyType(filename)
    filestr = string.match(filename, "^c4_safe_ktrace.log*")
    if (filestr == nil) then 
        return false
    end
    return true
end

function SafeKtrace_getTypeName()
    return "SafeKtrace"
end

--2019/01/15-07:39:05.212628
function SafeKtrace_getLineTime(lineStr)
print(lineStr)
    local entry = DateEntry()
    dateStr = string.match(lineStr, "^%d+/%d+/%d+-%d+:%d+:%d+.%d+%s+")
    if (dateStr == nil) then 
        entry.exist = false
        return entry
    end
    year, month, day, hour, minute, second, mSecond = string.match(lineStr, "^(%d%d%d%d)/(%d%d)/(%d%d)-(%d%d):(%d%d):(%d%d).(%d+)%s")
    print(day, month, year, hour, minute, second, mSecond)

    entry.exist = true
    entry.year = tonumber(year)
    entry.month = tonumber(month)
    entry.day = tonumber(day)
    entry.hour = tonumber(hour)
    entry.minute = tonumber(minute)
    entry.second = tonumber(second)
    entry.mSecond = tonumber(mSecond)
    return entry
end

function SafeKtrace_getElement(lineStr)

end