local pattern_import_lua = [[([%w_]+)%s*=%s*import%("([%w_%.%s]+)"%)]]
local pattern_class_lua = [[([%w_]+)%s*=%s*class%("[%w_]+"(["%%()%,%.%s%w_]*)%)]]
local pattern_tb_function_lua = [[function%s+([%w_]+)[.:]([%w_]+)%s*(%(.*%))]]
local pattern_function_lua = [[function%s+([%w_]+)%s*(%(.*%))]]
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

function Class:addFunction(func)
    table.insert(self.functions_, func)
    
    return self
end

function Class:putFunctionsInto(tb)    
    for _, v in ipairs(self:getFunctions()) do
        table.insert(tb, v)
    end
    
    for _, v in ipairs(self:getFields()) do
        table.insert(tb, v)
    end
    
	for _, v in pairs(self:getExtends()) do
		v:putFunctionsInto(tb)
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
	local tb = {}
	
    local functions = {}
	self:putFunctionsInto(functions)
	for _, v in ipairs(functions) do
		table.insert(tb, v)
	end
    
    local apis = {}
    -- override
    for i = #tb, 1, -1 do
        local name, args = string.match(tb[i], "([%w_]+)%s*(%(.*%))")
        if name and args then
            apis[name] = name .. args
        else
            apis[tb[i]] = tb[i]
        end
    end
    
	return apis
end

--[[ functions ]]
function parseClass(current_file_dir, import_path)
    local filename = nil
    if strStartWith(strTrimLeft(import_path), ".") then
        filename = current_file_dir .. strRelaceAll(import_path, ".", "/") .. ".lua"
    else
        filename = file.currentPath() .. "/src/" .. strRelaceAll(import_path, ".", "/") .. ".lua"
    end
    
    local class = nil
    
    local f = io.open(filename, "r")
    if f then
        class = Class:new()
        
        local line = f:read("*line")
        while line do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "--") then
                    break
                end
                
                local tb, func, param = string.match(line, pattern_tb_function_lua)
                if tb and func and param and tb == class:getName() then
                    class:addFunction(string.format("%s%s", func, param))
                    if func == "ctor" then
                        class:addFunction(string.format("%s%s", "create", param))
                    end
                    break
                end
                
                local tb, field = string.match(line, pattern_field_lua)
                if tb and field and tb == class:getName() then
                    class:addField(field)
                end
                
                local name, path = string.match(line, pattern_import_lua)
                if name and path then
                    class:addImport(name, strTrim(path))
                    break
                end
                
                local name, extends = string.match(line, pattern_class_lua)
                if name and extends then
                    class:setName(name)
                    local tb = strSplit(extends, ",")
                    for k, v in pairs(tb) do
                        local import = strTrim(v)
                        if string.len(import) > 0 then
                            local import_path = class:getImport(strTrim(v))
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
  
    local path, name = file.splitPathname(filename)
    local class = parseClass(path, "." .. file.fileBaseName(filename))

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