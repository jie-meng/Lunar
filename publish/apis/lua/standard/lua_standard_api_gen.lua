local pattern_function_lua = [[^([%w_%.]+)%s*%((.*)%)$]]
local pattern_method_lua = [[^([%w_]+:[%w_]+)%s*%((.*)%)$]]
local pattern_variable_lua = [[^([%w_%.]+)$]]

print('Input api document segment file:')
local doc = io.read()

local apis = {}

local f = io.open(doc, 'r')
if f then
    local line = f:read('*line')
    while line do
        repeat
            local trimmed_line = strTrim(line)
            if trimmed_line == '' or trimmed_line == 'end' then
                break
            end
            
            local func, args = string.match(trimmed_line, pattern_function_lua)
            if func and args then
                if not apis[func] then
                    apis[func] = func .. '(' .. args .. ')'
                end
                break
            end
			
			local method, args2 = string.match(trimmed_line, pattern_method_lua)
            if method and args2 then
                if not apis[method] then
                    apis[method] = method .. '(' .. args2 .. ')'
                end
                break
            end
            
            local variable = string.match(trimmed_line, pattern_variable_lua)
            if variable then
                if not apis[variable] then
                    apis[variable] = variable
                end
				break
            end
            
        until true
        line = f:read('*line')
    end
    io.close(f)
end

-- sort
local tb = {}
for _, v in pairs(apis) do
    table.insert(tb, v)
end
table.sort(tb)

print('Save api file to lua_standard.api ...')
local f = io.open('lua_standard.api', 'w')
if f then
    for _, v in pairs(tb) do
        f:write(v .. '\n')
    end
end
print('Done')

