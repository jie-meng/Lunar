function processPath(path)

    if util.isPathFile(path) then
        if (util.fileExtension(path) ~= "api") then
            local text = util.readTextFile(path)
            text = strRelaceAll(text, "\"", "")
            text = strRelaceAll(text, "\n", " ")  
            local keywords = util.strSplit(text, " ")
            local trimed_keywords = {}
            for key, value in pairs(keywords) do
                if util.strTrim(value) ~= "" then
                    table.insert(trimed_keywords, util.strTrim(value))
                end
            end
            text = util.strJoin(trimed_keywords, "\n")
            local d, f = util.splitPathname(path)
            
            util.writeTextFile(d .. "/" .. "keywords.api", text)
        end
    elseif util.isPathDir(path) then
        local files = util.findFilesInDirRecursively(path)
        for key, value in pairs(files) do
            processPath(value)
        end 
    end
end

processPath("C:/Users/jmeng/tools/Lunar/apis/cpp")
print("ok")