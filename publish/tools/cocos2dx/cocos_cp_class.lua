print(string.format("Cocos copy class in path <%s>", util.currentPath()))

print()

print("Input src file name:")
local src = util.strTrim(io.read())
if src == "" or not util.isPathFile(src) then
    print("Error: Src file does not exist.")
    os.exit(0)
end
print("Src: " .. src)
local src_class = util.fileBaseName(src)

print("Input dst files names (split with ','):")
local dst = util.strTrim(io.read())
if dst == "" then
    print("Error: Find text cannot be empty")
    os.exit(0)
end
print("Dst: " .. dst)

print()

local tb = util.strSplit(dst, ",")
for _, v in ipairs(tb) do
    v = util.strTrim(v)
    local v_class = util.fileBaseName(v)
    util.writeTextFile(v, util.strReplaceAll(util.readTextFile(src), src_class, v_class))
    
    print(string.format([[Cocos copy class "%s" to "%s" in path <%s> ok.]], src_class, v_class, util.currentPath()))
end
