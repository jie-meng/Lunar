local file_ext = require('file_ext')
local table_ext = require('table_ext')

local pattern_ctor = [[function%s+[%w_]+:ctor%(%s*%)]]
local pattern_field = [[self.([%w_]+)]]

print("Input target class name:")
local class = util.strTrim(io.read())
local filepath = util.currentPath() .. '/' .. class .. '.lua'
print("File: " .. filepath)

if not util.isPathFile(filepath) then
    print(filepath .. ' is not a valid file')
    os.exit(-1)
end

local fields = {}

file_ext.foreachLine(filepath,
    function (line, line_no, info)
        local trimmed_line = util.strTrim(line)
        if info.start then
            if trimmed_line == 'end' then
                return true
            end
            
            local field = string.match(trimmed_line, pattern_field)
            if field then
                local parts = table_ext.filterCopy(util.strSplit(field, '_'), 
                    function (v)
                        return string.len(v) > 0
                    end)
                    
                local func_name = util.strJoin(table_ext.mapCopy(parts,
                    function (v)
                        return string.upper(string.sub(v, 1, 1)) .. string.sub(v, 2)
                    end), '')

                table.insert(fields, { var = util.strJoin(parts, '_'), func = func_name })
                
                return
            end
        else
            if string.match(trimmed_line, pattern_ctor) then
                info.start  = true
                return
            end
        end
    end)


local gets_sets = table_ext.mapCopy(fields,
    function (v)
        return string.format('function get%s()\n    return self.%s_\nend\n\nfunction set%s(%s)\n    self.%s_ = %s\n    return self\nend', 
            v.func, v.var, v.func, v.var, v.var, v.var)
    end)
    
local file_content = util.readTextFile(filepath)
local gets_sets = {}
table_ext.foreach(fields,
    function (v)
        local get = string.format('function %s:get%s()\n    return self.%s_\nend', 
            class, v.func, v.var)
        if not util.strContains(file_content, get) then
            table.insert(gets_sets, get)
        end
        
        local set = string.format('function %s:set%s(%s)\n    self.%s_ = %s\n    return self\nend', 
            class, v.func, v.var, v.var, v.var)
        if not util.strContains(file_content, set) then
            table.insert(gets_sets, set)
        end
    end)
    
local class_return = 'return ' .. class
if util.strContains(file_content, class_return) then
    file_content = util.strReplace(file_content, class_return, '')
end

table_ext.foreach(gets_sets,
    function (v)
        file_content = file_content .. '\n\n' .. v
    end)
    
file_content = file_content .. '\n\n' .. class_return

util.writeTextFile(filepath, file_content)

print('OK')
