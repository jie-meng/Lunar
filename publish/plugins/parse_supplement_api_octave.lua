local kRegexFunctionOctave = [[function\s+((\[.*\]\s*=)|(\w+\s*=))?\s*(?<api>\w+\s*(?<param>\(.*\)))]]

function parseSupplementApi(filename, cursor_line, project_src_dir)
    
    local apis = {}
    
    parseSupplementApiCurrent(filename, apis)
    parseSupplementApiInPath(filename, apis)
    
    return apis
end

function parseSupplementApiCurrent(filename, apis)

    local re_func = util.newRegex(kRegexFunctionOctave)
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "%") or util.strStartWith(util.strTrimLeft(line), "#") then
                    break
                end
            
                if util.match(re_func, util.strTrim(line)) then
                    local api = util.getMatchedGroupByName(re_func, "api")
                    if api ~= "" then
                        table.insert(apis, api)
                    end
                    break
                end            
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    re_func:delete()
end

function parseSupplementApiInPath(filename, apis)

    local dir, name = util.splitPathname(filename)
    local re_func = util.newRegex(kRegexFunctionOctave)
    
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
                
                if re_func:match(util.strTrim(line)) then
                    local param = re_func:getMatchedGroupByName("param")
                    if param ~= "" then
                        table.insert(apis, util.fileBaseName(value) .. param)
                    end
                    break_while = true
                    break
                end
            until true
            
            if break_while then
                break
            end
            line = f:read("*line")
        end
        
        io.close(f)
    end
    
    re_func:delete()
end
