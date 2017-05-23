local table_ext = require('table_ext')
local file_ext = require('file_ext')

local pattern_class_begin = [[([%w_%.]+)%s*=%s*([%w_%.]+)%.extend%s*%(]]
local pattern_method = [[([%w_]+)%s*:%s*function%s*%((.*)%)]]
local pattern_arrow_method = [[([%w_]+)%s*:%s*%((.*)%)%s*=>]]
local pattern_static_method = [[([%w_%.]+)%s*=%s*function%s*%((.*)%)]]
local pattern_static_arrow_method = [[([%w_%.]+)%s*=%s*%((.*)%)%s*=>]]
local pattern_static_variable = [[([%w_%.]+)%s*=]]
local pattern_res_begin = [[const%s+res%s*=%s*{]]
local pattern_res_name = [[([%w_]+)%s*:]]
local pattern_import = [[import%s+([%w_]+)%s+from%s+'([%w_-%/%.]+)']]
local pattern_export_default = [[export%s+default%s+([%w_]+)]]
local pattern_import_multiple = [[import%s+{([%w%s_,]+)}%s+from%s+'([%w_-%/%.]+)']]
local pattern_export_multiple = [[export%s+{([%w%s_,]+)}]]

function extractMultipleImports(imports)
    return nil
end

function extractMultipleExports(filename)
    return nil
end

function extractExportDefaultModule(filename)
    return string.match(util.readTextFile(filename), pattern_export_default)
end

function parseFile(filename, classes, import)
    local export_module = nil
    if import then
        export_module = extractExportDefaultModule(filename)
    end
    
    file_ext.foreachLine(filename, function (line, line_number, info)
        local trim_line = util.strTrim(line)
        --comments
        if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
            return
        end
        
        --match import
        local import_class, import_path = string.match(trim_line, pattern_import)
        if import_class and import_path then
            local relative_path = util.currentPath() .. '/mysrc'
            
            if util.strStartWith(import_path, '.') then
                relative_path = util.splitPathname(filename)
            end
            
            local imp, imf = util.splitPathname(import_path)
            if util.fileExtension(imf) == '' then
                import_path = import_path .. '.js'
            end
            parseFile(relative_path .. '/' .. import_path, classes, import_class)          
        end
        
        --match class start
        local class, super = string.match(trim_line, pattern_class_begin)
        if class and super then
            current_class = { name = class, extends = {}, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
            table.insert(current_class.extends, super)
            return
        end
        
        if current_class then
            local method, params = string.match(trim_line, pattern_method)
            if method and params then
                table.insert(current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
            
            method, params = string.match(trim_line, pattern_arrow_method)
            if method and params then
                table.insert(current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
        else
            --static methods, variables
            local method, params = string.match(trim_line, pattern_static_method)
            if method and params then
                if import and export_module then
                    if util.fileBaseName(method) == export_module then
                        local cls = classes[import]
                        if cls then
                            table.insert(cls.methods, { name = util.fileExtension(method), args = params, file = filename, line_number = line_number, line = line })
                        end
                    end
                else
                    local cls = classes[util.fileBaseName(method)]
                    if cls then
                        table.insert(cls.methods, { name = util.fileExtension(method), args = params, file = filename, line_number = line_number, line = line })
                    end
                end

                return
            end
            
            method, params = string.match(trim_line, pattern_static_arrow_method)
            if method and params then
                if import and export_module then
                    if util.fileBaseName(method) == export_module then
                        local cls = classes[import]
                        if cls then
                            table.insert(cls.methods, { name = util.fileExtension(method), args = params, file = filename, line_number = line_number, line = line })
                        end
                    end
                else
                    local cls = classes[util.fileBaseName(method)]
                    if cls then
                        table.insert(cls.methods, { name = util.fileExtension(method), args = params, file = filename, line_number = line_number, line = line })
                    end
                end

                return
            end
            
            local variable = string.match(trim_line, pattern_static_variable)
            if variable then
                local cls = classes[util.fileBaseName(variable)]
                if cls then
                    table.insert(cls.fields, { name = util.fileExtension(variable), file = filename, line_number = line_number, line = line })
                end
                return
            end
        end
        
        --match class end
        if util.strStartWith(line, '});') then
            if current_class then
                if import then
                    if current_class.name == export_module then
                        current_class.name = import
                        classes[current_class.name] = current_class
                    end
                else
                    classes[current_class.name] = current_class
                end
                current_class = nil
            end
            return
        end
    end)
end

function findInExtends(text, class, classes, results)
    for _, m in ipairs(class.methods) do
        if text == m.name then
            table.insert(results, string.format("%s\n%d\n%s", m.file, m.line_number, m.line)) 
        end
    end
    for _, f in ipairs(class.fields) do
        if text == f.name then
            table.insert(results, string.format("%s\n%d\n%s", f.file, f.line_number, f.line)) 
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
    local return_class = nil
    local out_of_range = false
    if f then
        local line = f:read('*line')
        local current_class = nil
        local line_number = 1
        while line do
            repeat
                if current_class and line_number == current_line_number then
                    return_class = current_class
                    break
                end
                
                local trim_line = util.strTrim(line)
                --comments
                if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
                    break
                end

                --match class start
                local class, super = string.match(trim_line, pattern_class_begin)
                if class and super then
                    current_class = class
                    if line_number > current_line_number then
                        out_of_range = true
                    end
                    break
                end
                
                --match class end
                if util.strStartWith(line, '});') then
                    if current_class then
                        current_class = nil
                    end
                    break
                end
            until true
            line = f:read("*line")
            line_number = line_number + 1
            
            if return_class or out_of_range then
                break
            end
        end
        io.close(f)
    end
    
    return return_class
end

function parseResourceFile(classes)
    local f = io.open('mysrc/resource.js', "r")
    if f then
        local cls = { name = 'res', extends = {}, methods = {}, fields = {}, file = 'mysrc/resource.js', line_number = 0, line = '' }
        classes['res'] = cls
        local line = f:read('*line')
        local line_number = 1
        local in_range = false
        local finish = false
        while line do
            repeat
                local trim_line = util.strTrim(line)
                --comments
                if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
                    break
                end
                
                if not finish and in_range then
                    local name = string.match(trim_line, pattern_res_name)
                    if name then
                        table.insert(cls.fields, { name = name, file = cls.file, line_number = line_number, line = line })
                        break
                    end
                end

                --match class start
                if string.match(trim_line, pattern_res_begin) then
                    in_range = true
                    cls.line_number = line_number
                    cls.line = line
                    break
                end
                
                --match class end
                if util.strEndWith(trim_line, '};') then
                    in_range = false
                    finish = true
                    break
                end
            until true
            line = f:read("*line")
            line_number = line_number + 1
        end
        io.close(f)
    end
end

function getLineStringFromLineNo(filename, line_no)
    local line_str = nil
    local f = io.open(filename, "r")
    if f then
        local line = f:read('*line')
        local line_number = 1
        while line do
            if line_number == line_no then
                line_str = line
                break
            end
            
            line = f:read("*line")
            line_number = line_number + 1
        end
        io.close(f)
    end
    
    return line_str
end

function gotoDefinition(text, line, filename, project_src_dir)
    local results = {}
    local line_str = getLineStringFromLineNo(filename, line)
    if line_str then
        local import_class, import_path = string.match(getLineStringFromLineNo(filename, line), pattern_import)
        if import_class and import_path then
            local relative_path = util.currentPath() .. '/mysrc'
                    
            if util.strStartWith(import_path, '.') then
                relative_path = util.splitPathname(filename)
            end
            
            local imp, imf = util.splitPathname(import_path)
            if util.fileExtension(imf) == '' then
                import_path = import_path .. '.js'
            end
            
            local absolute_path = relative_path .. '/' .. import_path
            if util.isPathFile(absolute_path) then
                --Each column in one result item must not be empty string !!! So the third column is given to ' ' not '' !!!
                table.insert(results, string.format("%s\n%d\n%s", absolute_path, 1, ' '))
            end
            return results
        end
    end

    --load index
    local classes = table_ext.load('cocos_api_tb')
    
    --parse src
    parseFile(filename, classes)
    
    --parse resource
    parseResourceFile(classes)
    
    local class = classes[text]
    if class then
        --find class
        table.insert(results, string.format("%s\n%d\n%s", class.file, class.line_number, class.line))
    else
        if util.strContains(text, '.') then
            --absolute method/fields
            local cls = classes[util.fileBaseName(text)]
            if cls then
                findInExtends(util.fileExtension(text), cls, classes, results)
            end
        else    
            --relative (local) method/fields, find in self range
            local range_class_name = inClassRange(filename, line)
            if range_class_name then
                local range_class = classes[range_class_name]
                if range_class then
                    findInExtends(text, range_class, classes, results)
                end
            end
        end
    end
    
    return results
end
