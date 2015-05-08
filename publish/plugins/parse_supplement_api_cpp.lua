local kRegexMethod = [[(?<return_type>\w+)\s+(?<method>\w+)\s*(?<param>\(.*\)))]]
local kRegexClassMethod = [[(?<return_type>\w+)\s+(?<class>\w+)::(?<method>\w+)\s*(?<param>\(.*\)))]]
local kRegexClassBegin = "((class)|(struct)|(enum))\\s+(?<name>\\w+)"
local kRegexClassEnd = [[};]]
local kRegexClassInstanceOnStack = [[(?<class>\w+)\s+(?<obj>\w+)\s*;]]
local kRegexClassInstanceOnHeap = [[\w+((\*\s+)|(\s+\*)|(\s+\*\s+)|(\s+))(?<obj>\w+)\s+=\s+new\s+(?<class>\w+)\(.*\)\s*;]]
local kRegexInclude = [[#include\s+"(?<file>.+)"]]

kIncludeDirCfgFileName = "inc_dirs"

function parseSupplementApi(filename)

    local re_method = regex.create(kRegexMethod)
    local re_class_method = regex.create(kRegexClassMethod)
    local re_class_begin = regex.create(kRegexClassBegin)
    local re_class_end = regex.create(kRegexClassEnd)
    local re_class_instance_on_stack = regex.create(kRegexClassInstanceOnStack)
    local re_class_instance_on_heap = regex.create(kRegexClassInstanceOnHeap)
    local re_include = regex.create(kRegexInclude)
        
    local includes = {}
    local apis = {}    
    parseSupplementApiInFile(filename, true, includes, apis, re_method, re_class_method, re_class_begin, re_class_end, re_class_instance_on_stack, re_class_instance_on_heap, re_include)
    
    regex.destroy(re_method)
    regex.destroy(re_class_method)
    regex.destroy(re_class_begin)
    regex.destroy(re_class_end)
    regex.destroy(re_class_instance_on_stack)
    regex.destroy(re_class_instance_on_heap)
    regex.destroy(re_include)
    
    return apis
end

function addAreas(tb, method)
    for _, v in ipairs(tb) do
        method = v .. "." .. method
    end
    return method
end

function addClassInstance(class, obj, apis)
    
    local format_class = strRelaceAll(class, "::", ".")
    local tb = {}
    for _, v in ipairs(apis) do
        if strStartWith(v, format_class .. ".") then
            local obj_method = strReplace(v, format_class, obj)
            table.insert(tb, obj_method)
        end
    end
    
    for _, v in ipairs(tb) do
        table.insert(apis, v)
    end
end

function parseSupplementApiInFile(filename, first_file, includes, apis, re_method, re_class_method, re_class_begin, re_class_end, re_class_instance_on_stack, re_class_instance_on_heap, re_include)

    for _, v in ipairs(includes) do
        if v == filename then
            return
        end
    end
    
    local class_areas = {}
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            if regex.search(re_method, strTrim(line)) and regex.getMatchedGroupByName(re_method, "return_type") ~= "new" then
                local method = addAreas(class_areas, regex.getMatchedGroupByName(re_method, "method") .. regex.getMatchedGroupByName(re_method, "param"))
                table.insert(apis, method)
            elseif regex.search(re_class_method, strTrim(line)) and regex.getMatchedGroupByName(re_method, "return_type") ~= "new" then
                local method = addAreas(class_areas, regex.getMatchedGroupByName(re_class_method, "class") .. "." .. regex.getMatchedGroupByName(re_class_method, "method") .. regex.getMatchedGroupByName(re_class_method, "param"))
                table.insert(apis, method)
            elseif regex.match(re_class_begin, strTrim(line)) then
                table.insert(class_areas,  regex.getMatchedGroupByName(re_class_begin, "name"))
            elseif regex.match(re_class_end, strTrim(line)) then
                table.remove(class_areas)
            --[[elseif first_file and regex.match(re_class_instance_on_stack, strTrim(line)) then
                local str = strTrim(line)
                if (not strStartWith(str, "return ")) and
                   (not strStartWith(str, "char ")) and
                   (not strStartWith(str, "byte ")) and
                   (not strStartWith(str, "short ")) and
                   (not strStartWith(str, "int ")) and
                   (not strStartWith(str, "long ")) and
                   (not strStartWith(str, "size_t ")) and
                   (not strStartWith(str, "bool ")) and
                   (not strStartWith(str, "delete ")) and
                   (not strStartWith(str, "uint8 ")) and
                   (not strStartWith(str, "uint16 ")) and
                   (not strStartWith(str, "uint32 ")) and
                   (not strStartWith(str, "uint64 ")) and
                   (not strStartWith(str, "signed ")) and
                   (not strStartWith(str, "unsigned ")) then                   
                   addClassInstance(regex.getMatchedGroupByName(re_class_instance_on_stack, "class"), regex.getMatchedGroupByName(re_class_instance_on_stack, "obj"), apis)
                end
            elseif first_file and regex.match(re_class_instance_on_heap, strTrim(line)) then
                addClassInstance(regex.getMatchedGroupByName(re_class_instance_on_heap, "class"), regex.getMatchedGroupByName(re_class_instance_on_heap, "obj"), apis)
            elseif regex.match(re_include, strTrim(line)) then
                
                local inc_dir = file.splitPathname(filename)
                local inc_file = inc_dir .. "/" .. regex.getMatchedGroupByName(re_include, "file")
                
                if file.isPathFile(inc_file) then
                    parseSupplementApiInFile(inc_file, false, includes, apis, re_method, re_class_method, re_class_begin, re_class_end, re_class_instance_on_stack, re_class_instance_on_heap, re_include)
                else
                    
                    local inc_dir_cfg = file.currentPath() .. "/" .. kIncludeDirCfgFileName
                    if file.isPathFile(inc_dir_cfg) then
                        local inc_dir_f = io.open(inc_dir_cfg, "r")
                        if inc_dir_f ~= nil then
                            local inc_line = inc_dir_f:read("*line")
                            while (inc_line ~= nil) do
                                inc_file = strTrim(inc_line) .. "/" .. regex.getMatchedGroupByName(re_include, "file")
                                if file.isPathFile(inc_file) then
                                    parseSupplementApiInFile(inc_file, false, includes, apis, re_method, re_class_method, re_class_begin, re_class_end, re_class_instance_on_stack, re_class_instance_on_heap, re_include)
                                    break
                                end
                                inc_line = inc_dir_f:read("*line")
                            end
                            io.close(inc_dir_f)
                        end
                    end
                end--]]
            end
            line = f:read("*line")
        end
        io.close(f)
        table.insert(includes, filename)
    end
end