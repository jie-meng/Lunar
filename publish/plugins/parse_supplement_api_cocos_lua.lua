local pattern_import_lua = [[([%w_]+)%s*=%s*import%(["']([%w_%.%s]+)["']%)]]
local pattern_importer_lua = [[([%w_]+)%s*=%s*[%w]+%.import%(["']([%w_%.%s]+)["']%)]]
local pattern_class_lua = [[([%w_]+)%s*=%s*class%(["'][%w_]+["'](["%%()%,%.%s%w_]*)%)]]
local pattern_tb_function_lua = [[function%s+([%w_]+)[.:]([%w_]+)%s*%((.*)%)]]
local pattern_field_lua = [[([%w_]+)%.([%w_]+)%s=]]

--[[ Class ]]
local Class = {
    name_ = nil,
    extends_ = {},
    functions_ = {}
}

function Class:new(name)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o.name_ = name
    o.extends_ = {}
    o.functions_ = {}
    o.fields_ = {}
    o.imports_ = {}
    
    return o
end

function Class:getName()
    return self.name_
end

function Class:setName(name)
    self.name_ = name
    return self
end

function Class:getExtends()
    return self.extends_
end

function Class:addExtend(super_class)
    self.extends_[super_class:getName()] = super_class
    return self
end

function Class:removeExtend(name)
    self.extends_[name] = nil
    return self
end

function Class:getFunctions()
    return self.functions_
end

function Class:addFunction(func_name, func_args)
    table.insert(self.functions_, { name = func_name, args = func_args })
    return self
end

function Class:makeFunctionApi(func)
    return string.format("%s(%s)", func.name, func.args)
end

function Class:makeSuperMethodApi(func)
    if string.len(util.strTrim(func.args)) == 0 then
        return string.format("super.%s(self)", func.name)
    else
        return string.format("super.%s(self, %s)", func.name, func.args)
    end
end

function Class:putFunctionsInto(tb, is_base)    
    for _, v in ipairs(self:getFunctions()) do
        table.insert(tb, self:makeFunctionApi(v))
        if is_base then
            table.insert(tb, self:makeSuperMethodApi(v))
        end
    end
    
    for _, v in ipairs(self:getFields()) do
        table.insert(tb, v)
    end
    
	for _, v in pairs(self:getExtends()) do
		v:putFunctionsInto(tb, true)
	end
    
    return self
end

function Class:getFields()
    return self.fields_
end

function Class:addField(field)
    table.insert(self.fields_, field)
end

function Class:addImport(name, path)
    self.imports_[name] = path
end

function Class:getImport(name)
    return self.imports_[name]
end

function Class:getImports()
    return self.imports_
end

function Class:createApis()
    local functions = {}
    self:putFunctionsInto(functions)
    
    local apis = {}
    -- process override
    for i = #functions, 1, -1 do
        local name, args = string.match(functions[i], "([%w_%.]+)%s*(%(.*%))")
        if name and args then
            apis[name] = name .. args
        else
            apis[functions[i]] = functions[i]
        end
    end
    
	return apis
end

--[[ functions ]]
function parseClass(current_file_dir, import_path)
    local filename = nil
    if util.strStartWith(util.strTrimLeft(import_path), ".") then
        filename = current_file_dir .. util.strReplaceAll(import_path, ".", "/") .. ".lua"
    else
        filename = util.currentPath() .. "/src/" .. util.strReplaceAll(import_path, ".", "/") .. ".lua"
    end
    
    local class = nil
    local f = io.open(filename, "r")
    current_file_dir = util.splitPathname(filename)
    if f then
        class = Class:new()
        
        local line = f:read("*line")
        while line do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "--") then
                    break
                end
                
                local tb, func, param = string.match(line, pattern_tb_function_lua)
                if tb and func and param and tb == class:getName() then
                    class:addFunction(func, param)
                    if func == "ctor" then
                        class:addFunction("create", param)
                    end
                    break
                end
                
                local tb, field = string.match(line, pattern_field_lua)
                if tb and field and tb == class:getName() then
                    class:addField(field)
                end
                
                local name, path = string.match(line, pattern_import_lua)
                if name and path then
                    class:addImport(name, util.strTrim(path))
                    break
                end
                
                local name, path = string.match(line, pattern_importer_lua)
                if name and path then
                    class:addImport(name, util.strTrim(path))
                    break
                end
                
                local name, extends = string.match(line, pattern_class_lua)
                if name and extends then
                    class:setName(name)
                    local tb = util.strSplit(extends, ",")
                    for k, v in pairs(tb) do
                        local import = util.strTrim(v)
                        if string.len(import) > 0 then
                            local import_path = class:getImport(util.strTrim(v))
                            --print(string.format("class: %s  extends: %s  path: %s", class:getName(), v, tostring(import_path)))
                            if import_path then
                                local super_class = parseClass(current_file_dir, import_path)
                                if super_class then
                                    class:addExtend(super_class)
                                end
                            end
                        end
                    end
                    break
                end
            until true
            
            line = f:read("*line")
        end
        
        io.close(f)
    end
    
    return class
end

function parseSupplementApi(filename, cursor_line, project_src_dir)
    local apis = {}
  
    local path, name = util.splitPathname(filename)
    local class = parseClass(path, "." .. util.fileBaseName(filename))

    if class then
        local t_api = class:createApis()
        
        for k, v in pairs(t_api) do
            table.insert(apis, string.format("%s.%s", class:getName(), v))
            table.insert(apis, string.format("%s.%s", "self", v))
        end
        
        for k, v in pairs(class:getImports()) do
            local context_cls  = parseClass(path, v)
            context_cls:setName(k)
            
            local t_context_api = context_cls:createApis()
            for k1, v1 in pairs(t_context_api) do
                table.insert(apis, string.format("%s.%s", context_cls:getName(), v1))
            end
        end
    end
    
    return apis
end
