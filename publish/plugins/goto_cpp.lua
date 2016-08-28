function isFuncWithReturnType(line_str, text)
    local return_type, func_name = string.match(line_str, '([%w_]+)%s+(' .. text .. ')%s*%(')
    if return_type ~= "return" and func_name then
        return true
    end
    
    return_type, func_name = string.match(line_str, '([%w_]+)%s+(' .. text .. ')%s*(<[%w_:%s]*>)%s*%(')
    if return_type ~= "return" and func_name then
        return true
    end
end

function isMethodWithReturnType(line_str, text)
    local return_type, func_name = string.match(line_str, '([%w_]+)%s+[%w_]+::(' .. text .. ')%s*%(')
    if return_type ~= "return" and func_name then
        return true
    end
    
    return_type, func_name = string.match(line_str, '([%w_]+)%s+[%w_]+::(' .. text .. ')%s*(<[%w_:%s]*>)%s*%(')
    if return_type ~= "return" and func_name then
        return true
    end
end

function isClassOrStruct(line_str, text)
    return string.match(line_str, 'class%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'struct%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'enum%s+(' .. text .. ')') ~= nil or
        string.match(line_str, 'union%s+(' .. text .. ')') ~= nil
end

function isMacro(line_str, text)
    return string.match(line_str, '#%s*define%s+(' .. text .. ')') ~= nil
end

function isTypedef(line_str, text)
    return string.match(line_str, 'typedef%s+[%w_<>:%s]+(%s+' .. text .. ')%s*;') ~= nil
end

function isFunc(line_str, previous_line_str, text)
    if string.match(line_str, '(' .. text .. ')%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return true
    end
    
    if string.match(line_str, '(' .. text .. ')%s*(<[%w_:%s]*>)%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return true
    end
end

function isMethod(line_str, previous_line_str, text)
    if string.match(line_str, '[%w_]+::(' .. text .. ')%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return true
    end
    
    if string.match(line_str, '[%w_]+::(' .. text .. ')%s*(<[%w_:%s]*>)%s*%(') ~= nil 
        and string.match(previous_line_str, '([%w_]+)') == previous_line_str
        and previous_line_str ~= "if"
        and previous_line_str ~= "else"
        and previous_line_str ~= "elseif"
        and previous_line_str ~= "return" then
        return true
    end
end

function isInclude(line_str)
    local file = string.match(line_str, '#%s*include%s+[<"]([%w_%./]+)[>"]')
    local directive = strContains(line_str, "<")
    return file, directive
end

function parseFile(coll, filename, current_line_index, text, inc_path, recuresive)
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
                
                local matched = false
                local inc, directive = isInclude(trimmed_line)
                if inc then
                    if not recuresive and current_line_index == line_index then
                        --goto include file directly
                        if file.isPathFile(current_file_path .. "/" .. inc) then
                            local item = string.format("%s\n%d\n%s", current_file_path .. "/" .. inc, 1, inc)
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
                                        if file.isPathFile(file.currentPath() .. "/" .. v.path .. "/" .. inc) then
                                            local item = string.format("%s\n%d\n%s", file.currentPath() .. "/" .. v.path .. "/" .. inc, 1, inc)
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
                elseif isFuncWithReturnType(trimmed_line, text) then
                    matched = true
                elseif isMethodWithReturnType(trimmed_line, text) then
                    matched = true
                elseif isClassOrStruct(trimmed_line, text) then
                    matched = true
                elseif isMacro(trimmed_line, text) then
                    matched = true
                elseif isTypedef(trimmed_line, text) then
                    matched = true
                elseif isFunc(trimmed_line, previous_line_str, text) then
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
                            if parseFile(coll, v.path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                                found = true
                            end
                        end
                    end
                else
                    -- relative path ""
                    if file.isPathFile(current_file_path .. "/" .. inc.file) then
                        if parseFile(coll, current_file_path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                            found = true
                        end
                    elseif file.isPathFile(file.currentPath() .. "/" .. inc.file) then
                        if parseFile(coll, file.currentPath() .. "/" .. inc.file, current_line_index, text, inc_path, true) then
                            found = true
                        end
                    else                        
                        for _, v in ipairs(inc_path) do
                            if found then
                                break
                            end
                            
                            if not v.find then
                                if parseFile(coll, file.currentPath() .. "/" .. v.path .. "/" .. inc.file, current_line_index, text, inc_path, true) then
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
    local cfg = io.open(file.currentPath() .. "/" .. "lunar_cpp.cfg", "r")
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
    parseFile(coll, filename, line, text, inc_path)
    
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


--[[test]]
--local test_string = "void func(int a = 0)"
--local ret = string.match(test_string, pattern_func_with_return_type)
--if ret then
--    print(ret)
--end

--local test_string = "func("
--local ret = string.match(test_string, pattern_func)
--if ret then
--    print(ret)
--end

--test_string = "class A"
--ret = string.match(test_string, pattern_class)
--if ret then
--    print(ret)
--end

--test_string = "struct T"
--ret = string.match(test_string, pattern_struct)
--if ret then
--    print(ret)
--end

--test_string = "#define PI (3.14)"
--ret = string.match(test_string, pattern_define)
--if ret then
--    print(ret)
--end

--test_string = "typedef unsigned int INT;"
--ret = string.match(test_string, pattern_typedef)
--if ret then
--    print(ret)
--end

--test_string = "#include <base/test.hpp>"
--ret = string.match(test_string, pattern_include)
--if ret then
--    print(ret)
--end

--test_string = [[#include "base/test.hpp"]]
--ret = string.match(test_string, pattern_include)
--if ret then
--    print(ret)
--end
