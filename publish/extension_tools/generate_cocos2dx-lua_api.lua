--[[ ApiModule start --]]

local ApiModule = {
    name_ = nil,
    parent_ = nil,
    tmp_function_ = nil,
    extend_ = {},
    functions_ = {}
}

function ApiModule:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    
    o.extend_ = {}
    o.functions_ = {}
    
    return o
end

function ApiModule:getName()
    return self.name_
end

function ApiModule:getFullName()
    local fullname = ""
    if self.name_ then
        fullname = self.name_
        if self.parent_ then
            fullname = self.parent_ .. "." .. fullname
        end
    end
    return fullname
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
    self.extend_ = util.strSplit(extend, ",")
    for i, v in ipairs(self.extend_) do
        self.extend_[i] = util.strTrim(v)
    end
end

function ApiModule:addExtend(extend)
    table.insert(self.extend_, extend)
end

function ApiModule:removeExtend(extend)
    for i, v in ipairs(self.extend_) do
        if v == extend then
            table.remove(self.extend_, i)
            break
        end
    end
end

function ApiModule:setParent(parent)
    self.parent_ = parent
end

function ApiModule:getFunctions()
    return self.functions_
end

function ApiModule:addFunction(func)
    table.insert(self.functions_, func)
end

function ApiModule:StartFunc(func)
    self.tmp_function_ = func .. "("
end

function ApiModule:AddParamToFunc(param)
    if self.tmp_function_ then
        local prefix = ""
        if not util.strEndWith(self.tmp_function_, "(") then
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
        table.insert(self.functions_, string.format("%s) --> %s (%s)", self.tmp_function_, return_type, return_obj))
        self.tmp_function_ = nil
        return true
    else
        return false
    end
end

--[[ ApiModule end --]]

function createCocosApis()
    
    local apis = {}
    
    local t1 = util.findFilesInDirRecursively(util.currentPath() .. "/src/cocos", "lua")
    local t2 = util.findFilesInDirRecursively(util.currentPath() .. "/src/package", "lua")
    local t3 = util.findFilesInDirRecursively(util.currentPath() .. "/frameworks/cocos2d-x/cocos/scripting/lua-bindings/auto/api", "lua")
    local t4 = util.findFilesInDirRecursively(util.currentPath() .. "/frameworks/cocos2d-x/cocos/scripting/lua-bindings/manual", "cpp")
    
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
    local classes = {}
    for k, v in pairs(t3) do
        print("parse file " .. v)
        local api_module = parseAutoApi(v)
        -- keep a record for inherit process
        classes[api_module:getFullName()] = api_module
    end
    
    -- process inherit
    local processed_classes = processClassInherits(classes)
    --local processed_classes = classes
    -- add functions
    for k, v in pairs(processed_classes) do
        for i, f in ipairs(v:getFunctions()) do
            table.insert(apis, k .. "." .. f)
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
    
    local extfunctions = util.currentPath() .. "/src/extfunctions.lua"
    print("parse file " .. extfunctions)
    parseApi(extfunctions, apis)
    
    if #apis == 0 then
        print("no api found")
        return
    end
    
    print("parsed " .. tostring(#apis) .. " functions.")
    
    content = util.strJoin(apis, "\n")
    util.writeTextFile("cocos.api", content)
end

function parseApi(filename, apis)
    
    local pattern_tb_function_lua = [[function%s+([%w_]+)[.:]([%w_]+)%s*(%(.*%))]]
    local pattern_function_lua = [[function%s+([%w_]+)%s*(%(.*%))]]
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "--") then
                    break
                end
            
                local tb, func, param = string.match(line, pattern_tb_function_lua)
                if tb and func and param then
                    table.insert(apis, string.format("%s.%s%s", tb, func, param))
                    break
                end
                
                func, param = string.match(line, pattern_function_lua)
                if func and param then
                    table.insert(apis, string.format("%s%s", func, param))
                    break
                end
            
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
end

function parseAutoApi(filename)
    
    local pattern_module = [[--%s*@module%s+([%w_]+)]]
    local pattern_extend = [[--%s*@extend%s+([%w%s_,]+)]]
    local pattern_parent_module = [[--%s*@parent_module%s+([%w_]+)]]
    local pattern_function = [[--%s*@function%s+%[.+%]%s+([%w_]+)]]
    local pattern_param = [[--%s*@param%s+#(.+)]]
    local pattern_return = [[--%s*@return%s+.+%s+([%w_]+)%s+%(return%s+value:%s+(.+)%)]]
    
    local api_module = ApiModule:new()
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if util.strTrim(line) == "" then
                    break
                end
                
                local test_function = string.match(line, pattern_function)
                if test_function then
                    api_module:StartFunc(test_function)
                end 
               
                local test_param = string.match(line, pattern_param)
                if test_param then
                    api_module:AddParamToFunc(test_param)
                    break
                end
                
                local test_return_obj, test_return_type = string.match(line, pattern_return)
                if test_return_obj and test_return_type then
                    api_module:EndFunc(test_return_obj, test_return_type)
                    break
                end 
                
                local test_module = string.match(line, pattern_module)
                if test_module then                    
                    api_module:setName(test_module)
                    break
                end
                
                local test_extend = string.match(util.strTrim(line), pattern_extend)
                if test_extend then
                    api_module:setExtend(test_extend)
                    break
                end
                
                local test_parent_module = string.match(line, pattern_parent_module)
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
    
    local pattern_class = [[lua_pushstring%s*%(%s*.+%s*,%s*"([%w_]+)%.([%w_]+)"%s*%)]]
    local pattern_pushstring = [[lua_pushstring%s*%(%s*.+%s*,%s*"([%w_]+)"%s*%)]]
    local pattern_tolua_function = [[tolua_function%s*%(%s*.+%s*,%s*"([%w_]+)"%s*,%s*.+%s*%)%s*]]
    local pattern_tolua_begin_module = [[tolua_beginmodule%s*%(%s*.+%s*,%s*"([%w_]+)"%s*%)]]
    local pattern_tolua_end_module = [[tolua_endmodule%s*%(%s*.+%s*%)]]
    
    local tb = {}
    
    local f = io.open(filename, "r")
    if f ~= nil then
        local prefix_class = nil
        local push_string = nil
        local begin_modules = {}
    
        local line = f:read("*line")
        while (line ~= nil) do
            repeat
                if util.strTrim(line) == "" or util.strStartWith(util.strTrimLeft(line), "--") then
                    break
                end
            
                local tolua_function = string.match(line, pattern_tolua_function)
                if #begin_modules > 0 and tolua_function then
                    table.insert(tb, util.strJoin(begin_modules, ".") .. "." .. tolua_function .. "(?)")
                    push_string = nil
                    break
                end
                if tolua_function and prefix_class then
                    table.insert(tb, prefix_class .. "." .. tolua_function .. "(?)")
                    push_string = nil
                    break
                end
                
                local str = string.match(line, pattern_pushstring)
                if str then
                    push_string = str    
                    break
                end
                
                if util.strStartWith(line, "lua_pushcfunction") then
                    if prefix_class and push_string then
                        table.insert(tb, prefix_class .. "." .. push_string .. "(?)")
                        push_string = nil
                    end
                    break
                end
                
                if util.strStartWith(line, "lua_pop") then
                    prefix_class = nil
                    push_string = nil
                    break
                end
                
                local prefix, class = string.match(line, pattern_class)
                if prefix and class then
                    prefix_class = prefix .. "." .. class
                    push_string = nil
                    break
                end
                
                local bm = string.match(line, pattern_tolua_begin_module)
                if bm then
                    table.insert(begin_modules, bm)
                end
                
                local em = string.match(line, pattern_tolua_end_module)
                if em then
                    table.remove(begin_modules)
                end
                
                push_string = nil
            until true
            
            line = f:read("*line")
        end
        io.close(f)
    end
    
    return tb
end

function processClassInherits(classes)
    
    local classes_processed = {}
    
    while true do
        local left_count = 0
        for k, v in pairs(classes) do
            left_count = left_count + 1
            
            -- put base class into classes_processed
            if #v:getExtend() == 0 then
                classes_processed[k] = v
                classes[k] = nil
            else
                for i, e in ipairs(v:getExtend()) do 
                    repeat
                    -- super_class is in classes_processed, process it
                        local key = ""
                        if v:getParent() then
                            key = v:getParent() .. "." .. e
                        else
                            key = e
                        end
                    
                        if classes_processed[key] then
                            local functions = classes_processed[key]:getFunctions()
                            for _, f in ipairs(functions) do
                                v:addFunction(f)
                            end
                            --print("remove " .. key .. " from " .. v:getFullName())
                            v:removeExtend(e)
                            break
                        end
                        
                        -- super_class still not put into classes_processed, keep it and wait for next loop
                        if classes[key] then
                            break
                        end
                        
                        -- not extend from current classes, ignore it.
                        v:removeExtend(e)
                        
                    until true
                end
            end
        end -- main for
        
        if left_count == 0 then
            break
        end
    end -- while
    
    return classes_processed
end

-- main
createCocosApis()
print("create api finished")
