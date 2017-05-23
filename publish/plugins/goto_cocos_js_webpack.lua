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
local pattern_object_begin = [[(%w+)%s+([%w_]+)%s*=%s*{]]
local pattern_field = [[([%w_]+)%s*:%s*.+]]

local object_keywords = {}
object_keywords['const'] = true
object_keywords['let'] = true
object_keywords['var'] = true

function extractMultipleImports(import_str)
    local imports = util.strSplit(import_str, ',')
    local ret = {}
    table_ext.foreach(imports, function (k, v)
        ret[util.strTrim(v)] = true
    end)
    
    return ret
end

function extractMultipleExports(filename)
    local export_str = string.match(util.readTextFile(filename), pattern_export_multiple)
    local exports = util.strSplit(export_str, ',')
    local ret = {}
    table_ext.foreach(exports, function (k, v)
        ret[util.strTrim(v)] =  true
    end)
    
    return ret
end

function extractExportDefaultModule(filename)
    return string.match(util.readTextFile(filename), pattern_export_default)
end

function extractExportModule(import, filename)
    if import then
        if type(import) == 'table' then
            return extractMultipleExports(filename)
        else
            return extractExportDefaultModule(filename)
        end
    else
        return nil
    end
end

function isExportModule(module, export_module)
    if type(export_module) == 'table' then
        return export_module[module], false
    elseif type(export_module) == 'string' then
        return module == export_module, true
    else
        return false, false
    end
end

function parseFileClasses(filename, classes, import)
    local export_module = extractExportModule(import, filename)
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
            parseFileClasses(relative_path .. '/' .. import_path, classes, import_class)
            return
        end
        
        --match multiple imports
        local import_str, import_path = string.match(trim_line, pattern_import_multiple)
        if import_str and import_path then
            local imports = extractMultipleImports(import_str)
            if not imports then
                return
            end
            
            local relative_path = util.currentPath() .. '/mysrc'
            
            if util.strStartWith(import_path, '.') then
                relative_path = util.splitPathname(filename)
            end
            
            local imp, imf = util.splitPathname(import_path)
            if util.fileExtension(imf) == '' then
                import_path = import_path .. '.js'
            end
            parseFileClasses(relative_path .. '/' .. import_path, classes, imports)
            return
        end
        
        if string.len(util.strTrimLeft(line)) == string.len(line) then
            --match class begin
            local class, super = string.match(trim_line, pattern_class_begin)
            if  class and super then
                info.current_class = { name = class, extends = {}, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
                table.insert(info.current_class.extends, super)
                return
            end
            
            --match class end
            if util.strStartWith(trim_line, '});') then
                if info.current_class then
                    if export_module then
                        local is_export, is_default = isExportModule(info.current_class.name, export_module)
                        if is_export then
                            if is_default then
                                info.current_class.name = import
                            end
                            classes[info.current_class.name] = info.current_class
                        end
                    else
                        classes[info.current_class.name] = info.current_class
                    end
                    info.current_class = nil
                end
                return
            end
        end
        
        if info.current_class then
            local method, params = string.match(trim_line, pattern_method)
            if method and params then
                table.insert(info.current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
            
            method, params = string.match(trim_line, pattern_arrow_method)
            if method and params then
                table.insert(info.current_class.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
        else
            --static methods, variables
            local method, params = string.match(trim_line, pattern_static_method)
            if method and params then
                if export_module then
                    local is_export, is_default = isExportModule(util.fileBaseName(method), export_module)
                    if is_export then
                        local cls = classes[util.fileBaseName(method)]
                        if is_default then
                            cls = classes[import]
                        end
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
                if export_module then
                    local is_export, is_default = isExportModule(util.fileBaseName(method), export_module)
                    if is_export then
                        local cls = classes[util.fileBaseName(method)]
                        if is_default then
                            cls = classes[import]
                        end
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
                if export_module then
                    local is_export, is_default = isExportModule(util.fileBaseName(variable), export_module)
                    if is_export then
                        local cls = classes[util.fileBaseName(variable)]
                        if is_default then
                            cls = classes[import]
                        end
                        if cls then
                            table.insert(cls.fields, { name = util.fileExtension(variable), file = filename, line_number = line_number, line = line })
                        end
                    end
                else
                    local cls = classes[util.fileBaseName(variable)]
                    if cls then
                        table.insert(cls.fields, { name = util.fileExtension(variable), file = filename, line_number = line_number, line = line })
                    end
                end
                return
            end
        end
    end)
end

function parseFileObjects(filename, objects, import)
    local export_module = extractExportModule(import, filename)
    file_ext.foreachLine(filename, function (line, line_number, info)
        local trim_line = util.strTrim(line)
        --comments
        if string.len(trim_line) == 0 or util.strStartWith(trim_line, '/') or util.strStartWith(trim_line, '*') then
            return
        end
        
        --match import
        local import_class, import_path = string.match(trim_line, pattern_import)
        if import_class and import_path then
            --already in level-1 import file
            if import then
                return
            end
            
            local relative_path = util.currentPath() .. '/mysrc'
            
            if util.strStartWith(import_path, '.') then
                relative_path = util.splitPathname(filename)
            end
            
            local imp, imf = util.splitPathname(import_path)
            if util.fileExtension(imf) == '' then
                import_path = import_path .. '.js'
            end
            parseFileObjects(relative_path .. '/' .. import_path, objects, import_class)
            return
        end
        
        --match multiple imports
        local import_str, import_path = string.match(trim_line, pattern_import_multiple)
        if import_str and import_path then
            --already in level-1 import file
            if import then
                return
            end
        
            local imports = extractMultipleImports(import_str)
            if not imports then
                return
            end
            
            local relative_path = util.currentPath() .. '/mysrc'
            
            if util.strStartWith(import_path, '.') then
                relative_path = util.splitPathname(filename)
            end
            
            local imp, imf = util.splitPathname(import_path)
            if util.fileExtension(imf) == '' then
                import_path = import_path .. '.js'
            end
            parseFileObjects(relative_path .. '/' .. import_path, objects, imports)
            return
        end
        
        if string.len(util.strTrimLeft(line)) == string.len(line) then
            --match class begin
            if string.match(trim_line, pattern_class_begin) then
                info.current_class = true
            end
            --match class end
            if util.strStartWith(trim_line, '});') then
                info.current_class = nil
            end
            
            --match object begin
            local keyword, object = string.match(trim_line, pattern_object_begin)
            if keyword and object and object_keywords[keyword] then
                info.current_object = { name = object, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
                return
            end
            
            --match object end
            if util.strStartWith(trim_line, '};') then
                if info.current_object then
                    if export_module then
                        local is_export, is_default = isExportModule(info.current_object.name, export_module)
                        if is_export then
                            if is_default then
                                info.current_object.name = import
                            end
                            objects[info.current_object.name] = info.current_object
                        end
                    else
                        objects[info.current_object.name] = info.current_object
                    end
                    info.current_object = nil
                end
                return
            end
        end
        
        if not info.current_class and info.current_object then
            local method, params = string.match(trim_line, pattern_method)
            if method and params then
                table.insert(info.current_object.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
            
            method, params = string.match(trim_line, pattern_arrow_method)
            if method and params then
                table.insert(info.current_object.methods, { name = method, args = params, file = filename, line_number = line_number, line = line })
                return
            end
            
            local field = string.match(trim_line, pattern_field)
            if field then
                table.insert(info.current_object.fields, { name = field, file = filename, line_number = line_number, line = line })
                return
            end
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

function findInObject(text, object, results)
    for _, m in ipairs(object.methods) do
        if text == m.name then
            table.insert(results, string.format("%s\n%d\n%s", m.file, m.line_number, m.line)) 
        end
    end
    for _, f in ipairs(object.fields) do
        if text == f.name then
            table.insert(results, string.format("%s\n%d\n%s", f.file, f.line_number, f.line)) 
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

    --load index, parse classes
    local classes = table_ext.load('cocos_api_tb')
    parseFileClasses(filename, classes)
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
    
    local objects = {}
    parseFileObjects(filename, objects)
    local object = objects[text]
    if object then
        --find object
        table.insert(results, string.format("%s\n%d\n%s", object.file, object.line_number, object.line))
    else
        if util.strContains(text, '.') then
            --absolute method/fields
            local object = objects[util.fileBaseName(text)]
            if object then
                findInObject(util.fileExtension(text), object, results)
            end
        end
    end
    
    return results
end
