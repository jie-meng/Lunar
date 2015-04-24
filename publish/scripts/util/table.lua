--[[
The Lua standard libraries do not provide a function to copy a table. However, it is relatively simple to implement such a function.
A generic table.copy function is not guaranteed to suit all use-cases, as there are many different aspects which must be selected for the specific situation. For example: should metatables be shared or copied? Should we check userdata for a __copy metamethod? These questions (as well as many others) must be asked and answered by the developer.

Some of the Lua "extended standard libraries", such as Penlight and stdlib provide ready-made copy functions for convenience. Check if they suit your use-case before implementing your own.

The following functions provide a base to work off of:
--]]

--[[
Shallow Copy
This a simple, naive implementation. It only copies the top level value and its direct children; 
there is no handling of deeper children, metatables or special types such as userdata or coroutines. 
It is also susceptible to influence by the __pairs metamethod.
--]]
function shallowCopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in pairs(orig) do
            copy[orig_key] = orig_value
        end
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

--[[
Deep Copy
A deep copy copies all levels (or a specific subset of levels).
Here is a simple recursive implementation that additionally handles metatables and avoid the __pairs metamethod.
Additionally, it is recursive which means it may overflow the stack for large tables.
--]]
function deepCopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

--[[
Non-recursive Deep Copy
A more flexible (non-recursive) deepcopy implementation is available from [this GitHub gist]. 
It allows for varying rules on how to copy special types, metatables, and function upvalues (including joining). 
Please see the comments for usage.
--]]