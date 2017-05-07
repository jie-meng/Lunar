local json = require('json')

local pattern_class_begin = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.extend%s*%(]]
local pattern_method = [[([%w_]+)%s*:%s*function%s*%((.*)%)]]

function parseFile(filename, classes)
    local f = io.open(filename, "r")
    if f then
        local line = f:read('*line')
        local curent_class = nil
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
                    curent_class = { name = class, extends = {}, methods = {}, fields = {} }
                    table.insert(curent_class.extends, super)
                    break
                end
                
                if curent_class then
                    local method, params = string.match(trim_line, pattern_method)
                    if method and params then
                        table.insert(curent_class.methods, string.format('%s(%s)', method, params))
                        if method == 'ctor' then
                            table.insert(curent_class.methods, string.format('create(%s)', params))
                        end
                        break
                    end
                end
                
                --match class end
                if util.strStartWith(line, '});') then
                    if curent_class then
                        classes[curent_class.name] = curent_class
                        curent_class = nil
                    end
                    break
                end
            until true
            line = f:read("*line")
        end
        io.close(f)
    end
end

function processExtends(class, extends, classes, apis)
    for _, e in ipairs(extends) do
        local extend_class = classes[e]
        if extend_class then
                for _, v in ipairs(extend_class.methods) do
                    apis[class.name .. '.' .. v] = true
                end
            processExtends(class, extend_class.extends, classes, apis)
        end
    end
end


--main
local api_paths = {}

table.insert(api_paths, 'frameworks/cocos2d-html5/cocos2d')
table.insert(api_paths, 'frameworks/cocos2d-html5/extensions')

local classes = {}

for _, path in ipairs(api_paths) do
    if util.isPathDir(path) then
        local js_files = util.findFilesInDirRecursively(path, 'js')
        for _, file in ipairs(js_files) do
            parseFile(file, classes)
        end
    end
end

--generate api json
util.writeTextFile('api.json', json.encode(classes))
print('generate "api.json"')

--generate api file
local apis = {}
for _, c in pairs(classes) do
    for _, v in ipairs(c.methods) do
        apis[c.name .. '.' .. v] = true
    end
    processExtends(c, c.extends, classes, apis)
end

local array = {}
for k, _ in pairs(apis) do
    table.insert(array, k)
end

table.sort(array)
util.writeTextFile('cocos.api', util.strJoin(array, '\n'))
print('generate "cocos.api"')