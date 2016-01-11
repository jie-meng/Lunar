local kRegexFunctionLua = [[function\s+(?<api>(\w+((\.|:)\w+)*\s*\(.*\)))]]
local kRegexRequireLua = [[((local\s+)?(?<module>\w+)\s*=\s*)?require[\(\s]\s*\"(?<path>.+)\"\)?]]
local kRegexReturnModuleLua = [[return\s+(?<module>\w+)]]

function parseSupplementApi(filename, cursor_line, project_src_dir)
    
    local re_func = regex.create(kRegexFunctionLua)
    local re_require = regex.create(kRegexRequireLua)
    local re_return_module = regex.create(kRegexReturnModuleLua)
    
    local apis = {}    
    parseSupplementApiMain(filename, file.currentPath(), apis, re_func, re_require, re_return_module)
    
    regex.destroy(re_func)
    regex.destroy(re_require)
    regex.destroy(re_return_module)
    
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
            
                if regex.match(re_func, line_format) then
                    local api = regex.getMatchedGroupByName(re_func, "api")
                    if api ~= "" then
                        local api_format, _ = string.gsub(api, ":", ".")
                        table.insert(apis, api_format)
                    end
                    break
                end    
                
                if regex.match(re_require, line_format) then
                    parseSupplementApiRequire(
                        dir,
                        regex.getMatchedGroupByName(re_require, "module"), 
                        regex.getMatchedGroupByName(re_require, "path"), 
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
                
                if regex.match(re_func, line_format) then
                    
                
                    local api = regex.getMatchedGroupByName(re_func, "api")
                    if api ~= "" then
                        local api_format, _ = string.gsub(api, ":", ".")
                        table.insert(tmp_t, api_format)
                    end
                    break
                end   
                 
                if regex.match(re_require, line_format) and (not strStartWith(line_format, "local")) then
                    --local require in required file usually not return, just using locally, so ignore them.
                    parseSupplementApiRequire(
                        dir,
                        regex.getMatchedGroupByName(re_require, "module"), 
                        regex.getMatchedGroupByName(re_require, "path"), 
                        api_t,
                        re_func,
                        re_require,
                        re_return_module)
                    break
                end
                
                if regex.match(re_return_module, line_format) then
                    return_module = regex.getMatchedGroupByName(re_return_module, "module")
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