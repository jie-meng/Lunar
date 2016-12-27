local kRegexFunctionLua = [[function\s+(?<api>(\w+((\.|:)\w+)*\s*\(.*\)))]]
local kRegexRequireLua = [[((local\s+)?(?<module>\w+)\s*=\s*)?require[\(\s]\s*\"(?<path>.+)\"\)?]]
local kRegexReturnModuleLua = [[return\s+(?<module>\w+)]]

function parseSupplementApi(filename, cursor_line, project_src_dir)
    
    local re_func = regex.create(kRegexFunctionLua)
    local re_require = regex.create(kRegexRequireLua)
    local re_return_module = regex.create(kRegexReturnModuleLua)
    
    local apis = {}    
    parseSupplementApiMain(filename, file.currentPath(), apis, re_func, re_require, re_return_module)
    
    re_func:destroy()
    re_require:destroy()
    re_return_module:destroy()
    
    return apis
end

function parseSupplementApiMain(filename, dir, apis, re_func, re_require, re_return_module)

    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                local line_format = strTrim(line)
                
                if line_format == "" or strStartWith(line_format, "--") then
                    break
                end
            
                if re_func:match(line_format) then
                    local api = re_func:getMatchedGroupByName("api")
                    if api ~= "" then
                        local api_format, _ = string.gsub(api, ":", ".")
                        table.insert(apis, api_format)
                    end
                    break
                end    
                
                if re_require:match(line_format) then
                    parseSupplementApiRequire(
                        dir,
                        re_require:getMatchedGroupByName("module"), 
                        re_require:getMatchedGroupByName("path"), 
                        apis,
                        re_func,
                        re_require,
                        re_return_module)
                    break
                end
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
end

function parseSupplementApiRequire(dir, module_name, require_name, api_t, re_func, re_require, re_return_module)
    
    if (not strContains(require_name, "/")) and (not strContains(require_name, "\\")) then
        require_name = strRelaceAll(require_name, ".", "/")
        require_name = strRelaceAll(require_name, "//", "../")
    end
    
    local require_file = dir .. "/" .. require_name .. ".lua"
    if not file.isPathFile(require_file) then
        return
    end
    
    local f = io.open(require_file, "r")
    if f ~= nil then
        local tmp_t = {}
        local return_module = nil
        
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                local line_format = strTrim(line)
                
                if line_format == "" or strStartWith(line_format, "--") then
                    break
                end
                
                return_module = ""
                
                if re_func:match(line_format) then
                    local api = re_func:getMatchedGroupByName("api")
                    if api ~= "" then
                        local api_format, _ = string.gsub(api, ":", ".")
                        table.insert(tmp_t, api_format)
                    end
                    break
                end   
                 
                if re_require:match(line_format) and (not strStartWith(line_format, "local")) then
                    --local require in required file usually not return, just using locally, so ignore them.
                    parseSupplementApiRequire(
                        dir,
                        re_require:getMatchedGroupByName("module"), 
                        re_require:getMatchedGroupByName("path"), 
                        api_t,
                        re_func,
                        re_require,
                        re_return_module)
                    break
                end
                
                if re_return_module:match(line_format) then
                    return_module = re_return_module:getMatchedGroupByName("module")
                    break
                end
            until true
            
            --next line
            line = f:read("*line")
        end
        
        if return_module ~= "" then
            for k,v in pairs(tmp_t) do
                if strStartWith(v, return_module .. ".") then
                    if module_name ~= "" then
                        table.insert(api_t, strReplace(v, return_module .. ".", module_name .. "."))
                    end
                else
                    table.insert(api_t, v)
                end
            end
        else
            for k,v in pairs(tmp_t) do
                table.insert(api_t, v)
            end
        end
        
        io.close(f)
    end
end
