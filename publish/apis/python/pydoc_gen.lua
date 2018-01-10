--[[constants]]
kSep = '.'

kLabelName = 'NAME'
kLabelFile = 'FILE'
kLabelPackageContents = 'PACKAGE CONTENTS'
kLabelFunctions = 'FUNCTIONS'
kLabelClasses = 'CLASSES'
kLabelData = 'DATA'

local pattern_function_lua = [[^([%w_]+)%s*%((.*)%)$]]
local pattern_function_with_return_lua = [[^([%w_]+)%s*%((.*)%)%s*->%s*([%w_%[%]%.%,]+)$]]
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
    return util.strJoin(self:getPackage(), kSep)
end

function LineParser:addApi(api)
    if self:getColl() then
        local i = string.find(api, '%(')
        if i then
            local key = string.sub(api, 1, i-1)
            if self:getColl()[key] then
                if util.strContains(self:getColl()[key], '(...)') then
                    self:getColl()[key] = api
                end
            else
                self:getColl()[key] = api
            end
        else
            self:getColl()[api] = api
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
    local trimmed_line = util.strTrim(line)
    local name, args = string.match(trimmed_line, pattern_class_lua)
    if name and args then
        self.class_name_ = name
    else
        if self.class_name_ then
            local class_member_trimmed_line = util.strTrimLeft(util.strTrimLeftEx(trimmed_line, '|'))
            local name, args = string.match(class_member_trimmed_line, pattern_function_lua)
            if name and args then
                if not util.strStartWith(name, '__') or name == '__init__' then
                    local args = util.strTrim(args)
                    if util.strStartWith(args, 'self') then
                        args = util.strTrimLeft(util.strTrimLeftEx(util.strReplace(args, 'self', ''), ','))
                    end
                    self:addApi(self.class_name_ .. '.' .. name .. '(' .. util.strTrimLeft(args) .. ')')
                    if name == '__init__' then
                        self:addApi(self.class_name_ .. '(' .. util.strTrimLeft(args) .. ')')
                    end
                end
                return
            end

            local name, args, ret = string.match(class_member_trimmed_line, pattern_function_with_return_lua)
            if name and args and ret then
                if not util.strStartWith(name, '__') or name == '__init__' then
                    local args = util.strTrim(args)
                    if util.strStartWith(args, 'self') then
                        args = util.strTrimLeft(util.strTrimLeftEx(util.strReplace(args, 'self', ''), ','))
                    end
                    self:addApi(self.class_name_ .. '.' .. name .. '(' .. args .. ') -> ' .. ret)
                    if name == '__init__' then
                        self:addApi(self.class_name_ .. '(' .. util.strTrimLeft(args) .. ')')
                    end
                end
                return
            end

            local name = string.match(class_member_trimmed_line, pattern_data_lua)
            if name and not util.strStartWith(name, '__') then
                self:addApi(self.class_name_ .. '.' .. name)
                return
            end
        end
    end
end

function ClassParser:generatePrefix()
    local package = util.strJoin(self.package_, kSep)
    if string.len(package) == 0 then
        return self.class_name_
    end
    
    if string.len(self.class_name_) == 0 then
        return package
    end
    
    return package .. kSep .. self.class_name_
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
    local trimmed_line = util.strTrim(line)
    local name, args = string.match(trimmed_line, pattern_function_lua)
    if name and args and not util.strStartWith(name, '__') then
        self:addApi(name .. '(' .. args .. ')')
        return
    end

    local name, args, ret = string.match(trimmed_line, pattern_function_with_return_lua)
    if name and args and ret and not util.strStartWith(name, '__') then
        self:addApi(name .. '(' .. args .. ') -> ' .. ret)
        return
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
    local trimmed_line = util.strTrim(line)
    local name, args = string.match(trimmed_line, pattern_data_lua)
    if name and not util.strStartWith(name, '__') then
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
        if name and not util.strStartWith(name, '__') then
            local module = self:generatePrefix() .. kSep .. name
            local gen_doc = self.gen_root_dir_ .. '/' .. module
            if not util.isPathFile(gen_doc) then
                os.execute(string.format('%s %s > %s', self.pydoc_gen_cmd_, module, gen_doc))
                self.parse_doc_func_(self:getColl(), gen_doc, self.pydoc_gen_cmd_, self.gen_root_dir_)
            end
        end
    end
end

--[[function: getIndent]]
function getIndent(line)
    return string.len(line) - string.len(util.strTrimLeft(line))
end

--[[function: labelMatches]]
function labelMatches(label, line)
    return label == util.strTrim(line)
end

--[[function: parseDoc]]
function parseDoc(apis, doc, pydoc_gen_cmd, gen_root_dir)
    print(string.format('ParseDoc <%s>', doc))
    local f = io.open(doc, "r")
    if f then
        local _, file_basename = util.splitPathname(doc)
        if file_basename == 'builtins' or file_basename == '__builtin__' then
            file_basename = ''
        end
        local line_parser = nil
        local line = f:read("*line")
        while line do
            repeat
                if util.strTrim(line) == "" then
                    break
                end
                
                if getIndent(line) == 0 then
                    if labelMatches(kLabelClasses, line) then
                        line_parser = ClassParser:new(apis, util.strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelFunctions, line) then
                        line_parser = FunctionParser:new(apis, util.strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelData, line) then
                        line_parser = DataParser:new(apis, util.strSplit(file_basename, '.'))
                    elseif labelMatches(kLabelPackageContents, line) then
                        line_parser = PackageContentsParser:new({}, util.strSplit(file_basename, '.'), pydoc_gen_cmd, gen_root_dir, parseDoc)
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
        
        -- sort
        local tb = {}
        for _, v in pairs(apis) do
            table.insert(tb, v)
        end
        table.sort(tb)

        if #tb > 0 then
            util.writeTextFile(doc, util.strJoin(tb, '\n'))
        else
            print('Remove empty doc: ' .. doc)
            util.pathRemove(doc)
        end
    end
end

--[[main]]
local modules = {}
table.insert(modules, 'sys')
table.insert(modules, 'os')
table.insert(modules, 'os.path')
table.insert(modules, 'unittest')
table.insert(modules, 'subprocess')
table.insert(modules, 'stat')
table.insert(modules, 'time')
table.insert(modules, 'datetime')
table.insert(modules, 'platform')
table.insert(modules, 'random')
table.insert(modules, 'math')
table.insert(modules, 'cmath')
table.insert(modules, 're')
table.insert(modules, 'json')
table.insert(modules, 'copy')
table.insert(modules, 'operator')
table.insert(modules, 'hashlib')
table.insert(modules, 'md5')
table.insert(modules, 'shutil')
table.insert(modules, 'atexit')
table.insert(modules, 'fileinput')
table.insert(modules, 'shelve')
table.insert(modules, 'threading')
table.insert(modules, 'multiprocessing')
table.insert(modules, 'struct')
table.insert(modules, 'collections')
table.insert(modules, 'itertools')
table.insert(modules, 'xml')
table.insert(modules, 'html')
table.insert(modules, 'queue')
table.insert(modules, 'socket')
table.insert(modules, 'urllib')
table.insert(modules, 'email')
table.insert(modules, '_io')

-- Check python version on unix. If on windows, just set appropriate python version to environment path
local python_version = ''
print('Is python 3.x? (y/n)')
if util.strStartWith(io.read(), 'y', false) then
    python_version = '3'
    print("Parse python 3 ...")
    table.insert(modules, 'builtins')
else
    print("Parse python 2 ...")
    table.insert(modules, '__builtin__')
end

-- Generate cmd
local pydoc_gen_cmd = 'pydoc' .. python_version
if util.strContains(util.platformInfo(), 'windows', false) then
    pydoc_gen_cmd = 'python -m pydoc'
end

local gen_root_dir = util.currentPath() .. '/' .. 'pydoc_gen'
if not util.isPathDir(gen_root_dir) then
    util.mkDir(gen_root_dir)
end

for _, m in pairs(modules) do
    local gen_doc = gen_root_dir .. '/' .. m
    -- builtin should always be api file which do not need import
    if m == 'builtins' or m == '__builtin__' then
        gen_doc = gen_doc .. '.api'
    end
    if not util.isPathFile(gen_doc) then
        os.execute(string.format('%s %s > %s', pydoc_gen_cmd, m, gen_doc))
        if util.isPathFile(gen_doc) then
            parseDoc({}, gen_doc, pydoc_gen_cmd, gen_root_dir)
        end
    end
end

local gen_docs = util.findFilesInDir(gen_root_dir)
local tb = {}
for _, v in ipairs(gen_docs) do
    if not util.strEndWith(v, '.api') then
        table.insert(tb, util.strReplaceAll(v, gen_root_dir .. '/', ''))
    end
end
table.sort(tb)

print('\nGenerate pydoc_index.api')
util.writeTextFile('./pydoc_index.api', util.strJoin(tb, '\n'))

print('\nDone!')
