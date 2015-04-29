function parseFileType(filename)

	local path, name = file.splitPathname(filename)

	-- Lunar extension
	if name == "extension.lua" then
		if file.isPathFile(path .. "/Lunar") or file.isPathFile(path .. "/Lunar.exe") then
			return 
                {
                    type = "lua", 
                    auto_complete_type = 1, 
                    api = "apis/lua/standard,apis/lua/uextend.api,apis/lua/uextend_file.api,apis/lua/uextend_regex.api" 
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
                parse_supplement_api_func = "parseSupplementApi"
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
                parse_supplement_api_func = "parseSupplementApi"
            }
	end
    
	if string.lower(file.fileExtension(name)) == "m" then
		return 
            {
                type = "octave", 
                auto_complete_type = 0, 
                api = "apis/octave", 
                executor = "C:/Users/jmeng/programs/Octave-3.8.2/bin/octave.exe", 
                parse_supplement_api_script = "plugins/parse_supplement_api_octave.lua", 
                parse_supplement_api_func = "parseSupplementApi"
            }
	end
    
    if string.lower(file.fileExtension(name)) == "sh" then
        return { type = "bash" }
    end
    
	if string.lower(name) == "cmakelists.txt" then
		return { type = "cmake" }
	end
    
    if string.lower(file.fileExtension(name)) == "py" then
        return { type = "python" }
    end
    
    if string.lower(file.fileExtension(name)) == "tcl" then
        return { type = "tcl" }
    end
    
    if string.lower(file.fileExtension(name)) == "java" then
        return { type = "java" }
    end
    
    if string.lower(file.fileExtension(name)) == "cs" then
        return { type = "csharp" }
    end
    
    if string.lower(file.fileExtension(name)) == "xml" then
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
        return { type = "cpp" }
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

function ignoreFile(filename)
	local ext = file.fileExtension(filename)
	if ext == "so" or 
		ext == "o" or
		ext == "lib" or
		ext == "dll" or
		ext == "obj" or
		ext == "exe" or
		ext == "exp" or
		ext == "bin" or
		ext == "zip" or
		ext == "7z" or
		ext == "rar" or
		ext == "bmp" or
        ext == "ico" or
		ext == "png" or
		ext == "jpg" or
		ext == "jpeg" or
        ext == "wav" or
        ext == "ogg" or
        ext == "mp3" or
        ext == "wma" or
        ext == "wmv" or
        ext == "rmvb" or
        ext == "avi" or
		ext == "pdf" or
		ext == "chm" or
		ext == "doc" or
		ext == "docx" or
		ext == "xls" or
		ext == "xlsx" or
		ext == "ppt" or
		ext == "pptx"
		then
		return true
	end
	
	return false
end
