local pattern_import_line = [[local%s+([%w_]+)%s*=%s*import%s*%("([%w%._]+)"%)]]

local files = file.findFilesInDirRecursively(file.currentPath())

for _, f in ipairs(files) do
    if strEndWith(f, ".lua") then
        local text = file.readTextFile(f)
        local lines = strSplit(text, "\n")
        local imports = {}
        local codes = {}
        for _, l in ipairs(lines) do
            local line = strTrim(l)
            local var, path = string.match(line, pattern_import_line)
            if var and path then
                table.insert(imports, { sort_key = path, value = line, module = var })
            else
                table.insert(codes, l)
            end
        end
        
        --prepare codes text
        local codes_text = strJoin(codes, "\n")
        
        --trim imports
        local trimmed_imports = {}
        for _, impt in ipairs(imports) do
            if strContains(codes_text, impt.module) then
                trimmed_imports[impt.value] = impt
            end
        end
        
        --sort imports
        local sort_imports = {}
        for _, impt in pairs(trimmed_imports) do
            table.insert(sort_imports, impt)
        end
        
        --sort imports
        table.sort(sort_imports, 
            function(a, b)
                return a.sort_key < b.sort_key
            end)
        
        --prepare imports text
        imports = {}
        for _, impt in ipairs(sort_imports) do
            table.insert(imports, impt.value)
        end
        local imports_text = strTrim(strJoin(imports, "\n"))
        
        --write file
        local text_before_class = ""
        if string.len(imports_text) > 0 then
            text_before_class = imports_text .. "\n\n"
        end
        if file.writeTextFile(f, text_before_class .. strTrim(codes_text)) then
            print(string.format([[Sort imports of <%s> ok.]], f))
        end
    end
end

print()

print(string.format([[Sort all imports in path <%s> ok.]], file.currentPath()))