local pattern_func = [[def%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_func_init = [[def%s+__init__%s*%((.*)%)%s*:]]
local pattern_class = [[class%s+([%w_]+)%s*:]]
local pattern_class_extend = [[class%s+([%w_]+)%s*%((.*)%)%s*:]]
local pattern_class_static_field = [[([%w_]+)%s*=%s*(.+)]]
local pattern_import = [[import%s+([%w_%.]+)]]
local pattern_from_import = [[from%s+([%w_%.]+)%s+import]]
local pattern_object_assignment_string = [[([%w_%.]+)%s*=%s*['"].*['"]%s*]]
local pattern_object_assignment_bytes = [[([%w_%.]+)%s*=%s*b['"].*['"]%s*]]
local pattern_object_assignment_memoryview = "([%w_%.]+)%s*=%s*memoryview(.*)"
local pattern_object_assignment_list = "([%w_%.]+)%s*=%s*%[.*%]"
local pattern_object_assignment_dict = "([%w_%.]+)%s*=%s*{.*}"
local pattern_object_assignment_class = [[([%w_%.]+)%s*=%s*([%w_%.]+)%s*%(]]
local pattern_object_assignment = [[([%w_%.]+)%s*=%s*([%w_%.]+)]]
local pattern_object_assignment_string_encode = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.encode%(.*%)]]
local pattern_object_assignment_bytes_decode = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.decode%(.*%)]]
local pattern_object_cast_string = "([%w_%.]+)%s*=%s*str(.*)"
local pattern_object_cast_int = "([%w_%.]+)%s*=%s*int(.*)"
local pattern_object_cast_float = "([%w_%.]+)%s*=%s*float(.*)"
local pattern_object_cast_list = "([%w_%.]+)%s*=%s*list(.*)"
local pattern_object_cast_dict = "([%w_%.]+)%s*=%s*dict(.*)"
local pattern_object_cast_set = "([%w_%.]+)%s*=%s*set(.*)"
local pattern_object_del = [[del%s*([%w_%.]+)]]

local kstr_build_in = "__build_in__"

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
    is_from_import_ =false
}

function Import:new(name, is_from_import)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    self.name_ = name
    self.is_from_import_ = is_from_import
    
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

--[[ Import end ]]

function buildInClasses()
    local classes = {}
    
    local cls_string = Class:new("string", kstr_build_in, 0)
    cls_string:addFunction('capitalize()')
    cls_string:addFunction('casefold()')
    cls_string:addFunction('center(width[, fillchar])')
    cls_string:addFunction('count(sub[, start[, end]])')
    cls_string:addFunction('encode(encoding="utf-8", errors="strict")')
    cls_string:addFunction('endswith(suffix[, start[, end]])')
    cls_string:addFunction('expandtabs(tabsize=8)')
    cls_string:addFunction('find(sub[, start[, end]])')
    cls_string:addFunction('format(*args, **kwargs)')
    cls_string:addFunction('format_map(mapping)')
    cls_string:addFunction('index(sub[, start[, end]])')
    cls_string:addFunction('isalnum()')
    cls_string:addFunction('isalpha()')
    cls_string:addFunction('isdecimal()')
    cls_string:addFunction('isdigit()')
    cls_string:addFunction('isidentifier()')
    cls_string:addFunction('islower()')
    cls_string:addFunction('isnumeric()')
    cls_string:addFunction('isprintable()')
    cls_string:addFunction('isspace()')
    cls_string:addFunction('istitle()')
    cls_string:addFunction('isupper()')
    cls_string:addFunction('join(iterable)')
    cls_string:addFunction('ljust(width[, fillchar])')
    cls_string:addFunction('lower()')
    cls_string:addFunction('lstrip([chars])')
    cls_string:addFunction('maketrans(x[, y[, z]])')
    cls_string:addFunction('partition(sep)')
    cls_string:addFunction('replace(old, new[, count])')
    cls_string:addFunction('rfind(sub[, start[, end]])')
    cls_string:addFunction('rindex(sub[, start[, end]])')
    cls_string:addFunction('rjust(width[, fillchar])')
    cls_string:addFunction('rpartition(sep)')
    cls_string:addFunction('rsplit(sep=None, maxsplit=-1)')
    cls_string:addFunction('rstrip([chars])')
    cls_string:addFunction('split(sep=None, maxsplit=-1)')
    cls_string:addFunction('splitlines([keepends])')
    cls_string:addFunction('startswith(prefix[, start[, end]])')
    cls_string:addFunction('strip([chars])')
    cls_string:addFunction('swapcase()')
    cls_string:addFunction('title()')
    cls_string:addFunction('translate(map)')
    cls_string:addFunction('upper()')
    cls_string:addFunction('zfill(width)')
    classes[cls_string:getName()] = cls_string
    
    local cls_bytes = Class:new("bytes", kstr_build_in, 0)
    cls_bytes:addFunction('count(sub[, start[, end]])')
    cls_bytes:addFunction('decode(encoding="utf-8", errors="strict")')
    cls_bytes:addFunction('endswith(suffix[, start[, end]])')
    cls_bytes:addFunction('find(sub[, start[, end]])')
    cls_bytes:addFunction('index(sub[, start[, end]])')
    cls_bytes:addFunction('join(iterable)')
    cls_bytes:addFunction('maketrans(from, to)')
    cls_bytes:addFunction('partition(sep)')
    cls_bytes:addFunction('replace(old, new[, count])')
    cls_bytes:addFunction('rfind(sub[, start[, end]])')
    cls_bytes:addFunction('rindex(sub[, start[, end]])')
    cls_bytes:addFunction('rpartition(sep)')
    cls_bytes:addFunction('startswith(prefix[, start[, end]])')
    cls_bytes:addFunction('translate(table[, delete])')
    cls_bytes:addFunction('center(width[, fillbyte])')
    cls_bytes:addFunction('ljust(width[, fillbyte])')
    cls_bytes:addFunction('lstrip([chars])')
    cls_bytes:addFunction('rjust(width[, fillbyte])')
    cls_bytes:addFunction('rsplit(sep=None, maxsplit=-1)')
    cls_bytes:addFunction('rstrip([chars])')
    cls_bytes:addFunction('split(sep=None, maxsplit=-1)')
    cls_bytes:addFunction('strip([chars])')
    cls_bytes:addFunction('capitalize()')
    cls_bytes:addFunction('expandtabs(tabsize=8)')
    cls_bytes:addFunction('isalnum()')
    cls_bytes:addFunction('isalpha()')
    cls_bytes:addFunction('isdigit()')
    cls_bytes:addFunction('islower()')
    cls_bytes:addFunction('isspace()')
    cls_bytes:addFunction('istitle()')
    cls_bytes:addFunction('isupper()')
    cls_bytes:addFunction('lower()')
    cls_bytes:addFunction('splitlines(keepends=False)')
    cls_bytes:addFunction('swapcase()')
    cls_bytes:addFunction('title()')
    cls_bytes:addFunction('upper()')
    cls_bytes:addFunction('zfill(width)')
    classes[cls_bytes:getName()] = cls_bytes
    
    
    local cls_list = Class:new("list", kstr_build_in, 0)
    cls_list:addFunction('append(x)')
    cls_list:addFunction('extend(L)')
    cls_list:addFunction('insert(i, x)')
    cls_list:addFunction('remove(x)')
    cls_list:addFunction('pop([i])')
    cls_list:addFunction('clear()')
    cls_list:addFunction('index(x)')
    cls_list:addFunction('count(x)')
    cls_list:addFunction('sort()')
    cls_list:addFunction('reverse()')
    cls_list:addFunction('copy()')
    classes[cls_list:getName()] = cls_list
    
    local cls_dict = Class:new("dict", kstr_build_in, 0)
    cls_dict:addFunction('iter()')
    cls_dict:addFunction('clear()')
    cls_dict:addFunction('copy()')
    cls_dict:addFunction('fromkeys(seq[, value])')
    cls_dict:addFunction('get(key[, default])')
    cls_dict:addFunction('items()')
    cls_dict:addFunction('keys()')
    cls_dict:addFunction('pop(key[, default)')
    cls_dict:addFunction('popitem()')
    cls_dict:addFunction('keys()')
    cls_dict:addFunction('setdefault(key[, default])')
    cls_dict:addFunction('update([othter])')
    cls_dict:addFunction('values()')
    classes[cls_dict:getName()] = cls_dict
    
    local cls_set = Class:new("set", kstr_build_in, 0)
    cls_set:addFunction('len(s)')
    cls_set:addFunction('isdisjoint(other)')
    cls_set:addFunction('issubset(other)')
    cls_set:addFunction('issuperset(other)')
    cls_set:addFunction('union(other, ...)')
    cls_set:addFunction('intersection(other, ...)')
    cls_set:addFunction('difference(other, ...)')
    cls_set:addFunction('symmetric_difference(other)')
    cls_set:addFunction('copy()')
    cls_set:addFunction('update(other, ...)')
    cls_set:addFunction('intersection_update(other, ...)')
    cls_set:addFunction('difference_update(other, ...)')
    cls_set:addFunction('symmetric_difference_update(other)')
    cls_set:addFunction('add(elem)')
    cls_set:addFunction('remove(elem)')
    cls_set:addFunction('discard(elem)')
    cls_set:addFunction('pop()')
    cls_set:addFunction('clear()')
    classes[cls_set:getName()] = cls_set
    
    local cls_memoryview = Class:new("memoryview", kstr_build_in, 0)
    cls_memoryview:addFunction('tobytes()')
    cls_memoryview:addFunction('tolist()')
    cls_memoryview:addFunction('release()')
    cls_memoryview:addFunction('cast(format[, shape])')
    classes[cls_memoryview:getName()] = cls_memoryview
    
    return classes
end

local build_in_classes = buildInClasses()

function parseSupplementApi(filename, cursor_line, project_src_dir)

    local apis = {}
    
    local path, name = file.splitPathname(filename)
    local base = file.fileBaseName(name)
    
    local imports = parseImports(filename)
    local functions = parseFunctions("." .. base, path, false, true)
    local classes = parseClasses("." .. base, path)
    
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
    
    local objects = processCurrentFileObjects(filename, cursor_line, classes, imports)
    for _, v in pairs(objects) do
        if imports[v:getModuleName()] then
            parseClassesApis(apis, imports, v, v, true)
        end
    end
    
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

function getModuleFile(module_name, path)
    if strStartWith(module_name, ".") then
        -- python 3 package relative path parse
        local trdot_left = strTrimLeftEx(module_name, ".")
        local rep_dots = string.len(module_name) - string.len(trdot_left) - 1
        local relative_path = strRelaceAll(trdot_left, ".", "/")
        if rep_dots > 0 then
            for i=1, rep_dots, 1 do
                relative_path = "../" .. relative_path
            end
        end
        local filename = string.format("%s/%s.py", path, relative_path)
        if file.isPathFile(filename) then
            return filename
        end
    else
        -- current path parse
        local relative_path = strRelaceAll(module_name, ".", "/")
        local filename = string.format("%s/%s.py", file.currentPath(), relative_path)
        if file.isPathFile(filename) then
            return filename
        end
    end
    
    return nil
end

function parseImports(filename)
    local imports = {}
    imports[kstr_build_in] = Import:new(kstr_build_in, true)
    
    local f = io.open(filename, "r")
    if f then
        local line = f:read("*line")
        while line do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
                
                if getStartSpaceCount(line) > 0 then
                    break
                end
                
                if (not strStartWith(line, "import")) and (not strStartWith(line, "from")) then
                    break
                end
                
                local from_import_module = string.match(line, pattern_from_import)
                if from_import_module then
                    imports[from_import_module] = Import:new(from_import_module, true)
                    break
                end
                
                local import_module = string.match(line, pattern_import)
                if import_module then
                    imports[import_module] = Import:new(import_module, false)
                    break
                end
            
            until true
            line = f:read("*line")
        end
        io.close()
    end
    
    return imports
end

function parseFunctions(module_name, path, add_module_prefix, recursive, function_coll)

    local functions = function_coll or {}
    
    local filename = getModuleFile(module_name, path)
    if not filename then
        return functions
    end
    
    local f = io.open(filename, "r")
    if f then
        local filepath = file.splitPathname(filename)
        local line = f:read("*line")
        while line do
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
    if not filename then
        return classes
    end
    
    local f = io.open(filename, "r")
    if f then
        local filepath = file.splitPathname(filename)
        local line = f:read("*line")
        while line do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
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
                    local t_extends = strSplit(extends, ",")
                    for i, v in ipairs(t_extends) do
                        local super_name = strTrim(v)
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
                        if not strStartWith(func, "__") then
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
            if current_class:getIndent() == 0 then
                classes[current_class:getModuleName() .. "." .. current_class:getName()] = current_class
            end
            table.remove(class_scope_stack)
        end
    end
    
    return classes
end

function setSpecificClassObject(pattern, class_name, line, objects_table)
    local obj = string.match(line, pattern)
    if obj then
        local c = build_in_classes[class_name]:clone()
        c:setName(obj)
        objects_table[obj] = c
        return true
    end
    
    return false
end

function removeClassObject(pattern, line, objects_table)
    local obj = string.match(line, pattern)
    if obj then
        objects_table[obj] = nil
        return true
    end
    
    return false
end

function processCurrentFileObjects(filename, cursor_line, classes, imports)
    
    local objects = {}
    local f = io.open(filename, "r")
    if f then
        local line = f:read("*line")
        local current_line = 1
        while (line and current_line < cursor_line) do
            repeat
                if strTrim(line) == "" or strStartWith(strTrimLeft(line), "#") then
                    break
                end
                
                -- set specific class objects
                if setSpecificClassObject(pattern_object_assignment_string, "string", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_assignment_bytes, "bytes", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_assignment_list, "list", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_assignment_dict, "dict", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_cast_string, "string", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_cast_list, "list", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_cast_dict, "dict", line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_cast_set, "set", line, objects) then
                    break
                end
                
                if removeClassObject(pattern_object_cast_int, line, objects) then
                    break
                end
                
                if removeClassObject(pattern_object_cast_float, line, objects) then
                    break
                end
                
                if setSpecificClassObject(pattern_object_assignment_memoryview, "memoryview", line, objects) then
                    break
                end
                
                local left, right = string.match(line, pattern_object_assignment_string_encode)
                if left and right then
                    if objects[right] and objects[right]:getClassName() == "string" then
                        local c = build_in_classes["bytes"]:clone()
                        c:setClassName("bytes")
                        c:setName(left)
                        objects[left] = c
                    else
                        objects[left] = nil
                    end
                    break
                end
                
                local left, right = string.match(line, pattern_object_assignment_bytes_decode)
                if left and right then
                    if objects[right] and objects[right]:getClassName() == "bytes" then
                        local c = build_in_classes["string"]:clone()
                        c:setClassName("string")
                        c:setName(left)
                        objects[left] = c
                    else
                        objects[left] = nil
                    end
                    break
                end
                
                -- set user define class objects
                local left, right = string.match(line, pattern_object_assignment_class)
                if left and right and left ~= right then
                    if classes[right] then
                        local c = classes[right]:clone()
                        c:setName(left)
                        objects[left] = c
                    else
                        local flag = false
                        for _, v in pairs(classes) do
                            if v:getName() == right and imports[v:getModuleName()] then
                                local c = v:clone()
                                c:setName(left)
                                objects[left] = c
                                flag = true
                                break
                            end
                        end
                        
                        if not flag then
                            objects[left] = nil
                        end
                    end
                    break
                end
                
                local left, right = string.match(line, pattern_object_assignment)
                if left and right and left ~= right then
                    if objects[right] then
                        local c = objects[right]:clone()
                        c:setName(left)
                        objects[left] = c
                    else
                        objects[left] = nil
                    end
                    break
                end
                
                local obj = string.match(line, pattern_object_del)
                if obj then
                    objects[obj] = nil
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

--[[ test ]]
--local apis = parseSupplementApi("./test.py", 50)
--for _, v in pairs(apis) do
--    print(v)
--end
