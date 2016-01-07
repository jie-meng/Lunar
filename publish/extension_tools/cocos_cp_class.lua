print(string.format("Cocos copy class in path <%s>", file.currentPath()))

print()

print("Input src file name:")
local src = strTrim(io.read())
if src == "" or not file.isPathFile(src) then
    print("Error: Src file does not exist.")
    os.exit(0)
end
print("Src: " .. src)
local src_class = file.fileBaseName(src)

print("Input dst files names (split with ','):")
local dst = strTrim(io.read())
if dst == "" then
    print("Error: Find text cannot be empty")
    os.exit(0)
end
print("Dst: " .. dst)

print()

local tb = strSplit(dst, ",")
for _, v in ipairs(tb) do
    v = strTrim(v)
    local v_class = file.fileBaseName(v)
    file.writeTextFile(v, strRelaceAll(file.readTextFile(src), src_class, v_class))
    
    print(string.format([[Cocos copy class "%s" to "%s" in path <%s> ok.]], src_class, v_class, file.currentPath()))
end


