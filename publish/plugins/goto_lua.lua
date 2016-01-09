function gotoDefinition(project_dir, filename, text)
    local files = file.findFilesInDirRecursively(file.currentPath() .. "/" .. project_dir, "lua")
    table.sort(files, 
        function (a, b)
            return string.len(a) > string.len(b)
        end)
    
    local results = {}
    for _, v in ipairs(files) do
        local f = io.open(v, "r")
        if f then
            local line = f:read("*line")
            local line_index = 1
            while line do
                repeat
                    local trimmed_line = strTrim(line)
                    if trimmed_line == "" or strStartWith(trimmed_line, "--") then
                        break
                    end
                    
                    local matched = false
                    if string.match(trimmed_line, "function%s+" .. text .. "") then
                        matched = true
                    elseif string.match(trimmed_line, "function%s+[%w_]+%." .. text .. "%s*%(") then
                        matched = true
                    elseif string.match(trimmed_line, "function%s+[%w_]+:" .. text .. "%s*%(") then
                        matched = true
                    end
                    
                    if matched then
                        local _, j = string.find(v, file.currentPath())
                        if j then
                            table.insert(results, string.format("%s\n%d\n%s", string.sub(v, j+2), line_index, line))
                        end
                    end
                until true
                line = f:read("*line")
                line_index = line_index+1
            end
        end
        io.close(f)
    end
    
    return results
end
