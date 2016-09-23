local keywords = {}
keywords["if"] = true
keywords["else"] = true
keywords["elseif"] = true
keywords["switch"] = true
keywords["new"] = true
keywords["delete"] = true
keywords["operator"] = true
keywords["return"] = true
keywords["try"] = true
keywords["catch"] = true
keywords["throw"] = true
keywords["typedef"] = true

function isKeyWord(str)
    return keywords[str]
end

function isFuncWithReturnType(line_str, text)
    local return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+(' .. text .. ')%s*%(')
    if not isKeyWord(return_type) and func_name then
        return true
    end
    
    return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+(' .. text .. ')%s*(<[%w_:%s&%*,]*>)%s*%(')
    if not isKeyWord(return_type) and func_name then
        return true
    end
    
    return false
end

function isMethodWithReturnType(line_str, text)
    local return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+[%w_]+%s*::%s*(' .. text .. ')%s*%(')
    if not isKeyWord(return_type) and func_name then
        return true
    end
    
    return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+[%w_]+%s*(<[%w_:%s&%*,]*>)%s*::(' .. text .. ')%s*%(')
    if not isKeyWord(return_type) and func_name then
        return true
    end
    
    return false
end

function isClassOrStruct(line_str, text)
    return string.match(line_str, 'class%s+[%w_%(%)]+%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'struct%s+[%w_%(%)]+%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'enum%s+[%w_%(%)]+%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'union%s+[%w_%(%)]+%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'class%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'struct%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'enum%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'union%s+(' .. text .. ')') ~= nil
end

function isMacro(line_str, text)
    return string.match(line_str, '#define%s+(' .. text .. ')') ~= nil
end

function isTypedef(line_str, text)
    return string.match(line_str, 'typedef%s+[%w_<>%(%):%s]+(%s+' .. text .. ')%s*;') ~= nil
end

function isFunc(line_str, previous_line_str, text)
    if string.match(line_str, '(' .. text .. ')%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return true
    end
    
    if string.match(line_str, '(' .. text .. ')%s*(<[%w_:%s&%*,]*>)%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return true
    end
    
    return false
end

function isMethod(line_str, previous_line_str, text)
    if string.match(line_str, '[%w_]+%s*::%s*(' .. text .. ')%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return true
    end
    
    if string.match(line_str, '[%w_]+%s*(<[%w_:%s&%*,]*>)%s*::%s*(' .. text .. ')%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return true
    end
    
    return false
end

function isInclude(line_str)
    local file = string.match(line_str, '#%s*include%s+[<"]([%w_%./]+)[>"]')
    local directive = strContains(line_str, "<")
    return file, directive
end

function tryGetFuncWithReturnType(line_str)
    local return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+([%w_:]+)%s*%(')
    if not isKeyWord(return_type) and func_name then
        return func_name
    end
    
    return_type, func_name = string.match(line_str, '([%w_&%*]+)%s+([%w_:]+)%s*(<[%w_:%s&%*,]*>)%s*%(')
    if not isKeyWord(return_type) and func_name then
        return func_name
    end
end

function tryGetClassOrStruct(line_str)
    local class_name = string.match(line_str, 'class%s+[%w_%(%)]+%s+([%w_]+)')
    if class_name then
        return class_name
    end

    class_name = string.match(line_str, 'struct%s+[%w_%(%)]+%s+([%w_]+)')
    if class_name then
        return class_name
    end
    
    class_name = string.match(line_str, 'enum%s+[%w_%(%)]+%s+([%w_]+)')
    if class_name then
        return class_name
    end
    
    class_name = string.match(line_str, 'union%s+[%w_%(%)]+%s+([%w_]+)')
    if class_name then
        return class_name
    end
    
    class_name = string.match(line_str, 'class%s+([%w_]+)')
    if class_name then
        return class_name
    end

    class_name = string.match(line_str, 'struct%s+([%w_]+)')
    if class_name then
        return class_name
    end
    
    class_name = string.match(line_str, 'enum%s+([%w_]+)')
    if class_name then
        return class_name
    end
    
    class_name = string.match(line_str, 'union%s+([%w_]+)')
    if class_name then
        return class_name
    end
end

function tryGetMacro(line_str)
    return string.match(line_str, '#define%s+([%w_]+)')
end

function tryGetTypedef(line_str)
    return strTrim(string.match(line_str, 'typedef%s+[%w_<>:%(%)%s]+(%s+[%w_]+)%s*;'))
end

function tryGetFunc(line_str, previous_line_str)
    local func_name = string.match(line_str, '([%w_:]+)%s*%(')
    if func_name
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return func_name
    end
    
    func_name = string.match(line_str, '([%w_:]+)%s*(<[%w_:%s&%*,]*>)%s*%(')
    if func_name
        and string.match(previous_line_str, '([%w_&%*]+)') == previous_line_str
        and not isKeyWord(previous_line_str) then
        return func_name
    end
end

function getProjectSrcAbsoluteDir(project_src_dir)
    if strTrim(project_src_dir) == "" then
        return file.currentPath()
    else
        return string.format("%s/%s", file.currentPath(), project_src_dir)
    end
end

function parseFile(coll, parsed_files, project_src_dir, filename, current_line_index, text, inc_path, recuresive)
    if parsed_files[filename] then
        return false
    end
    parsed_files[filename] = true
    
    local found = false
    local f = io.open(filename, "r")
    if f then
        local current_file_path, current_file_name = file.splitPathname(filename)
        local project_dir_absolute = getProjectSrcAbsoluteDir(project_src_dir)
        local readline = f:read("*line")
        local line_index = 1
        local previous_line_str = ""
        local inc_coll = {}
        while readline do
            repeat
                local trimmed_line = strTrim(readline)
                if trimmed_line == "" or strStartWith(trimmed_line, "//") then
                    break
                end
                
                local matched = false
                local inc, directive = isInclude(trimmed_line)
                if inc then
                    if not recuresive and current_line_index == line_index then
                        --goto include file directly
                        if file.isPathFile(current_file_path .. "/" .. inc) then
                            local item = string.format("%s\n%d\n%s", current_file_path .. "/" .. inc, 1, inc)
                            coll[item] = true
                            found = true
                        elseif file.isPathFile(project_dir_absolute .. "/" .. inc) then
                            local item = string.format("%s\n%d\n%s", project_dir_absolute .. "/" .. inc, 1, inc)
                            coll[item] = true
                            found = true
                        else                            
                            for _, v in ipairs(inc_path) do
                                if v.find == directive then
                                    if directive then
                                        if file.isPathFile(v.path .. "/" .. inc) then
                                            local item = string.format("%s\n%d\n%s", v.path .. "/" .. inc, 1, inc)
                                            coll[item] = true
                                            found = true
                                        end
                                    else
                                        if file.isPathFile(project_dir_absolute .. "/" .. v.path .. "/" .. inc) then
                                            local item = string.format("%s\n%d\n%s", project_dir_absolute .. "/" .. v.path .. "/" .. inc, 1, inc)
                                            coll[item] = true
                                            found = true
                                        end
                                    end
                                end
                            end
                        end
                    else
                        table.insert(inc_coll, { file = inc, find = directive })
                    end
                elseif isFuncWithReturnType(trimmed_line, text) and tryGetFuncWithReturnType(trimmed_line) == text then
                    matched = true
                elseif isMethodWithReturnType(trimmed_line, text) then
                    matched = true
                elseif not strEndWith(trimmed_line, ";") and isClassOrStruct(trimmed_line, text) and tryGetClassOrStruct(trimmed_line) == text then
                    matched = true
                elseif isMacro(trimmed_line, text) and tryGetMacro(trimmed_line) == text then
                    matched = true
                elseif isTypedef(trimmed_line, text) and tryGetTypedef(trimmed_line) == text then
                    matched = true
                elseif isFunc(trimmed_line, previous_line_str, text) and tryGetFunc(trimmed_line, previous_line_str) == text then
                    matched = true
                elseif isMethod(trimmed_line, previous_line_str, text) then
                    matched = true
                end                
                
                if matched then
                    local item = string.format("%s\n%d\n%s", filename, line_index, readline)
                    coll[item] = true
                    found = true
                end
                previous_line_str = trimmed_line
            until true
            
            readline = f:read("*line")
            line_index = line_index + 1
        end
        
        if not found then
            -- cannot find text in current file, then parse include files
            for _, inc in ipairs(inc_coll) do
                if found then
                    break
                end
                
                if inc.find then
                    -- directive path <>                    
                    for _, v in ipairs(inc_path) do
                        if found then
                            break
                        end
                        
                        if v.find then
                            if parseFile(coll, parsed_files, project_src_dir, v.path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                                found = true
                            end
                        end
                    end
                else
                    -- relative path ""
                    if file.isPathFile(current_file_path .. "/" .. inc.file) then
                        if parseFile(coll, parsed_files, project_src_dir, current_file_path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                            found = true
                        end
                    elseif file.isPathFile(project_dir_absolute .. "/" .. inc.file) then
                        if parseFile(coll, parsed_files, project_src_dir, project_dir_absolute .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                            found = true
                        end
                    else                        
                        for _, v in ipairs(inc_path) do
                            if found then
                                break
                            end
                            
                            if not v.find then
                                if parseFile(coll, parsed_files, project_src_dir, project_dir_absolute .. "/" .. v.path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                                    found = true
                                end
                            end
                        end
                    end
                end
            end
        end
        
        io.close(f)
    end
    
    return found
end

function gotoDefinition(text, line, filename, project_src_dir)
    local inc_path = {}
	table.insert(inc_path, { path = project_src_dir, find = false})
    local cfg = io.open(getProjectSrcAbsoluteDir(project_src_dir) .. "/" .. "lunar_cpp.cfg", "r")
    if cfg then
        local region = nil
        local readline = cfg:read("*line")
        while readline do
            repeat
                local trimmed_line = strTrim(readline)
                if trimmed_line == "" or strStartWith(trimmed_line, "#") then
                    break
                end 
                
                if trimmed_line == "INCLUDEPATH:" then
                    region = "INCLUDEPATH"
                else
                    if region == "INCLUDEPATH" then
                        if strStartWith(trimmed_line, "/") or ( string.len(trimmed_line) > 2 and string.sub(trimmed_line, 2, 2) == ":") then
                            table.insert(inc_path, { path = trimmed_line, find = true})
                        else
                            table.insert(inc_path, { path = trimmed_line, find = false})
                        end
                    end
                end
            until true
            
            readline = cfg:read("*line")
        end
        io.close(cfg)
    end
    
    local coll = {}
    local parsed_files = {}
    parseFile(coll, parsed_files, project_src_dir, filename, line, text, inc_path)
    
    local results = {}
    for k, v in pairs(coll) do
        table.insert(results, k)
    end
    table.sort(results, 
        function (a, b)
            local array_a = strSplit(a, "\n")
            local array_b = strSplit(b, "\n")
            
            if array_a[1] == array_b[1] then
                return  tonumber(array_a[2]) < tonumber(array_b[2])
            else
                return array_a[1] < array_b[1]
            end
        end)
    
    return results
end
