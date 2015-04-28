kRegexFunctionLua = [[function\s+(?<api>(\w+((\.|:)\w+)*\s*\(.*\)))]]

function parseSupplementApi(filename)
    
    --[[if (#apis > 0) then
        return apis
    end--]]
    
    local apis = {}
  
    t_app = file.findFilesInDirRecursively(file.currentPath() .. "/src/app", "lua")
    
    for k, v in pairs(t_app) do
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