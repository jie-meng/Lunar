kRegexFunctionLua = "function\\s+(?<api>(\\w+((\\.|:)\\w+)*\\s*\\(.*\\)))"

apis = {}

function parseSupplementApi(filename)
    
    if (#apis > 0) then
        return apis
    end
    
    t1 = file.findFilesInDirRecursively(file.currentPath() .. "/src/cocos", "lua")
    t2 = file.findFilesInDirRecursively(file.currentPath() .. "/src/package", "lua")
    
    for k, v in pairs(t1) do
        parseApi(v, apis)
    end
    
    for k, v in pairs(t2) do
        parseApi(v, apis)
    end
    
    return apis
end

function parseApi(filename, apis)
    
    local re_func = regex.create(kRegexFunctionLua)
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            if regex.match(re_func, strTrim(line)) then
                local api = regex.getMatchedGroupByName(re_func, "api")
                if api ~= "" then
                    local api_format, _ = string.gsub(api, ":", ".")
                    table.insert(apis, api_format)
                end
            end
            line = f:read("*line")
        end
        io.close(f)
    end
    
    regex.destroy(re_func)
end