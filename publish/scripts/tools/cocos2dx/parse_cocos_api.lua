--[[ ApiModule start --]]

local ApiModule = {
    name_ = nil,
    extend_ = nil,
    parent_ = nil,
    tmp_function_ = nil,
    functions_ = {}
}

function ApiModule:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    
    o.functions_ = {}
    
    return o
end

function ApiModule:getName()
    return self.name_
end

function ApiModule:getExtend()
    return self.extend_
end

function ApiModule:getParent()
    return self.parent_
end

function ApiModule:setName(name)
    self.name_ = name
end

function ApiModule:setExtend(extend)
    self.extend_ = extend
end

function ApiModule:setParent(parent)
    self.parent_ = parent
end

function ApiModule:getFunctions()
    return self.functions_
end

function ApiModule:StartFunc(func)
    self.tmp_function_ = func .. "("
end

function ApiModule:AddParamToFunc(param)
    if self.tmp_function_ then
        local prefix = ""
        if not strEndWith(self.tmp_function_, "(") then
            prefix = ", "
        end
        self.tmp_function_ = self.tmp_function_ .. prefix .. "[" .. param .. "]"
        return true
    else
        return false
    end
end

function ApiModule:EndFunc(return_obj, return_type)
    if self.tmp_function_ then
        table.insert(self.functions_, self.tmp_function_ .. ") -- return " .. return_obj .. " " .. return_type)
        self.tmp_function_ = nil
        return true
    else
        return false
    end
end

--[[ ApiModule end --]]

function createCocosApis()
    
    local apis = {}
    
    local t1 = file.findFilesInDirRecursively(file.currentPath() .. "/src/cocos", "lua")
    local t2 = file.findFilesInDirRecursively(file.currentPath() .. "/src/package", "lua")
    local t3 = file.findFilesInDirRecursively(file.currentPath() .. "/frameworks/cocos2d-x/cocos/scripting/lua-bindings/auto/api", "lua")
    local t4 = file.findFilesInDirRecursively(file.currentPath() .. "/frameworks/cocos2d-x/cocos/scripting/lua-bindings/manual", "cpp")
    
    -- lua api
    for k, v in pairs(t1) do
        print("parse file " .. v)
        parseApi(v, apis)
    end
    
    for k, v in pairs(t2) do
        print("parse file " .. v)
        parseApi(v, apis)
    end
    
    -- auto api
    for k, v in pairs(t3) do
        print("parse file " .. v)
        local api_module = parseAutoApi(v)
        local functions = api_module:getFunctions()
        local prefix = ""
        if api_module:getName() then
            prefix = api_module:getName() .. "."
            if api_module:getParent() then
                prefix = api_module:getParent() .. "." .. prefix
            end
        end
        
        for _, v in pairs(functions) do
            table.insert(apis, prefix .. v)
            --print(prefix .. v)
        end
    end
    
    -- manual api
    for k, v in ipairs(t4) do
        print("parse file " .. v)
        local api_file = parseManualApi(v)
        for _, vx in ipairs(api_file) do
            table.insert(apis, vx)
        end
    end
    
    if #apis == 0 then
        print("no api found")
        return
    end
    
    print("parsed " .. tostring(#apis) .. " functions.")
    
    content = strJoin(apis, "\n")
    file.writeTextFile("cocos.api", content)
end

function parseApi(filename, apis)
    
    local kRegexFunctionLua = "function\\s+(?<api>(\\w+((\\.|:)\\w+)*\\s*\\(.*\\)))"
    local re_func = regex.create(kRegexFunctionLua)
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            if regex.match(re_func, strTrim(line)) then
                local api = regex.getMatchedGroupByName(re_func, "api")
                if api ~= "" then
                    local api_format, _ = string.gsub(api, ":", ".")
                    table.insert(apis, api_format)
                end
            end
            line = f:read("*line")
        end
        io.close(f)
    end
    
    regex.destroy(re_func)
end

function parseAutoApi(filename)
    
    local pattern_module = [[--%s*@module%s+(%w+)]]
    local pattern_extend = [[--%s*@extend%s+(%w+)]]
    local pattern_parent_module = [[--%s*@parent_module%s+(%w+)]]
    local pattern_function = [[--%s*@function%s+%[.+%]%s+(%w+)]]
    local pattern_param = [[--%s*@param%s+#(.+)]]
    local pattern_return = [[--%s*@return%s+.+%s+(%w+)%s+%(return%s+value:%s+(.+)%)]]
    
    local api_module = ApiModule:new()
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
            
                local test_function = string.match(strTrim(line), pattern_function)
                if test_function then
                    api_module:StartFunc(test_function)
                end 
                
                local test_param = string.match(strTrim(line), pattern_param)
                if test_param then
                    api_module:AddParamToFunc(test_param)
                    break
                end
                
                local test_return_obj, test_return_type = string.match(strTrim(line), pattern_return)
                if test_return_obj and test_return_type then
                    api_module:EndFunc(test_return_obj, test_return_type)
                    break
                end 
                
                local test_module = string.match(strTrim(line), pattern_module)
                if test_module then                    
                    api_module:setName(test_module)
                    break
                end
                
                local test_extend = string.match(strTrim(line), pattern_extend)
                if test_extend then
                    api_module:setExtend(test_extend)
                    break
                end
                
                local test_parent_module = string.match(strTrim(line), pattern_parent_module)
                if test_parent_module then
                    api_module:setParent(test_parent_module)
                    break
                end
            until true
            
            --if parent_module and module and func
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    return api_module
end

function parseManualApi(filename)
    
    local pattern_class = [[lua_pushstring%s*%(%s*.+%s*,%s*"(%w+)%.(%w+)"%s*%)]]
    local pattern_pushstring = [[lua_pushstring%s*%(%s*.+%s*,%s*"(%w+)"%s*%)]]
    local pattern_tolua_function = [[tolua_function%s*%(%s*.+%s*,%s*"(%w+)"%s*,%s*.+%s*%)%s*]]
    
    local tb = {}
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local prefix_class = nil
        local push_string = nil
    
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                local tolua_function = string.match(strTrim(line), pattern_tolua_function)
                if tolua_function ~= nil and prefix_class ~= nil  then
                    table.insert(tb, prefix_class .. "." .. tolua_function .. "(?)")
                    push_string = nil    
                    break
                end
                
                local str = string.match(strTrim(line), pattern_pushstring)
                if str ~= nil then
                    push_string = str    
                    break
                end
                
                if strStartWith(strTrim(line), "lua_pushcfunction") then
                    if prefix_class ~= nil and push_string ~= nil then
                        table.insert(tb, prefix_class .. "." .. push_string .. "(?)")
                        push_string = nil
                    end
                    break
                end
                
                if strStartWith(strTrim(line), "lua_pop") then
                    prefix_class = nil
                    push_string = nil
                    break
                end
                
                local prefix, class = string.match(strTrim(line), pattern_class)
                if prefix ~= nil and class ~= nil then
                    prefix_class = prefix .. "." .. class
                    push_string = nil
                    break
                end
                
                push_string = nil
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    return tb
end

-- main
createCocosApis()
print("create api finished")