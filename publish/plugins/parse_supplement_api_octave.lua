local kRegexFunctionOctave = [[function\s+((\[.*\]\s*=)|(\w+\s*=))?\s*(?<api>\w+\s*(?<param>\(.*\)))]]

function parseSupplementApi(filename)
    
    local apis = {}
    
    parseSupplementApiCurrent(filename, apis)
    parseSupplementApiInPath(filename, apis)
    
    return apis
end

function parseSupplementApiCurrent(filename, apis)

    local re_func = regex.create(kRegexFunctionOctave)
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            
            if regex.match(re_func, strTrim(line)) then
                local api = regex.getMatchedGroupByName(re_func, "api")
                if api ~= "" then
                    table.insert(apis, api)
                end
            end
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    regex.destroy(re_func)
end

function parseSupplementApiInPath(filename, apis)

    local dir, name = file.splitPathname(filename)
    local re_func = regex.create(kRegexFunctionOctave)
    
    local files = file.findFilesInDir(dir, "m")
    for key, value in pairs(files) do
        local f = io.open(value, "r")
        local line = f:read("*line")
        while (line ~= nil) do
            if string.sub(line, 1, 1) ~= "#" and string.sub(line, 1, 1) ~= "%" then
                if regex.match(re_func, strTrim(line)) then
                    local param = regex.getMatchedGroupByName(re_func, "param")
                    if param ~= "" then
                        table.insert(apis, file.fileBaseName(value) .. param)
                    else
                        break
                    end
                end
            end
            
            line = f:read("*line")
        end
        
        io.close(f)
    end
    
    regex.destroy(re_func)
end