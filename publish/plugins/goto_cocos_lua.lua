local table_ext = require('table_ext')

local pattern_import_lua = [[([%w_]+)%s*=%s*import%(["']([%w_%.%s]+)["']%)]]
local pattern_importer_lua = [[([%w_]+)%s*=%s*[%w]+%.import%(["']([%w_%.%s]+)["']%)]]
local pattern_class_lua = [[([%w_]+)%s*=%s*class%(["'][%w_]+["'](["%%()%,%.%s%w_]*)%)]]

function gotoDefinition(text, line, filename, project_src_dir)
    local imports = {}
    findImports(filename, imports, true)
    
    local find_path = util.currentPath()
    if util.strTrim(project_src_dir) ~= "" then
        find_path = find_path .. "/" .. project_src_dir
    end
    local all_files_in_project = util.findFilesInDirRecursively(find_path, "lua")
    
    local files = {}
    for _, v in ipairs(all_files_in_project) do
        if imports[v] then
            table.insert(files, v)
        end
    end
    all_files_in_project = nil
    imports = nil
    
    table.sort(files, 
        function (a, b)
            return string.len(a) > string.len(b)
        end)
    -- insert current file to the first one
    table.insert(files, 1, filename)
    
    local results = {}
    for _, v in ipairs(files) do
        local f = io.open(v, "r")
        if f then
            local readline = f:read("*line")
            local line_index = 1
            while readline do
                repeat
                    local trimmed_line = util.strTrim(readline)
                    if trimmed_line == "" or util.strStartWith(trimmed_line, "--") then
                        break
                    end
                    
                    local matched = false
                    if string.match(trimmed_line, "function%s+" .. text .. "%s*%(") then
                        matched = true
                    elseif string.match(trimmed_line, "function%s+[%w_]+%." .. text .. "%s*%(") then
                        matched = true
                    elseif string.match(trimmed_line, "function%s+[%w_]+:" .. text .. "%s*%(") then
                        matched = true
                    elseif string.match(trimmed_line, text .. "%s*=%s*class%s*%(") then
                        matched = true
                    end
                    
                    if matched then
                        local _, j = string.find(v, util.currentPath())
                        if j then
                            table.insert(results, string.format("%s\n%d\n%s", string.sub(v, j+2), line_index, readline))
                        end
                    end
                until true
                readline = f:read("*line")
                line_index = line_index+1
            end
            io.close(f)
        end
    end
    
    local api_index_tb = table_ext.load('api_index_tb')
    local index_found = api_index_tb[text] or nil
    if index_found then
        local _, j = string.find(index_found.file, util.currentPath())
        if j then
            table.insert(results, string.format("%s\n%d\n%s", string.sub(index_found.file, j+2), index_found.line_number, index_found.line))
        end
    end
    
    return results
end

function findImports(filename, out_imports, current_file)
    local import_files = {}
    
    local f = io.open(filename, "r")
    if f then
        local readline = f:read("*line")
        while readline do
            repeat
                local trimmed_line = util.strTrim(readline)
                if trimmed_line == "" or util.strStartWith(trimmed_line, "--") then
                    break
                end
                
                local current_file_path = util.splitPathname(filename)
                local name, path = string.match(trimmed_line, pattern_import_lua)
                if name and path then
                    local import_file = nil
                    if util.strStartWith(util.strTrimLeft(path), ".") then
                        import_file = current_file_path .. util.strReplaceAll(path, ".", "/") .. ".lua"
                    else
                        import_file = util.currentPath() .. "/src/" .. util.strReplaceAll(path, ".", "/") .. ".lua"
                    end
                    import_files[name] = import_file
                    if current_file then
                        out_imports[import_file] = true
                    end
                    
                    break
                end
                
                local name, path = string.match(trimmed_line, pattern_importer_lua)
                if name and path then
                    local import_file = nil
                    if util.strStartWith(util.strTrimLeft(path), ".") then
                        import_file = current_file_path .. util.strReplaceAll(path, ".", "/") .. ".lua"
                    else
                        import_file = util.currentPath() .. "/src/" .. util.strReplaceAll(path, ".", "/") .. ".lua"
                    end
                    import_files[name] = import_file
                    if current_file then
                        out_imports[import_file] = true
                    end
                    
                    break
                end
                
                local name, extends = string.match(readline, pattern_class_lua)
                if name and extends then
                    local tb = util.strSplit(extends, ",")
                    for _, v in ipairs(tb) do
                        local extend = util.strTrim(v)
                        if string.len(extend) > 0 and import_files[extend] then
                            out_imports[import_files[extend]] = true
                            findImports(import_files[extend], out_imports, false)
                        end
                    end
                    
                    break
                end
                
            until true
            readline = f:read("*line")
        end
        io.close(f)
    end
end
