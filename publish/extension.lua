function checkJavaScriptType(filename)
    if string.lower(util.fileExtension(fileBaseName)) == "jsx" then
        return "react"
    end

    local content = util.readTextFile(filename)
    local path = util.splitPathname(filename)

    if util.strContains(content, "phantom.exit()") then
        return "phantom"
    end

    if util.strContains(content, "</")
        or util.strContains(content, "/>")
        or util.strContains(content, "React.")
        or util.strContains(content, "ReactDOM")
        or util.strContains(content, "React.Component") then
        return "react"
    end

    if util.strContains(content, "document.")
        or util.strContains(content, "alert(")
        or util.strContains(content, ".getElementById(")
        or util.strContains(content, ".getElementsByClassName(")
        or util.strContains(content, ".getElementsByName(")
        or util.strContains(content, ".getElementsByTagName(") then
        return "web"
    end

    return "node"
end

function parseFileType(filename)

	local path, name = util.splitPathname(filename)

	-- Lunar extension
	if name == "extension.lua" then
		if util.isPathFile(path .. "/Lunar") or util.isPathFile(path .. "/Lunar.exe") then
			return
                {
                    type = "lua",
                    auto_complete_type = 1,
                    api = "apis/lua/standard,apis/lua/util" ,
                    comment_line = "--",
					comment_block_begin = "--[[",
					comment_block_end = "]]",
                    templates = "templates/lua"
                }
		end
	end

    --cocos2dx project
    if util.isPathFile('.cocos-project.json') then
        if string.lower(util.fileExtension(name)) == "lua" then
            return
                {
                    type = "lua",
                    auto_complete_type = 1,
                    api = "apis/lua/standard,./cocos.api",
                    executor = "luaexec",
                    project_src_dir = "src/app",
                    plugin_goto = "plugins/goto_cocos_lua.lua",
                    plugin_parse_api = "plugins/parse_supplement_api_cocos_lua.lua",
                    comment_line = "--",
                    comment_block_begin = "--[[",
                    comment_block_end = "]]"
                }
        end

        if string.lower(util.fileExtension(name)) == "js" then
            local conf = {
                    type = "javascript",
                    auto_complete_type = 1,
                    api = "apis/javascript/js,./cocos.api",
                    executor = "luaexec",
                    project_src_dir = "src",
                    plugin_goto = "plugins/goto_cocos_js.lua",
                    plugin_parse_api = "plugins/parse_supplement_api_cocos_js.lua",
                    comment_line = "//",
                    comment_block_begin = "/*",
                    comment_block_end = "*/"
                }

            if util.isPathFile('webpack.config.js') then
                conf.plugin_goto = "plugins/goto_cocos_js_webpack.lua"
                conf.plugin_parse_api = "plugins/parse_supplement_api_cocos_js_webpack.lua"
            end

            return conf
        end
    end

	if string.lower(util.fileExtension(name)) == "lua" then
		return
            {
                type = "lua",
                auto_complete_type = 1,
                api = "apis/lua",
                executor = "luaexec",
                plugin_goto = "plugins/goto_lua.lua",
                plugin_parse_api = "plugins/parse_supplement_api_lua.lua",
                comment_line = "--",
				comment_block_begin = "--[[",
				comment_block_end = "]]",
                templates = "templates/lua"
            }
	end

	if string.lower(util.fileExtension(name)) == "m" then
		return
            {
                type = "octave",
                auto_complete_type = 0,
                api = "apis/octave",
                executor = "octave",
                plugin_parse_api = "plugins/parse_supplement_api_octave.lua",
                comment_line = "%"
            }
	end

	if string.lower(util.fileExtension(name)) == "js" or
        string.lower(util.fileExtension(name)) == "jsx" then
        local js_type = checkJavaScriptType(filename)
        local js_tb = {
            type = "javascript",
            auto_complete_type = 1,
            api = "apis/javascript/js",
            comment_line = "//",
            comment_block_begin = "/*",
            comment_block_end = "*/"
        }

        if js_type == "phantom" then
            js_tb.api = js_tb.api .. ',apis/javascript/node'
            js_tb.executor = 'phantomjs'
        end

        if js_type == "react" then
            js_tb.api = js_tb.api .. ',apis/javascript/react,apis/javascript/node,apis/javascript/web'
        end

        if js_type == "web" then
            js_tb.api = js_tb.api .. ',apis/javascript/web'
        end

        if js_type == "node" then
            js_tb.api = js_tb.api .. ',apis/javascript/node'
            js_tb.executor = 'node'
        end

        return js_tb
	end

	if string.lower(util.fileExtension(name)) == "html" or
        string.lower(util.fileExtension(name)) == "htm" then
		return
            {
                type = "html",
                auto_complete_type = 0,
				api = "apis/html",
                executor = "w3m",
				comment_block_begin = "<!--",
				comment_block_end = "-->"
            }
	end

	if string.lower(util.fileExtension(name)) == "css" or
        string.lower(util.fileExtension(name)) == "scss" then
		return
            {
                type = "css",
                auto_complete_type = 0,
				api = "apis/css",
				comment_block_begin = "/*",
				comment_block_end = "*/"
            }
	end

    if string.lower(util.fileExtension(name)) == "sh" then
        return { type = "bash", comment_line = "#", api = "apis/bash", executor = "sh", templates = "templates/bash" }
    end

	if string.lower(name) == "cmakelists.txt" or string.lower(util.fileExtension(name)) == "cmake" then
		return { type = "cmake", comment_line = "#", api = "apis/cmake", executor = "cmake" }
	end

    if util.fileBaseName(name) == "Dockerfile" then
		return { type = "", comment_line = "#", api = "apis/dockerfile", executor = "" }
	end

    if string.lower(util.fileExtension(name)) == "py" then
        local content = util.readTextFile(filename)
        if util.strContains(content, 'monkeyrunner', false) then
            return
            {
                type = "python",
                auto_complete_type = 1,
                api = "apis/python",
                executor = "monkeyrunner",
                plugin_goto = "plugins/goto_python.lua",
                plugin_parse_api = "plugins/parse_supplement_api_python.lua",
                comment_line = "#",
                templates = "templates/python,templates/python/2"
            }
        end

        local python3 = "python3"
        if util.strContains(util.platformInfo(), "win", false) then
            python3 = "python"
        end

        return
            {
                -- python3
                type = "python",
                auto_complete_type = 1,
                api = "apis/python/keywords.api,apis/python/monkeyrunner.api,apis/python/pydoc_gen3",
                executor =  python3 .. " -u",
                plugin_goto = "plugins/goto_python3.lua",
                plugin_parse_api = "plugins/parse_supplement_api_python3.lua",
                comment_line = "#",
                templates = "templates/python,templates/python/3"

                -- python2
                --type = "python",
                --auto_complete_type = 1,
                --api = "apis/python/keywords.api,apis/python/monkeyrunner.api,apis/python/pydoc_gen",
                --executor = "python -u",
                --plugin_goto = "plugins/goto_python.lua",
                --plugin_parse_api = "plugins/parse_supplement_api_python.lua",
                --comment_line = "#",
                --templates = "templates/python,templates/python/2"
            }
    end

    if string.lower(util.fileExtension(name)) == "rb" then
        return
            {
                type = "ruby",
                auto_complete_type = 0,
                api = "apis/ruby",
                executor = "ruby",
                comment_line = "#",
                comment_block_begin = "=begin\n",
				comment_block_end = "\n=end"
            }
    end


    if string.lower(util.fileExtension(name)) == "tcl" then
        return { type = "tcl", comment_line = "#" }
    end

    if string.lower(util.fileExtension(name)) == "java" then
        return
			{
				type = "java",
				api = "apis/java",
				comment_line = "//",
				comment_block_begin = "/*",
				comment_block_end = "*/"
			}
    end

    if string.lower(util.fileExtension(name)) == "cs" then
        return
			{
				type = "csharp",
				comment_line = "//",
				comment_block_begin = "/*",
				comment_block_end = "*/"
			}
    end

    if string.lower(util.fileExtension(name)) == "xml" or
        string.lower(util.fileExtension(name)) == "axml" or
        string.lower(util.fileExtension(name)) == "xaml" or
        string.lower(util.fileExtension(name)) == "xsd" or
        string.lower(util.fileExtension(name)) == "jxl" or
        string.lower(util.fileExtension(name)) == "rxl" or
        string.lower(util.fileExtension(name)) == "tmx" or
        string.lower(util.fileExtension(name)) == "svg" or
        string.lower(util.fileExtension(name)) == "iml" or
        string.lower(util.fileExtension(name)) == "project" then
        return
			{
				type = "xml",
				comment_block_begin = "<!--",
				comment_block_end = "-->"
			}
    end

    if string.lower(util.fileExtension(name)) == "c" or
        string.lower(util.fileExtension(name)) == "h" or
        string.lower(util.fileExtension(name)) == "cpp" or
        string.lower(util.fileExtension(name)) == "hpp" or
        string.lower(util.fileExtension(name)) == "cxx" or
        string.lower(util.fileExtension(name)) == "hxx" then
        return
            {
                type = "cpp",
                api = "apis/cpp",
                auto_complete_type = 1,
                templates = "templates/cpp",
				plugin_goto = "plugins/goto_cpp.lua",
				plugin_parse_api = "plugins/parse_supplement_api_cpp.lua",
                comment_line = "//",
				comment_block_begin = "/*",
				comment_block_end = "*/"
            }
    end

    -- github pages (jekyll) project
    if util.isPathFile('_config.yml') and util.isPathFile('Gemfile') and util.isPathDir('_posts') then
        if string.lower(util.fileExtension(name)) == "md" and util.isPathDir() then
            return
                {
                    type = "markdown",
                    auto_complete_type = 0,
                    templates = "templates/markdown,templates/gh_pages"
                }
        end
    end

    if string.lower(util.fileExtension(name)) == "md" then
        return
            {
                type = "markdown",
                auto_complete_type = 0,
                templates = "templates/markdown"
            }
    end

    if string.lower(util.fileExtension(name)) == "json" then
        return
        {
            type = "json",
            api = "apis/json",
            auto_completion_type = 0,
            comment_line = "//",
            comment_block_begin = "/*",
            comment_block_end = "*/"
        }
    end
end

function fileFilter()
	local filter = {}

    table.insert(filter, "All Files(*.*)")
	table.insert(filter, "Lua Files(*.lua)")
    table.insert(filter, "Python Files(*.py)")
    table.insert(filter, "Bash Files(*.sh)")
	table.insert(filter, "JSON Files(*.json)")
    table.insert(filter, "Markdown Files(*.md)")
    table.insert(filter, "C/C++ Files(*.cpp;*.hpp;*.cxx;*.hxx;*.c;*.h)")
    table.insert(filter, "JavaScript Files(*.js;*.jsx)")
	table.insert(filter, "CSS Files(*.css;*.scss)")
    table.insert(filter, "Html Files(*.html;*.htm)")
    table.insert(filter, "Ruby Files(*.rb)")
    table.insert(filter, "Tcl Files(*.tcl)")
	table.insert(filter, "C# Files(*.csharp)")
	table.insert(filter, "Java Files(*.java)")
    table.insert(filter, "Xml Files(*.xml)")
    table.insert(filter, "Octave Files(*.m)")

	return filter
end

local legalFileExtTable = {}
legalFileExtTable["lua"] = true
legalFileExtTable["py"] = true
legalFileExtTable["rb"] = true
legalFileExtTable["sh"] = true
legalFileExtTable["m"] = true
legalFileExtTable["bat"] = true
legalFileExtTable["cmd"] = true
legalFileExtTable["txt"] = true
legalFileExtTable["cmake"] = true
legalFileExtTable["tcl"] = true
legalFileExtTable["java"] = true
legalFileExtTable["cs"] = true
legalFileExtTable["xml"] = true
legalFileExtTable["axml"] = true
legalFileExtTable["xaml"] = true
legalFileExtTable["tmx"] = true
legalFileExtTable["xsd"] = true
legalFileExtTable["html"] = true
legalFileExtTable["htm"] = true
legalFileExtTable["c"] = true
legalFileExtTable["h"] = true
legalFileExtTable["cpp"] = true
legalFileExtTable["hpp"] = true
legalFileExtTable["cxx"] = true
legalFileExtTable["hxx"] = true
legalFileExtTable["js"] = true
legalFileExtTable["jsx"] = true
legalFileExtTable["css"] = true
legalFileExtTable["scss"] = true
legalFileExtTable["inf"] = true
legalFileExtTable["mk"] = true
legalFileExtTable["json"] = true
legalFileExtTable["api"] = true
legalFileExtTable["properties"] = true
legalFileExtTable["gitignore"] = true
legalFileExtTable["classpath"] = true
legalFileExtTable["md"] = true
legalFileExtTable["cfg"] = true
legalFileExtTable["gradle"] = true
legalFileExtTable["log"] = true
legalFileExtTable["svg"] = true

function isLegalFile(filename)
    if legalFileExtTable[string.lower(util.fileExtension(filename))] then
        return true
    end

    local base  = util.fileBaseName(filename)
    if string.lower(base) == "makefile" then
        return true
    end

    if util.fileSize(filename) == 0 then
        return true
    end

    return util.isTextFile(filename)
end

function compareFileWithLength(a, b)
    local path_a, name_a = util.splitPathname(a)
    local path_b, name_b = util.splitPathname(b)
    if string.len(name_a) == string.len(name_b) then
        return string.len(a) < string.len(b)
    else
        return string.len(name_a) < string.len(name_b)
    end
end

function sortFoundFiles(files, find_with_text)
    local result1 = {}
    local result2 = {}
    for _, v in ipairs(files) do
        v = util.strReplaceAll(v, '\\', '/')

        local path, name = util.splitPathname(v)
        if util.strStartWith(string.lower(name), find_with_text) then
            table.insert(result1, v)
        else
            table.insert(result2, v)
        end
    end

    table.sort(result1, compareFileWithLength)
    table.sort(result2, compareFileWithLength)

    for i, v in ipairs(result2) do
        table.insert(result1, v)
    end

    return result1
end

function findFilesInDir(dir, func)
    local paths = util.findFilesInDir(dir)
    local ret = {}
    for _, v in ipairs(paths) do
        if not func or func(v) then
            table.insert(ret, v)
        end
    end
    return ret
end

function findPathInDir(dir, func)
    local paths = util.findPathInDir(dir)
    local ret = {}
    for _, v in ipairs(paths) do
        if not func or func(v) then
            table.insert(ret, v)
        end
    end
    return ret
end

function findFilesFlatRecursively(path_deque, path_filter, file_filter, output, limit, stop_flag_address)
    if #path_deque > 0 then
        local dir = table.remove(path_deque, 1)
        local files = findFilesInDir(dir, file_filter)
        for _, v in ipairs(files) do
            if util.memGetUInt32(stop_flag_address) == 1 then
                return
            end

            table.insert(output, v)
            if limit and #output >= limit then
                return
            end
        end
        local dirs = findPathInDir(dir, path_filter)
        for _, v in ipairs(dirs) do
            if util.memGetUInt32(stop_flag_address) == 1 then
                return
            end

            table.insert(path_deque, v)
        end

        findFilesFlatRecursively(path_deque, path_filter, file_filter, output, limit, stop_flag_address)
    end
end

function findFilesFlat(dir, path_filter, file_filter, limit, stop_flag_address)
    local output = {}
    local deque = {}
    table.insert(deque, dir)
    findFilesFlatRecursively(deque, path_filter, file_filter, output, limit, stop_flag_address)
    return output
end

function findFiles(find_with_text, stop_flag_address)
    find_with_text = string.lower(find_with_text)
    local files = findFilesFlat(util.currentPath(),
        function(d)
            local path, name = util.splitPathname(d)
            return not util.strStartWith(b, '.')
        end,
        function(f)
            local path, name = util.splitPathname(f)
            return util.strContains(name, find_with_text, false)
        end,
        100, stop_flag_address)

    files = sortFoundFiles(files, find_with_text)

    local result_files = {}
    local non_legal_files = {}
    for i, v in ipairs(files) do
        if legalFileExtTable[util.fileExtension(v)] then
            table.insert(result_files, v)
        else
            table.insert(non_legal_files, v)
        end
    end

    for _, v in ipairs(non_legal_files) do
        table.insert(result_files, v)
    end

    return result_files
end

function buildIndentStr(indent)
    local t = {}
    for i = 1, indent do
        table.insert(t, ' ')
    end
    return table.concat(t, '')
end

function templateFileInfo(template_file, indent)
    local flag_begin = '{{BEGIN}}'
    local flag_end = '{{END}}'
    local indent_str = buildIndentStr(indent)

    local lines = {}
    local f = io.open(template_file, "r")
    if f then
        local line = f:read("*line")

        while line do
            repeat
                if #lines == 0 then
                    table.insert(lines, line)
                else
                    table.insert(lines, indent_str .. line)
                end
            until true
            line = f:read("*line")
        end

        io.close(f)
    end

    local content =  util.strTrim(table.concat(lines, '\n'))
    local begin_pos = string.find(content, flag_begin)

    if begin_pos then
        content = util.strReplace(content, flag_begin, '')
    end

    local end_pos = string.find(content, flag_end)
    if end_pos then
        content = util.strReplace(content, flag_end)
    end

    if not begin_pos or not end_pos then
        begin_pos = string.len(content) + 1
        end_pos = string.len(content) + 1
    end

    return content, begin_pos - 1, end_pos - 1
end
