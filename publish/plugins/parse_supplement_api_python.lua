--[[ Class start --]]

local Class = {
    name_ = nil,
    extends_ = {},
    indent_ = 0,
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

function Class:addExtends(super_class)
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

function parseSupplementApi(filename)

    local apis = {}
    local classes = {}
    
    local current_path = file.currentPath()
    local files = file.findFilesInDirRecursively(current_path, "py")
    
    for i, v in ipairs(files) do
        parseSupplementApiInFile(v, apis, classes)
    end
    
    processClassInherits(apis, classes)
    
    return apis
end

function parseSupplementApiInFile(filename, apis, classes)
    
    local class_scope_stack = {}
    
    local f = io.open(filename, "r")
    if f ~= nil then
        
        local line = f:read("*line")
        while (line ~= nil) do
            
            repeat
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
                        table.insert(apis, string.format("%s(%s)", getCurrentScopeFullName(class_scope_stack), removeSelfFromParams(param_init)))
                        --sendLog(string.format("%s(%s)", getCurrentScopeFullName(class_scope_stack), removeSelfFromParams(param_init)))
                    end
                    break
                end
                
                local func, param = string.match(line, pattern_func)
                if func and param then
                    if #class_scope_stack > 0 then
                        --sendLog(string.format("%s.%s(%s)", getCurrentScopeFullName(class_scope_stack), func, removeSelfFromParams(param)))
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:addFunction(string.format("%s(%s)", func, removeSelfFromParams(param)))
                    else
                        table.insert(apis, string.format("%s(%s)", func, param))
                        --sendLog(string.format("%s(%s)", func, param))
                    end
                    break
                end
                
                local class_name1 = string.match(line, pattern_class)
                if class_name1 then
                    local c = Class:new()
                    c:setName(class_name1)
                    c:setIndent(getStartSpaceCount(line))
                    table.insert(class_scope_stack, c)
                    break
                end
                
                local class_name2, extends = string.match(line, pattern_class_extend)
                if class_name2 and extends then
                    local c = Class:new()
                    c:setName(class_name2)
                    c:setIndent(getStartSpaceCount(line))
                    local t_extends = strSplit(extends, ",")
                    --sendLog("name: " .. class_name2)
                    for i, v in ipairs(t_extends) do
                        --sendLog(strTrim(v))
                        c:addExtends(strTrim(v))
                    end
                    table.insert(class_scope_stack, c)
                    break
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

function processClassInherits(apis, classes)
    
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
    
    for k, v in pairs(classes_processed) do
        for i, f in ipairs(v:getFunctions()) do
            --sendLog(string.format("%s.%s", k, f))
            table.insert(apis, string.format("%s.%s", k, f))
        end
    end
end
