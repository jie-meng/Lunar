local table_ext = require('table_ext')

local pattern_class_begin = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.extend%s*%(]]
local pattern_method = [[([%w_]+)%s*:%s*function%s*%((.*)%)]]
local pattern_arrow_method = [[([%w_]+)%s*:%s*%((.*)%)%s*=>]]
local pattern_cc_method = [[(cc.[%w_%.]+)%s*=%s*function%s*%((.*)%)]]
local pattern_cc_arrow_method = [[(cc.[%w_%.]+)%s*=%s*%((.*)%)%s*=>]]
local pattern_cc_field = [[(cc.[%w_%.]+)%s*=]]

function parseFile(filename, classes, apis)
    local f = io.open(filename, "r")
    if f then
        local line = f:read('*line')
        local current_class = nil
        local line_number = 1
        while line do
            repeat
                local trim_line = util.strTrim(line)

                --comments
                if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
                    break
                end
                
                --match class start
                local class, super = string.match(trim_line, pattern_class_begin)
                if class and super then
                    current_class = { name = class, extends = {}, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
                    table.insert(current_class.extends, super)
                    break
                end
                
                if current_class then
                    local method, params = string.match(trim_line, pattern_method)
                    if method and params then
                        table.insert(current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                        if method == 'ctor' then
                            table.insert(current_class.methods, { name = 'create', args = params, file = filename, line_number = line_number, line = line })
                        end
                        break
                    end
                    
                    method, params = string.match(trim_line, pattern_arrow_method)
                    if method and params then
                        table.insert(current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                        if method == 'ctor' then
                            table.insert(current_class.methods, { name = 'create', args = params, file = filename, line_number = line_number, line = line })
                        end
                        break
                    end
                else
                    local method, params = string.match(trim_line, pattern_cc_method)
                    if method and params then
                        apis[string.format('%s(%s)', method, params)] = true
                        break
                    end
                    
                    method, params = string.match(trim_line, pattern_cc_arrow_method)
                    if method and params then
                        apis[string.format('%s(%s)', method, params)] = true
                        break
                    end
                    
                    local field = string.match(trim_line, pattern_cc_field)
                    if field then
                        apis[field] = true
                        break
                    end
                end
                
                --match class end
                if util.strStartWith(line, '});') then
                    if current_class then
                        classes[current_class.name] = current_class
                        current_class = nil
                    end
                    break
                end
            until true
            line = f:read("*line")
            line_number = line_number + 1
        end
        io.close(f)
    end
end

function processExtends(class, extends, classes, apis)
    for _, e in ipairs(extends) do
        local extend_class = classes[e]
        if extend_class then
                for _, v in ipairs(extend_class.methods) do
                    apis[string.format('%s.%s(%s)', class.name, v.name, v.args)] = true
                end
            processExtends(class, extend_class.extends, classes, apis)
        end
    end
end


--main
local api_paths = {}

table.insert(api_paths, { name = 'frameworks/cocos2d-html5', recursive = false })
table.insert(api_paths, { name = 'frameworks/cocos2d-html5/cocos2d', recursive = true })
table.insert(api_paths, { name = 'frameworks/cocos2d-html5/extensions', recursive = true })

local apis = {}
local classes = {}

for _, path in ipairs(api_paths) do
    if util.isPathDir(path.name) then
        local js_files = nil
        if path.recursive then
            js_files = util.findFilesInDirRecursively(path.name, 'js')
        else
            js_files = util.findFilesInDir(path.name, 'js')
        end
        for _, file in ipairs(js_files) do
            parseFile(file, classes, apis)
        end
    elseif util.isPathFile(path.name) then
        parseFile(path.name, classes, apis)
    end
end

--generate cocos_api_tb
table_ext.save(classes, 'cocos_api_tb')
print('generate "cocos_api_tb"')

--generate api file (non-class api only)
local array = {}
for k, _ in pairs(apis) do
    table.insert(array, k)
end

table.sort(array)
util.writeTextFile('cocos.api', util.strJoin(array, '\n'))
print('generate "cocos.api"')