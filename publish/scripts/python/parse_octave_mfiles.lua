local kPatternOctaveFunction1 = [[function%s+([%w_]+)%s*=%s*([%w_]+)%s*(%(.*%))]]
local kPatternOctaveFunction2 = [[function%s+(%[.*%])%s*=%s*([%w_]+)%s*(%(.*%))]]
local kPatternOctaveFunction3 = [[function%s+([%w_]+)%s*(%(.*%))]]

function parseApi(mfiles_path)
    
    local apis = {}
    
    local files = util.findFilesInDirRecursively(mfiles_path, "m")
    for _, v in ipairs(files) do
        parseApiInPath(v, apis)
    end
    
    return apis
end

function parseApiInPath(filename, apis)

    local dir, name = util.splitPathname(filename)
    
    local files = util.findFilesInDir(dir, "m")
    for key, value in pairs(files) do
        local f = io.open(value, "r")
        local line = f:read("*line")
        while (line ~= nil) do
            local break_while = false
            
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "%") or util.strStartWith(util.strTrimLeft(line), "#") then
                    break
                end
                
                local ret, func, param
                ret, func, param = string.match(line, kPatternOctaveFunction1)
                if ret and func and param then
                    local api = string.format("%s%s --> %s", func, param, ret)
                    table.insert(apis, api)
                    print(api)
                    break
                end
                
                ret, func, param = string.match(line, kPatternOctaveFunction2)
                if ret and func and param then
                    local api = string.format("%s%s --> %s", func, param, ret)
                    table.insert(apis, api)
                    print(api)
                    break
                end
                
                func, param = string.match(line, kPatternOctaveFunction3)
                if func and param then
                    local api = string.format("%s%s", func, param)
                    table.insert(apis, api)
                    print(api)
                    break
                end
            until true
            line = f:read("*line")
        end
        
        io.close(f)
    end
end

local apis = parseApi("./mfiles")
print('finish')