--[[ Class start --]]

local Class = {
    name_ = nil,
    indent_ = nil,
    extends_ = {},
    functions_ = {}
}

function Class:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    
    o.extends_ = {}
    o.functions_ = {}
    
    return o
end

function Class:clone()
    local c = Class:new()
    c:setName(self.name_)
    c:setIndent(self.indent_)
    
    for i, v in ipairs(self.extends_) do
        c:addExtend(v)
    end
    
    for i, v in ipairs(self.functions_) do
        c:addFunction(v)
    end
    
    return c
end

function Class:getName()
    return self.name_
end

function Class:getExtends()
    return self.extends_
end

function Class:getIndent()
    return self.indent_
end

function Class:setName(name)
    self.name_ = name
end

function Class:addExtend(super_class)
    table.insert(self.extends_, super_class)
end

function Class:removeExtends(super_class)
    for i, v in ipairs(self.extends_) do
        if v == super_class then
            table.remove(self.extends_, i)
            break
        end
    end
end

function Class:setIndent(indent)
    self.indent_ = indent
end

function Class:getFunctions()
    return self.functions_
end

function Class:addFunction(func)
    table.insert(self.functions_, func)
end

--[[ Class end --]]

function getStartSpaceCount(line)
    return string.len(line) - string.len(strTrimLeft(line))
end

function getCurrentClassInScopeStack(stack)
    if #stack > 0 then
        return stack[#stack]
    else
        return nil
    end
end

function getCurrentScopeFullName(stack)
    local fullname = ""
    for i=1, #stack do
        if i == 1 then
            fullname = stack[i]:getName()
        else
            fullname = fullname .. "." .. stack[i]:getName()
        end
    end
    return fullname
end

function removeSelfFromParams(params)
    local params = strTrim(params)
    if strStartWith(params, "self") then
        params = strReplace(params, "self", "")
    end
    params = strTrim(params)
    if strStartWith(params, ",") then
        params = strReplace(params, ",", "")
    end
    return strTrim(params)
end

local pattern_func = [[def%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_func_init = [[def%s+__init__%s*%((.*)%)%s*:]]
local pattern_class = [[class%s+([%w_]+)%s*:]]
local pattern_class_extend = [[class%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_import = [[import%s+([%w_%.]+)]]
local pattern_from_import = [[from%s+([%w_%.]+)%s+import%s*%*]]

function parseSupplementApi(filename, cursor_line)

    local apis = {}
    local classes = {}
    
    local imports = {}
    local from_imports = {}
    parseSupplementApiInFile(filename, "", apis, classes, imports, from_imports)
    
    local current_file_path = file.splitPathname(filename)
    for _, v in ipairs(from_imports) do
        local path = strRelaceAll(v, ".", "/")
        parseSupplementApiInFile(string.format("%s/%s.py", current_file_path, path), "", apis, classes, nil, nil)
    end
    
    for _, v in ipairs(imports) do
        local path = strRelaceAll(v, ".", "/")
        parseSupplementApiInFile(string.format("%s/%s.py", current_file_path, path), v .. ".", apis, classes, nil, nil)
    end
    
    -- process class inherits
    local classes_processed = processClassInherits(classes)
    for k, v in pairs(classes_processed) do
        for i, f in ipairs(v:getFunctions()) do
            --sendLog(string.format("%s.%s", k, f))
            table.insert(apis, string.format("%s.%s", k, f))
        end
    end
    
    -- process current file objects
    local objects = processCurrentFileObjects(filename, cursor_line,classes_processed)
    for k, v in pairs(objects) do
        for i, f in ipairs(v:getFunctions()) do
            --sendLog(string.format("Add object function: %s.%s", k, f))
            table.insert(apis, string.format("%s.%s", k, f))
        end
    end
    
    for _, v in ipairs(apis) do
        sendLog(v)
    end
    
    return apis
end

function parseSupplementApiInFile(filename, prefix, apis, classes, imports, from_imports)
    
    local class_scope_stack = {}
    
    local f = io.open(filename, "r")
    sendLog(filename)
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
            
                local current_class = getCurrentClassInScopeStack(class_scope_stack)
                if current_class then
                    if getStartSpaceCount(line) == current_class:getIndent() then
                        classes[current_class:getName()] = current_class
                        table.remove(class_scope_stack)
                        -- do not break here
                    end
                end
                
                local param_init = string.match(line, pattern_func_init)
                if param_init then
                    if #class_scope_stack > 0 then
                        --put constructor directly to apis
                        table.insert(apis, string.format("%s(%s)", current_class:getName(), removeSelfFromParams(param_init)))
                    end
                    break
                end
                
                local func, param = string.match(line, pattern_func)
                if func and param then
                    if #class_scope_stack > 0 then
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:addFunction(string.format("%s(%s)", func, removeSelfFromParams(param)))
                    else
                        table.insert(apis, string.format("%s%s(%s)", prefix, func, param))
                    end
                    break
                end
                
                local class_name1 = string.match(line, pattern_class)
                if class_name1 then
                    local c = Class:new()
                    c:setName(prefix .. class_name1)
                    c:setIndent(getStartSpaceCount(line))
                    table.insert(class_scope_stack, c)
                    break
                end
                
                local class_name2, extends = string.match(line, pattern_class_extend)
                if class_name2 and extends then
                    local c = Class:new()
                    c:setName(prefix .. class_name2)
                    c:setIndent(getStartSpaceCount(line))
                    local t_extends = strSplit(extends, ",")
                    for i, v in ipairs(t_extends) do
                        c:addExtend(strTrim(v))
                    end
                    table.insert(class_scope_stack, c)
                    break
                end
                
                if from_imports then
                    local from_import_module = string.match(line, pattern_from_import)
                    if from_import_module then
                        table.insert(from_imports, from_import_module)
                        break
                    end
                end
                
                if imports then
                    local import_module = string.match(line, pattern_import)
                    if import_module then
                        table.insert(imports, import_module)
                        break
                    end
                end
                
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    while #class_scope_stack > 0 do
        local current_class = getCurrentClassInScopeStack(class_scope_stack)
        if current_class then
            classes[current_class:getName()] = current_class
            table.remove(class_scope_stack)
        end
    end
end

function processClassInherits(classes)
    
    local classes_processed = {}
    
    while true do
        local left_count = 0
        for k, v in pairs(classes) do
            left_count = left_count + 1
            
            -- put base class into classes_processed
            if #v:getExtends() == 0 then
                classes_processed[k] = v
                classes[k] = nil
            else
                for i, e in ipairs(v:getExtends()) do                    
                    repeat
                    -- super_class is in classes_processed, process it
                        if classes_processed[e] then
                            local functions = classes_processed[e]:getFunctions()
                            for _, f in ipairs(functions) do
                                v:addFunction(f)
                            end
                            v:removeExtends(e)
                            break
                        end
                        
                        -- super_class still not put into classes_processed, keep it and wait for next loop
                        if classes[e] then
                            break
                        end
                        
                        -- not extend from current classes, ignore it.
                        v:removeExtends(e)
                    until true
                end
            end
        end -- main for
        
        if left_count == 0 then
            break
        end
    end -- while
    
    return classes_processed
end

local pattern_object_assignment = [[([%w_]+)%s*=%s*([%w_%.]+)%s*%(]]

function processCurrentFileObjects(filename, cursor_line, processed_class)
    
    local objects = {}
    local f = io.open(filename, "r")
    if f ~= nil then
        
        local line = f:read("*line")
        local current_line = 0
        while (line ~= nil and current_line < cursor_line) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
                
                local left, right = string.match(line, pattern_object_assignment)
                if left and right and left ~= right then
                    if processed_class[right] then
                        objects[left] = processed_class[right]:clone()
                    end
                    break
                end
            until true
            line = f:read("*line")
            current_line = current_line + 1
        end
        io.close(f)
    end
    
    return objects
end
