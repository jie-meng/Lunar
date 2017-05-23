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
            info.current_class = { name = class, extends = {}, methods = {}, fields = {}, file = filename, line_number = line_number, line = line }
            table.insert(info.current_class.extends, super)
            return
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
            if info.current_class then
                if import then
                    if info.current_class.name == export_module then
                        info.current_class.name = import
                        classes[info.current_class.name] = info.current_class
                    end
                else
                    classes[info.current_class.name] = info.current_class
                end
                info.current_class = nil
            end
            return
        end
    end)
end

function processExtends(class, extends, classes, apis)
    for _, e in ipairs(extends) do
        local extend_class = classes[e]
        if extend_class then
            for _, v in ipairs(extend_class.methods) do
                apis[string.format('%s.%s(%s)', class.name, v.name, v.args)] = true
            end
            for _, v in ipairs(extend_class.fields) do
                apis[string.format('%s.%s', class.name, v.name)] = true
            end
            processExtends(class, extend_class.extends, classes, apis)
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

function parseSupplementApi(filename, cursor_line, project_src_dir)
    local apis = {}
    
    --load index
    local classes = table_ext.load('cocos_api_tb')
    
    --parse src
    parseFile(filename, classes)
    
    --parse resource
    parseResourceFile(classes)
    
    -- process this in range
    local range_class_name = inClassRange(filename, cursor_line)
    if range_class_name then
        local range_class = classes[range_class_name]
        if range_class then
            local this_tb = table_ext.shallowCopy(range_class)
            this_tb.name = 'this'
            classes['this'] = this_tb
        end
    end

    --process extends
    local apis = {}
    for _, c in pairs(classes) do
        for _, v in ipairs(c.methods) do
            apis[string.format('%s.%s(%s)', c.name, v.name, v.args)] = true
        end
        for _, v in ipairs(c.fields) do
            apis[string.format('%s.%s', c.name, v.name)] = true
        end
        processExtends(c, c.extends, classes, apis)
    end

    local array = {}
    for k, _ in pairs(apis) do
        table.insert(array, k)
    end

    table.sort(array)

    return array
end
