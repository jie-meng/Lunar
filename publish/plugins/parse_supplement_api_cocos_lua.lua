local pattern_tb_function_lua = [[function%s+([%w_]+)[.:]([%w_]+)%s*(%(.*%))]]
local pattern_function_lua = [[function%s+([%w_]+)%s*(%(.*%))]]

function parseSupplementApi(filename)
    
    local apis = {}
  
    t_app = file.findFilesInDirRecursively(file.currentPath() .. "/src/app", "lua")
    
    for k, v in pairs(t_app) do
        parseApi(v, apis)
    end
    
    return apis
end

function parseApi(filename, apis)
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "--") then
                    break
                end
            
                local tb, func, param = string.match(line, pattern_tb_function_lua)
                if tb and func and param then
                    table.insert(apis, string.format("%s.%s%s", tb, func, param))
                    break
                end
                
                func, param = string.match(line, pattern_function_lua)
                if func and param then
                    table.insert(apis, string.format("%s%s", func, param))
                    break
                end
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
end