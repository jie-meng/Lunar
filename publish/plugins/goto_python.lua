local pattern_import = [[import%s+([%w_%.]+)]]
local pattern_from_import = [[from%s+([%w_%.]+)%s+import]]

function getModuleFile(module_name, path, search_path)
    local relative_path = util.strReplaceAll(module_name, ".", "/")

    -- check relative path first
    local filename = string.format("%s/%s.py", path, relative_path)
    if util.isPathFile(filename) then
        return filename
    end

    -- then check from currentpath
    filename = string.format("%s/%s.py", search_path, relative_path)
    if util.isPathFile(filename) then
        return filename
    end

    return nil
end

function parseDoc(results, search_path, filename, text, depth)
    local f = io.open(filename, "r")
    local fpath, _ = util.splitPathname(filename)
    if f then
        local readline = f:read("*line")
        local line_index = 1
        while readline do
            repeat
                local trimmed_line = util.strTrim(readline)
                if trimmed_line == "" or util.strStartWith(trimmed_line, "#") then
                    break
                end

                local from_import_module = string.match(trimmed_line, pattern_from_import)
                if from_import_module then
                    local module_file = getModuleFile(from_import_module, fpath, search_path)
                    if module_file then
                        parseDoc(results, search_path, module_file, text, depth + 1)
                    end
                    break
                end

                local import_module = string.match(trimmed_line, pattern_import)
                if import_module then
                    local module_file = getModuleFile(import_module, fpath, search_path)
                    if module_file then
                        parseDoc(results, search_path, module_file, text, depth + 1)
                    end
                    break
                end


                local matched = false
                if string.match(trimmed_line, "class%s+" .. text) then
                    matched = true
                end

                if depth < 2 and not matched and string.match(trimmed_line, "def%s+" .. text .. "%s*%(") then
                    matched = true
                end

                if matched then
                    local _, j = string.find(filename, search_path, 1, true)
                    if j then
                        table.insert(results, string.format("%s\n%d\n%s", string.sub(filename, j+2), line_index, readline))
                    end
                    break
                end
            until true
            readline = f:read("*line")
            line_index = line_index + 1
        end
        io.close()
    end
end

function gotoDefinition(text, line, filename, project_src_dir)
    local results = {}

    local search_path = util.currentPath()
    if util.strTrim(project_src_dir) ~= "" then
        search_path = search_path .. "/" .. project_src_dir
    end

    parseDoc(results, search_path, filename, text, 0)

    table.sort(results,
        function (a, b)
            local at = util.strSplit(a, '\n')
            local bt = util.strSplit(b, '\n')
            if string.len(at[1]) == string.len(bt[1]) then
                return tonumber(at[2]) < tonumber(bt[2])
            else
                return string.len(at[1]) < string.len(bt[1])
            end
        end)

    return results
end
