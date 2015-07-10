local pattern_func = [[def%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_func_init = [[def%s+__init__%s*%((.*)%)%s*:]]
local pattern_class = [[class%s+([%w_]+)%s*:]]
local pattern_class_extend = [[class%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_import = [[import%s+([%w_%.]+)]]
local pattern_from_import = [[from%s+([%w_%.]+)%s+import%s*%*]]
local pattern_object_assignment = [[([%w_]+)%s*=%s*([%w_%.]+)%s*%(]]

--[[ Class begin --]]

local Class = {
    name_ = nil,
    module_name_ = nil,
    indent_ = nil,
    
    extends_ = {},
    functions_ = {}
}

function Class:new(name, module_name, indent)
    local o = {}
    setmetatable(o, self)
    self.__index = self
        
    o.name_ = name
    o.module_name_ = module_name
    o.indent_ = indent
    
    o.ctor_ = nil
    o.extends_ = {}
    o.functions_ = {}
    
    return o
end

function Class:clone()
    local c = Class:new(self:getName(), self:getModuleName(), self:getIndent())
    
    for i, v in ipairs(self:getExtends()) do
        c:addExtend(v)
    end
    
    for i, v in ipairs(self:getFunctions()) do
        c:addFunction(v)
    end
    
    return c
end

function Class:getName()
    return self.name_
end

function Class:setName(name)
    self.name_ = name
    
    return self
end

function Class:getModuleName()
    return self.module_name_
end

function Class:setModuleName(module_name)
    self.module_name_ = module_name
    
    return self
end

function Class:getIndent()
    return self.indent_
end

function Class:setIndent(indent)
    self.indent_ = indent
    
    return self
end

function Class:getCtor()
    return self.ctor_
end

function Class:setCtor(ctor)
    self.ctor_ = ctor
end

function Class:getFunctions()
    return self.functions_
end

function Class:addFunction(func)
    table.insert(self.functions_, func)
    
    return self
end

function Class:getExtends()
    return self.extends_
end

function Class:addExtend(super_class)
    table.insert(self.extends_, super_class)
    
    return self
end

function Class:removeExtends(super_class)
    for i, v in ipairs(self.extends_) do
        if v == super_class then
            table.remove(self.extends_, i)
            break
        end
    end
    
    return self
end

--[[ Class end --]]

function parseSupplementApi(filename, cursor_line)

    local apis = {}
    
    local path, name = file.splitPathname(filename)
    local base = file.fileBaseName(name)
    
    local functions = parseFunctions(base, path, false, true)
    local classes = parseClasses(base, path)
    
    for _, v in pairs(functions) do
        table.insert(apis, v)
    end
    
    for _, v in pairs(classes) do
        local ctor = findCtorInClass(v)
        if ctor then
            --table.insert(apis, v:getModuleName() .. "." .. v:getName() .. ctor)
            table.insert(apis, v:getName() .. ctor)
        end
    end
    
    for _, v in pairs(classes) do
        parseClassesApis(v, v, apis)
    end
    
    local objects = processCurrentFileObjects(filename, cursor_line, classes)
    for _, v in pairs(objects) do
        parseClassesApis(v, v, apis)
    end
    
    return apis
end

function findCtorInClass(cls)
    if cls:getCtor() then
        return cls:getCtor()
    end
    
    for _, v in ipairs(cls:getExtends()) do
        return findCtorInClass(v)
    end
    
    return nil
end

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

function parseClassesApis(derived, cls, apis)
    for _, v in pairs(cls:getExtends()) do
        parseClassesApis(derived, v, apis)
    end
    
    for _, v in pairs(cls:getFunctions()) do
        table.insert(apis, string.format("%s.%s", derived:getName(), v))
    end
end

function findClass(class_desc, classes)
    for _, v in pairs(classes) do
        if class_desc == v:getModuleName() .. "." .. v:getName() then
            return v
        end
    end
    
    for _, v in pairs(classes) do
        if class_desc == v:getName() then
            return v
        end
    end
    
    return nil
end

function getModuleFile(module_name, path)
    local relative_path = strRelaceAll(module_name, ".", "/")
    if relative_path == module_name then
        local filename = string.format("%s/%s.py", path, relative_path)
        if not file.isPathFile(filename) then
            filename = string.format("%s/%s.py", file.currentPath(), relative_path)
        end
        return filename
    else
        local relative_path = strRelaceAll(module_name, ".", "/")
        return string.format("%s/%s.py", file.currentPath(), relative_path)
    end
end

function parseFunctions(module_name, path, add_module_prefix, recursive, function_coll)

    local functions = function_coll or {}
    
    local filename = getModuleFile(module_name, path)
    local f = io.open(filename, "r")
    if f ~= nil then
        local filepath = file.splitPathname(filename)
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
                
                if getStartSpaceCount(line) > 0 then
                    break
                end
                
                local func, param = string.match(line, pattern_func)
                if func and param then
                    if add_module_prefix then
                        table.insert(functions, string.format("%s.%s(%s)", module_name, func, param))
                    else
                        table.insert(functions, string.format("%s(%s)", func, param))
                    end
                    
                    break
                end
                
                if recursive then
                    local from_import_module = string.match(line, pattern_from_import)
                    if from_import_module then
                        parseFunctions(from_import_module, filepath, false, false, functions)
                        break
                    end
                    
                    local import_module = string.match(line, pattern_import)
                    if import_module then
                        parseFunctions(import_module, filepath, true, false, functions)
                        break
                    end
                end
                
            until true
            line = f:read("*line")
        end
        io.close(f)
    end
    
    return functions
end

function parseClasses(module_name, path, class_coll)
    
    local classes = class_coll or {}
    local class_scope_stack = {}
    
    local filename = getModuleFile(module_name, path)
    local f = io.open(filename, "r")
    if f ~= nil then
        local filepath = file.splitPathname(filename)
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
                
                local current_class = getCurrentClassInScopeStack(class_scope_stack)
                if current_class then
                    if getStartSpaceCount(line) == current_class:getIndent() then
                        table.insert(classes, current_class)
                        table.remove(class_scope_stack)
                        -- do not break here
                    end
                end
                
                local class_name1 = string.match(line, pattern_class)
                if class_name1 then
                    local c = Class:new(class_name1, module_name, getStartSpaceCount(line))
                    table.insert(class_scope_stack, c)
                    
                    break
                end
                
                local class_name2, extends = string.match(line, pattern_class_extend)
                if class_name2 and extends then
                    local c = Class:new(class_name2, module_name, getStartSpaceCount(line))
                    local t_extends = strSplit(extends, ",")
                    for i, v in ipairs(t_extends) do
                        local super = findClass(strTrim(v), classes)
                        if super then
                            c:addExtend(super)
                        end
                    end
                    table.insert(class_scope_stack, c)
                    
                    break
                end
                
                local param_init = string.match(line, pattern_func_init)
                if param_init then
                    if #class_scope_stack > 0 then
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:setCtor(string.format("(%s)", removeSelfFromParams(param_init)))
                    end
                    break
                end
                
                local func, param = string.match(line, pattern_func)
                if func and param then
                    if #class_scope_stack > 0 then
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:addFunction(string.format("%s(%s)", func, removeSelfFromParams(param)))
                    end
                    break
                end
                
                local from_import_module = string.match(line, pattern_from_import)
                if from_import_module then
                    parseClasses(from_import_module, filepath, classes)
                    break
                end
                        
                local import_module = string.match(line, pattern_import)
                if import_module then
                    parseClasses(import_module, filepath, classes)
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
            classes[current_class:getModuleName() .. "." .. current_class:getName()] = current_class
            table.remove(class_scope_stack)
        end
    end
    
    return classes
end

function processCurrentFileObjects(filename, cursor_line, classes)
    
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
                    local flag = false
                    for _, v in pairs(classes) do
                        if v:getModuleName() .. "." .. v:getName() == right then
                            local c = v:clone()
                            c:setName(left)
                            table.insert(objects, c)
                            flag = true
                        end
                    end
                    
                    if not flag then
                        for _, v in pairs(classes) do
                            if v:getName() == right then
                                local c = v:clone()
                                c:setName(left)
                                table.insert(objects, c)
                            end
                        end
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

local apis = parseSupplementApi("./test.py", 50)
for _, v in pairs(apis) do
    print(v)
end