--[[constants]]
kLabelName = 'NAME'
kLabelFile = 'FILE'
kLabelPackageContents = 'PACKAGE CONTENTS'
kLabelFunctions = 'FUNCTIONS'
kLabelClasses = 'CLASSES'
kLabelData = 'DATA'

local pattern_function_lua = [[^([%w_]+)%s*%((.*)%)$]]
local pattern_class_lua = [[^class%s+([%w_]+)%s*%((.*)%)$]]
local pattern_data_lua = [[^([%w_]+)%s*=%s*(.*)$]]
local pattern_package_contents_lua = [[([%w_]+)]]

--[[class: LineParser]]
local LineParser = {
    coll_ = nil,
    package_ = nil
}

function LineParser:new(coll, package)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o.coll_ = coll
    o.package_ = package
    
    return o
end

function LineParser:getPackage()
    return self.package_
end

function LineParser:getColl()
    return self.coll_
end

function LineParser:parse(line)
end

function LineParser:generatePrefix()
    return strJoin(self:getPackage(), '.')
end

function LineParser:addApi(api)
    if self:getColl() then
        if strStartWith(api, '(') then
            if not self:getColl()[self:generatePrefix()] then
                self:getColl()[self:generatePrefix()] = self:generatePrefix() .. api
            end
        else
            local prefix = self:generatePrefix()
            if string.len(prefix) > 0 then
                prefix = prefix .. '.'
            end
            local i = string.find(api, '%(')
            if i then                
                self:getColl()[prefix .. string.sub(api, 1, i-1)] = prefix .. api
            else
                self:getColl()[prefix .. api] = prefix .. api
            end
        end
    end
end

--[[class: ClassParser]]
local ClassParser = LineParser:new()
ClassParser.class_name_ = nil

function ClassParser:new(coll, package)
    local o = LineParser:new(coll, package)
    setmetatable(o, self)
    self.__index = self
    
    o.class_name_ = nil
    
    return o
end

function ClassParser:parse(line)
    local trimmed_line = strTrim(line)
    local name, args = string.match(trimmed_line, pattern_class_lua)
    if name and args then
        self.class_name_ = name
    else
        if self.class_name_ then
            local class_member_trimmed_line = strTrimLeft(strTrimLeftEx(trimmed_line, '|'))
            local name, args = string.match(class_member_trimmed_line, pattern_function_lua)
            if name and args then
                if not strStartWith(name, '__') or name == '__init__' then                    
                    local args = strTrim(args)
                    if strStartWith(args, 'self') then
                        args = strTrimLeft(strTrimLeftEx(strReplace(args, 'self', ''), ','))
                    end
                    self:addApi(name .. '(' .. strTrimLeft(args) .. ')')
                    if name == '__init__' then
                        self:addApi('(' .. strTrimLeft(args) .. ')')
                    end
                end
            else
                local name, args = string.match(class_member_trimmed_line, pattern_data_lua)
                if name and not strStartWith(name, '__') then
                    self:addApi(name)
                end
            end
        end
    end
end

function ClassParser:generatePrefix()
    local package = strJoin(self.package_, '.')
    if string.len(package) == 0 then
        return self.class_name_
    end
    
    if string.len(self.class_name_) == 0 then
        return package
    end
    
    return package .. '.' .. self.class_name_
end

--[[class: FunctionParser]]
local FunctionParser = LineParser:new()

function FunctionParser:new(coll, package)
    local o = LineParser:new(coll, package)
    setmetatable(o, self)
    self.__index = self
    
    return o
end

function FunctionParser:parse(line)
    local trimmed_line = strTrim(line)
    local name, args = string.match(trimmed_line, pattern_function_lua)
    if name and args and not strStartWith(name, '__') then
        self:addApi(name .. '(' .. args .. ')')
    end
end

--[[class: DataParser]]
local DataParser = LineParser:new()

function DataParser:new(coll, package)
    local o = LineParser:new(coll, package)
    setmetatable(o, self)
    self.__index = self
    
    return o
end

function DataParser:parse(line)
    local trimmed_line = strTrim(line)
    local name, args = string.match(trimmed_line, pattern_data_lua)
    if name and not strStartWith(name, '__') then
        self:addApi(name)
    end
end

--[[class: PackageContentsParser]]
local PackageContentsParser = LineParser:new()

function PackageContentsParser:new(coll, package, pydoc_gen_cmd, gen_root_dir, parse_doc_func)
    local o = LineParser:new(coll, package)
    setmetatable(o, self)
    self.__index = self
    
    self.pydoc_gen_cmd_ = pydoc_gen_cmd
    self.gen_root_dir_ = gen_root_dir
    self.parse_doc_func_ = parse_doc_func
    
    return o
end

function PackageContentsParser:parse(line)
    if self.parse_doc_func_ then
        local name = string.match(line, pattern_package_contents_lua)
        if name and not strStartWith(name, '__') then
            local module = self:generatePrefix() .. '.' .. name
            local gen_doc = self.gen_root_dir_ .. '/' .. module
            os.execute(string.format('%s %s > %s', self.pydoc_gen_cmd_, module, gen_doc))
            self.parse_doc_func_(self:getColl(), gen_doc, self.pydoc_gen_cmd_, self.gen_root_dir_)
        end
    end
end

--[[function: getIndent]]
function getIndent(line)
    return string.len(line) - string.len(strTrimLeft(line))
end

--[[function: labelMatches]]
function labelMatches(label, line)
    return label == strTrim(line)
end

--[[function: parseDoc]]
function parseDoc(apis, doc, pydoc_gen_cmd, gen_root_dir)
    print(string.format('ParseDoc <%s>', doc))
    local f = io.open(doc, "r")
    if f then
        local _, file_basename = file.splitPathname(doc)
        if file_basename == 'builtins' or file_basename == '__builtin__' then
            file_basename = ''
        end
        local line_parser = nil
        local line = f:read("*line")
        while line do
            repeat
                if strTrim(line) == "" then
                    break
                end
                
                if getIndent(line) == 0 then
                    if labelMatches(kLabelClasses, line) then
                        line_parser = ClassParser:new(apis, strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelFunctions, line) then
                        line_parser = FunctionParser:new(apis, strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelData, line) then
                        line_parser = DataParser:new(apis, strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelPackageContents, line) then
                        line_parser = PackageContentsParser:new(apis, strSplit(file_basename, '.'), pydoc_gen_cmd, gen_root_dir, parseDoc)
                    else
                        line_parser = nil
                    end
                else
                    if line_parser then
                        line_parser:parse(line)
                    end
                end
            until true
            line = f:read("*line")
        end
        io.close(f)
    end
end

--[[main]]
local modules = {}
table.insert(modules, 'sys')
table.insert(modules, 'os')
table.insert(modules, 'platform')
table.insert(modules, 'fileinput')
table.insert(modules, 'shelve')
table.insert(modules, 'time')
table.insert(modules, 'random')
table.insert(modules, 'shutil')
table.insert(modules, 're')
table.insert(modules, 'json')
table.insert(modules, 'email')
--table.insert(modules, 'xlrd')
--table.insert(modules, 'xlwt')
--table.insert(modules, 'numpy')

-- Check python version on unix. If on windows, just set appropriate python version to environment path
local python_version = ''
if strContains(platformInfo(), 'unix', false) then
    print('Is python 3.x? (y/n)')
    if strStartWith(io.read(), 'y', false) then
        python_version = '3'
        print("Parse python 3 ...")
        table.insert(modules, 'builtins')
    else
        print("Parse python 2 ...")
        table.insert(modules, '__builtin__')
    end
end

-- Generate cmd
local pydoc_gen_cmd = 'pydoc' .. python_version
if strContains(platformInfo(), 'windows', false) then
    pydoc_gen_cmd = 'python -m pydoc'
end

local gen_root_dir = file.currentPath() .. '/' .. 'pydoc_gen'
file.pathRemoveAll(gen_root_dir)
if not file.mkDir(gen_root_dir) then
    print(string.format('Error: [%s] already exists', gen_root_dir))
    os.exit(0)
end

local apis = {}
for _, m in pairs(modules) do
    local gen_doc = gen_root_dir .. '/' .. m
    os.execute(string.format('%s %s > %s', pydoc_gen_cmd, m, gen_doc))
    if file.isPathFile(gen_doc) then
        parseDoc(apis, gen_doc, pydoc_gen_cmd, gen_root_dir)
    end
end

print('Remove tmp dir [' .. gen_root_dir .. '] ...')
file.pathRemoveAll(gen_root_dir)
print('Ok')

-- sort
local tb = {}
for _, v in pairs(apis) do
    table.insert(tb, v)
end
table.sort(tb)

print('Save api file to python.api ...')
local f = io.open('python.api', 'w')
if f then
    for _, v in pairs(tb) do
        f:write(v .. '\n')
    end
end
print('Done')
