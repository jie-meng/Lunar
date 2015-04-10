function processPath(path)

    if file.isPathFile(path) then
        if (file.fileExtension(path) ~= "api") then
            local text = file.readTextFile(path)
            text = strRelaceAll(text, "\"", "")
            text = strRelaceAll(text, "\n", " ")  
            local keywords = strSplit(text, " ")
            local trimed_keywords = {}
            for key, value in pairs(keywords) do
                if strTrim(value) ~= "" then
                    table.insert(trimed_keywords, strTrim(value))
                end
            end
            text = strJoin(trimed_keywords, "\n")
            local d, f = file.splitPathname(path)
            
            file.writeTextFile(d .. "/" .. "keywords.api", text)
        end
    elseif file.isPathDir(path) then
        local files = file.findFilesInDirRecursively(path)
        for key, value in pairs(files) do
            processPath(value)
        end 
    end
end

processPath("D:/tools/Lunar/api/html")
print("ok")