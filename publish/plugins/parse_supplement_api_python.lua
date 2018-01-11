local pattern_func = [[def%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_func_init = [[def%s+__init__%s*%((.*)%)%s*:]]
local pattern_class = [[class%s+([%w_]+)%s*:]]
local pattern_class_extend = [[class%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_class_static_field = [[([%w_]+)%s*=%s*(.+)]]
local pattern_import = [[import%s+([%w_%.]+)]]
local pattern_from_import = [[from%s+([%w_%.]+)%s+import]]

--[[ Class begin --]]

local Class = {
    name_ = nil,
    class_name_ = nil,
    module_name_ = nil,
    indent_ = nil,
    
    extends_ = {},
    functions_ = {},
    static_fields_ = {}
}

function Class:new(name, module_name, indent)
    local o = {}
    setmetatable(o, self)
    self.__index = self
        
    o.name_ = name
    o.class_name_ = name
    o.module_name_ = module_name
    o.indent_ = indent
    
    o.ctor_ = nil
    o.extends_ = {}
    o.functions_ = {}
    o.static_fields_ = {}
    
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

function Class:getClassName()
    return self.class_name_
end

function Class:setClassName(name)
    self.class_name_ = name
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

function Class:getStaticFields()
    return self.static_fields_
end

function Class:addStaticField(f)
    table.insert(self.static_fields_, f)
end

--[[ Class end --]]

--[[ Import begin ]]

local Import = {
    name_ = nil,
    is_from_import_ = false,
    from_import_component_ = nil,
    import_as_ = nil
}

function Import:new(name, is_from_import)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o.name_ = name
    o.is_from_import_ = is_from_import
    
    return o
end

function Import:getName()
    return self.name_
end

function Import:setName(name)
    self.name_ = name
    return self
end

function Import:isFromImport()
    return self.is_from_import_
end

function Import:setFromImport(is_from_import)
    self.is_from_import_ = is_from_import
    return self
end

function Import:getFromImportComponent()
    return self.from_import_component_
end

function Import:setFromImportComponent(from_import_component)
    self.from_import_component_ = from_import_component
    return self
end

function Import:getImportAs()
    return self.import_as_
end

function Import:setImportAs(import_as)
    self.import_as_ = import_as
    return self
end

--[[ Import end ]]

function parsePydocGenApi(apis, imports)
    local api_trimmer = {}
    for module_name, module in pairs(imports) do
        local app_path, _ = util.splitPathname(util.appPath())
        local pydoc_gen_path = app_path .. '/apis/python/pydoc_gen'
        if util.isPathFile(pydoc_gen_path .. '/' .. module_name) then
            if module:isFromImport() then
                local f = io.open(pydoc_gen_path .. '/' .. module_name, "r")
                if f then
                    local line = f:read("*line")
                    while line do
                        if string.len(util.strTrim(line)) > 1 then
                            api_trimmer[line] = line
                        end
                        line = f:read("*line")
                    end
                    io.close()
                end

                --parse imported packages
                if not util.strContains(module:getFromImportComponent(), '*') then
                    local components = util.strSplit(module:getFromImportComponent(), ',')
                    for _, v in ipairs(components) do
                        local f = io.open(pydoc_gen_path .. '/' .. module_name .. '.' .. util.strTrim(v), "r")
                        if f then
                            local line = f:read("*line")
                            while line do
                                if string.len(util.strTrim(line)) > 1 then
                                    api_trimmer[line] = line
                                end
                                line = f:read("*line")
                            end
                            io.close()
                        end
                    end
                end
            else
                local f = io.open(pydoc_gen_path .. '/' .. module_name, "r")
                if f then
                    local line = f:read("*line")
                    while line do
                        if string.len(util.strTrim(line)) > 1 then
                            if module:getImportAs() then
                                module_name = module:getImportAs()
                            end
                            api_trimmer[module_name .. '.' .. line] = module_name .. '.' .. line
                        end
                        line = f:read("*line")
                    end
                    io.close()
                end
            end
        end
    end

    --insert imports apis
    for _, v in pairs(api_trimmer) do
        table.insert(apis, v)
    end
end

function parseSupplementApi(filename, cursor_line, project_src_dir)

    local apis = {}
    
    local path, name = util.splitPathname(filename)
    local base = util.fileBaseName(name)
    
    local search_path = util.currentPath()
    if util.strTrim(project_src_dir) ~= "" then
        search_path = search_path .. "/" .. project_src_dir
    end
    
    local imports = parseImports(filename)
    --current file api is exactly same as from import * format
    imports[""] = Import:new("", true)
    local functions = parseFunctions(base, path, search_path, false, true)
    local classes = parseClasses(base, path, search_path, true)
    
    for _, v in pairs(functions) do
        table.insert(apis, v)
    end
    
    -- ctors
    for _, v in pairs(classes) do
        if imports[v:getModuleName()] then
            parseCtorApis(apis, imports, v)
        end
    end
    
    for _, v in pairs(classes) do
        if imports[v:getModuleName()] then
            parseClassesApis(apis, imports, v, v, false)
        end
    end
    
    --parse pydoc_gen api
    for module_name, module in pairs(imports) do
        local app_path, _ = util.splitPathname(util.appPath())
        local pydoc_gen_path = app_path .. '/apis/python/pydoc_gen'
        local f = io.open(pydoc_gen_path .. '/' .. module_name, "r")
        if f then
            local line = f:read("*line")
            while line do
                if module:isFromImport() then
                    table.insert(apis, line)
                else
                    table.insert(apis, module_name .. '.' .. line)
                end
                line = f:read("*line")
            end
            io.close()
        end
    end

    parsePydocGenApi(apis, imports)
    
    return apis
end

function findCtorInClass(cls)
    if cls:getCtor() then
        return cls:getCtor()
    end
    
    for _, v in ipairs(cls:getExtends()) do
        local ctor = findCtorInClass(v)
        if ctor then
            return ctor
        end
    end
    
    return nil
end

function getStartSpaceCount(line)
    return string.len(line) - string.len(util.strTrimLeft(line))
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
    local params = util.strTrim(params)
    if util.strStartWith(params, "self") then
        params = util.strReplace(params, "self", "")
    end
    params = util.strTrim(params)
    if util.strStartWith(params, ",") then
        params = util.strReplace(params, ",", "")
    end
    return util.strTrim(params)
end

function parseCtorApis(apis, imports, cls)
    local ctor = findCtorInClass(cls)
    if ctor then
        if imports[cls:getModuleName()]:isFromImport() then
            table.insert(apis, cls:getName() .. ctor)
        else
            table.insert(apis, cls:getModuleName() .. "." .. cls:getName() .. ctor)
        end
    end
end

function parseClassesApis(apis, imports, derived, cls, is_object)
    for _, v in pairs(cls:getExtends()) do
        parseClassesApis(apis, imports, derived, v, is_object)
    end
    
    for _, v in pairs(cls:getFunctions()) do
        if is_object or imports[derived:getModuleName()]:isFromImport() then
            table.insert(apis, string.format("%s.%s", derived:getName(), v))
        else
            table.insert(apis, string.format("%s.%s", derived:getModuleName() .. "." .. derived:getName(), v))
        end
    end
    
    for _, v in pairs(cls:getStaticFields()) do
        if is_object or imports[derived:getModuleName()]:isFromImport() then
            table.insert(apis, string.format("%s.%s", derived:getName(), v))
        else
            table.insert(apis, string.format("%s.%s", derived:getModuleName() .. "." .. derived:getName(), v))
        end
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

function getModuleFile(module_name, path, search_path)
    local relative_path = util.strReplaceAll(module_name, ".", "/")
    
    -- check relative path first
    local filename = string.format("%s/%s.py", path, relative_path)
    if util.isPathFile(filename) then
        return filename
    end
    
    -- then check from currentpath
    filename = string.format("%s/%s.py", search_path, relative_path)
    if util.isPathFile(filename) then
        return filename
    end
    
    return nil
end

function parseImports(filename)
    local imports = {}
    
    local f = io.open(filename, "r")
    if f then
        local line = f:read("*line")
        while line do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "#") then
                    break
                end
                
                if getStartSpaceCount(line) > 0 then
                    break
                end
                
                if (not util.strStartWith(line, "import")) and (not util.strStartWith(line, "from")) then
                    break
                end
                
                local from_import_module = string.match(line, pattern_from_import)
                if from_import_module then
                    local _, stop = string.find(line, pattern_from_import)
                    imports[from_import_module] = Import:new(from_import_module, true)
                    imports[from_import_module]:setFromImportComponent(string.sub(line, stop + 1))
                    break
                end
                
                local import_module = string.match(line, pattern_import)
                if import_module then
                    imports[import_module] = Import:new(import_module, false)
                    local _, stop = string.find(line, pattern_import)
                    local left_info = util.strTrim(string.sub(line, stop + 1))
                    if util.strStartWith(left_info, 'as ') then
                        local as_import = util.strTrim(util.strReplace(left_info, 'as', ''))
                        imports[import_module]:setImportAs(as_import)
                    end
                    break
                end
            
            until true
            line = f:read("*line")
        end
        io.close()
    end
    
    return imports
end

function parseFunctions(module_name, path, search_path, add_module_prefix, recursive, function_coll)

    local functions = function_coll or {}
    
    local filename = getModuleFile(module_name, path, search_path)
    if not filename then
        return functions
    end
    
    local f = io.open(filename, "r")
    if f then
        local filepath = util.splitPathname(filename)
        local line = f:read("*line")
        while line do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "#") then
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
                        parseFunctions(from_import_module, filepath, search_path, false, false, functions)
                        break
                    end
                    
                    local import_module = string.match(line, pattern_import)
                    if import_module then
                        parseFunctions(import_module, filepath, search_path, true, false, functions)
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

function parseClasses(module_name, path, search_path, is_current_file, class_coll)
    
    local classes = class_coll or {}
    local class_scope_stack = {}
    
    local filename = getModuleFile(module_name, path, search_path)
    if not filename then
        return classes
    end
    
    local f = io.open(filename, "r")
    if f then
        if is_current_file then
            module_name = ""
        end
        
        local filepath = util.splitPathname(filename)
        local line = f:read("*line")
        while line do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "#") then
                    break
                end
                
                local current_class = getCurrentClassInScopeStack(class_scope_stack)
                if current_class then
                    if getStartSpaceCount(line) == current_class:getIndent() then
                        if current_class:getIndent() == 0 then
                            classes[current_class:getModuleName() .. "." .. current_class:getName()] = current_class
                        end
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
                    local t_extends = util.strSplit(extends, ",")
                    for i, v in ipairs(t_extends) do
                        local super_name = util.strTrim(v)
                        if super_name ~= "object" then
                            local super = findClass(super_name, classes)
                            if super then
                                c:addExtend(super)
                            end
                        end
                    end
                    table.insert(class_scope_stack, c)
                    
                    break
                end
                
                if #class_scope_stack > 0 then
                    local param_init = string.match(line, pattern_func_init)
                    if param_init then                        
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:setCtor(string.format("(%s)", removeSelfFromParams(param_init)))
                        
                        break
                    end
                    
                    local func, param = string.match(line, pattern_func)
                    if func and param then
                        if not util.strStartWith(func, "__") then
                            local current_class = getCurrentClassInScopeStack(class_scope_stack)
                            current_class:addFunction(string.format("%s(%s)", func, removeSelfFromParams(param)))
                        end
                        break
                    end
                    
                    local field = string.match(line, pattern_class_static_field)
                    -- do not consider inner class's fields
                    if field and getStartSpaceCount(line) == 4 then
                        local current_class = getCurrentClassInScopeStack(class_scope_stack)
                        current_class:addStaticField(field)
                        break
                    end
                end
                
                local from_import_module = string.match(line, pattern_from_import)
                if from_import_module then
                    parseClasses(from_import_module, filepath, search_path, false, classes)
                    break
                end
                        
                local import_module = string.match(line, pattern_import)
                if import_module then
                    parseClasses(import_module, filepath, search_path, false, classes)
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
            if current_class:getIndent() == 0 then
                classes[current_class:getModuleName() .. "." .. current_class:getName()] = current_class
            end
            table.remove(class_scope_stack)
        end
    end
    
    return classes
end
