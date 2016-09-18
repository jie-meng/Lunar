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

function isKeyWord(str)
    return keywords[str] ~= nil
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
    return string.match(line_str, 'typedef%s+[%w_<>:%(%)%s]+(%s+[%w_]+)%s*;')
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

function tryGetInclude(line_str)
    local file = string.match(line_str, '#%s*include%s+[<"]([%w_%./]+)[>"]')
    local directive = strContains(line_str, "<")
    return file, directive
end

function getProjectSrcAbsoluteDir(project_src_dir)
    if strTrim(project_src_dir) == "" then
        return file.currentPath()
    else
        return string.format("%s/%s", file.currentPath(), project_src_dir)
    end
end

function parseFile(coll, parsed_files, project_src_dir, filename, current_line_index, inc_path, recuresive)
    if parsed_files[filename] then
        return false
    end
    parsed_files[filename] = true
    
    local f = io.open(filename, "r")
    if f then
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
                
                local inc, directive = tryGetInclude(trimmed_line)
                if inc then
                    table.insert(inc_coll, { file = inc, find = directive })
                end 
                
                local get_value = tryGetFuncWithReturnType(trimmed_line)
                if not inc and not get_value then
                    get_value = tryGetClassOrStruct(trimmed_line)
                end
                
                if not inc and not get_value then
                    get_value = tryGetMacro(trimmed_line)
                end
                
                if not inc and not get_value then
                    get_value = tryGetTypedef(trimmed_line)
                end
                
                if not inc and not get_value then
                    get_value = tryGetFunc(trimmed_line, previous_line_str)
                end
                
                if get_value then
                    coll[get_value] = true
                end
                previous_line_str = trimmed_line
            until true
            
            readline = f:read("*line")
            line_index = line_index + 1
        end
        
        -- cannot find text in current file, then parse include files
        for _, inc in ipairs(inc_coll) do
            if inc.find then
                -- directive path <>                    
                for _, v in ipairs(inc_path) do
                    if v.find then
                        parseFile(coll, parsed_files, project_src_dir, v.path .. "/" .. inc.file, current_line_index, inc_path, true)
                    end
                end
            else
                -- relative path ""
                local current_file_path, current_file_name = file.splitPathname(filename)
                local project_dir_absolute = getProjectSrcAbsoluteDir(project_src_dir)
                
                if file.isPathFile(current_file_path .. "/" .. inc.file) then
                    parseFile(coll, parsed_files, project_src_dir, current_file_path .. "/" .. inc.file, current_line_index, inc_path, true)
                elseif file.isPathFile(project_dir_absolute .. "/" .. inc.file) then
                    parseFile(coll, parsed_files, project_src_dir, project_dir_absolute .. "/" .. inc.file, current_line_index, inc_path, true)
                else                        
                    for _, v in ipairs(inc_path) do
                        if not v.find then
                            parseFile(coll, parsed_files, project_src_dir, project_dir_absolute .. "/" .. v.path .. "/" .. inc.file, current_line_index, inc_path, true)
                        end
                    end
                end
            end
        end
        
        io.close(f)
    end
end

function parseSupplementApi(filename, cursor_line, project_src_dir)
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
    parseFile(coll, parsed_files, project_src_dir, filename, line, inc_path)
    
    local apis = {}
    for k, v in pairs(coll) do
        table.insert(apis, k)
    end
    table.sort(apis)
    
    return apis
end
