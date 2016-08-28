function tryGetFuncWithReturnType(line_str)
    local return_type, func_name = string.match(line_str, '([%w_]+)%s+([%w_:]+)%s*%(')
    if return_type ~= "return" and func_name then
        return func_name
    end
    
    return_type, func_name = string.match(line_str, '([%w_]+)%s+([%w_:]+)%s*(<[%w_:%s]*>)%s*%(')
    if return_type ~= "return" and func_name then
        return func_name
    end
end

function tryGetClassOrStruct(line_str)
    local class_name = string.match(line_str, 'class%s+([%w_]+)')
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
    return string.match(line_str, '#%s*define%s+([%w_]+)')
end

function tryGetTypedef(line_str)
    return string.match(line_str, 'typedef%s+[%w_<>:%s]+(%s+[%w_]+)%s*;')
end

function tryGetFunc(line_str, previous_line_str)
    local func_name = string.match(line_str, '([%w_:]+)%s*%(')
    if func_name
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return func_name
    end
    
    func_name = string.match(line_str, '([%w_:]+)%s*(<[%w_:%s]*>)%s*%(')
    if func_name
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return func_name
    end
end

function tryGetInclude(line_str)
    local file = string.match(line_str, '#%s*include%s+[<"]([%w_%./]+)[>"]')
    local directive = strContains(line_str, "<")
    return file, directive
end

function parseFile(coll, project_src_dir, filename, current_line_index, inc_path, recuresive)
    local found = false
    local f = io.open(filename, "r")
    if f then
        local current_file_path, current_file_name = file.splitPathname(filename)
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
                        parseFile(coll, project_src_dir, v.path .. "/" .. inc.file, current_line_index, inc_path, true)
                    end
                end
            else
                -- relative path ""
                if file.isPathFile(current_file_path .. "/" .. inc.file) then
                    parseFile(coll, project_src_dir, current_file_path .. "/" .. inc.file, current_line_index, inc_path, true)
                elseif file.isPathFile(project_src_dir .. "/" .. inc.file) then
                    parseFile(coll, project_src_dir, project_src_dir .. "/" .. inc.file, current_line_index, inc_path, true)
                else                        
                    for _, v in ipairs(inc_path) do
                        if not v.find then
                            parseFile(coll, project_src_dir, project_src_dir .. "/" .. v.path .. "/" .. inc.file, current_line_index, inc_path, true)
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
    local cfg = io.open(project_src_dir .. "/" .. "lunar_cpp.cfg", "r")
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
    parseFile(coll, project_src_dir, filename, line, inc_path)
    
    local apis = {}
    for k, v in pairs(coll) do
        table.insert(apis, k)
    end
    table.sort(apis)
    
    return apis
end
