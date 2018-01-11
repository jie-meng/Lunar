function gotoDefinition(text, line, filename, project_src_dir)
    local find_path = util.currentPath()
    if util.strTrim(project_src_dir) ~= "" then
        find_path = find_path .. "/" .. project_src_dir
    end
    local files = util.findFilesInDirRecursively(find_path, "py")
    
    table.sort(files, 
        function (a, b)
            return string.len(a) > string.len(b)
        end)
    
    local results = {}
    for _, v in ipairs(files) do
        local f = io.open(v, "r")
        if f then
            local readline = f:read("*line")
            local line_index = 1
            while readline do
                repeat
                    local trimmed_line = util.strTrim(readline)
                    if trimmed_line == "" or util.strStartWith(trimmed_line, "#") then
                        break
                    end
                    
                    local matched = false
                    if string.match(trimmed_line, "def%s+" .. text .. "%s*%(") then
                        matched = true
                    elseif string.match(trimmed_line, "class%s+" .. text) then
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
    
    return results
end
