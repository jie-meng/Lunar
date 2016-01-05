print(string.format("Replace text in path <%s>", file.currentPath()))

print()

print("Input target file ext (Empty means all files):")
local ext = strTrim(io.read())
print("Ext: " .. ext)

print("Input find text:")
local find_text = strTrim(io.read())
print("Find text: " .. find_text)

print("Input replace text:")
local replace_text = strTrim(io.read())
print("Replace text: " .. replace_text)

local tb = file.findFilesInDirRecursively(file.currentPath(), ext)
for _, v in ipairs(tb) do
    local text = file.readTextFile(v)
    if strContains(text, find_text) then
        local replace = strRelaceAll(text, find_text, replace_text)
        print("Replace in file " .. v .. "  ok.")
        file.writeTextFile(v, replace)
    end
end

print()

print(string.format([[Replace all "%s" to "%s" in path <%s> finish.]], find_text, replace_text, file.currentPath()))
