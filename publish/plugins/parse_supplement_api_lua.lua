kRegexFunctionLua = "function\\s+(?<api>(\\w+((\\.|:)\\w+)*\\s*\\(.*\\)))"
kRegexIncludeLua = "include\\(\"(?<path>.+)\"\\)"
kRegexRequireLua = "(?<module>\\w+)\\s*=\\s*require\\(\"(?<path>.+)\"\\)"
kRegexReturnModuleLua = "return\\s+(?<module>\\w+)"

function parseSupplementApi(filename)
    
    local dir, name = file.splitPathname(filename)
    local apis = {}
    
    parseSupplementApiRecursively(filename, dir, apis)
    
    return apis
end

function parseSupplementApiRecursively(filename, dir, apis)

    local re_func = regex.create(kRegexFunctionLua)
    local re_inc = regex.create(kRegexIncludeLua)
    local re_require = regex.create(kRegexRequireLua)
    
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
            elseif regex.match(re_inc, strTrim(line)) then
                local file_include = dir .. "/" .. regex.getMatchedGroupByName(re_inc, "path")
                
                if file.isPathFile(file_include) then
                    parseSupplementApiRecursively(file_include, dir, apis)
                end
            elseif regex.match(re_require, strTrim(line)) then
                parseSupplementApiRequire(
                    dir,
                    regex.getMatchedGroupByName(re_require, "module"), 
                    regex.getMatchedGroupByName(re_require, "path"), 
                    apis,
                    re_func)
            end
            line = f:read("*line")
        end
        io.close(f)
    end
    
    regex.destroy(re_func)
    regex.destroy(re_inc)
    regex.destroy(re_require)
end

function parseSupplementApiRequire(dir, module_name, require_name, api_t, re_func)
    
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
        local re_return_module = regex.create(kRegexReturnModuleLua)
        local tmp_t = {}
        local return_module = nil
        
        local line = f:read("*line")
        while (line ~= nil) do
            
            if string.len(strTrim(line)) ~= 0 then
                return_module = nil
            end
            
            if regex.match(re_func, strTrim(line)) then
                local api = regex.getMatchedGroupByName(re_func, "api")
                if api ~= "" then
                    local api_format, _ = string.gsub(api, ":", ".")
                    table.insert(tmp_t, api_format)
                end
            elseif regex.match(re_return_module, strTrim(line)) then
                return_module = regex.getMatchedGroupByName(re_return_module, "module")
            end
            
            --next line
            line = f:read("*line")
        end
        
        if return_module ~= nil then
            for k,v in pairs(tmp_t) do
                if strStartWith(v, return_module .. ".") then
                    table.insert(api_t, strReplace(v, return_module .. ".", module_name .. "."))
                else
                    table.insert(api_t, v)
                end
            end
        else
            for k,v in pairs(tmp_t) do
                table.insert(api_t, v)
            end
        end

        regex.destroy(re_return_module)
        io.close(f)
    end
end