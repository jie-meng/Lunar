local json = require('json')

local pattern_class_begin = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.extend%s*%(]]
local pattern_method = [[([%w_]+)%s*:%s*function%s*%((.*)%)]]

function parseFile(filename, classes)
    local f = io.open(filename, "r")
    if f then
        local line = f:read('*line')
        local curent_class = nil
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
                    curent_class = { name = class, extends = {}, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
                    table.insert(curent_class.extends, super)
                    break
                end
                
                if curent_class then
                    local method, params = string.match(trim_line, pattern_method)
                    if method and params then
                        table.insert(curent_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                        if method == 'ctor' then
                            table.insert(curent_class.methods, { name = 'create', args = params, file = filename, line_number = line_number, line = line })
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
            line_number = line_number + 1
        end
        io.close(f)
    end
end

function findInExtends(text, class, classes, results)
    for _, m in ipairs(class.methods) do
        if text == m.name then
            table.insert(results, string.format("%s\n%d\n%s", m.file, m.line_number, m.line)) 
        end
    end

    for _, e in ipairs(class.extends) do
        local extend_class = classes[e]
        if extend_class then
            findInExtends(text, extend_class, classes, results)
        end
    end
end

function inClassRange(filename, current_line_number)
    local f = io.open(filename, "r")
    if f then
        local line = f:read('*line')
        local curent_class = nil
        local line_number = 1
        while line do
            repeat
                local trim_line = util.strTrim(line)
                --comments
                if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
                    break
                end
                
                if curent_class and line_number == current_line_number then
                    return curent_class
                end
                
                --match class start
                local class, super = string.match(trim_line, pattern_class_begin)
                if class and super then
                    curent_class = class
                    break
                end
                
                --match class end
                if util.strStartWith(line, '});') then
                    if curent_class then
                        curent_class = nil
                    end
                    break
                end
            until true
            line = f:read("*line")
            line_number = line_number + 1
        end
        io.close(f)
    end
    
    return nil
end

function gotoDefinition(text, line, filename, project_src_dir)
    local classes = {}
    local js_files = util.findFilesInDirRecursively(project_src_dir, 'js')
    for _, v in ipairs(js_files) do
        parseFile(v, classes)
    end
    
    local results = {}
    local class = classes[text]
    if class then
        --find class
        table.insert(results, string.format("%s\n%d\n%s", class.file, class.line_number, class.line))
    else
        --find method in range
        local range_class_name = inClassRange(filename, line)
        if range_class_name then
            local range_class = classes[range_class_name]
            if range_class then
                findInExtends(text, range_class, classes, results)
            end
        end
    end
    
    return results
end
