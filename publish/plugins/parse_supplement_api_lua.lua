kRegexFunctionLua = "function\\s+(?<api>(\\w+((\\.|:)\\w+)*\\s*\\(.*\\)))"
kRegexIncludeLua = "include\\(\"(?<path>.+)\"\\)"

function parseSupplementApi(filename)
    
    local dir, name = file.splitPathname(filename)
    local apis = {}
    
    parseSupplementApiRecursively(filename, dir, apis)
    
    return apis
end

function parseSupplementApiRecursively(filename, dir, apis)

    local re_func = regex.create(kRegexFunctionLua)
    local re_inc = regex.create(kRegexIncludeLua)
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            if regex.match(re_func, line) then
                local api = regex.getMatchedGroupByName(re_func, "api")
                if api ~= "" then
                    local api_format, _ = string.gsub(api, ":", ".")
                    table.insert(apis, api_format)
                end
            elseif regex.match(re_inc, line) then
                local file_include = dir .. "/" .. regex.getMatchedGroupByName(re_inc, "path")
                
                if file.isPathFile(file_include) then
                    parseSupplementApiRecursively(file_include, dir, apis)
                end
            end
            line = f:read("*line")
        end
        io.close(f)
    end
    
    regex.destroy(re_func)
    regex.destroy(re_inc)
end