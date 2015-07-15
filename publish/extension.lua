function parseFileType(filename)

	local path, name = file.splitPathname(filename)

	-- Lunar extension
	if name == "extension.lua" then
		if file.isPathFile(path .. "/Lunar") or file.isPathFile(path .. "/Lunar.exe") then
			return 
                {
                    type = "lua", 
                    auto_complete_type = 1, 
                    api = "apis/lua/standard,apis/lua/uextend.api,apis/lua/uextend_file.api,apis/lua/uextend_regex.api" ,
                    comment_line = "--"
                }
		end
	end

    if string.lower(file.fileExtension(name)) == "lua" and file.isPathFile("./cocos_run.lua") then
		return 
            {
                type = "lua", 
                auto_complete_type = 1, 
                api = "apis/lua/standard,cocos.api", 
                executor = "luaexec", 
                parse_supplement_api_script = "plugins/parse_supplement_api_cocos_lua.lua", 
                parse_supplement_api_func = "parseSupplementApi",
                comment_line = "--"
            }
	end

	if string.lower(file.fileExtension(name)) == "lua" then
		return 
            {
                type = "lua", 
                auto_complete_type = 1, 
                api = "apis/lua", 
                executor = "luaexec", 
                parse_supplement_api_script = "plugins/parse_supplement_api_lua.lua", 
                parse_supplement_api_func = "parseSupplementApi",
                comment_line = "--"
            }
	end
    
	if string.lower(file.fileExtension(name)) == "m" then
		return 
            {
                type = "octave", 
                auto_complete_type = 0, 
                api = "apis/octave", 
                executor = "octave", 
                parse_supplement_api_script = "plugins/parse_supplement_api_octave.lua", 
                parse_supplement_api_func = "parseSupplementApi",
                comment_line = "%"
            }
	end
    
    if string.lower(file.fileExtension(name)) == "sh" then
        return { type = "bash", comment_line = "#" }
    end
    
	if string.lower(name) == "cmakelists.txt" or string.lower(file.fileExtension(name)) == "cmake" then
		return { type = "cmake", comment_line = "#" }
	end
    
    if string.lower(file.fileExtension(name)) == "py" then
        return 
            {
                -- python3
                type = "python",
                auto_complete_type = 1,
                api = "apis/python/python3.api",
                executor = "python3 -u",
                parse_supplement_api_script = "plugins/parse_supplement_api_python3.lua", 
                parse_supplement_api_func = "parseSupplementApi",
                comment_line = "#"
                
                -- python2
                --type = "python",
                --auto_complete_type = 1,
                --api = "apis/python/python.api",
                --executor = "python -u",
                --parse_supplement_api_script = "plugins/parse_supplement_api_python.lua", 
                --parse_supplement_api_func = "parseSupplementApi",
                --comment_line = "#"
            }
    end
    
    if string.lower(file.fileExtension(name)) == "tcl" then
        return { type = "tcl", comment_line = "#" }
    end
    
    if string.lower(file.fileExtension(name)) == "java" then
        return { type = "java", comment_line = "//" }
    end
    
    if string.lower(file.fileExtension(name)) == "cs" then
        return { type = "csharp", comment_line = "//" }
    end
    
    if string.lower(file.fileExtension(name)) == "xml" or
       string.lower(file.fileExtension(name)) == "axml" or
       string.lower(file.fileExtension(name)) == "tmx" then
        return { type = "xml" }
    end
    
    if string.lower(file.fileExtension(name)) == "html" then
        return { type = "html" }
    end
    
    if string.lower(file.fileExtension(name)) == "c" or
       string.lower(file.fileExtension(name)) == "h" or
       string.lower(file.fileExtension(name)) == "cpp" or
       string.lower(file.fileExtension(name)) == "hpp" or
       string.lower(file.fileExtension(name)) == "cxx" or
       string.lower(file.fileExtension(name)) == "hxx" then
        return 
            { 
                type = "cpp",
                api = "apis/cpp",
                auto_complete_type = 1,
                comment_line = "//"
            }
    end
end

function fileFilter()
	filter = {}
	table.insert(filter, "Lua Files(*.lua)")
    table.insert(filter, "Octave Files(*.m)")
    table.insert(filter, "Bash Files(*.sh)")
    table.insert(filter, "Python Files(*.py)")
    table.insert(filter, "Tcl Files(*.tcl)")
    table.insert(filter, "Java Files(*.java)")
    table.insert(filter, "C# Files(*.csharp)")
    table.insert(filter, "Xml(*.xml)")
    table.insert(filter, "Html(*.html)")
    table.insert(filter, "C/C++ Files(*.cpp;*.hpp;*.cxx;*.hxx;*.c;*.h)")
	return filter
end

function isLegalFile(filename)
	local ext = file.fileExtension(filename)
    if ext == "lua" or
        ext == "m" or
        ext == "sh" or
        ext == "txt" or
        ext == "cmake" or
        ext == "py" or
        ext == "tcl" or
        ext == "java" or
        ext == "cs" or
        ext == "xml" or
        ext == "axml" or
        ext == "tmx" or
        ext == "html" or
        ext == "c" or
        ext == "h" or
        ext == "cpp" or
        ext == "cxx" or
        ext == "hpp" or
        ext == "hxx" or
        ext == "js" or
        ext == "inf" or
        ext == "log" then
        
        return true
    end
	
	return false
end
