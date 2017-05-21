local pattern_func = [[function%s+([%w_]+)%s*%((.*)%)]]
local pattern_var_func = [[([%w_]+)%s*=%s*[%w_]+%s*%(%s*function%s*%((.*)%)]]
local pattern_export = [[lodash%.([%w_]+)%s*=%s*([%w_]+)]]

local content = util.readTextFile('lodash.js')

local keys = {}
local methods = {}

local tb = util.strSplit(content, '\n')
for _, line in ipairs(tb) do
    repeat
        local method, arg = string.match(line, pattern_func)
        if method and arg then
            print(string.format('%s(%s)', method, arg))
            methods[method] = string.format('_.%s(%s)', method, arg)
            break
        end
        
        local var_method, var_arg = string.match(line, pattern_var_func)
        if var_method and var_arg then
            print(string.format('%s(%s)', var_method, var_arg))
            methods[var_method] = string.format('_.%s(%s)', base_method, var_arg)
            break
        end
        
        local key, value = string.match(line, pattern_export)
        if key and value and key == value then
            keys[key] = true
            break
        end
    until true
end

local export_methods = {}
for k, v in pairs(methods) do
    if keys[k] then
        table.insert(export_methods, v)
        keys[k] = nil
    end
end

--left
for k, v in pairs(keys) do
    print(k)
    table.insert(export_methods, '_.' .. k)
end

util.writeTextFile('lodash.api', util.strJoin(export_methods, '\n'))
print('\ndone!')
